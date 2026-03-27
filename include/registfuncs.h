#pragma once

#include <Windows.h>

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
