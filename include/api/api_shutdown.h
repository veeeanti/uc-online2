S_API void S_CALLTYPE SteamAPI_Shutdown()
{
	UCOLOG("[UCOnline2] SteamAPI_Shutdown\r\n");

	if (g_pSteamClient)
	{
		g_bClientReady = false;

		g_ClientCtx.Clear();

		if (g_ClientUser != 0)
		{
			g_pSteamClient->ReleaseUser(g_ClientPipe, g_ClientUser);
			g_ClientUser = 0;
		}

		if (g_ClientPipe != 0)
		{
			g_pSteamClient->BReleaseSteamPipe(g_ClientPipe);
			g_ClientPipe = 0;
		}

		g_pUtilsForCallbacks = nullptr;
		g_pControllerForCallbacks = nullptr;
		g_pInputForCallbacks = nullptr;

		bool bClosed = g_pSteamClient->BShutdownIfAllPipesClosed();
		if (!bClosed)
		{
			SteamAPI_ReleaseCurrentThreadMemory();
		}
		else
		{
			if (s_bDispatcherInited)
				GetDispatcher()->Shutdown();
		}

		if (!g_bServerReady)
		{
			g_pfnReleaseThreadLocal = nullptr;
			g_pSteamClientSafe = nullptr;
		}


		g_pSteamClient = nullptr;
		g_DispatchMode = 3;

		PlatFreeLibrary(g_ClientModule);
		g_ClientModule = PLAT_INVALID_MODULE;

		g_pfnBreakpadWriteDump = nullptr;
		g_pfnBreakpadSetComment = nullptr;
		g_pfnBreakpadSetSteamID = nullptr;
		g_pfnBreakpadSetAppID = nullptr;
		g_pfnBreakpadInit = nullptr;

		g_CtxCounter++;
	}
}

S_API void S_CALLTYPE SteamGameServer_Shutdown()
{
	UCOLOG("[UCOnline2] SteamGameServer_Shutdown\r\n");

	if (g_pGameServer)
	{
		if (g_pGameServer->BLoggedOn())
		{
			g_pGameServer->LogOff();
			while (g_pGameServer->BLoggedOn()) {}
		}
		g_pGameServer = nullptr;
	}

	if (g_ServerClient)
	{
		g_bServerReady = false;

		g_ServerCtx.Clear();
		g_pServerUtils = nullptr;

		if (g_ServerPipe != 0 && g_ServerUser != 0)
		{
			g_ServerClient->ReleaseUser(g_ServerPipe, g_ServerUser);
			g_ServerClient->BReleaseSteamPipe(g_ServerPipe);
			g_ServerUser = 0;
			g_ServerPipe = 0;
		}

		bool bClosed = g_ServerClient->BShutdownIfAllPipesClosed();
		if (!bClosed)
		{
			SteamAPI_ReleaseCurrentThreadMemory();
		}
		else
		{
			if (s_bDispatcherInited)
				GetDispatcher()->Shutdown();
		}

		if (!g_bClientReady)
		{
			g_pfnReleaseThreadLocal = nullptr;
			g_pSteamClientSafe = nullptr;
		}


		g_ServerClient = nullptr;
		g_DispatchMode = 3;

		PlatFreeLibrary(g_ServerModule);
		g_ServerModule = PLAT_INVALID_MODULE;

		g_pfnBreakpadWriteDump = nullptr;
		g_pfnBreakpadSetComment = nullptr;
		g_pfnBreakpadSetSteamID = nullptr;
		g_pfnBreakpadSetAppID = nullptr;
		g_pfnBreakpadInit = nullptr;

		g_CtxCounter++;
	}
}
