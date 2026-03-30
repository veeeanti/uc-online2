#pragma once

// ============================================================
// Cross-platform abstraction layer for uc-online2
// ============================================================

#if defined(_WIN32)

// ---- Windows ----
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>

#define PLAT_SEP '\\'
#define PLAT_DLL_EXT ".dll"
#define PLAT_LIB_PREFIX ""
#define PLAT_MODULE_T HMODULE
#define PLAT_INVALID_MODULE NULL
#define PLAT_CALLTYPE __cdecl

// getpid() shim for Windows (POSIX function)
inline int getpid() { return (int)GetCurrentProcessId(); }

#define PlatLoadLibraryW(path) LoadLibraryW(path)
#define PlatLoadLibraryA(path) LoadLibraryA(path)
#define PlatLoadLibraryExA(path, reserved, flags) LoadLibraryExA(path, reserved, flags)
#define PlatGetProcAddress(mod, name) GetProcAddress(mod, name)
#define PlatFreeLibrary(mod) FreeLibrary(mod)
#define PlatGetModuleHandleW(name) GetModuleHandleW(name)

typedef SRWLOCK PlatLock;
inline void PlatLockInit(PlatLock& lock) { InitializeSRWLock(&lock); }
inline void PlatLockAcquire(PlatLock& lock) { AcquireSRWLockExclusive(&lock); }
inline void PlatLockRelease(PlatLock& lock) { ReleaseSRWLockExclusive(&lock); }

#else

// ---- Linux / macOS ----
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <cctype>
#include <cerrno>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>

#if defined(__linux__)
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <mach-o/dyld.h>
#endif

// ---- Platform basics ----
#define PLAT_SEP '/'
#define PLAT_LIB_EXT ".so"
#define PLAT_LIB_PREFIX "lib"
#define PLAT_MODULE_T void*
#define PLAT_INVALID_MODULE nullptr
#define PLAT_CALLTYPE

#define PlatLoadLibraryA(path) dlopen(path, RTLD_LAZY)
#define PlatLoadLibraryW(path) dlopen(path, RTLD_LAZY)
#define PlatGetProcAddress(mod, name) dlsym(mod, name)
#define PlatFreeLibrary(mod) dlclose(mod)
#define PlatGetModuleHandleW(name) nullptr

// ---- Lock abstraction ----
typedef std::mutex PlatLock;
inline void PlatLockInit(PlatLock& lock) { /* mutex self-initializes */ (void)lock; }
inline void PlatLockAcquire(PlatLock& lock) { lock.lock(); }
inline void PlatLockRelease(PlatLock& lock) { lock.unlock(); }

// ---- Windows type stubs ----
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef unsigned char* LPBYTE;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* FARPROC;

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((void*)(long)-1)
#endif

// Console color stubs (ignored on non-Windows)
#ifndef FOREGROUND_RED
#define FOREGROUND_RED       0x0004
#define FOREGROUND_GREEN     0x0002
#define FOREGROUND_BLUE      0x0001
#define FOREGROUND_INTENSITY 0x0008
#endif

// ---- Compat shims for MSVC functions ----
#ifndef _stricmp
#define _stricmp strcasecmp
#endif

#ifndef _strnicmp
#define _strnicmp strncasecmp
#endif

#ifndef _vsnprintf_s
#define _vsnprintf_s(buf, size, count, fmt, args) vsnprintf(buf, size, fmt, args)
#endif

#ifndef _snprintf_s
// Inline function overloads that properly forward variadic format args
template<typename... Args>
inline int _snprintf_s_impl(char* buf, size_t size, int count, const char* fmt, Args... args) {
    return snprintf(buf, size, fmt, args...);
}
template<typename... Args>
inline int _snprintf_s_impl(char* buf, size_t size, int count, int truncate, const char* fmt, Args... args) {
    return snprintf(buf, size, fmt, args...);
}
// Use a simple inline wrapper; callers use _snprintf_s(buf, size, _TRUNCATE, fmt, ...)
#define _snprintf_s(...) _snprintf_s_impl(__VA_ARGS__)
#endif

#ifndef _TRUNCATE
#define _TRUNCATE -1
#endif

#ifndef fopen_s
inline int fopen_s(FILE** pFile, const char* filename, const char* mode) {
    if (!pFile) return EINVAL;
    *pFile = fopen(filename, mode);
    return (*pFile) ? 0 : errno;
}
#endif

#ifndef SecureZeroMemory
inline void SecureZeroMemory(void* ptr, size_t len) {
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (len--) *p++ = 0;
}
#endif

#ifndef __debugbreak
#if defined(__GNUC__) || defined(__clang__)
#define __debugbreak() __builtin_trap()
#else
#define __debugbreak() raise(SIGTRAP)
#endif
#endif

#ifndef strcasestr
inline const char* strcasestr_local(const char* haystack, const char* needle) {
    if (!*needle) return haystack;
    for (; *haystack; ++haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (tolower((unsigned char)*h) == tolower((unsigned char)*n))) {
            ++h; ++n;
        }
        if (!*n) return haystack;
    }
    return nullptr;
}
#define StrStrIA(haystack, needle) strcasestr_local(haystack, needle)
#elif !defined(StrStrIA)
#define StrStrIA strcasestr
#endif

// ---- Path helpers ----
inline BOOL PathRemoveFileSpecA(char* path) {
    if (!path || !path[0]) return FALSE;
    char* last_sep = strrchr(path, PLAT_SEP);
    if (last_sep) { *last_sep = '\0'; return TRUE; }
    return FALSE;
}

inline BOOL PathAppendA(char* path, const char* more) {
    if (!path || !more) return FALSE;
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] != PLAT_SEP) {
        char sep = PLAT_SEP;
        strncat(path, &sep, 1);
    }
    strncat(path, more, MAX_PATH - strlen(path) - 1);
    return TRUE;
}

// Get path of the shared library containing the given address
inline std::string PlatGetModulePath(void* addr) {
    Dl_info info;
    if (dladdr(addr, &info) && info.dli_fname) {
        return std::string(info.dli_fname);
    }
    return "";
}

// Get directory of the current executable
inline std::string PlatGetExeDir() {
    char buf[MAX_PATH] = {0};
#if defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len > 0) buf[len] = '\0';
#elif defined(__APPLE__)
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) != 0) buf[0] = '\0';
#endif
    std::string path(buf);
    auto pos = path.rfind(PLAT_SEP);
    return (pos != std::string::npos) ? path.substr(0, pos) : path;
}

// ---- Environment variables ----
inline BOOL SetEnvironmentVariableA(const char* name, const char* value) {
    if (value) return setenv(name, value, 1) == 0;
    unsetenv(name);
    return TRUE;
}

// ---- Process helpers (for Steam detection) ----
#if defined(__linux__)
inline bool PlatIsProcessRunning(pid_t pid) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d", (int)pid);
    struct stat st;
    return stat(path, &st) == 0;
}
#elif defined(__APPLE__)
inline bool PlatIsProcessRunning(pid_t pid) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "kill -0 %d 2>/dev/null", (int)pid);
    return system(cmd) == 0;
}
#endif

#endif // !_WIN32
