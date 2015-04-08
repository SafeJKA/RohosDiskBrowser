
// TraceMaker.cpp: implementation of the CTraceMaker class.
// Rajeev Sadasivan, Trivandrum, INDIA
// 22/Nov/2002
//////////////////////////////////////////////////////////////////////

//>> #include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "TraceMaker.h"
#include <common1.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TRACER_WINDOW_CLASS _T("_TracerWindow")
#define TRACER_WINDOW_NAME  _T("Tracer")
#define TRACER_ENTER_FN		_T(">> Entering ")
#define TRACER_LEAVE_FN		_T("<< Leaving ")
#define TRACER_FONT			_T("Arial")
#define TRACER_NEWLINE      _T("\r\n")

#define TRACER_WND_LEFT		200
#define TRACER_WND_RIGHT	200
#define TRACER_WND_WIDTH	600
#define TRACER_WND_HEIGHT	400

CTraceMaker *CTraceMaker::m_psTraceMaker = CTraceMaker::GetTracer();
HWND CTraceMaker::m_hEdit = NULL;

CTraceFunction::CTraceFunction(LPCTSTR szFunction)
{
	_tcscpy(m_szFunction, szFunction);

	TCHAR szBuffer[2*TRACER_MAX_BUFFER];
	_stprintf(szBuffer, _T("%s %s"), TRACER_ENTER_FN, m_szFunction);
	TRACER.Print(szBuffer);
}

CTraceFunction::~CTraceFunction()
{
	TCHAR szBuffer[2*TRACER_MAX_BUFFER];
	_stprintf(szBuffer, _T("%s %s\r\n"), TRACER_LEAVE_FN, m_szFunction);
	TRACER.Print(szBuffer);
}

CTraceMaker::CTraceMaker()
{
	//_tcscpy(m_szFile, TRACER_DEFAULT_FILE);
	_tcscpy(m_szFile, TEXT(""));
	m_MaxFileSizeKb	= 600;//T1_MB;
	m_enumTarget= FILE_ONLY;
	m_hEditFont	= NULL;
	m_enabled = true; 
}

CTraceMaker::~CTraceMaker()
{

}

void CTraceMaker::Initialize(LPCTSTR pszFile, DWORD dwSizeKb/*MB default*/, 
								TraceTarget target /*FILE_ONLY default*/, 
								bool bWndPerProcess /*false default*/)
{
	m_bWndPerProcess = bWndPerProcess;

	m_MaxFileSizeKb = dwSizeKb;


	if (1==ReadReg(HKEY_CURRENT_USER, TEXT("Software\\Rohos"), TEXT("DisableLog"), 0) )
		return;

	if (1==ReadReg(HKEY_LOCAL_MACHINE, TEXT("Software\\Rohos"), TEXT("DisableLog"), 0) )
		return;
	
	if ( (target < FILE_ONLY) || (target > FILE_AND_WINDOW) )
		target = FILE_ONLY;
	switch(target)
	{
	case FILE_ONLY:
		m_enumTarget = target;
		if (NULL != pszFile)
			_tcscpy(m_szFile, pszFile);
		break;
	case WINDOW_ONLY:
		m_enumTarget = target;
		InitiateWindow();
		break;
	case FILE_AND_WINDOW:
		m_enumTarget = target;
		if (NULL != pszFile)
			_tcscpy(m_szFile, pszFile);
		InitiateWindow();
		break;
	}

	
	TCHAR ver[30], date[40];
	get_file_version(NULL, ver, date);

	LogFile("\n\n");
	LogFile("================================================== ");
	WriteLog("============================= (%s, %s, %s)", __DATE__, ver, date);

}

CTraceMaker* CTraceMaker::GetTracer()
{
	if (NULL == m_psTraceMaker)
	{
		m_psTraceMaker = new CTraceMaker();
	}

	return m_psTraceMaker;
}

void CTraceMaker::Print(LPCTSTR szType, LPCTSTR szData, ...)
{
#ifdef TRACER_ENABLED
	TCHAR Buffer[8*TRACER_MAX_BUFFER] = {0};
	va_list pArgList;

	va_start(pArgList, szData);
	 // The last argument to wvsprintf points to the arguments
    _vsntprintf(Buffer, sizeof (Buffer), 
                  szData, pArgList) ;
	va_end(pArgList);

	TCHAR szFinalLog[3*TRACER_MAX_BUFFER];
	_stprintf(szFinalLog, _T("%s%s\r\n"), szType, Buffer);

	LogFile(szFinalLog);
#endif // TRACER_ENABLED
}

void CTraceMaker::Print(LPCTSTR szData, ...)
{
#ifdef TRACER_ENABLED
	TCHAR Buffer[8*TRACER_MAX_BUFFER] = {0};
	va_list pArgList;

	va_start(pArgList, szData);
	 // The last argument to wvsprintf points to the arguments
    _vsntprintf(Buffer, sizeof (Buffer), 
                  szData, pArgList) ;
	va_end(pArgList);
	_tcscat(Buffer, _T("\r\n"));
	LogFile(Buffer);
#endif // TRACER_ENABLED
}

void CTraceMaker::LogFile(LPCSTR szData)
{

	char data[15500]="";

	if (1==ReadReg(HKEY_CURRENT_USER, TEXT("Software\\Rohos"), TEXT("DisableLog"), 0) )
		return;

	if (1==ReadReg(HKEY_LOCAL_MACHINE, TEXT("Software\\Rohos"), TEXT("DisableLog"), 0) )
		return;


#ifdef TRACER_ENABLED

	if (!m_enabled)
		return;
	

	//PutTexttoWindow(szData);
	HANDLE hLog = INVALID_HANDLE_VALUE;
	ULONG unBytesWritten = 0;
	while (1)	// To control logging size;
	{
		hLog = CreateFile( m_szFile, GENERIC_READ|GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,
							NULL);
		if (INVALID_HANDLE_VALUE == hLog)
			return;
		
		DWORD dwSize = GetFileSize (hLog, NULL);
		if ( dwSize > m_MaxFileSizeKb * 1024 ) // Size Checking.
		{
			/// \todo make rotation, not beginig from start
			CloseHandle(hLog);
			DeleteFile(m_szFile);
			continue;
		}
		break;
	}

	SYSTEMTIME	time; 
	GetLocalTime( &time);	
	sprintf(data, "%2d:%2d : %2d:%3d   ", time.wHour, time.wMinute, time.wSecond , time.wMilliseconds );
	strcat(data, szData);
	strcat(data, "\x0D\x0A\x00" );
	int i = strlen(data) * sizeof(char);

	SetFilePointer(hLog, 0, NULL, FILE_END);
	WriteFile(hLog, data, i, &unBytesWritten, NULL);

	if (INVALID_HANDLE_VALUE != hLog)
		CloseHandle(hLog);  
#endif // TRACER_ENABLED
}

void CTraceMaker::LogFile(LPCWSTR szData)
{


	if (1==ReadReg(HKEY_CURRENT_USER, TEXT("Software\\Rohos"), TEXT("DisableLog"), 0) )
		return;


#ifdef UNICODE
#ifdef TRACER_ENABLED
	WCHAR data[500]=TEXT("");
	

	PutTexttoWindow(szData);
	HANDLE hLog = INVALID_HANDLE_VALUE;
	ULONG unBytesWritten = 0;
	while (1)	// To control logging size;
	{
		hLog = CreateFile( m_szFile, GENERIC_READ|GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,
							NULL);
		if (INVALID_HANDLE_VALUE == hLog)
			return;
		
		DWORD dwSize = GetFileSize (hLog, NULL);
		if ( dwSize > m_MaxFileSizeKb * 1024 ) // Size Checking.
		{
			/// \todo make rotation, not beginig from start
			CloseHandle(hLog);
			DeleteFile(m_szFile);
			continue;
		}
		break;
	}

	SYSTEMTIME	time; 

	GetLocalTime( &time);	
	_stprintf(data, TEXT("%2d:%2d : %2d:%3d   "), time.wHour, time.wMinute, time.wSecond , time.wMilliseconds );
	_tcscat(data, szData);
	_tcscat(data, TEXT("\x0D\x0A\x00") );
	int i = _tcslen(data) * sizeof(TCHAR);

	SetFilePointer(hLog, 0, NULL, FILE_END);
	WriteFile(hLog, data, i, &unBytesWritten, NULL);

	if (INVALID_HANDLE_VALUE != hLog)
		CloseHandle(hLog);
#endif // TRACER_ENABLED
#endif
}

CTraceMaker CTraceMaker::operator << (int nValue)
{
#ifdef TRACER_ENABLED
	char szLog[TRACER_NUMSIZE] = {0};
	sprintf(szLog, "%d ", nValue);
	LogFile(szLog);
#endif // TRACER_ENABLED

	return (*this);
}

CTraceMaker CTraceMaker::operator << (unsigned int unValue)
{
#ifdef TRACER_ENABLED
	char szLog[TRACER_NUMSIZE] = {0};
	sprintf(szLog, "%d ", unValue);
	LogFile(szLog);
#endif // TRACER_ENABLED

	return (*this);
}

CTraceMaker CTraceMaker::operator << (short sValue)
{
#ifdef TRACER_ENABLED
	char szLog[TRACER_NUMSIZE] = {0};
	sprintf(szLog, "%s ", sValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (float fValue)
{
#ifdef TRACER_ENABLED
	char szLog[TRACER_NUMSIZE] = {0};
	sprintf(szLog, "%f ", fValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (double dfValue)
{
#ifdef TRACER_ENABLED
	TCHAR szLog[TRACER_NUMSIZE] = {0};
	_stprintf(szLog, _T("%f "), dfValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (bool bValue)
{
#ifdef TRACER_ENABLED
	char szLog[TRACER_NUMSIZE] = {0};
	sprintf(szLog, "%d ", bValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (unsigned char byValue)
{
#ifdef TRACER_ENABLED
	TCHAR szLog[TRACER_NUMSIZE] = {0};
	_stprintf(szLog, _T("%d "), byValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (char cValue)
{
#ifdef TRACER_ENABLED
	TCHAR szLog[TRACER_NUMSIZE] = {0};
	_stprintf(szLog, _T("%c "), cValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (wchar_t wcValue)
{
#ifdef TRACER_ENABLED
	TCHAR szLog[TRACER_NUMSIZE] = {0};
	_stprintf(szLog, _T("%c "), wcValue);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

CTraceMaker CTraceMaker::operator << (char* pszValue)
{
#ifdef TRACER_ENABLED
	if ( !pszValue ) return (*this);
	LogFile(pszValue);
#endif // TRACER_ENABLED
	return *this;
}

CTraceMaker CTraceMaker::operator << (wchar_t *pszValue)
{
#ifdef TRACER_ENABLED
	if ( !pszValue ) return (*this);

	TCHAR szLog[2*TRACER_MAX_BUFFER]	= {0};
	char  szSource[2*TRACER_MAX_BUFFER] = {0};
	TCHAR *pLodAddr = (TCHAR*)pszValue;
#if !defined (UNICODE) && !defined (_UNICODE)
	wcstombs(szSource, pszValue, wcslen(pszValue));
	pLodAddr = (TCHAR*)szSource;
#endif	// (_UNICODE || UNICODE)

	_stprintf(szLog, _T("%s "), pLodAddr);
	LogFile(szLog);
#endif // TRACER_ENABLED
	return (*this);
}

/* 
	If this is called, there won't be any tracer 
	object, till next GetTracer() is called. 
*/
void CTraceMaker::Release()
{
	if ( m_psTraceMaker )
	{
		delete m_psTraceMaker;
		m_psTraceMaker = NULL;
	}
}

LRESULT CALLBACK CTraceMaker::TracerWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SIZE:
		{
			RECT rect = {0};
			GetClientRect(hwnd, &rect);
			MoveWindow(CTraceMaker::m_hEdit, rect.left, rect.top, 
						rect.right-rect.left, rect.bottom-rect.top, TRUE);
		}
		break;
	case WM_ERASEBKGND:
		return TRUE;	
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}

	return 1;
}
 
void CTraceMaker::RegisterWindow(HINSTANCE hInst) 
{ 
	#ifndef _USRDLL
    WNDCLASSEX wcx = {0}; 
 
    // Fill in the window class structure with parameters 
    // that describe the main window. 
    wcx.cbSize		= sizeof(wcx);				// size of structure 
    wcx.style		= CS_HREDRAW | CS_VREDRAW;	// redraw if size changes 
    wcx.lpfnWndProc	= TracerWndProc;			// points to window procedure 
    wcx.cbClsExtra	= 0;						// no extra class memory 
    wcx.cbWndExtra	= 0;						// no extra window memory 
    wcx.hInstance	= hInst;					// handle of instance 
    wcx.hIcon		= LoadIcon(NULL, IDI_WARNING);// predefined app. icon 
    wcx.hCursor		= LoadCursor(NULL, IDC_IBEAM);// predefined arrow 
    wcx.hbrBackground= (HBRUSH)GetStockObject( WHITE_BRUSH); // white background brush 
    wcx.lpszMenuName =  NULL;						// name of menu resource 
    wcx.lpszClassName= TRACER_WINDOW_CLASS;			// name of window class 
    wcx.hIconSm		 = NULL;						// Stabdard Icon
 
    // Register the window class. 
    RegisterClassEx(&wcx); 

#endif
} 
 
bool CTraceMaker::InitiateWindow() 
{ 
	#ifndef _USRDLL
	do
	{
		// Find existing window, if any.
		HWND hExistWnd = FindWindow(TRACER_WINDOW_CLASS, TRACER_WINDOW_NAME);
		if (NULL == hExistWnd)
			break;

		DWORD dwCurProcId = GetCurrentProcessId();
		DWORD dwProcId  = 0;
		GetWindowThreadProcessId(hExistWnd, &dwProcId);
		if ( (dwProcId == dwCurProcId) || (false == m_bWndPerProcess) )
		{
			m_hEdit = FindWindowEx(hExistWnd, NULL, _T("EDIT"), _T(""));
			ShowWindow(hExistWnd, SW_RESTORE);
			SetForegroundWindow(hExistWnd);
			return true;
		}
	} while(0);

    HWND hWnd = NULL; 
    HINSTANCE hInst= GetModuleHandle(NULL); 
	RegisterWindow(hInst);

    // Create the main window. 
    hWnd = CreateWindow( // ES_AUTOHSCROLL ES_AUTOVSCROLL
        TRACER_WINDOW_CLASS, // name of window class 
        TRACER_WINDOW_NAME,  // title-bar string 
        WS_OVERLAPPEDWINDOW, // top-level window 
        TRACER_WND_LEFT,     // default horizontal position 
        TRACER_WND_RIGHT,    // default vertical position 
        TRACER_WND_WIDTH,    // default width 
        TRACER_WND_HEIGHT,   // default height 
        (HWND)NULL,          // no owner window 
        (HMENU)NULL,         // use class menu 
        hInst,				 // handle of application instance 
        (LPVOID)NULL);       // no window-creation data 
 
    if (!hWnd) 
        return false; 
	
	// Create Edit window as per parent window size.
	RECT rcParent = {0};
	GetClientRect(hWnd, &rcParent);
    m_hEdit = CreateWindow( 
        _T("EDIT"), _T(""), 
		WS_CHILD|WS_VISIBLE|WS_VSCROLL| //ES_AUTOHSCROLL|ES_AUTOVSCROLL|WS_HSCROLL| 
		ES_MULTILINE|ES_READONLY,
        0, 0, (rcParent.right-rcParent.left), (rcParent.bottom-rcParent.top), 
        (HWND)hWnd, (HMENU)NULL, hInst, (LPVOID)NULL);

	// Change FONT to Arial.
	LOGFONT lFont = {0};
	lFont.lfHeight		= 15;
	lFont.lfWidth		= 0;
	lFont.lfEscapement	= 0;
	lFont.lfOrientation	= 0;
	lFont.lfWeight		= 200;
	lFont.lfItalic		= 0;
	lFont.lfUnderline	= 0;
	lFont.lfStrikeOut	= 0;
	lFont.lfCharSet		= ANSI_CHARSET;
	lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lFont.lfClipPrecision= CLIP_DEFAULT_PRECIS;
	lFont.lfQuality		= DEFAULT_QUALITY;
	lFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	_tcscpy(lFont.lfFaceName, TRACER_FONT);
	m_hEditFont = CreateFontIndirect(&lFont);
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)m_hEditFont, MAKELPARAM(0, 0));

	// Show the window and send a WM_PAINT message to the window procedure. 
    ShowWindow(hWnd, SW_SHOWNORMAL); 
    UpdateWindow(hWnd); 

	#endif
    return true; 
} 

// Append the given data to end of edit control.
void CTraceMaker::PutTexttoWindow(LPCTSTR szData)
{ 
	int cntChars = SendMessage(m_hEdit, WM_GETTEXTLENGTH, 0, 0);
	
	HLOCAL hEditCurrent = (HLOCAL)SendMessage(m_hEdit, EM_GETHANDLE, 0, 0);
	TCHAR *pOldData = (TCHAR*)LocalLock(hEditCurrent);
	if (NULL == pOldData)
	{
		// Probably, window is created by another process.
		HLOCAL hNewBlock= LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT, 64*TRACER_MAX_BUFFER);
		TCHAR *pNewData = (TCHAR*)LocalLock(hNewBlock);
		SendMessage(m_hEdit, WM_GETTEXT, (WPARAM)(64*TRACER_MAX_BUFFER), (LPARAM)pNewData);
		if ( (cntChars != 0) && (_T('\n') != pNewData[cntChars-1]) )
			_tcscat(pNewData, _T(" "));
		_tcscat(pNewData, szData);
		SendMessage(m_hEdit, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)pNewData);
		LocalUnlock(pNewData);
		LocalFree(hNewBlock);
		return;
	}
	DWORD dwNewSize	= LocalSize(hEditCurrent) + 
						(_tcslen(szData)*sizeof(TCHAR)) + sizeof(TCHAR);
	HLOCAL hNewBlock= LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT, dwNewSize);
	TCHAR *pNewData = (TCHAR*)LocalLock(hNewBlock);
	memcpy(pNewData, pOldData, cntChars * sizeof(TCHAR));
	LocalUnlock(hEditCurrent);
	LocalFree(hEditCurrent);
	if ( (cntChars != 0) && (_T('\n') != (pNewData[cntChars-1])) )
		_tcscat(pNewData, _T(" "));
	_tcscat(pNewData, szData);
	LocalUnlock(hNewBlock);

	SendMessage(m_hEdit, EM_SETHANDLE, (WPARAM)(HLOCAL)hNewBlock, 0);
	SendMessage(m_hEdit, EM_SETMODIFY, 1, 0);
}

/* 
- Copy the contents to main(), if its a console application AND window trace is needed.
int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, int nCmdShow ) 
{ 
    MSG msg = {0}; 
 
    while ( WM_QUIT != GetMessage(&msg, (HWND) NULL, 0, 0) ) 
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 

    return msg.wParam; 
} 
*/

int WriteLog(unsigned int level, WCHAR* prefix, WCHAR* format, ... ) {

#ifdef _UNICODE

	va_list		ArgList;
	TCHAR		szOutString[512];


	va_start(ArgList, format);	
	_vstprintf(szOutString,  format, ArgList);	
	va_end(ArgList);


	TRACER.Print(szOutString);
#endif
	return 0;
}


int WriteLog(unsigned int level, char* prefix, char* format, ... ) {

#ifdef TRACER_ENABLED

	va_list		ArgList;
	char		szOutString[512];


	va_start(ArgList, format);	
	vsprintf(szOutString,  format, ArgList);	
	va_end(ArgList);


	TRACER_STREAM(szOutString);
#endif
	return 0;
}


int WriteLog( char* format, ... ) {

#ifdef TRACER_ENABLED

	va_list		ArgList;
	char		szOutString[1512];


	va_start(ArgList, format);	
	vsprintf(szOutString,  format, ArgList);	
	va_end(ArgList);


	TRACER_STREAM(szOutString);
#endif
	return 0;
}


int WriteLog( WCHAR* format, ... ) {

//#ifdef _UNICODE
#ifdef TRACER_ENABLED

	va_list		ArgList;
	WCHAR		szOutStringW[512+2];
	char		szOutString[512+1];

	va_start(ArgList, format);	
	vswprintf(szOutStringW,  format, ArgList);	
	va_end(ArgList);

	WideCharToMultiByte(CP_ACP, 0, szOutStringW , -1, szOutString, 500, NULL, NULL);	
	TRACER_STREAM(szOutString);
//#endif
#endif
	return 0;
}



void set_stat(char* str, int i, char* s2, int i2 ) {

#ifdef TRACER_ENABLED
	char	szStr[712];	

	if (s2) {
		sprintf(szStr, "%s, %X, %s, %X", str, i, s2, i2);
	}
	else {
		sprintf(szStr, "%s, %X", str, i);
	}

	TRACER_STREAM(szStr);

#endif
}




void set_stat(WCHAR* str, int i , WCHAR* s2, int i2) {


	if (s2) {
		TRACER_STREAM( (wchar_t*)str << i  << (wchar_t*)s2 << i2);
	}
	else {
		TRACER_STREAM( (wchar_t*)str << i  );
	}


}




/** получить версию файла
*/
bool get_file_version(LPCTSTR file_name, LPTSTR ver, LPTSTR date)
{
	bool result = false;
	DWORD dwLen= 0, dummy = 0;
    LPCTSTR p = file_name;
    TCHAR file_name_local[MAX_PATH];

    if( !p )
    {
        GetModuleFileName(NULL,file_name_local,MAX_PATH);    
        p = file_name_local;
    }
	if( dwLen = GetFileVersionInfoSize(p, &dummy) )
	{
	  char *pBuf = new char[dwLen];
	  if( GetFileVersionInfo(p ,NULL, dwLen, pBuf) )
	  {
		unsigned int infoLen = 0;
		LPVOID pInfo;
		if( VerQueryValue(pBuf, TEXT("\\"), &pInfo, &infoLen))
		{
			DWORD v = ((VS_FIXEDFILEINFO*)pInfo)->dwFileVersionMS;
			DWORD d = ((VS_FIXEDFILEINFO*)pInfo)->dwFileDateMS;
			DWORD d1 = ((VS_FIXEDFILEINFO*)pInfo)->dwFileDateLS;
			_stprintf(ver, TEXT("%d.%d") ,v>>16,(v<<16)>>16);
			if (date)
				_stprintf(date, TEXT("%d.%d.%d.%d") , HIWORD(d) , LOWORD(d), HIWORD(d1) , LOWORD(d1) );

			result = true;
		}
	  }
	  if( pBuf )
		  delete[] pBuf;
	}
	return result;
}