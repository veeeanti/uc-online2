#pragma once

#include "platform.h"

#ifdef _WIN32

LSTATUS GetRegistryDWORD(const char* cszPath, const char* cszKey, DWORD& dwValue)
{
	HKEY hKeyNode = nullptr;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	LSTATUS GetRegKey = ERROR_SUCCESS;

	GetRegKey = RegOpenKeyExA(HKEY_CURRENT_USER, cszPath, 0, KEY_READ, &hKeyNode);

	if (GetRegKey != ERROR_SUCCESS)
		return GetRegKey;

	GetRegKey = RegQueryValueExA(hKeyNode, cszKey, nullptr, &dwType, (LPBYTE)&dwValue, &dwSize);

	RegCloseKey(hKeyNode);

	return GetRegKey;
}

LSTATUS GetRegistryString(const char* cszPath, const char* cszKey, char* szString, unsigned int uMaxBuf)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = uMaxBuf;

	LSTATUS GetRegKey = RegGetValueA(HKEY_CURRENT_USER, cszPath, cszKey, RRF_RT_REG_SZ, &dwType, szString, &dwSize);

	return GetRegKey;
}

#else // Linux / macOS

// Registry functions are Windows-only. On Linux/Mac, Steam config is read from
// filesystem paths. These stubs return errors; the actual logic is in api_client.h.

#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2

inline int GetRegistryDWORD(const char*, const char*, DWORD&)
{
	return ERROR_FILE_NOT_FOUND;
}

inline int GetRegistryString(const char*, const char*, char*, unsigned int)
{
	return ERROR_FILE_NOT_FOUND;
}

#endif
