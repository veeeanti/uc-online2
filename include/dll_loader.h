#pragma once

#include "platform.h"
#include <vector>
#include <algorithm>
#include <string>

#ifdef _WIN32

class CDLLLoader
{
private:
	std::vector<HMODULE> m_Modules;
	char m_IniPath[MAX_PATH];

public:
	CDLLLoader() { m_IniPath[0] = '\0'; }
	~CDLLLoader() { UnloadAll(); }

	void ReadConfig()
	{
		char exeDir[MAX_PATH] = { 0 };
		DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
		if (len == 0) return;
		if (!PathRemoveFileSpecA(exeDir)) return;

		_snprintf_s(m_IniPath, MAX_PATH, _TRUNCATE, "%s\\union-crax.ini", exeDir);

		DWORD attribs = GetFileAttributesA(m_IniPath);
		if (attribs == INVALID_FILE_ATTRIBUTES)
			m_IniPath[0] = '\0';
	}

	uint32 GetAppId()
	{
		if (m_IniPath[0] == '\0')
			return 480;

		char buf[16] = { 0 };
		GetPrivateProfileStringA("Settings", "AppId", "480", buf, sizeof(buf), m_IniPath);

		if (buf[0] == '\0')
			return 480;

		uint32 id = (uint32)strtoul(buf, nullptr, 10);
		return (id == 0) ? 480 : id;
	}

	void LoadPlugins()
	{
		if (m_IniPath[0] == '\0')
			return;

		char exeDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
		PathRemoveFileSpecA(exeDir);

		char folderName[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("Settings", "PluginsFolder", "", folderName, MAX_PATH, m_IniPath);

		if (folderName[0] == '\0')
			return;

		char dllPath[MAX_PATH] = { 0 };
		if (_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\%s", exeDir, folderName) == _TRUNCATE)
			return;

		DWORD folderAttribs = GetFileAttributesA(dllPath);
		if (folderAttribs == INVALID_FILE_ATTRIBUTES || !(folderAttribs & FILE_ATTRIBUTE_DIRECTORY))
			return;

		char findPattern[MAX_PATH] = { 0 };
		if (_snprintf_s(findPattern, MAX_PATH, _TRUNCATE, "%s\\*.dll", dllPath) == _TRUNCATE)
			return;

		std::vector<std::string> names;
		std::vector<std::string> paths;

		WIN32_FIND_DATAA fd = { 0 };
		HANDLE hFind = FindFirstFileA(findPattern, &fd);

		if (hFind == INVALID_HANDLE_VALUE)
			return;

		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			char fullPath[MAX_PATH] = { 0 };
			if (_snprintf_s(fullPath, MAX_PATH, _TRUNCATE, "%s\\%s", dllPath, fd.cFileName) == _TRUNCATE)
				continue;

			names.push_back(fd.cFileName);
			paths.push_back(fullPath);
		} while (FindNextFileA(hFind, &fd));

		FindClose(hFind);

		for (size_t i = 0; i < names.size(); i++)
		{
			size_t minIdx = i;
			for (size_t j = i + 1; j < names.size(); j++)
			{
				if (_stricmp(names[j].c_str(), names[minIdx].c_str()) < 0)
					minIdx = j;
			}

			if (minIdx != i)
			{
				std::swap(names[i], names[minIdx]);
				std::swap(paths[i], paths[minIdx]);
			}
		}

		for (size_t i = 0; i < names.size(); i++)
		{
			HMODULE hMod = LoadLibraryExA(paths[i].c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (hMod)
			{
				m_Modules.push_back(hMod);
				UCOLOG("[UCOnline2] Loaded plugin: %s", names[i].c_str());
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to load plugin: %s (error %lu)", names[i].c_str(), GetLastError());
			}
		}
	}

	void UnloadAll()
	{
		for (size_t i = 0; i < m_Modules.size(); i++)
		{
			if (m_Modules[i])
				FreeLibrary(m_Modules[i]);
		}
		m_Modules.clear();
	}

	size_t LoadedCount() const { return m_Modules.size(); }
};

#else // Linux / macOS

// Simple INI file parser for non-Windows platforms
static std::string IniReadString(const char* iniPath, const char* section, const char* key, const char* defaultVal)
{
	FILE* f = fopen(iniPath, "r");
	if (!f) return defaultVal;

	char line[512];
	bool inSection = false;
	size_t sectionLen = strlen(section);
	size_t keyLen = strlen(key);

	while (fgets(line, sizeof(line), f))
	{
		// Trim leading whitespace
		char* p = line;
		while (*p == ' ' || *p == '\t') p++;

		// Remove trailing newline
		char* nl = strchr(p, '\n');
		if (nl) *nl = '\0';
		nl = strchr(p, '\r');
		if (nl) *nl = '\0';

		// Skip empty lines and comments
		if (*p == '\0' || *p == ';' || *p == '#') continue;

		// Section header
		if (*p == '[')
		{
			p++;
			char* end = strchr(p, ']');
			if (end)
			{
				*end = '\0';
				inSection = (_stricmp(p, section) == 0);
			}
			continue;
		}

		// Key=value within our section
		if (inSection)
		{
			char* eq = strchr(p, '=');
			if (eq)
			{
				*eq = '\0';
				char* k = p;
				char* v = eq + 1;

				// Trim key trailing whitespace
				char* kEnd = k + strlen(k) - 1;
				while (kEnd > k && (*kEnd == ' ' || *kEnd == '\t')) { *kEnd = '\0'; kEnd--; }

				// Trim value leading whitespace
				while (*v == ' ' || *v == '\t') v++;

				if (_stricmp(k, key) == 0)
				{
					std::string result(v);
					fclose(f);
					return result;
				}
			}
		}
	}

	fclose(f);
	return defaultVal;
}

class CDLLLoader
{
private:
	std::vector<PLAT_MODULE_T> m_Modules;
	char m_IniPath[MAX_PATH];

public:
	CDLLLoader() { m_IniPath[0] = '\0'; }
	~CDLLLoader() { UnloadAll(); }

	void ReadConfig()
	{
		std::string exeDir = PlatGetExeDir();
		if (exeDir.empty()) return;

		snprintf(m_IniPath, MAX_PATH, "%s%cunion-crax.ini", exeDir.c_str(), PLAT_SEP);

		if (access(m_IniPath, R_OK) != 0)
			m_IniPath[0] = '\0';
	}

	uint32 GetAppId()
	{
		if (m_IniPath[0] == '\0')
			return 480;

		std::string val = IniReadString(m_IniPath, "Settings", "AppId", "480");
		if (val.empty()) return 480;

		uint32 id = (uint32)strtoul(val.c_str(), nullptr, 10);
		return (id == 0) ? 480 : id;
	}

	void LoadPlugins()
	{
		if (m_IniPath[0] == '\0')
			return;

		std::string exeDir = PlatGetExeDir();
		std::string folderName = IniReadString(m_IniPath, "Settings", "PluginsFolder", "");

		if (folderName.empty())
			return;

		std::string pluginDir = exeDir + PLAT_SEP + folderName;

		struct stat st;
		if (stat(pluginDir.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			return;

		std::vector<std::string> names;
		std::vector<std::string> paths;

		DIR* dir = opendir(pluginDir.c_str());
		if (!dir) return;

		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			std::string name(entry->d_name);

			// Skip . and .. and non-.so/.dylib files
			if (name == "." || name == "..") continue;

#if defined(__linux__)
			if (name.size() < 4 || name.substr(name.size() - 3) != ".so") continue;
#elif defined(__APPLE__)
			if (name.size() < 8 || name.substr(name.size() - 6) != ".dylib") continue;
#endif

			std::string fullPath = pluginDir + PLAT_SEP + name;
			names.push_back(name);
			paths.push_back(fullPath);
		}
		closedir(dir);

		// Sort alphabetically (selection sort like original)
		for (size_t i = 0; i < names.size(); i++)
		{
			size_t minIdx = i;
			for (size_t j = i + 1; j < names.size(); j++)
			{
				if (_stricmp(names[j].c_str(), names[minIdx].c_str()) < 0)
					minIdx = j;
			}

			if (minIdx != i)
			{
				std::swap(names[i], names[minIdx]);
				std::swap(paths[i], paths[minIdx]);
			}
		}

		for (size_t i = 0; i < names.size(); i++)
		{
			PLAT_MODULE_T hMod = PlatLoadLibraryA(paths[i].c_str());
			if (hMod)
			{
				m_Modules.push_back(hMod);
				UCOLOG("[UCOnline2] Loaded plugin: %s", names[i].c_str());
			}
			else
			{
				UCOLOG("[UCOnline2] Failed to load plugin: %s (%s)", names[i].c_str(), dlerror());
			}
		}
	}

	void UnloadAll()
	{
		for (size_t i = 0; i < m_Modules.size(); i++)
		{
			if (m_Modules[i])
				PlatFreeLibrary(m_Modules[i]);
		}
		m_Modules.clear();
	}

	size_t LoadedCount() const { return m_Modules.size(); }
};

#endif // !_WIN32
