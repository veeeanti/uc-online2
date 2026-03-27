S_API void* S_CALLTYPE SteamInternal_CreateInterface(const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] SteamInternal_CreateInterface -> %s\r\n", ver);

		HMODULE hMod = g_ClientModule;
		if (g_ServerModule) hMod = g_ServerModule;

		if (hMod)
		{
			g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(hMod, "CreateInterface");
			if (g_pfnCreateInterface)
				return g_pfnCreateInterface(ver, nullptr);
		}
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] CreateInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamGameServerInternal_CreateInterface(const char* iface)
{
	if (iface)
	{
		UCOLOG("[UCOnline2] SteamGameServerInternal_CreateInterface -> %s\r\n", iface);

		if (g_ServerModule)
		{
			g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(g_ServerModule, "CreateInterface");
			if (g_pfnCreateInterface)
				return g_pfnCreateInterface(iface, nullptr);
		}
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GameServerCreateInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateUserInterface(HSteamUser hUser, const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] FindOrCreateUserInterface -> %s\r\n", ver);

		if (g_pSteamClient && g_ClientPipe != 0)
		{
			void* pIface = g_pSteamClient->GetISteamGenericInterface(hUser, g_ClientPipe, ver);
			if (!pIface)
				WarnMissingInterface(g_ClientPipe, ver);

			return pIface;
		}

		char msg[MAX_PATH] = { 0 };
		_snprintf_s(msg, MAX_PATH, _TRUNCATE, "[UCOnline2] Tried to access %s before SteamAPI_Init\r\n", ver);
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, msg);
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] FindOrCreateUserInterface: returning null\r\n");
	return nullptr;
}

S_API void* S_CALLTYPE SteamInternal_FindOrCreateGameServerInterface(HSteamUser hUser, const char* ver)
{
	if (ver)
	{
		UCOLOG("[UCOnline2] FindOrCreateGameServerInterface -> %s\r\n", ver);

		if (g_ServerClient && g_ServerPipe != 0)
		{
			void* pIface = g_ServerClient->GetISteamGenericInterface(hUser, g_ServerPipe, ver);
			if (!pIface)
				WarnMissingInterface(g_ServerPipe, ver);

			return pIface;
		}

		char msg[MAX_PATH] = { 0 };
		_snprintf_s(msg, MAX_PATH, _TRUNCATE, "[UCOnline2] Tried to access %s before GameServer_Init\r\n", ver);
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, msg);
	}

	UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] FindOrCreateGameServerInterface: returning null\r\n");
	return nullptr;
}
