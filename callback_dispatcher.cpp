#include <Windows.h>
#include <Shlwapi.h>
#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/callback_dispatcher.h"
#include "include/globals.h"

void UCOColor(WORD color, const char* text);
extern HSteamUser g_ClientUser;
extern HSteamUser g_ServerUser;

static bool s_bDispatcherReady = false;

CCallbackDispatcher::CCallbackDispatcher()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher constructed\r\n");

	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
	s_bDispatcherReady = true;
}

CCallbackDispatcher::~CCallbackDispatcher()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher destroyed\r\n");

	s_bDispatcherReady = false;
	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
}

void CCallbackDispatcher::Shutdown()
{
	UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] CCallbackDispatcher shutdown\r\n");

	s_bDispatcherReady = false;
	m_pfnBGetCallback = nullptr;
	m_pfnFreeLastCallback = nullptr;
	m_pfnGetAPICallResult = nullptr;
	m_CurrentUser = 0;
	m_ManualCbId = 0;
	m_ManualCbSize = 0;
	m_bProcessing = false;
	m_CallbackMap.clear();
	m_CallResultMap.clear();
	m_BufferMap.clear();
}

void CCallbackDispatcher::ExecuteCallResult(HSteamPipe hPipe, SteamAPICall_t hCall, CCallbackBase* pCb)
{
	UCOLOG("[UCOnline2] ExecuteCallResult -> call=%lld cb=%d size=%d\r\n", hCall, pCb->GetICallback(), pCb->GetCallbackSizeBytes());

	BYTE* pBuffer = new BYTE[pCb->GetCallbackSizeBytes()]();
	bool bFailed = false;

	bool bResult = m_pfnGetAPICallResult(hPipe, hCall, pBuffer, pCb->GetCallbackSizeBytes(), pCb->GetICallback(), &bFailed);

	if (bResult && !bFailed)
	{
		size_t countBefore = m_CallbackMap.size();

		pCb->Run(pBuffer, bFailed, hCall);

		if (countBefore != m_CallbackMap.size())
		{
			UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] Callback map changed during CallResult execution\r\n");

			m_CallbackMap.erase(LobbyEnter_t::k_iCallback);
			pCb->Run(pBuffer);
		}

		m_BufferMap.insert(std::make_pair(hCall, pBuffer));
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] GetAPICallResult failed\r\n");
		delete[] pBuffer;
	}
}

void CCallbackDispatcher::DispatchFrame(HSteamPipe hPipe, bool bServer)
{
	if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
		return;

	CallbackMsg_t msg = { 0 };
	if (!m_pfnBGetCallback(hPipe, &msg))
		return;

	UCOLOG("[UCOnline2] Callback received -> %d\r\n", msg.m_iCallback);
	m_CurrentUser = msg.m_hSteamUser;

	if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
	{
		SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

		if (!m_CallResultMap.empty())
		{
			for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
			{
				if (pCompleted->m_hAsyncCall == it->first &&
					pCompleted->m_iCallback == it->second->GetICallback() &&
					pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
				{
					ExecuteCallResult(hPipe, it->first, it->second);
				}
			}
		}
	}
	else
	{
		if (!m_CallbackMap.empty())
		{
			for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
			{
				CCallbackBase* pCb = it->second;

				if (it->first == msg.m_iCallback && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
				{
					if (msg.m_hSteamUser == g_ServerUser && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && bServer)
					{
						UCOLOG("[UCOnline2] Server callback -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
						pCb->Run(msg.m_pubParam);
						break;
					}
					else if (msg.m_hSteamUser == g_ClientUser && !(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && !bServer)
					{
						UCOLOG("[UCOnline2] Client callback -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);

						bool bSkip = false;

						if (msg.m_iCallback == HTML_NeedsPaint_t::k_iCallback && msg.m_cubParam == sizeof(HTML_NeedsPaint_t))
						{
							HTML_NeedsPaint_t* pPaint = (HTML_NeedsPaint_t*)msg.m_pubParam;
							if (pPaint->unWide == 1 || pPaint->unTall == 1)
								bSkip = true;
						}

						if (!bSkip)
							pCb->Run(msg.m_pubParam);

						break;
					}
				}
			}
		}
	}

	UCOLOG("[UCOnline2] Freeing callback -> %d\r\n", msg.m_iCallback);
	SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
	m_pfnFreeLastCallback(hPipe);
}

void CCallbackDispatcher::DispatchFrameSafe(HSteamPipe hPipe, bool bServer)
{
	try
	{
		if (!m_pfnBGetCallback || !m_pfnFreeLastCallback || !m_pfnGetAPICallResult)
			return;

		CallbackMsg_t msg = { 0 };
		if (!m_pfnBGetCallback(hPipe, &msg))
			return;

		UCOLOG("[UCOnline2] Callback (safe) -> %d\r\n", msg.m_iCallback);
		m_CurrentUser = msg.m_hSteamUser;

		if (msg.m_iCallback == SteamAPICallCompleted_t::k_iCallback && msg.m_cubParam == sizeof(SteamAPICallCompleted_t))
		{
			SteamAPICallCompleted_t* pCompleted = (SteamAPICallCompleted_t*)msg.m_pubParam;

			if (!m_CallResultMap.empty())
			{
				for (auto it = m_CallResultMap.begin(); it != m_CallResultMap.end(); ++it)
				{
					if (pCompleted->m_hAsyncCall == it->first &&
						pCompleted->m_iCallback == it->second->GetICallback() &&
						pCompleted->m_cubParam == (uint32)it->second->GetCallbackSizeBytes())
					{
						ExecuteCallResult(hPipe, it->first, it->second);
					}
				}
			}
		}
		else
		{
			if (!m_CallbackMap.empty())
			{
				for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
				{
					CCallbackBase* pCb = it->second;

					if (it->first == msg.m_iCallback && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
					{
						if (msg.m_hSteamUser == g_ServerUser && (pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && bServer)
						{
							UCOLOG("[UCOnline2] Server callback (safe) -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
							pCb->Run(msg.m_pubParam);
							break;
						}
						else if (msg.m_hSteamUser == g_ClientUser && !(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsGameServer) && !bServer)
						{
							UCOLOG("[UCOnline2] Client callback (safe) -> %d flags=%d\r\n", msg.m_iCallback, pCb->m_nCallbackFlags);
							pCb->Run(msg.m_pubParam);
							break;
						}
					}
				}
			}
		}

		UCOLOG("[UCOnline2] Freeing callback (safe) -> %d\r\n", msg.m_iCallback);
		SecureZeroMemory(msg.m_pubParam, msg.m_cubParam);
		m_pfnFreeLastCallback(hPipe);
	}
	catch (...)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Exception in callback dispatch\r\n");
	}
}

void CCallbackDispatcher::Add(CCallbackBase* pCb, int iCallback)
{
	if (!pCb)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Add: null callback ptr\r\n");
		return;
	}

	if (pCb->GetCallbackSizeBytes() == 0)
		return;

	UCOLOG("[UCOnline2] Register callback -> %d size=%d flags=%d\r\n", iCallback, pCb->GetCallbackSizeBytes(), pCb->m_nCallbackFlags);

	pCb->m_nCallbackFlags |= pCb->k_ECallbackFlagsRegistered;
	pCb->m_iCallback = iCallback;
	m_CallbackMap.insert(std::make_pair(iCallback, pCb));
}

void CCallbackDispatcher::AddCallResult(CCallbackBase* pCb, SteamAPICall_t hCall)
{
	if (!pCb || hCall == k_uAPICallInvalid)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: invalid args\r\n");
		return;
	}

	if (pCb->GetICallback() == 0 || pCb->GetCallbackSizeBytes() == 0)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: zero callback\r\n");
		return;
	}

	UCOLOG("[UCOnline2] Register call result -> %lld cb=%d size=%d\r\n", hCall, pCb->GetICallback(), pCb->GetCallbackSizeBytes());

	pCb->m_nCallbackFlags |= pCb->k_ECallbackFlagsRegistered;

	auto existing = m_CallResultMap.find(hCall);
	if (existing == m_CallResultMap.end())
	{
		m_CallResultMap.insert(std::make_pair(hCall, pCb));
	}
	else
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] AddCallResult: already registered\r\n");
		existing->second = pCb;
	}
}

void CCallbackDispatcher::Remove(CCallbackBase* pCb)
{
	if (!pCb)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Remove: null callback ptr\r\n");
		return;
	}

	if (!(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] Remove: not registered\r\n");
		return;
	}

	if (m_CallbackMap.empty())
		return;

	UCOLOG("[UCOnline2] Unregister callback -> %d flags=%d\r\n", pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

	for (auto it = m_CallbackMap.begin(); it != m_CallbackMap.end(); ++it)
	{
		if (it->second == pCb)
		{
			m_CallbackMap.erase(it);
			break;
		}
	}
}

void CCallbackDispatcher::RemoveCallResult(CCallbackBase* pCb, SteamAPICall_t hCall)
{
	if (!pCb || hCall == k_uAPICallInvalid)
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] RemoveCallResult: invalid args\r\n");
		return;
	}

	if (!(pCb->m_nCallbackFlags & pCb->k_ECallbackFlagsRegistered))
	{
		UCOColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "[UCOnline2] RemoveCallResult: not registered\r\n");
		return;
	}

	UCOLOG("[UCOnline2] Unregister call result -> %lld cb=%d flags=%d\r\n", hCall, pCb->GetICallback(), pCb->m_nCallbackFlags);
	pCb->m_nCallbackFlags &= ~pCb->k_ECallbackFlagsRegistered;

	auto bufIt = m_BufferMap.find(hCall);
	if (bufIt != m_BufferMap.end())
	{
		UCOColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY, "[UCOnline2] Freeing call result buffer\r\n");
		delete[] bufIt->second;
		m_BufferMap.erase(bufIt);
	}
}

CCallbackDispatcher* GetDispatcher()
{
	static CCallbackDispatcher instance;
	return &instance;
}
