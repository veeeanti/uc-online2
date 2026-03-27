/*  This should only be a debug build feature, as it is not needed by myself really.
    This also only dumps crashes that may occur when using this, I cannot guarantee stability. 
    So do expect potential issues. Thank you for your patience. */

#include "include/dump_handler.h"

#ifdef _DEBUG

#include <Windows.h>
#include <string>
#include <DbgHelp.h>

CDumpHandler::CDumpHandler()
{
	m_Comment.clear();
	m_hDbgHelp = nullptr;
	m_pfnWriteDump = nullptr;
	m_bReady = false;

	m_hDbgHelp = LoadLibraryExW(L"DbgHelp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!m_hDbgHelp)
		return;

	FARPROC fp = GetProcAddress(m_hDbgHelp, "MiniDumpWriteDump");
	if (!fp)
	{
		FreeLibrary(m_hDbgHelp);
		m_hDbgHelp = nullptr;
		return;
	}

	m_pfnWriteDump = (Fn_MiniDumpWriteDump)fp;

	InitializeSRWLock(&m_Lock);
	m_bReady = true;
}

CDumpHandler::~CDumpHandler()
{
	m_pfnWriteDump = nullptr;
	m_Comment.clear();

	if (m_hDbgHelp)
	{
		FreeLibrary(m_hDbgHelp);
		m_hDbgHelp = nullptr;
	}

	m_bReady = false;
}

bool CDumpHandler::IsReady()
{
	return m_bReady;
}

void CDumpHandler::SetComment(const wchar_t* comment)
{
	m_Comment.assign(comment);
}

size_t CDumpHandler::GetCommentByteSize()
{
	if (m_Comment.empty()) return 0;
	return m_Comment.length() * sizeof(wchar_t);
}

const wchar_t* CDumpHandler::GetComment()
{
	return m_Comment.c_str();
}

void CDumpHandler::ClearComment()
{
	m_Comment.clear();
}

void CDumpHandler::WriteDump(DWORD exceptionCode, _EXCEPTION_POINTERS* pExceptionInfo)
{
	if (!m_hDbgHelp || !m_pfnWriteDump)
		return;

	HANDLE hProcess = GetCurrentProcess();
	DWORD pid = GetCurrentProcessId();

	AcquireSRWLockExclusive(&m_Lock);

	MINIDUMP_EXCEPTION_INFORMATION excInfo = { 0 };
	excInfo.ThreadId = GetCurrentThreadId();
	excInfo.ExceptionPointers = pExceptionInfo;
	excInfo.ClientPointers = FALSE;

	MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
		MiniDumpWithProcessThreadData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo);

	if (GetCommentByteSize() != 0)
	{
		MINIDUMP_USER_STREAM streams[2] = { 0 };

		streams[0].Type = CommentStreamW;
		streams[0].BufferSize = (ULONG)GetCommentByteSize();
		streams[0].Buffer = (LPVOID)GetComment();

		MINIDUMP_EXCEPTION_STREAM excStream = { 0 };
		excStream.ExceptionRecord.ExceptionCode = exceptionCode;

		streams[1].Type = ExceptionStream;
		streams[1].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
		streams[1].Buffer = &excStream;

		MINIDUMP_USER_STREAM_INFORMATION streamInfo = { 0 };
		streamInfo.UserStreamCount = 2;
		streamInfo.UserStreamArray = streams;

		m_pfnWriteDump(hProcess, pid, nullptr, dumpType, &excInfo, &streamInfo, nullptr);
	}
	else
	{
		MINIDUMP_USER_STREAM streams[1] = { 0 };

		MINIDUMP_EXCEPTION_STREAM excStream = { 0 };
		excStream.ExceptionRecord.ExceptionCode = exceptionCode;

		streams[0].Type = ExceptionStream;
		streams[0].BufferSize = sizeof(MINIDUMP_EXCEPTION_STREAM);
		streams[0].Buffer = &excStream;

		MINIDUMP_USER_STREAM_INFORMATION streamInfo = { 0 };
		streamInfo.UserStreamCount = 1;
		streamInfo.UserStreamArray = streams;

		m_pfnWriteDump(hProcess, pid, nullptr, dumpType, &excInfo, &streamInfo, nullptr);
	}

	ReleaseSRWLockExclusive(&m_Lock);
}

#endif
