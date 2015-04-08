/**********************************************************************
** Copyright (C) 2005-2015 Tesline-Service S.R.L. All rights reserved.
**
** Rohos Disk Browser, Rohos Mini Drive Portable.
** 
**
** This file may be distributed and/or modified under the terms of the
** GNU Affero General Public license version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. If not, see <http://www.gnu.org/licenses/>
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.rohos.com/ for GPL licensing information.
**
** Contact info@rohos.com http://rohos.com
**/


#include <windows.h>

#include <stdio.h>
#include <LMWksta.h>
#include <LMERR.H>

#include "TCHAR.H"

#include <TraceMaker.h>
#include <shlobj.h>
#include "Shlwapi.h"
#include <io.h>

#include <common1.h>
#include "htmlhelp.h"
#include "strings.h"

//#include "DiskSector.h"

#include <dbt.h>
#include <Wtsapi32.h>


#ifndef GWL_USERDATA
#define GWL_USERDATA        (-21)
#endif


// language files
static char* lpStrings = NULL;
static char* lpStringDefs = NULL;
static BOOL	is_lang_unicode = false;
static char* lpStrings_Js = NULL;
extern HINSTANCE h_lang; // lang hinstance


/** return path with \\
*/
void GetPodNogamiPath(LPTSTR path, int include_name, HMODULE handle)
{
    GetModuleFileName(handle, path, MAX_PATH);
	if ( include_name != 0 ) return;

    LPTSTR ch = _tcsrchr(path, '\\');
    if ( ch ) *(ch+1) = 0;
}


/** is it readonly drive ?
*/
bool isReadOnlyDrive(LPCTSTR root_path) {

	TCHAR path[40];
	if (!root_path) return true;
	if (_tcslen(root_path)<3) return true;

	_tcscpy(path, root_path);
	if (root_path[_tcslen(root_path)-1]=='\\')
		_tcscat(path, TEXT("rhtest.txt") );
	else
		_tcscat(path, TEXT("\\rhtest.txt") );

	HANDLE h = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE, NULL );
	
	if (INVALID_HANDLE_VALUE ==h) {
		return true;
	}

	DWORD num;
	if ( !WriteFile(h, TEXT("RohosTest"), 4, &num, NULL ) ) {
		CloseHandle(h);
		return true;
	}

	CloseHandle(h);
	return false;

}


/** Вернуть текст ошибки GLE (GetLastError)

  \param		gle
  \param[out]	szMessage - указатель на строку 256 байт
*/
void GetSystemErrorMessage(DWORD GLE, LPTSTR szMessage)
{	
	DWORD ret;
	

    if (GLE >= NERR_BASE)
    {
        
        HMODULE hNetMsg = LoadLibrary(TEXT("netmsg.dll"));
        
        ret =FormatMessage(
            FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
            hNetMsg,                               // ignored
            GLE,                                  // message id
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),   // message language
            szMessage,                  // address of buffer pointer
            199,                                  // minimum buffer size
            NULL );                              // no other arguments
    }

    ret = FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,                               // ignored
            GLE,                     // message id
            0, /*MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)*/   // message language
            szMessage,                  // address of buffer pointer
            199,                                  // minimum buffer size
            NULL );                              // no other arguments

	if (0==ret)
		_stprintf(szMessage, TEXT("An error occured. Error code: 0x%X   (%X)"), GLE, GetLastError() );

    //return( pWlxFuncs->WlxMessageBox(hGlobalWlx, hWnd, szMessage, pszTitleBar, Buttons));

}

bool IsMiniDriveKey(LPCTSTR key) {

	TCHAR str[500];
	LPCTSTR r = ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), TEXT("RegNumber"), str, 300);		


	if (key) {
		if ( _tcsncmp(key, TEXT("2BFF10"), 6) ==0 )
			return true;

	} else {
		if ( _tcsncmp(str, TEXT("2BFF10"), 6) ==0 )
			return true;		
	}
		

	return false;
	
}



#ifndef _SYSDLL
void enableItems(HWND parent, bool en, ...)
{
	va_list ap;
	int item;
	va_start(ap, en);

	while ((item= va_arg(ap,int)) != 0)	
	{
		//if (item!=1)
		EnableWindow( GetDlgItem(parent,item), en);
	}
		
	va_end(ap);
}

/** показать / скрыть контролы у окна
\param show = 0, 1

*/
void showItems(HWND parent, int show, ...)
{
	va_list ap;
	int item;
	va_start(ap, show);

	while ((item= va_arg(ap,int)) != 0)			
		ShowWindow( GetDlgItem(parent, item), show);
		
	va_end(ap);
}

#endif

/** read a global setting from registry (at User)
params - none
key, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER
*/
LPCTSTR ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPTSTR ret_value, DWORD buff_len) 
{	
	HKEY hKey;
	
	if (!ret_value) return 0;
		
	if( RegOpenKeyEx(key, path, 0, KEY_READ/*|KEY_SET_VALUE*/ | KEY_WOW64_32KEY, &hKey)) 		
		RegOpenKeyEx(key, path, 0, KEY_READ , &hKey) ;

	
	buff_len = (buff_len-1) * sizeof(TCHAR);
	memset(ret_value, 0, buff_len);
	buff_len -= 2;
	DWORD type = REG_SZ;	
	
	LONG ret = RegQueryValueEx(hKey, val_name, NULL, &type, (LPBYTE)ret_value, &buff_len);

	if (type==REG_DWORD && ret == ERROR_SUCCESS) 
	{ 
		DWORD i = *(LPDWORD)ret_value;
		_stprintf(ret_value, TEXT("%d"), i);		
	}

	/*
	if(ERROR_SUCCESS !=ret && 
		_tcsicmp(val_name, TEXT("RegNumber"))  && 
		_tcsicmp(val_name, TEXT("RegNumberWelcome")) &&
		_tcsicmp(val_name, TEXT("ginadll"))   &&
		_tcsicmp(val_name, TEXT("PrevGina"))   
		){		
	
		//MessageBox(NULL, path, val_name, 0);
		//WriteLog( TEXT("ReadReg failed %X %X %s %s"), ret, buff_len , path, val_name);		
	}*/	
	
	RegCloseKey(hKey);

	return ret_value;
}




/** read a global setting from registry (at User)
params - none
*/
DWORD ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, DWORD default_value) //-----------------
{	
	HKEY hKey;
	DWORD len, val[20]={0};
	DWORD type=0;
	
	if( RegOpenKeyEx(key, path, 0, KEY_READ/*|KEY_SET_VALUE*/ | KEY_WOW64_32KEY, &hKey)) 		
		RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS , &hKey);
	
	len = sizeof(DWORD)*5;
	LONG ret = RegQueryValueEx(hKey, val_name, NULL, &type, (LPBYTE)&val, &len);
	if ( ERROR_SUCCESS != ret) 
	{
		//WriteLog( TEXT("ReadReg0 failed %X %s %s"), ret, path, val_name);
		val[0] = default_value;
	}

	if (type==REG_SZ) 
	{ // это "0" или "1"
		int i2=default_value;
		val[7]=0;
		LPCTSTR s = (LPCTSTR)val;
		if ( s[0] == '0')
			i2 =0;
		else {
			if ( _tcslen((LPCTSTR)val) )
				i2 = _ttoi( (LPCTSTR)val);
			if (i2==0)
				i2 = default_value;	
			if ( _tcschr((LPCTSTR)val, '.') )
				i2 = default_value;	
		}
		
		memset(val, 0, 7);
		val[0] = i2;
	}
		
	RegCloseKey(hKey);

	return val[0];
}


/** write a local setting from registry (at User)
params - none
*/
DWORD ReadReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPBYTE value, LPDWORD len) //-------------
{
	
	HKEY hKey;
	LONG ret;
	DWORD type = REG_BINARY;
		
	if( RegOpenKeyEx(key, path, 0, KEY_READ/*|KEY_SET_VALUE*/ | KEY_WOW64_32KEY, &hKey)) 		
		RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS , &hKey);

	if (!value && len) 
	{		
		ret = RegQueryValueEx(hKey, val_name, 0, &type, NULL, len);
	}
	else {		
		ret = RegQueryValueEx(hKey, val_name, 0, &type, value, len);
		if ( ERROR_SUCCESS != ret) 
		{
			//WriteLog( TEXT("ReadReg failed %X %s %s"), ret, path, val_name);
			*len=0;
		}
	}

	RegCloseKey(hKey);

	return *len;
}


/** write a global setting from registry (at User)
params - none
*/
bool WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPCTSTR value) 
{	
	HKEY hKey;
	LONG ret;
		
	if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey) ) {
	//	if (ret==5)		
			if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS , &hKey) )
				if ( ret=RegCreateKeyEx(key, path, 0, NULL, 0, KEY_WOW64_32KEY | KEY_ALL_ACCESS, NULL, &hKey, NULL) )	
					RegCreateKeyEx(key, path, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	//WriteLog(" WR1 %X", hKey);

	if (value==NULL) 
		ret = RegDeleteValue(hKey, val_name);
	else {		
		
		ret = RegSetValueEx(hKey, val_name, 0, REG_SZ, (LPBYTE)value, _tcslen(value) * sizeof(TCHAR));
		if ( ERROR_SUCCESS != ret)		
			WriteLog( TEXT("WriteReg-s failed %X %s %s"), ret, path, val_name);

		//WriteLog(" WR2 %X", hKey);
		
	}
	RegCloseKey(hKey);

	//WriteLog(" WR2 %X", ret);

	if (ret == ERROR_SUCCESS) 
		return true;
	else
		return false;

}


/** write a to registry (at User)
params - none
*/
void WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, LPBYTE value, DWORD len) 
{	
	HKEY hKey;
	LONG ret;
		
	if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey) ) {
		//if (ret==5)		
			if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS , &hKey) )
				if ( ret=RegCreateKeyEx(key, path, 0, NULL, 0, KEY_WOW64_32KEY | KEY_ALL_ACCESS, NULL, &hKey, NULL) )	
					RegCreateKeyEx(key, path, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (value==NULL) 
		ret = RegDeleteValue(hKey, val_name);
	else {		
		ret = RegSetValueEx(hKey, val_name, 0, REG_BINARY, value, len);
		if ( ERROR_SUCCESS != ret)
			WriteLog( TEXT("WriteReg failed %X %s %s"), ret, path, val_name);
	}

	RegCloseKey(hKey);

	return ;
}



/** write a global setting from registry (at User)
params - none
*/

bool WriteReg(HKEY key, LPCTSTR path, LPCTSTR val_name, DWORD value) 
{	
	HKEY hKey;
	LONG ret=0;
		
	if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hKey) ) {		
			if( ret=RegOpenKeyEx(key, path, 0, KEY_ALL_ACCESS , &hKey) )
				if ( ret=RegCreateKeyEx(key, path, 0, NULL, 0, KEY_WOW64_32KEY | KEY_ALL_ACCESS, NULL, &hKey, NULL) )	
					RegCreateKeyEx(key, path, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (value==-1) 
		ret = RegDeleteValue(hKey, val_name);
	else {
		ret = RegSetValueEx(hKey, val_name, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
	}

	RegCloseKey(hKey);

	if (ret == ERROR_SUCCESS) 
		return true;
	else
		return false;

}




/** get disk space in Megabytes
*/
__int64 GetDiskSpaceMB(LPCTSTR root_path) {

	if (root_path==NULL) return 0;
	ULARGE_INTEGER FreeBytesAvailable;    // bytes available to caller
	ULARGE_INTEGER TotalNumberOfBytes;    // bytes on disk
	//ULARGE_INTEGER TotalNumberOfFreeBytes // free bytes on disk

	if ( GetDiskFreeSpaceEx( root_path, &FreeBytesAvailable, 
				&TotalNumberOfBytes, NULL) ) 
	{

		if (TotalNumberOfBytes.QuadPart < (1 * 1024 * 1024)) // less than 1 MB
			return 1;

		return TotalNumberOfBytes.QuadPart / 1048000;
	}
	else {
		//return 0;

		/*WriteLog("GetDiskSpaceMB failed %X %s", GetLastError(),root_path  );

		if (!root_path) return 0;
		if (_tcslen(root_path)<2) return 0;

		TCHAR str1[100];
		_tcscpy(str1, root_path);
		_tcscat(str1, TEXT("_rohos"));

		if ( _taccess(str1, 00) == 0)
			return 128;
		*/
		return 0;
	}

}

/** get disk Free space in Megabytes
*/
__int64 GetDiskFreeSpaceMB(LPCTSTR root_path) {

	//if (root_path[1]==':')
	//	root_path

	if (!root_path) return 0;
	if (_tcslen(root_path)<2) return 0;

	ULARGE_INTEGER FreeBytesAvailable;    // bytes available to caller
	ULARGE_INTEGER TotalNumberOfBytes;    // bytes on disk
	ULARGE_INTEGER TotalNumberOfFreeBytes; // free bytes on disk

	if ( GetDiskFreeSpaceEx( root_path, &FreeBytesAvailable, 
				&TotalNumberOfBytes, &TotalNumberOfFreeBytes) ) 

		return TotalNumberOfFreeBytes.QuadPart / 1048000;
	else {

		WriteLog("GetDiskFreeSpaceMB failed %X", GetLastError() );

		return 0;
	}

}

/*LPTSTR StrFormatByteSize64(          LONGLONG qdw,
    LPTSTR pszBuf,
    UINT uiBufSize
);*/

/** Converts a Disk  Space into a string that represents the number 
expressed as a size value in bytes, kilobytes, megabytes, or gigabytes, depending on the size.
\return LPCTSTR - pointer to static buffer

*/
LPCTSTR GetFormatedDiskSpace(LPCTSTR root_path) {

	ULARGE_INTEGER FreeBytesAvailable;    // bytes available to caller
	ULARGE_INTEGER TotalNumberOfBytes;    // bytes on disk
	//ULARGE_INTEGER TotalNumberOfFreeBytes // free bytes on disk
	static TCHAR str[60];

	if ( GetDiskFreeSpaceEx( root_path, &FreeBytesAvailable, 
				&TotalNumberOfBytes, NULL) ) 
		StrFormatByteSize64( TotalNumberOfBytes.QuadPart, str, 50);
	else
		_tcscpy(str, TEXT("0 Kb") );


	return str;


}

/** Converts a Disk Free Space into a string that represents the number 
expressed as a size value in bytes, kilobytes, megabytes, or gigabytes, depending on the size.
\return LPCTSTR - pointer to static buffer

*/
LPCTSTR GetFormatedFreeDiskSpace(LPCTSTR root_path) {

	ULARGE_INTEGER FreeBytesAvailable;    // bytes available to caller
	ULARGE_INTEGER TotalNumberOfBytes;    // bytes on disk
	ULARGE_INTEGER TotalNumberOfFreeBytes; // free bytes on disk
	static TCHAR str[60];

	if ( GetDiskFreeSpaceEx( root_path, &FreeBytesAvailable, 
				&TotalNumberOfBytes, &TotalNumberOfFreeBytes) ) 
		StrFormatByteSize64( TotalNumberOfFreeBytes.QuadPart, str, 50);
	else
		_tcscpy(str, TEXT("0 Kb") );


	return str;


}


bool is_floppy(LPCTSTR root_path)
{	
	
	// get the dos device name (like \device\floppy0)
	// to decide if it's a floppy or not
	TCHAR szDosDeviceName[MAX_PATH];
	TCHAR szDevicePath[15];
	_tcscpy(szDevicePath, root_path);
	szDevicePath[2]=0; // "c:"

	/*DWORD res =QueryDosDevice(szDevicePath,szDosDeviceName,MAX_PATH); // removed since 2.8
	if ( res ) {

		bool IsFloppy = (_tcsstr(szDosDeviceName,
			TEXT("\\Floppy")) != NULL); // is there a better way?
		
		return IsFloppy;
	}*/

	
	if ( _tcsicmp(root_path, TEXT("A:\\")) && _tcsicmp(root_path, TEXT("B:\\"))  ) // not A:\ and not B:
		return false;
	
	return true;
	

	// http://vbnet.mvps.org/index.html?code/disk/floppyready.htm
}

bool is_rohos_files_exist(LPCTSTR _reader) {
	
	TCHAR path[100];

	if (_tcslen(_reader) == 0)
		return false;
	
	_tcscpy(path, _reader);
	_tcscat(path, TEXT("_rohos\\roh.roh") );

	if ( _taccess(path, 0) == 0)
	{
		HANDLE h = CreateFile(path, FILE_READ_ATTRIBUTES, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		DWORD size = GetFileSize(h, NULL);

		CloseHandle(h);

		//WriteLog("is_rohos_files_exist %d", size);

		if (size > 3000 || size == INVALID_FILE_SIZE)
			return true;
	}

	return false;
}



/** Get removable USB Disk letter, or Error string about this media
  предполагается что размер флешки не может быть больше 20 Гиг
*/

LPCTSTR GetUSB_Disk_old(bool need_for_writing , DWORD deviceArrivalData ) {

	static TCHAR str[500];
	_tcscpy(str, TEXT("") );
	TCHAR temp[500];


	memset(temp, 0, sizeof(temp));

	// проверить тут
	if (deviceArrivalData ) {
		
		DEV_BROADCAST_HDR *hdr = (DEV_BROADCAST_HDR*)deviceArrivalData;
		
		if ( hdr->dbch_devicetype == DBT_DEVTYP_VOLUME ) {			
			DEV_BROADCAST_VOLUME *volume = (DEV_BROADCAST_VOLUME*)deviceArrivalData;
			TCHAR letter;
			
			for (letter='A'; letter<='Z'; letter++) {
				if ( volume->dbcv_unitmask & 1)
					break;
				volume->dbcv_unitmask = volume->dbcv_unitmask >> 1;
			}
			
			if (letter <='Z' || (volume->dbcv_unitmask&1) ) {								
				_tcscpy( str, TEXT("c:\\") );
				str[0] = letter;

				// это USB stick ?
				
				if ( GetDriveType(str) != DRIVE_REMOVABLE && 
					GetDriveType(str) != DRIVE_FIXED)
				{

					WriteLog( TEXT("GetUSB_Disk2, arrived: %s. fixed drive"),  str);

					_tcscpy(str, TEXT("") );
					return str;
				}
                                

				
				return str;
			}
		}		
	}


	LPTSTR cur_temp = temp;
	GetLogicalDriveStrings(490, temp);
	DWORD i = 0;
	while ( *cur_temp ) {
		
		if ( is_floppy(cur_temp) ) {
			cur_temp += _tcslen(cur_temp) + 1;
			continue;
		}
		
		if (GetDriveType(cur_temp) == DRIVE_REMOVABLE ) {				

	//		WriteLog( TEXT("GetUSB_Disk, check:  %s"),  cur_temp);
			
			
			
			if ( is_rohos_files_exist(cur_temp)) {
				_tcscpy(str, cur_temp);					
				str[0] = toupper(str[0]); // делаем букву большой.
				break;
				
			} 
	
			if (need_for_writing) {				
				//  запомнить этот диск и продолжать искать другой диск...				
				//  только при операци записи...
				if ( GetDiskSpaceMB(cur_temp) ) {					
					_tcscpy(str, cur_temp);	
					
					// проверить можно ли писать на этот диск
					if (isReadOnlyDrive(cur_temp) ) {
						WriteLog( TEXT("GetUSB_Disk, write-protected:  %s"),  cur_temp);
#ifndef _SYSDLL
						_stprintf(str, LS(IDS_USB_X_WRITEPROTECT), cur_temp); 
#else
						_tcscpy(str, "USB key is write protected"); 
#endif
						cur_temp += _tcslen(cur_temp) + 1;
						continue;// а вдруг дальше найдем другой драйв
					}
				}
			}
						
		}
	
		cur_temp += _tcslen(cur_temp) + 1;
	}
	
	return str;
	
}





/** return text description of system error
*/
LPCTSTR GetErrorMessage(DWORD GLE)
{
    static TCHAR   szMessage[256];
	
	szMessage[0]=0; szMessage[1]=0;


    if (GLE >= NERR_BASE)
    {
        
            HMODULE hNetMsg = LoadLibrary(TEXT("netmsg.dll"));
        
        FormatMessage(
            FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
            hNetMsg,                               // ignored
            GLE,                                  // message id
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),   // message language
            szMessage,                  // address of buffer pointer
            240,                                  // minimum buffer size
            NULL );                              // no other arguments

		FreeLibrary(hNetMsg);

    }

    FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,                               // ignored
            (GLE),                     // message id
            0 /*MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)*/,   // message language
            szMessage,                  // address of buffer pointer
            240,                                  // minimum buffer size
            NULL );                              // no other arguments

	return szMessage;

}


/** Диалог для выбора папки
*/
BOOL SelectFolderDialog( HWND hDlg, LPTSTR Title, LPTSTR FolderPath, DWORD flags, DWORD clsid_root )
{
	BROWSEINFO BrowseInfo;
	ITEMIDLIST *pItemIDList;

	ITEMIDLIST *RootIDList = NULL;

	if (clsid_root)
		SHGetFolderLocation(NULL, clsid_root, NULL, NULL, &RootIDList);

	memset( &BrowseInfo, 0, sizeof( BROWSEINFO ) );
	 

	BrowseInfo.hwndOwner = hDlg;
	BrowseInfo.pidlRoot = RootIDList;
	BrowseInfo.pszDisplayName = FolderPath;
	BrowseInfo.lpszTitle = Title;
	if (flags==0)
		BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	else 
		BrowseInfo.ulFlags = flags ;
	BrowseInfo.lpfn = NULL;
	BrowseInfo.lParam = NULL;
	BrowseInfo.iImage = NULL;
	if( ( pItemIDList = SHBrowseForFolder( (LPBROWSEINFO)&BrowseInfo ) ) != NULL &&
		SHGetPathFromIDList( pItemIDList, FolderPath ) )
			return TRUE;
 return FALSE;   
}



// Creates ToolTip for Control in dialog
HWND CreateToolTip(HWND hWnd, UINT idControl, LPCTSTR lpString, UINT iTime, bool baloon  )
{
	DWORD flags = TTS_NOPREFIX | TTS_ALWAYSTIP;
	if (baloon)
		flags = WS_POPUP | TTS_NOPREFIX | TTS_BALLOON | TTS_CLOSE | TTS_ALWAYSTIP;

				// CREATE A TOOLTIP WINDOW
	HWND hWndTT = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS, NULL, flags, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL 	);

	if (hWndTT ==NULL)
		WriteLog("CreateToolTip CreateWindowEx err %X", GetLastError() );
	
	TOOLINFO ti;	
	SetWindowLong( GetDlgItem(hWnd, idControl), GWL_USERDATA, (LONG)hWndTT);
	
	memset(&ti, 0, sizeof(TOOLINFO) );
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND ;
	
	ti.hwnd = hWnd ;
	ti.hinst = h_lang;
	ti.uId = (UINT)GetDlgItem(hWnd, idControl);
	ti.lpszText = (LPTSTR)lpString; 
	if (baloon) {
		ti.lpszText = NULL; 
		ti.uId = idControl; 
		ti.uFlags = TTF_TRANSPARENT | TTF_CENTERTIP | TTF_TRACK | TTF_SUBCLASS;
	}
	
	
	SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	
	SendMessage(hWndTT, TTM_SETMAXTIPWIDTH, 0, (LPARAM) 200);
	SendMessage(hWndTT, TTM_SETDELAYTIME, TTDT_INITIAL, iTime); // ... time to wait before the ToolTip window appears
	SendMessage(hWndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, 8000); // ... time to Tip - remains visible 

	//if (baloon)
	//  чтобы показать ТулТип надо вызвать DisplayToolTip

	return hWndTT;
}

// показывает tootip и выставляет таймер (15) на удаление
// MessageId номер строки в ресурсах, если 0 то берется из string
void DisplayBallonToolTip(HWND hwnd, UINT idControl, UINT MessageId, LPCTSTR string, UINT idTitle, UINT icon) {
	
	TOOLINFO ti;
	TCHAR str[560];
	
	memset(&ti, 0, sizeof(TOOLINFO) );
	ti.cbSize = sizeof(TOOLINFO);

	HWND _hwnd_ToolTip = (HWND)GetWindowLong( GetDlgItem(hwnd, idControl), GWL_USERDATA);
	if (!_hwnd_ToolTip) return;	
	
	ti.hwnd = hwnd;
	ti.uId = idControl; //(UINT)GetDlgItem(hWnd, idControl);
	//.hinst = AfxGetApp()->m_hInstance;
	
		
	if (string!=NULL)
		ti.lpszText = (LPTSTR)string;

	if (MessageId) {		
		_tcscpy( str, LS(MessageId) ); 
		
		// убрать теги их HTML сообщений 
		LPTSTR s = _tcsstr(str, TEXT("<br>"));
		if ( s > 0 ) 
			*s =0;
		
		ti.lpszText = (LPTSTR)str; 
	}
	
	if (MessageId == 0 && string==NULL) {
		//скрыть алерт 
		::SendMessage(_hwnd_ToolTip ,TTM_TRACKACTIVATE,(WPARAM)FALSE, (LPARAM)&ti);
		return;
	}
	
	
	::SendMessage(_hwnd_ToolTip, TTM_UPDATETIPTEXT, 1, (LPARAM)&ti);
	
	::GetWindowRect(GetDlgItem(hwnd, idControl), &ti.rect);
	
	TCHAR title[100];
	if (idTitle)
		_tcscpy(title, LS(idTitle) );
	else 
		_tcscpy(title, TEXT("Rohos") );
	
	
	::SendMessage(_hwnd_ToolTip, TTM_SETTITLE, icon, (LPARAM)(LPCTSTR)title); // show it
	::SendMessage(_hwnd_ToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 13000); // ... time to Tip - remains visible 
	::SendMessage(_hwnd_ToolTip, TTM_SETMAXTIPWIDTH, 0, (LPARAM) 280);
	
	/*
	if (MessageId == IDS_FAILED_TO_AUTOFILL2) {
		::SendMessage(_hwnd_ToolTip, TTM_SETMAXTIPWIDTH, 0, (LPARAM) 350);
		::SendMessage(_hwnd_ToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 23000);
		::SendMessage(_hwnd_ToolTip, TTM_SETTITLE, 3, (LPARAM)(LPCTSTR)"Rohos"); // show it
	}*/
	
	::SendMessage(_hwnd_ToolTip ,TTM_TRACKPOSITION,0, (LPARAM)(DWORD) MAKELONG(ti.rect.left + 40, ti.rect.bottom) );
	::SendMessage(_hwnd_ToolTip ,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);
	
	KillTimer(hwnd, 15);
	SetTimer(hwnd, 15, 12900, NULL); // закрыть тулТип через 7 сек

}



LPCWSTR LSW(UINT id) 
{
    //WriteLog("LSW(%d) called. calling LS()", id);    
	
    static WCHAR str[800];
    
    //::LoadStringW(h_lang, id, str, 790);
    
    LPCTSTR as=LS(id);    
    //WriteLog("LS() returned: %s", as);
#ifndef UNICODE
    MultiByteToWideChar(CP_ACP, 0, as, -1, str, 700);
#else
	wcscpy(str, as);
#endif
    return str;
}

char* strchr_rev (char* str, char code) {

	int i=0;

	while (i++ < 60) {
		if (*(str--) == code)
			return str+1;
	}

	return NULL;
}

LPWSTR strchr_rev(LPWSTR str, WCHAR code) {

	int i=0;

	while (i++ < 60) {
		if (*(str--) == code)
			return str+1;
	}

	return NULL;

}

// загрузить строку.
LPCTSTR LS(UINT id) {

	static TCHAR strOut[800];

	if (lpStrings == NULL ) {

		// ********************** загрузка строковых файлов  ******************************

		TCHAR str1[150];
		char str[150];

		lpStrings = (char*)0xFFFFFFFF; // не удалось прочитать. 

		// загрузить строки - 1 раз. 
		TCHAR path[500];
		//GetPodNogamiPath(path, false);
		ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), TEXT("RohosPath"), path, 300);

		if ( _tcslen(path) ==0) 
		{
			// диретории Rohos не найдено, это Rohos Mini 
			GetPodNogamiPath(path, false);
			_tcscat(path,TEXT("_rohos\\lang.txt") );
		} else
		{
			
			_tcscat(path,TEXT("languages\\") );
			ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), "Language", str1, 30);
			
			_tcscat(path, str1 );
			_tcscat(path, TEXT(".txt") );
		}

		//::MessageBox(0, path, "rohos", 0);

		if ( _taccess(path, 0) ==0 ) {
			// есть такой lang файл ? 
			HANDLE h = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
			DWORD readSz=0, fileSize = GetFileSize(h, NULL);
			lpStrings = new char[fileSize+10];
			memset(lpStrings, 0, fileSize+5);
			ReadFile(h, lpStrings, fileSize, &readSz, NULL);
			CloseHandle(h);

			if (readSz ==0) {
				lpStrings = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 
				WriteLog("LS: error read %s", path);
			}
			else {
				// определить уникод 
				is_lang_unicode  = IsTextUnicode(lpStrings, 50, NULL);
			}
		}	
			else
			lpStrings = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 
			

		//GetPodNogamiPath(path, false);
		ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), TEXT("RohosPath"), path, 300);

		if ( _tcslen(path) ==0) 
		{
			// диретории Rohos не найдено, это Rohos Mini 
			GetPodNogamiPath(path, false);
			_tcscat(path,TEXT("_rohos\\strings.h") );
		} else
		
			_tcscat(path, TEXT("languages\\strings.h") );		

		//::MessageBox(0, path, "rohos", 0);

		if ( _taccess(path, 0) ==0 ) {
			// есть такой lang файл ? 
			HANDLE h = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
			DWORD readSz=0, fileSize = GetFileSize(h, NULL);
			lpStringDefs = new char[fileSize+10];
			memset(lpStringDefs, 0, fileSize+5);
			ReadFile(h, lpStringDefs, fileSize, &readSz, NULL);
			CloseHandle(h);

			if (readSz ==0) {
				WriteLog("LS: error read2 %s", path);
				lpStrings = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 
			}
		}	else
			lpStrings = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 


	}

	if (lpStrings != (char*)0xFFFFFFFF && is_lang_unicode == false) {
		// найти и венуть строку
		

		char defString[500] = {0};
		char String[500] = {0};

		memset(String, 0, 490);
		char str[150];

		strcpy(defString, ""); // копируем 
		
		itoa(id, str, 10);

		//WriteLog("LSd(%s)",str );
		
		// ********************** поиcк IDS_STRING_ID по номеру ******************************
		char* lpDef = strstr(lpStringDefs, str);
		if (lpDef) {
			lpDef = strchr_rev(lpDef, '#');	// ищем назад первый #
			if (lpDef) {
				lpDef+=8;
				char* strend = strpbrk(lpDef, " \x9");  // ищем вперед любой пробел либо TAB (9)
				if ( strend && strend-lpDef < 400)
					strncpy(defString, lpDef, strend-lpDef); // копируем 				
				
			}
		}

		// ********************** поиcк строки по  IDS_STRING_ID ******************************
		if ( strlen(defString) ) {
			// 

			strcat(defString, " ");
			lpDef = strstr(lpStrings, defString);

			if (!lpDef) {
				defString[ strlen(defString)-1 ] = '\x9';				
				lpDef = strstr(lpStrings, defString);
			}

			//WriteLog("LS0(%s)",defString );
			
			if (lpDef ) {
				lpDef++;
				lpDef = strstr(lpDef, "\"");
				if(lpDef) {
					lpDef++;
					char* strend = strpbrk(lpDef, "\x0D\x0A");  // ищем вперед первый " (конец строки)
					if ( strend && strend-lpDef < 490)
						strncpy(String, lpDef, strend-lpDef-1); // копируем 
				}

			}

			//WriteLog("LS01(%s)",String );

			// поиск \т и замена на \x0A
			if ( strlen(String) ) {
				char* s1 ;
				while (s1 = strstr(String, "\\n") ){				
					*s1 = ' ';
					*(s1+1) = '\n';

				}

				int i1=strlen(String);

				if (String[i1-1]=='\x22') // убираем в конце случайный "
					String[i1-1]=0;
			}


		} else {
			LoadString(h_lang, id, strOut, 200);
			WriteLog("LS(%d)",id );
			return strOut;
			//strcpy(String, "Unknown: "); // ошибка поиска строки
			//strcat(String, str ); 

		}



		if ( strlen(String) ) {
			
#ifdef UNICODE			
			MultiByteToWideChar(CP_ACP, 0, String, -1, strOut, 750);
#else
		strcpy(strOut, String);
#endif
		
		return strOut;
		} else {
			WriteLog("LS2(%d).",id );
		}
		

	}


	// загрузка только уникода 
	if (lpStrings != (char*)0xFFFFFFFF && is_lang_unicode) {
		// найти и венуть строку
		

		WCHAR defString[500] = {0};
		WCHAR String[500] = {0};

		memset(String, 0, 490);
		WCHAR str[150];

		wcscpy(defString, L"" ); // копируем 
		
		_itow(id, str, 10);

		LPWSTR lpDef = wcsstr((LPWSTR)lpStringDefs, str);
		if (lpDef) {
			lpDef = strchr_rev(lpDef, '#');	// ищем назад первый #
			if (lpDef) {
				lpDef+=8;
				LPWSTR strend = wcspbrk(lpDef, L" \x9");  // ищем вперед любой пробел либо TAB (9)
				if ( strend && strend-lpDef < 400)
					wcsncpy(defString, lpDef, strend-lpDef); // копируем 
				
			}
		}

		if ( wcslen(defString) ) {
			// 

			wcscat(defString, L" ");
			lpDef = wcsstr((LPWSTR)lpStrings, defString);

			if (!lpDef) {
				defString[ wcslen(defString)-1 ] = '\x9';				
				lpDef = wcsstr((LPWSTR)lpStrings, defString);
			}

			if (lpDef) {
				lpDef++;
				lpDef = wcsstr(lpDef, L"\"");
				if(lpDef) {
					lpDef++;
					LPWSTR strend = wcspbrk(lpDef,L"\"");  // ищем вперед первый " (конец строки)
					if ( strend && strend-lpDef < 490)
						wcsncpy(String, lpDef, strend-lpDef); // копируем 
				}

			}

			// поиск \т и замена на \x0A
			LPWSTR s1 ;
			while (s1 = wcsstr(String, L"\\n") ){				
				*s1 = ' ';
				*(s1+1) = '\n';
				
			}

		} else {
			//wcscpy(String, L"Unknown: "); // ошибка поиска строки
			//wcscat(String, str ); 
			LoadStringW(GetModuleHandle(NULL), id, String, 200);

		}



		if ( wcslen(String) ) {			
			
#ifdef UNICODE						
			_tcscpy(strOut, String);
#else
			WideCharToMultiByte(CP_ACP, 0,  String, -1, strOut, 255, NULL, NULL);
			
#endif
			
			return strOut;
		}

	}

	::LoadString(h_lang, id, strOut, 790);
	WriteLog("LS3(%d).",id );

	//::MessageBox(0, strOut, "rohos2", 0);

	//if( id==2316 )
	//	MessageBox(0, strOut, "LS(", 0);

	return strOut;
}



// загрузить строку from JS file
LPCTSTR LS(LPCSTR js_string_name) {

	static TCHAR strOut[800];

	strOut[0]=0;

	if (lpStrings_Js == NULL ) {

		// ********************** загрузка строковых файлов  ******************************

		TCHAR str1[150];
		char str[150];

		lpStrings_Js = (char*)0xFFFFFFFF; // не удалось прочитать. 

		// загрузить строки - 1 раз. 
		TCHAR path[500];
		//GetPodNogamiPath(path, false);
		ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), TEXT("RohosPath"), path, 300);

		if ( _tcslen(path) ==0) 
		{
			// диретории Rohos не найдено, это Rohos Mini 
			GetPodNogamiPath(path, false);
			_tcscat(path,TEXT("_rohos\\lang.js") );
		} else
		{
			
			_tcscat(path,TEXT("languages\\") );
			ReadReg(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Rohos"), "Language", str1, 30);
			
			_tcscat(path, str1 );
			_tcscat(path, TEXT(".js") );
		}

		//::MessageBox(0, path, "rohos", 0);

		if ( _taccess(path, 0) ==0 ) {
			// есть такой lang файл ? 
			HANDLE h = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
			DWORD readSz=0, fileSize = GetFileSize(h, NULL);
			lpStrings_Js = new char[fileSize+10];
			memset(lpStrings_Js, 0, fileSize+5);
			ReadFile(h, lpStrings_Js, fileSize, &readSz, NULL);
			CloseHandle(h);

			if (readSz ==0) {
				lpStrings_Js = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 
				WriteLog("LS: error read %s", path);
			}
			
		}	
			else
			lpStrings_Js = (char*)0xFFFFFFFF; // забудь про это тфайл. не удалось прочитать. 

	}

	if (lpStrings_Js != (char*)0xFFFFFFFF ) {
		// найти и венуть строку


		_tcscpy(strOut, _T(""));

		char defString[500] = {0};
		char String[500] = {0};

		memset(String, 0, 490);
		char str[150];

		strcpy(defString, js_string_name); // копируем 


		// ********************** поиcк строки по имени  ******************************
		if ( strlen(defString) ) {
			// 
			strcat(defString, " "); // копируем 


			const char* lpDef = strstr(lpStrings_Js, defString);

			if (!lpDef) {
				defString[ strlen(defString)-1 ] = '\x9';				
				lpDef = strstr(lpStrings_Js, defString);
				if (!lpDef) {
					defString[ strlen(defString)-1 ] = '=';				
					lpDef = strstr(lpStrings_Js, defString);
				} 
			} 			
			

			//WriteLog("LS0(%s)",defString );

			if (lpDef ) {
				lpDef++;
				lpDef = strstr(lpDef, "\"");
				if(lpDef) {
					lpDef++;
					const char* strend = strpbrk(lpDef, "\x0D\x0A");  // ищем вперед первый " (конец строки)
					if ( strend && strend-lpDef < 490)
						strncpy(String, lpDef, strend-lpDef-1); // копируем 
				}

			}

			//WriteLog("LS01(%s)",String );

			// поиск \т и замена на \x0A
			if ( strlen(String) ) {
				char* s1 ;
				while (s1 = strstr(String, "\\n") ){				
					*s1 = ' ';
					*(s1+1) = '\n';

				}

				int i1=strlen(String);

				if (String[i1-1]=='\x22') // убираем в конце случайный "
					String[i1-1]=0;
			}


		} else {
			
			WriteLog("LS(%s)",js_string_name );
			return strOut;
			//strcpy(String, "Unknown: "); // ошибка поиска строки
			//strcat(String, str ); 

		}



		if ( strlen(String) ) {

#ifdef UNICODE			
			MultiByteToWideChar(CP_ACP, 0, String, -1, strOut, 750);
#else
			strcpy(strOut, String);
#endif
			return strOut;

		}
	}

		

		WriteLog("LS(%s).",js_string_name );


	return strOut;
}




bool set_foreground(HWND hWnd)
{
	HWND hCurrWnd;
	int iMyTID;
	int iCurrTID;
	
	hCurrWnd = ::GetForegroundWindow();
	iMyTID   = GetCurrentThreadId();
	iCurrTID = GetWindowThreadProcessId(hCurrWnd,0);
	
	if ( AttachThreadInput(iMyTID, iCurrTID, TRUE) )
	
	{
		// hWnd - дескриптор окна.
		SetForegroundWindow(hWnd);
		BringWindowToTop(hWnd);

		AttachThreadInput(iMyTID, iCurrTID, FALSE);	

		return true;
	} else
	{
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		SetForegroundWindow(hWnd);
		//BringWindowToTop(hWnd);
		

	}

	return false;
    
	
}


/** return TRUE for Vista and Higher 
*/
bool IsWindowsVista() {

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&verInfo);

	if ( verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		verInfo.dwMajorVersion >= 6 )
		return true;

	return false;
   
}

/** return TRUE for 8 or Higher 
*/
bool IsWindows8() {

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&verInfo);

	if ( verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		verInfo.dwMajorVersion >= 6 && 
		verInfo.dwMinorVersion >= 2 )
		return true;

	return false;
   
}


/** return TRUE for any server platform
*/
bool IsWindowsServer() {

	OSVERSIONINFOEX verInfo = {0};
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx((OSVERSIONINFO*)&verInfo);

	if ( verInfo.wProductType == VER_NT_SERVER || 
		verInfo.wProductType  == VER_NT_DOMAIN_CONTROLLER )
		return true;

	return false;
   
}


/** Получить имя юзера и домена из сессис
*/
void GetSessionUsername(LPTSTR username, LPTSTR domainname, DWORD bufferlen)
{
	
	//HANDLE user_token;
	//PTOKEN_USER   ptgUser        = NULL;
	//BOOL success = false;

	
	_tcscpy(username, TEXT("") );
	_tcscpy(domainname, TEXT("") );

	DWORD len2 = bufferlen;

	DWORD session_id = 0xFFFFFF;
	ProcessIdToSessionId( GetCurrentProcessId() , &session_id );

	LPTSTR str =0;
	_tcscpy(username, TEXT(""));


	len2 = bufferlen;
	WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE , session_id, WTSUserName, &str, &len2);
	if (str)
		_tcscpy(username, str);

	len2 = bufferlen;
	WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE , session_id, WTSDomainName, &str, &len2);
	if (str)
		_tcscpy(domainname, str);
	
	//success = true;	
	

	// в Win2000 вслучае неудачи просто получить имя юзера и компьютера 
	if (_tcslen(username)==0 && IsWindowsVista()==0 ) {

		len2 = bufferlen;
		GetUserName(username, &len2);		
		len2 = bufferlen;
		GetComputerName(domainname, &len2);
		WriteLog("Session user name2: %s [%s] (session %d)", username, domainname, session_id );

		if (IsWindowsVista() && _tcsicmp(username, TEXT("SYSTEM") ) ==0) {
			_tcscpy(username, TEXT("") );
		}

	} else
		WriteLog("Session user name: %s [%s] (session %d)", username, domainname, session_id );

	return;
}

/** Получить имя юзера и домена из сессис
*/
void GetSessionUsername2(int sessid,  LPTSTR username, LPTSTR domainname, DWORD bufferlen)
{
	
	//HANDLE user_token;
	//PTOKEN_USER   ptgUser        = NULL;
	//BOOL success = false;

	
	_tcscpy(username, TEXT("") );
	_tcscpy(domainname, TEXT("") );

	DWORD len2 = bufferlen;

	DWORD session_id = 0xFFFFFF;

	session_id = sessid;

	if (sessid == -1)
		ProcessIdToSessionId( GetCurrentProcessId() , &session_id );


	LPTSTR str =0;
	_tcscpy(username, TEXT(""));


	len2 = bufferlen;
	WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE , session_id, WTSUserName, &str, &len2);
	if (str)
		_tcscpy(username, str);

	len2 = bufferlen;
	WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE , session_id, WTSDomainName, &str, &len2);
	if (str)
		_tcscpy(domainname, str);
	
	
	return;
}

//#include "Muiload.h"

// загрузить строку using MUI resources
// 

typedef struct UI_STRING_
{
	 int rohos_str_id;

	 int win_2000_str_id;
	 int win_xp_str_id;
	 int win_2003_str_id;
	 int win_vista_str_id;
	 int win_seven_str_id;
	 int win_8_str_id;
	 TCHAR default_str_en[100];

}UI_STRING;

LPCTSTR MUI(UINT id) {

	
	UI_STRING strings_array[50] = { //					w2k		XP		2003	Vista	Seven	8
		{IDS_CREATE_FOLDER,								4104,	4104,	4104,	4104,	30396,	0,	TEXT("Create Folder")},
		{IDS_NEW_FOLDER,								30320,	30320,	30320,	30396,	30396,	0,	TEXT("New Folder")},
		{IDS_ERR_WHILE_CREATE_FOLDER,					6460,	6460,	6460,	6460,	6461,	0,	TEXT("Error while creating folder")},
		{IDS_SMALL_ICONS,								6460,	6460,	6460,	33578,	33578,	0,	TEXT("Small")},
		{IDS_LIST,										6460,	6460,	6460,	33579,	33579,	0,	TEXT("List")},
		{IDS_REPORT,									6460,	6460,	6460,	33580,	33580,	0,	TEXT("Report")},
		{IDS_FILE_IS_IN_USE,							6264,	6264,	6264,	6264,	6264,	0,	TEXT("File/Folder is used by abother Application.")},
		{IDS_FOLDER_DOESNT_EXIST,						4135,	4135,	4135,	6458,	6458,	0,	TEXT("The Folder does not exist")},

		{0, 0, 0, 0, 0}
	};

	static TCHAR strOut[800];

	strOut[0]=0;

	static HMODULE mui_module=0;

	if ( IsWindowsVista() /*Vista or Seven*/ && mui_module == 0)
	{
		//mui_module  = LoadMUILibrary("shell32.dll", MUI_LANGUAGE_NAME, LOCALE_USER_DEFAULT);
	} 
	

	if (mui_module == 0)
		mui_module = LoadLibrary( TEXT("shell32.dll"));

	int i=0;
	while (strings_array[i].rohos_str_id != id && strings_array[i].rohos_str_id !=0)
		i++;


	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&verInfo);

	if ( verInfo.dwMajorVersion == 5 && verInfo.dwMinorVersion ==0) // win2000
		LoadString( mui_module, strings_array[i].win_2000_str_id , strOut, 800);	

	if ( verInfo.dwMajorVersion == 5 && verInfo.dwMinorVersion ==1) // winXP
		LoadString( mui_module, strings_array[i].win_xp_str_id , strOut, 800);	

	if ( verInfo.dwMajorVersion == 5 && verInfo.dwMinorVersion ==2) // win2003
		LoadString( mui_module, strings_array[i].win_2003_str_id , strOut, 800);	


	if ( verInfo.dwMajorVersion == 6 && verInfo.dwMinorVersion ==0) // Vista
		LoadString( mui_module, strings_array[i].win_vista_str_id , strOut, 800);	

	if ( verInfo.dwMajorVersion == 6 && verInfo.dwMinorVersion ==1) // Seven + 2008
		LoadString( mui_module, strings_array[i].win_seven_str_id , strOut, 800);	

	if ( _tcslen(strOut) ==0)
	{
		_tccpy(strOut, strings_array[i].default_str_en);
	}

	return strOut;

}

HANDLE h_accFile =0;



LPCTSTR make_hex(BYTE *inps,int sz)
{
	static char hex_str[500] = {""};
	static WCHAR wstr_hex_str[500] = {L""};

	memset(hex_str,0,400);
	if(sz)
	{
		
		
		int k=0, i=0;
		for(i=0; i<sz && i<55; ++i)
		{
			k = sprintf(hex_str+(i*2),"%.2X",inps[i])+1;
		}
		if (i>=55)
			strcat(hex_str, "..");
	}
#ifdef UNICODE
	mbstowcs(wstr_hex_str, hex_str, 200);
	return wstr_hex_str;
#else
	return hex_str;
#endif
}




