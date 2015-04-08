//
// Softanics, Artem A. Razin, All rights reserved, Copyright 2004 - 2010
//
// BoxedApp SDK
//
// Ask your questions here: http://boxedapp.com/support.html
// Our forum: http://boxedapp.com/forum/
// License SDK: http://boxedapp.com/boxedappsdk/order.html
// Online help: http://boxedapp.com/boxedappsdk/help/
//

#ifndef __BOXEDAPPSDK_H__
#define __BOXEDAPPSDK_H__

#define BOXEDAPPSDKAPI __stdcall

#include <pshpack4.h>

// IStream* declaration
#include <objidl.h>



#ifdef __cplusplus
extern "C" {
#endif

// Options
#define DEF_BOXEDAPPSDK_OPTION__ALL_CHANGES_ARE_VIRTUAL (1) // default: 0 (FALSE)
#define DEF_BOXEDAPPSDK_OPTION__EMBED_BOXEDAPP_IN_CHILD_PROCESSES (2) // default: 1 (FALSE, don't enable BoxedApp to a new process by default)
#define DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_FILE_SYSTEM (3) // default: 1 (TRUE)
#define DEF_BOXEDAPPSDK_OPTION__RECREATE_VIRTUAL_FILE_AS_VIRTUAL (4) // default: 1 (TRUE)
#define DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_REGISTRY (5) // default: 1 (TRUE)
#define DEF_BOXEDAPPSDK_OPTION__HIDE_VIRTUAL_FILES_FROM_FILE_DIALOG (6) // default: 0 (FALSE)
#define DEF_BOXEDAPPSDK_OPTION__EMULATE_OUT_OF_PROC_COM_SERVERS (7) // default: 0 (FALSE)

#define DEF_BOXEDAPPSDK_OPTION__INHERIT_OPTIONS (8) // default: 0 (FALSE)



// Initialization
BOOL BOXEDAPPSDKAPI BoxedAppSDK_Init();
// Finalization
void BOXEDAPPSDKAPI BoxedAppSDK_Exit();

// Enable / disable logging
void BOXEDAPPSDKAPI BoxedAppSDK_EnableDebugLog(BOOL bEnable);

// Log file
void BOXEDAPPSDKAPI BoxedAppSDK_SetLogFileA(LPCSTR szLogFilePath);
void BOXEDAPPSDKAPI BoxedAppSDK_SetLogFileW(LPCWSTR szLogFilePath);

#ifdef UNICODE
#define BoxedAppSDK_SetLogFile BoxedAppSDK_SetLogFileW
#else
#define BoxedAppSDK_SetLogFile BoxedAppSDK_SetLogFileA
#endif // UNICODE

void WINAPI BoxedAppSDK_WriteLogA(LPCSTR szMessage);
void WINAPI BoxedAppSDK_WriteLogW(LPCWSTR szMessage);

#ifdef UNICODE
#define BoxedAppSDK_WriteLog BoxedAppSDK_WriteLogW
#else
#define BoxedAppSDK_WriteLog BoxedAppSDK_WriteLogA
#endif // UNICODE

// Virtual file system

#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualFile BoxedAppSDK_CreateVirtualFileW
#else
#define BoxedAppSDK_CreateVirtualFile BoxedAppSDK_CreateVirtualFileA
#endif // UNICODE

// Create new virtual file
HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileA(
	LPCSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);

HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileW(
	LPCWSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);


#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualDirectory BoxedAppSDK_CreateVirtualDirectoryW
#else
#define BoxedAppSDK_CreateVirtualDirectory BoxedAppSDK_CreateVirtualDirectoryA
#endif // UNICODE

// Create new virtual directory
BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualDirectoryA(
	LPCSTR lpPathName, 
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
);

BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualDirectoryW(
	LPCWSTR lpPathName, 
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
);

// Virtual registry

#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualRegKey BoxedAppSDK_CreateVirtualRegKeyW
#else
#define BoxedAppSDK_CreateVirtualRegKey BoxedAppSDK_CreateVirtualRegKeyA
#endif // UNICODE

LONG BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualRegKeyA(
	HKEY hKey,
	LPCSTR lpSubKey,
	DWORD Reserved,
	LPCSTR lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition
);

LONG BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualRegKeyW(
	HKEY hKey,
	LPCWSTR lpSubKey,
	DWORD Reserved,
	LPCWSTR lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition
);

#ifdef UNICODE
#define BoxedAppSDK_DeleteVirtualRegKey BoxedAppSDK_DeleteVirtualRegKeyW
#else
#define BoxedAppSDK_DeleteVirtualRegKey BoxedAppSDK_DeleteVirtualRegKeyA
#endif // UNICODE

LONG BOXEDAPPSDKAPI BoxedAppSDK_DeleteVirtualRegKeyA(HKEY hKey, LPCSTR lpSubKey);
LONG BOXEDAPPSDKAPI BoxedAppSDK_DeleteVirtualRegKeyW(HKEY hKey, LPCWSTR lpSubKey);

LONG BOXEDAPPSDKAPI BoxedAppSDK_DeleteVirtualRegKeyByHandle(HKEY hKey);

void BOXEDAPPSDKAPI BoxedAppSDK_SetContext(LPCSTR szContext);

DWORD BOXEDAPPSDKAPI BoxedAppSDK_RegisterCOMLibraryInVirtualRegistryA(LPCSTR szPath);
DWORD BOXEDAPPSDKAPI BoxedAppSDK_RegisterCOMLibraryInVirtualRegistryW(LPCWSTR szPath);

#ifdef UNICODE
#define BoxedAppSDK_RegisterCOMLibraryInVirtualRegistry BoxedAppSDK_RegisterCOMLibraryInVirtualRegistryW
#else
#define BoxedAppSDK_RegisterCOMLibraryInVirtualRegistry BoxedAppSDK_RegisterCOMLibraryInVirtualRegistryA
#endif // UNICODE

void BOXEDAPPSDKAPI BoxedAppSDK_EnableOption(DWORD dwOptionIndex, BOOL bEnable);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_IsOptionEnabled(DWORD dwOptionIndex);

void BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_EnableOption(DWORD dwProcessId, DWORD dwOptionIndex, BOOL bEnable);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_IsOptionEnabled(DWORD dwProcessId, DWORD dwOptionIndex);

typedef enum _ENUM_BOXEDAPPSDK_REQUEST_ID
{
	EnumBoxedAppSDK_RequestId__RegQueryValue = 1, 
	EnumBoxedAppSDK_RequestId__RedirectFilePath = 2

} ENUM_BOXEDAPPSDK_REQUEST_ID;

typedef DWORD (BOXEDAPPSDKAPI *PBOXEDAPPHANDLER)(PVOID Param, ENUM_BOXEDAPPSDK_REQUEST_ID RequestId, PVOID pAdditionalInfo);
DWORD BOXEDAPPSDKAPI BoxedAppSDK_AddHandler(PBOXEDAPPHANDLER pHandler, PVOID Param);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_RemoveHandler(DWORD dwHandlerId);

struct SBoxedAppSDK__RegQueryValue
{
	// in
	HKEY m_Root;
	LPCWSTR m_szPath;
	LPCWSTR m_szValue;

	// out
	BOOL m_bHandled;

	DWORD m_dwType;
	PVOID m_pData;
	DWORD m_dwSize;
};

struct SBoxedAppSDK__RedirectFilePath
{
	// in
	LPCWSTR m_szPath;

	// out
	BOOL m_bHandled;

	LPWSTR m_szRedirectToPath;
};

PVOID BOXEDAPPSDKAPI BoxedAppSDK_Alloc(DWORD dwSize);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_Free(PVOID pData);

DWORD BOXEDAPPSDKAPI BoxedAppSDK_DeleteFileFromVirtualFileSystemW(LPCWSTR szPath);
DWORD BOXEDAPPSDKAPI BoxedAppSDK_DeleteFileFromVirtualFileSystemA(LPCSTR szPath);

#ifdef UNICODE
#define BoxedAppSDK_DeleteFileFromVirtualFileSystem BoxedAppSDK_DeleteFileFromVirtualFileSystemW
#else
#define BoxedAppSDK_DeleteFileFromVirtualFileSystem BoxedAppSDK_DeleteFileFromVirtualFileSystemA
#endif // UNICODE

BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateProcessFromMemoryA(
	LPCVOID pBuffer, 
	DWORD dwSize, 

	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
);

BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateProcessFromMemoryW(
	LPCVOID pBuffer, 
	DWORD dwSize, 

	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
);

#ifdef UNICODE
#define BoxedAppSDK_CreateProcessFromMemory BoxedAppSDK_CreateProcessFromMemoryW
#else
#define BoxedAppSDK_CreateProcessFromMemory BoxedAppSDK_CreateProcessFromMemoryA
#endif // UNICODE

// Set param #0
void BOXEDAPPSDKAPI BoxedAppSDK_SetParam0(LPCSTR param);

// Create a virtual file based on IStream
HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileBasedOnIStreamA(
	LPCSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile, 

	LPSTREAM pStream
);

HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileBasedOnIStreamW(
	LPCWSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile, 

	LPSTREAM pStream
);

#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualFileBasedOnIStream BoxedAppSDK_CreateVirtualFileBasedOnIStreamW
#else
#define BoxedAppSDK_CreateVirtualFileBasedOnIStream BoxedAppSDK_CreateVirtualFileBasedOnIStreamA
#endif // UNICODE

HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileBasedOnBufferA(
	LPCSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile, 

	PVOID pData, DWORD dwSize
);

HANDLE BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualFileBasedOnBufferW(
	LPCWSTR szPath, 
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile, 

	PVOID pData, DWORD dwSize
);

#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualFileBasedOnBuffer BoxedAppSDK_CreateVirtualFileBasedOnBufferW
#else
#define BoxedAppSDK_CreateVirtualFileBasedOnBuffer BoxedAppSDK_CreateVirtualFileBasedOnBufferA
#endif // UNICODE

// Attach BoxedApp SDK to another process
BOOL BOXEDAPPSDKAPI BoxedAppSDK_AttachToProcess(HANDLE hProcess);
// Detach BoxedApp SDK from a process
BOOL BOXEDAPPSDKAPI BoxedAppSDK_DetachFromProcess(HANDLE hProcess);

// Execute .net application
DWORD BOXEDAPPSDKAPI BoxedAppSDK_ExecuteDotNetApplicationW(LPCWSTR szPath, LPCWSTR szArgs);
DWORD BOXEDAPPSDKAPI BoxedAppSDK_ExecuteDotNetApplicationA(LPCSTR szPath, LPCSTR szArgs);

#ifdef UNICODE
#define BoxedAppSDK_ExecuteDotNetApplication BoxedAppSDK_ExecuteDotNetApplicationW
#else
#define BoxedAppSDK_ExecuteDotNetApplication BoxedAppSDK_ExecuteDotNetApplicationA
#endif // UNICODE

// Internal; don't use
DWORD BOXEDAPPSDKAPI BoxedAppSDK_GetInternalValue(DWORD nValueId, LPVOID buf, DWORD dwSize);

// Function hooking
HANDLE BOXEDAPPSDKAPI BoxedAppSDK_HookFunction(PVOID pFunction, PVOID pHook, BOOL bEnable);
PVOID BOXEDAPPSDKAPI BoxedAppSDK_GetOriginalFunction(HANDLE hHook);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_EnableHook(HANDLE hHook, BOOL bEnable);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_UnhookFunction(HANDLE hHook);

// Remote process function hooking

// Callback that is called when a function is called
typedef void (__stdcall *P_BoxedAppSDK_RemoteProcessHook)(HANDLE hContext);

#define DEF_BOXEDAPPSDK_FUNCTION_TYPE__STDCALL (0)

// Setups a hook
HANDLE BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_HookFunction(
	DWORD dwProcessId, 
	PVOID pFunction, 
	DWORD dwFunctionType, 
	DWORD nArgumentCount, 
	P_BoxedAppSDK_RemoteProcessHook pHook, 
	BOOL bEnable);

void BOXEDAPPSDKAPI BoxedAppSDK_HookContext_SetContinueFlag(HANDLE hContext, BOOL bContinue);
void BOXEDAPPSDKAPI BoxedAppSDK_HookContext_SetReturnValue(HANDLE hContext, DWORD dwRetValue);

#ifndef DWORD_PTR
#define DWORD_PTR SIZE_T
#endif // !DWORD_PTR

DWORD_PTR BOXEDAPPSDKAPI BoxedAppSDK_HookContext_GetArgument(HANDLE hContext, DWORD nArgumentIndex);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_HookContext_SetArgument(HANDLE hContext, DWORD nArgumentIndex, DWORD_PTR Value);

DWORD BOXEDAPPSDKAPI BoxedAppSDK_HookContext_GetProcessId(HANDLE hContext);

BOOL BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_ReadStringW(DWORD dwProcessId, const VOID* pAddress, LPWSTR* pString);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_ReadStringA(DWORD dwProcessId, const VOID* pAddress, LPSTR* pString);

#ifdef UNICODE
#define BoxedAppSDK_RemoteProcess_ReadString BoxedAppSDK_RemoteProcess_ReadStringW
#else
#define BoxedAppSDK_RemoteProcess_ReadString BoxedAppSDK_RemoteProcess_ReadStringA
#endif // UNICODE

PVOID BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_Alloc(DWORD dwProcessId, DWORD dwSize);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_Free(DWORD dwProcessId, PVOID pMemory);

PVOID BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_AllocStringW(DWORD dwProcessId, LPCWSTR szSource);
PVOID BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_AllocStringA(DWORD dwProcessId, LPCSTR szSource);

#ifdef UNICODE
#define BoxedAppSDK_RemoteProcess_AllocString BoxedAppSDK_RemoteProcess_AllocStringW
#else
#define BoxedAppSDK_RemoteProcess_AllocString BoxedAppSDK_RemoteProcess_AllocStringA
#endif // UNICODE

HMODULE BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_LoadLibraryA(DWORD dwProcessId, LPCSTR szPath);
HMODULE BOXEDAPPSDKAPI BoxedAppSDK_RemoteProcess_LoadLibraryW(DWORD dwProcessId, LPCWSTR szPath);

#ifdef UNICODE
#define BoxedAppSDK_RemoteProcess_LoadLibrary BoxedAppSDK_RemoteProcess_LoadLibraryW
#else
#define BoxedAppSDK_RemoteProcess_LoadLibrary BoxedAppSDK_RemoteProcess_LoadLibraryA
#endif // UNICODE


DWORD BOXEDAPPSDKAPI BoxedAppSDK_RegisterCOMServerInVirtualRegistryA(LPCSTR szCommandLine);
DWORD BOXEDAPPSDKAPI BoxedAppSDK_RegisterCOMServerInVirtualRegistryW(LPCWSTR szCommandLine);

#ifdef UNICODE
#define BoxedAppSDK_RegisterCOMServerInVirtualRegistry BoxedAppSDK_RegisterCOMServerInVirtualRegistryW
#else
#define BoxedAppSDK_RegisterCOMServerInVirtualRegistry BoxedAppSDK_RegisterCOMServerInVirtualRegistryA
#endif // UNICODE


BOOL BOXEDAPPSDKAPI BoxedAppSDK_IsMainProcess();
BOOL BOXEDAPPSDKAPI BoxedAppSDK_IsVirtualProcessStub();
BOOL BOXEDAPPSDKAPI BoxedAppSDK_IsVirtualProcessId(DWORD dwProcessId);


typedef BOOL (WINAPI *P_BoxedAppSDK_EnumVirtualRegKeysCallbackA)(HKEY hRootKey, LPCSTR szSubKey, LPARAM lParam);
typedef BOOL (WINAPI *P_BoxedAppSDK_EnumVirtualRegKeysCallbackW)(HKEY hRootKey, LPCWSTR szSubKey, LPARAM lParam);

BOOL BOXEDAPPSDKAPI BoxedAppSDK_EnumVirtualRegKeysA(P_BoxedAppSDK_EnumVirtualRegKeysCallbackA pEnumFunc, LPARAM lParam);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_EnumVirtualRegKeysW(P_BoxedAppSDK_EnumVirtualRegKeysCallbackW pEnumFunc, LPARAM lParam);

#ifdef UNICODE
#define BoxedAppSDK_EnumVirtualRegKeys BoxedAppSDK_EnumVirtualRegKeysW
#define P_BoxedAppSDK_EnumVirtualRegKeysCallback P_BoxedAppSDK_EnumVirtualRegKeysCallbackW
#else
#define BoxedAppSDK_EnumVirtualRegKeys BoxedAppSDK_EnumVirtualRegKeysA
#define P_BoxedAppSDK_EnumVirtualRegKeysCallback P_BoxedAppSDK_EnumVirtualRegKeysCallbackA
#endif // UNICODE

typedef BOOL (WINAPI *P_BoxedAppSDK_RegFileParserCallbackA)(
	HKEY hRootKey, 
	LPCSTR szSubKey, 
	DWORD dwValueType, 
	PVOID pValueData, 
	DWORD dwValueDataSize, 
	LPARAM lParam);
typedef BOOL (WINAPI *P_BoxedAppSDK_RegFileParserCallbackW)(
	HKEY hRootKey, 
	LPCWSTR szSubKey, 
	DWORD dwValueType, 
	PVOID pValueData, 
	DWORD dwValueDataSize, 
	LPARAM lParam);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_Utils_ParseRegFileA(
	LPSTREAM pStream, 
	P_BoxedAppSDK_RegFileParserCallbackA pCallback, 
	LPARAM lParam);
BOOL BOXEDAPPSDKAPI BoxedAppSDK_Utils_ParseRegFileW(
	LPSTREAM pStream, 
	P_BoxedAppSDK_RegFileParserCallbackW pCallback, 
	LPARAM lParam);

#ifdef UNICODE
#define P_BoxedAppSDK_RegFileParserCallback P_BoxedAppSDK_RegFileParserCallbackW
#define BoxedAppSDK_Utils_ParseRegFile BoxedAppSDK_Utils_ParseRegFileW
#else
#define P_BoxedAppSDK_RegFileParserCallback P_BoxedAppSDK_RegFileParserCallbackA
#define BoxedAppSDK_Utils_ParseRegFile BoxedAppSDK_Utils_ParseRegFileA
#endif // UNICODE

BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorageA(
	LPCSTR lpPathName, 
	IStorage* pStorage, 
	LPSECURITY_ATTRIBUTES lpSecurityAttributes);

BOOL BOXEDAPPSDKAPI BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorageW(
	LPCWSTR lpPathName, 
	IStorage* pStorage, 
	LPSECURITY_ATTRIBUTES lpSecurityAttributes);

#ifdef UNICODE
#define BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorage BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorageW
#else
#define BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorage BoxedAppSDK_CreateVirtualDirectoryBasedOnIStorageA
#endif // UNICODE



#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif // !__BOXEDAPPSDK_H__
