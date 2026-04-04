S_API ISteamApps* S_CALLTYPE SteamApps()
{
	UCOLOG("[UCOnline2] SteamApps\r\n");
	return g_bClientReady ? g_ClientCtx.SteamApps() : nullptr;
}

S_API ISteamClient* S_CALLTYPE SteamClient()
{
	UCOLOG("[UCOnline2] SteamClient\r\n");
	return g_pSteamClientSafe;
}

S_API ISteamController* S_CALLTYPE SteamController()
{
	UCOLOG("[UCOnline2] SteamController\r\n");
	return g_bClientReady ? g_ClientCtx.SteamController() : nullptr;
}

S_API ISteamFriends* S_CALLTYPE SteamFriends()
{
	UCOLOG("[UCOnline2] SteamFriends\r\n");
	return g_bClientReady ? g_ClientCtx.SteamFriends() : nullptr;
}

S_API ISteamClient* S_CALLTYPE SteamGameServerSteamClient()
{
	UCOLOG("[UCOnline2] SteamGameServerSteamClient\r\n");
	return g_bServerReady ? g_ServerCtx.SteamClient() : nullptr;
}

S_API ISteamGameServer* S_CALLTYPE SteamGameServer()
{
	UCOLOG("[UCOnline2] SteamGameServer\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServer() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamGameServerApps()
{
	UCOLOG("[UCOnline2] SteamGameServerApps\r\n");
	return g_bServerReady ? g_ServerCtx.SteamApps() : nullptr;
}

S_API ISteamHTTP* S_CALLTYPE SteamGameServerHTTP()
{
	UCOLOG("[UCOnline2] SteamGameServerHTTP\r\n");
	return g_bServerReady ? g_ServerCtx.SteamHTTP() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamGameServerInventory()
{
	UCOLOG("[UCOnline2] SteamGameServerInventory\r\n");
	return g_bServerReady ? g_ServerCtx.SteamInventory() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamGameServerNetworking()
{
	UCOLOG("[UCOnline2] SteamGameServerNetworking\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerNetworking() : nullptr;
}

S_API ISteamGameServerStats* S_CALLTYPE SteamGameServerStats()
{
	UCOLOG("[UCOnline2] SteamGameServerStats\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerStats() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamGameServerUGC()
{
	UCOLOG("[UCOnline2] SteamGameServerUGC\r\n");
	return g_bServerReady ? g_ServerCtx.SteamUGC() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamGameServerUtils()
{
	UCOLOG("[UCOnline2] SteamGameServerUtils\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerUtils() : nullptr;
}

S_API ISteamGameSearch* S_CALLTYPE SteamGameSearch()
{
	UCOLOG("[UCOnline2] SteamGameSearch\r\n");
	return g_bClientReady ? g_ClientCtx.SteamGameSearch() : nullptr;
}

S_API ISteamHTMLSurface* S_CALLTYPE SteamHTMLSurface()
{
	UCOLOG("[UCOnline2] SteamHTMLSurface\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTMLSurface() : nullptr;
}

S_API ISteamHTTP* S_CALLTYPE SteamHTTP()
{
	UCOLOG("[UCOnline2] SteamHTTP\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTTP() : nullptr;
}

S_API ISteamInput* S_CALLTYPE SteamInput()
{
	UCOLOG("[UCOnline2] SteamInput\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInput() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamInventory()
{
	UCOLOG("[UCOnline2] SteamInventory\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInventory() : nullptr;
}

S_API ISteamMatchmaking* S_CALLTYPE SteamMatchmaking()
{
	UCOLOG("[UCOnline2] SteamMatchmaking\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmaking() : nullptr;
}

S_API ISteamMatchmakingServers* S_CALLTYPE SteamMatchmakingServers()
{
	UCOLOG("[UCOnline2] SteamMatchmakingServers\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmakingServers() : nullptr;
}

S_API ISteamMusic* S_CALLTYPE SteamMusic()
{
	UCOLOG("[UCOnline2] SteamMusic\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMusic() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamNetworking()
{
	UCOLOG("[UCOnline2] SteamNetworking\r\n");
	return g_bClientReady ? g_ClientCtx.SteamNetworking() : nullptr;
}

S_API ISteamParentalSettings* S_CALLTYPE SteamParentalSettings()
{
	UCOLOG("[UCOnline2] SteamParentalSettings\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParentalSettings() : nullptr;
}

S_API ISteamParties* S_CALLTYPE SteamParties()
{
	UCOLOG("[UCOnline2] SteamParties\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParties() : nullptr;
}

S_API ISteamRemotePlay* S_CALLTYPE SteamRemotePlay()
{
	UCOLOG("[UCOnline2] SteamRemotePlay\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemotePlay() : nullptr;
}

S_API ISteamRemoteStorage* S_CALLTYPE SteamRemoteStorage()
{
	UCOLOG("[UCOnline2] SteamRemoteStorage\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemoteStorage() : nullptr;
}

S_API ISteamScreenshots* S_CALLTYPE SteamScreenshots()
{
	UCOLOG("[UCOnline2] SteamScreenshots\r\n");
	return g_bClientReady ? g_ClientCtx.SteamScreenshots() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamUGC()
{
	UCOLOG("[UCOnline2] SteamUGC\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUGC() : nullptr;
}

S_API ISteamUser* S_CALLTYPE SteamUser()
{
	UCOLOG("[UCOnline2] SteamUser\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUser() : nullptr;
}

S_API ISteamUserStats* S_CALLTYPE SteamUserStats()
{
	UCOLOG("[UCOnline2] SteamUserStats\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUserStats() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamUtils()
{
	UCOLOG("[UCOnline2] SteamUtils\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUtils() : nullptr;
}

S_API ISteamVideo* S_CALLTYPE SteamVideo()
{
	UCOLOG("[UCOnline2] SteamVideo\r\n");
	return g_bClientReady ? g_ClientCtx.SteamVideo() : nullptr;
}

//
//

S_API ISteamUser* S_CALLTYPE SteamAPI_SteamUser_v023()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUser_v023\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUser() : nullptr;
}

S_API ISteamUserStats* S_CALLTYPE SteamAPI_SteamUserStats_v013()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUserStats_v013\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUserStats() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamAPI_SteamUtils_v010()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUtils_v010\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUtils() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamUGC_v021()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamUGC_v021\r\n");
	return g_bClientReady ? g_ClientCtx.SteamUGC() : nullptr;
}

S_API ISteamFriends* S_CALLTYPE SteamAPI_SteamFriends_v018()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamFriends_v018\r\n");
	return g_bClientReady ? g_ClientCtx.SteamFriends() : nullptr;
}

S_API ISteamUtils* S_CALLTYPE SteamAPI_SteamGameServerUtils_v010()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerUtils_v010\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerUtils() : nullptr;
}

S_API ISteamUGC* S_CALLTYPE SteamAPI_SteamGameServerUGC_v021()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerUGC_v021\r\n");
	return g_bServerReady ? g_ServerCtx.SteamUGC() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamAPI_SteamGameServerApps_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerApps_v008\r\n");
	return g_bServerReady ? g_ServerCtx.SteamApps() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamAPI_SteamGameServerNetworking_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworking_v006\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerNetworking() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamGameServerNetworkingMessages_SteamAPI_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworkingMessages_SteamAPI_v002\r\n");
	return SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
}

S_API void* S_CALLTYPE SteamAPI_SteamGameServerNetworkingSockets_SteamAPI_v012()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerNetworkingSockets_SteamAPI_v012\r\n");
	return SteamInternal_FindOrCreateGameServerInterface(g_ServerUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
}

S_API ISteamHTTP* S_CALLTYPE SteamAPI_SteamGameServerHTTP_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerHTTP_v003\r\n");
	return g_bServerReady ? g_ServerCtx.SteamHTTP() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamAPI_SteamGameServerInventory_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerInventory_v003\r\n");
	return g_bServerReady ? g_ServerCtx.SteamInventory() : nullptr;
}

S_API ISteamGameServer* S_CALLTYPE SteamAPI_SteamGameServer_v015()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServer_v015\r\n");
	return g_bServerReady ? g_pGameServer : nullptr;
}

S_API ISteamGameServerStats* S_CALLTYPE SteamAPI_SteamGameServerStats_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamGameServerStats_v001\r\n");
	return g_bServerReady ? g_ServerCtx.SteamGameServerStats() : nullptr;
}

S_API ISteamMatchmaking* S_CALLTYPE SteamAPI_SteamMatchmaking_v009()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMatchmaking_v009\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmaking() : nullptr;
}

S_API ISteamMatchmakingServers* S_CALLTYPE SteamAPI_SteamMatchmakingServers_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMatchmakingServers_v002\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMatchmakingServers() : nullptr;
}

S_API ISteamMusic* S_CALLTYPE SteamAPI_SteamMusic_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamMusic_v001\r\n");
	return g_bClientReady ? g_ClientCtx.SteamMusic() : nullptr;
}

S_API ISteamParties* S_CALLTYPE SteamAPI_SteamParties_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamParties_v002\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParties() : nullptr;
}

S_API ISteamParentalSettings* S_CALLTYPE SteamAPI_SteamParentalSettings_v001()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamParentalSettings_v001\r\n");
	return g_bClientReady ? g_ClientCtx.SteamParentalSettings() : nullptr;
}

S_API ISteamRemoteStorage* S_CALLTYPE SteamAPI_SteamRemoteStorage_v016()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamRemoteStorage_v016\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemoteStorage() : nullptr;
}

S_API ISteamRemotePlay* S_CALLTYPE SteamAPI_SteamRemotePlay_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamRemotePlay_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamRemotePlay() : nullptr;
}

S_API ISteamApps* S_CALLTYPE SteamAPI_SteamApps_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamApps_v008\r\n");
	return g_bClientReady ? g_ClientCtx.SteamApps() : nullptr;
}

S_API ISteamNetworking* S_CALLTYPE SteamAPI_SteamNetworking_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworking_v006\r\n");
	return g_bClientReady ? g_ClientCtx.SteamNetworking() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingSockets_SteamAPI_v012()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingSockets_SteamAPI_v012\r\n");
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGSOCKETS_INTERFACE_VERSION);
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingUtils_SteamAPI_v004()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingUtils_SteamAPI_v004\r\n");

	void* pResult = SteamInternal_FindOrCreateUserInterface(0, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
	if (!pResult)
		pResult = SteamInternal_FindOrCreateGameServerInterface(0, STEAMNETWORKINGUTILS_INTERFACE_VERSION);

	return pResult;
}

S_API void* S_CALLTYPE SteamAPI_SteamNetworkingMessages_SteamAPI_v002()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamNetworkingMessages_SteamAPI_v002\r\n");
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
}

S_API ISteamScreenshots* S_CALLTYPE SteamAPI_SteamScreenshots_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamScreenshots_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamScreenshots() : nullptr;
}

S_API void* S_CALLTYPE SteamAPI_SteamTimeline_v004()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamTimeline_v004\r\n");

	if (!g_bClientReady) return nullptr;
	return SteamInternal_FindOrCreateUserInterface(g_ClientUser, STEAMTIMELINE_INTERFACE_VERSION);
}

S_API ISteamHTTP* S_CALLTYPE SteamAPI_SteamHTTP_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamHTTP_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTTP() : nullptr;
}

S_API ISteamHTMLSurface* S_CALLTYPE SteamAPI_SteamHTMLSurface_v005()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamHTMLSurface_v005\r\n");
	return g_bClientReady ? g_ClientCtx.SteamHTMLSurface() : nullptr;
}

S_API ISteamInput* S_CALLTYPE SteamAPI_SteamInput_v006()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamInput_v006\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInput() : nullptr;
}

S_API ISteamInventory* S_CALLTYPE SteamAPI_SteamInventory_v003()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamInventory_v003\r\n");
	return g_bClientReady ? g_ClientCtx.SteamInventory() : nullptr;
}

S_API ISteamController* S_CALLTYPE SteamAPI_SteamController_v008()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamController_v008\r\n");
	return g_bClientReady ? g_ClientCtx.SteamController() : nullptr;
}

S_API ISteamVideo* S_CALLTYPE SteamAPI_SteamVideo_v007()
{
	UCOLOG("[UCOnline2] SteamAPI_SteamVideo_v007\r\n");
	return g_bClientReady ? g_ClientCtx.SteamVideo() : nullptr;
}
