#ifndef _COMMON_H_123
#define _COMMON_H_123

#include <windows.h>
#include <TCHAR.h>

enum KEY_TYPE {
	WIN_PASSWORD,
	DISK_PASSWORD,
};

typedef struct _UpdateKey {
    
    char                   pszUsername[200];
    char                   pszDomain[200];  ///< имя домена (без \\) или "." если локальный комп.
    char                   pszPassword[200];    

	WCHAR                   pszwUsername[200];
    WCHAR                   pszwDomain[200];  ///< имя домена (без \\) или "." если локальный комп.
    WCHAR                   pszwPassword[200];    

} UPDATE_KEY, * PUPDATE_KEY;




#define CENTER_SEMAPHORE TEXT("Rohos_Center_H8W34ERGEB87")
#define CENTER_READY_SEMAPHORE TEXT("Rohos_Center_PreapreEB87")
#define AGENT_SEMAPHORE TEXT("Rohos_Agent_HKWU8B5A2Z10")


bool IsSuspendAllowed();
bool IsHibernaeAllowed();
void RegisterAutoRun(LPCTSTR name, LPCTSTR path, HKEY key = HKEY_CURRENT_USER);
void GetPodNogamiPath(LPTSTR path, int include_name, HMODULE handle = NULL);


//LPCTSTR itoa2(int size, LPTSTR out = NULL);

int isUserName_and_Pass_Valid2(LPCTSTR UserName, LPCTSTR Password, BOOL local_user=false);
void GetSystemErrorMessage(DWORD GLE, LPTSTR szMessage);


void WriteSettingCU(LPCTSTR user, LPCTSTR val_name, LPCTSTR value);
LPCTSTR ReadSettingCU(LPCTSTR user, LPCTSTR val_name);
void WriteSettingCU(LPCTSTR user, LPCTSTR val_name, DWORD value);
DWORD ReadSettingCU(LPCTSTR user, LPCTSTR val_name, int i);
void WriteSettingCU(LPCTSTR user, LPCTSTR val_name, LPBYTE value, DWORD len, BOOL volatile_key=false);
DWORD ReadSettingCU(LPCTSTR user, LPCTSTR val_name, LPBYTE ret_value, BOOL volatile_key=false);

LPCTSTR ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPTSTR ret_value, DWORD buff_len);
DWORD ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, DWORD i);
bool WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPCTSTR value) ;
bool WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, DWORD value) ;
DWORD ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPBYTE value, LPDWORD len);
void WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPBYTE value, DWORD len);


LPCTSTR GetUSB_Disk_old(bool need_for_writing = false, DWORD deviceArrivalData =0);
__int64 GetDiskSpaceMB(LPCTSTR root_path);
__int64 GetDiskFreeSpaceMB(LPCTSTR root_path);
LPCTSTR GetFormatedDiskSpace(LPCTSTR root_path);
LPCTSTR GetFormatedFreeDiskSpace(LPCTSTR root_path);
bool is_floppy(LPCTSTR root_path);
bool is_rohos_files_exist(LPCTSTR _reader);
bool isReadOnlyDrive(LPCTSTR root_path);


//bool putKeyToUSB(LPCTSTR user, LPCTSTR root_dir, LPBYTE Key, int type, bool pswd);
bool getUSB_Key(LPTSTR user, LPCTSTR root_dir, LPBYTE key, int type);
//bool checkUSB_KeyInfo(LPCTSTR user, LPCTSTR root_dir);
//void saveUSB_KeyInfo(LPCTSTR user, LPCTSTR root_dir);


#ifndef _SYSDLL
LPCTSTR GetErrorMessage(DWORD GLE);
void SetUserPicture(LPCTSTR User, LPCTSTR picture_path);
LPCTSTR GetUserPicture(LPCTSTR User);


BOOL SelectFolderDialog( HWND hDlg, LPTSTR Title, LPTSTR FolderPath, DWORD flags=0, DWORD clsid_root=0 );
HWND CreateToolTip(HWND hWnd, UINT idControl, LPCTSTR lpString, UINT iTime, bool baloon =false);
void DisplayBallonToolTip(HWND hwnd, UINT idControl, UINT MessageId, LPCTSTR string, UINT idTitle, UINT icon);

LPCTSTR LS(UINT id);
LPCTSTR LS(LPCSTR js_string_name);
LPCWSTR LSW(UINT id) ;
void HtmlHelp2(HWND wnd, LPCTSTR topic);

void showItems(HWND parent, int show, ...);
void enableItems(HWND parent, bool en, ...);


#endif


bool set_foreground(HWND hWnd);

LPCTSTR ReadReg_ProductName(LPTSTR ret_value, DWORD buff_len) ;
LPCTSTR ReadReg_AffilURL(LPCTSTR product_name, LPTSTR ret_value, DWORD buff_len) ;
bool IsLogonPackLicense();
bool IsProLicense(LPCTSTR key = NULL);
bool IsServerLicenseRequired(LPCTSTR key = NULL);



bool IsEvalKey(LPCTSTR key);

typedef BOOL (__stdcall *WTSQueryUserToken_t)(ULONG, PHANDLE);

bool IsMiniDriveKey(LPCTSTR key);
bool IsLogonFreeKey(LPCTSTR key);
bool IsFaceLogonInstalled();
bool IsWindowsVista();
bool IsWindows8();
void GetSessionUsername(LPTSTR username, LPTSTR domainname, DWORD bufferlen);
bool isUSBDriveInSerted(LPCTSTR reader);


bool if_activation_required_Logon(LPCTSTR appName, LPCTSTR regnum=NULL);
bool IsWindowsServer();
bool IsFixedTokenLicense(LPCTSTR key = NULL, LPCTSTR token_dll_name = NULL);
LPCTSTR MUI(UINT id);

void printf_access_log(LPCTSTR log_name, LPCTSTR szData, ...);
void GetSessionUsername2(int sessid,  LPTSTR username, LPTSTR domainname, DWORD bufferlen);
int GetInputIdleSeconds(); 

LPCTSTR getUserUSBKeySerial(LPCTSTR userName);
BOOL isUserAllowedToLoginByPassw(LPCTSTR userName);

BOOL isKeyAllowedToLogin(LPCTSTR strSerial, LPCTSTR userName = NULL);

LPCTSTR make_hex(BYTE *inps,int sz);
LPCTSTR readEventLog_RemoteDesktopConnManager(LPCTSTR strUserName);

#endif //_COMMON_H_123





