
// TraceMaker.h: implementation of the CTraceMaker class.
// Rajeev Sadasivan, Trivandrum, INDIA
// 22/Nov/2002
//////////////////////////////////////////////////////////////////////

/*	-------------------------------- U S A G E --------------------------------
0. #define TRACER_ENABLED, if trace to be active.

1. TRACE_INIT (trace file name, maximum file size,  
		trace target, PerProcessTraceWnd)
	First is the trace file path. Second parameter is maximum size 
	that file can have. Default Values are "_TraceLog.txt" and 1 MB.
	Just specify 2,3,4 etc. Maximum size is 10 MB. Modify it if needed.
	If that file size is crossed, existing log file will be deleted.
	Output type can also be specified. Default is File Only.
	Options are 
	FILE_ONLY, WINDOW_ONLY, FILE_AND_WINDOW; If window is enabled,
	an output trace window will be displayed.
	if PerProcessTraceWnd is false, there will be only ONE window per system;
	Otherwise each trace window will be spawned per process.

2. TRACE_FUNCTION_IN (function name)
	Log ENTER & EXIT of a function. EXIT logging is automatically done.
	This will log "Entering function name" and after the called function
	exit, it will log "Leaving function name". This should be the FIRST 
	line of a function. Function EXIT logging is done automaticaaly.
	NO CODE is needed for logging "Leaving functinon name". NEWLINE inserted.

3. TRACER_STREAM(" Value = " << nValue << "\r\n")
	This can have same format as cout << "Value is << nValue << "\r\n";
	e.g. TRACER_STREAM(_T("Value = ) << nNvalue << "Path = %s"<< szPath << "\r\n")
	NEWLINE(\r\n) not inserted. if "\r\n" is needed, use << "\r\n"

These data types are supported. 
int, unsigned int, short, double, float, bool, byte, char, w_char, char*, w_char*.
	
This 3 macros execution can be controlled by defining TRACER_ENABLED.
If the TRACER_ENABLED is defined, TRACE will be outputted. 
If NOT, the macros do nothing.

Functions: If TRACER_ENABLED NOT defined, this won't do anything.
5. TRACER.Print(traceType, traceString);
	This can have same format as printf. NewLine(\r\n) will be inserted automatically.
	e.g. TRACER.Print(TRACER_OPE_PROGRESS, _T("Value = %d, Path = %s"), 
	nNvalue, szPath);
	If the TraceType is used, type of tracelog can be easily distinguished. 
	HERE type of logging is also added to beginning of trace line.
	TRACER_OPE_PROGRESS		_T("** PROGRESS:")
	TRACER_OPE_ERROR		_T("## ERROR   :")
	TRACER_OPE_EXCEPTION    _T("@@ EXCEPT  :")
	TRACER_OPE_MESSAGE		_T("$$ MESSAGE :") ; 

6. TRACER.Print(traceString);
	This can have same format as printf. NewLine(\r\n) will be inserted automatically.
	e.g. TRACER.Print(_T("Value = %d, Path = %s"), nNvalue, szPath);
	There is NO trace type identifier in string.
-------------------------------- U S A G E -------------------------------- */

#ifndef _TRACE_MAKER_H

#define _TRACE_MAKER_H

#define TRACER_DEFAULT_FILE		_T("_TraceLog.txt")
#define TRACER_MAX_BUFFER		1024
#define TRACER_NUMSIZE			24
#define TRACER_DEFAULT_SIZE		1048576

typedef enum enumTraceFileSize
{
	T0_MB = (500000),
	T1_MB = (1048576),
	T2_MB = (2 * 1048576),
	T3_MB = (3 * 1048576),
	T4_MB = (4 * 1048576),
	T5_MB = (5 * 1048576),
	T6_MB = (6 * 1048576),
	T7_MB = (7 * 1048576),
	T8_MB = (8 * 1048576),
	T9_MB = (9 * 1048576),
	T10_MB = (10 * 1048576)
} TraceFileSize;

typedef enum enumTraceTarget
{
	FILE_ONLY = 0, 
	WINDOW_ONLY, 
	FILE_AND_WINDOW
} TraceTarget;

#define TRACER_OPE_PROGRESS		_T("** PROGRS: ")// To indicate progress of operation.
#define TRACER_OPE_ERROR		_T("## ERROR : ")// To indicate Error is happened.
#define TRACER_OPE_EXCEPTION    _T("@@ EXCEPT: ")// To indicate exceptions.
#define TRACER_OPE_MESSAGE		_T("$$ MSG   : ")// To indicate message display.

class CTraceMaker  
{
public:		// Methods.		
	void Print(LPCTSTR szData, ...);
	void Print(LPCTSTR szType, LPCTSTR szData, ...);
	void Initialize(LPCTSTR pszFile, DWORD dwSizeKb = 200/*Kb*/, 
			TraceTarget target= FILE_ONLY, bool bWndPerProcess=false);
	virtual ~CTraceMaker();
	CTraceMaker operator << (int nValue);
	CTraceMaker operator << (unsigned int unValue);
	CTraceMaker operator << (short sValue);
	CTraceMaker operator << (float fValue);
	CTraceMaker operator << (double dfValue);
	CTraceMaker operator << (bool bValue);
	CTraceMaker operator << (char cValue);
	CTraceMaker operator << (unsigned char byValue);
	CTraceMaker operator << (wchar_t wcValue);
	CTraceMaker operator << (char *pszValue);
	CTraceMaker operator << (wchar_t *pszValue);

	BOOL m_enabled;

public:		// Static methods.
	static void Release();	
	static CTraceMaker* GetTracer();
	static LRESULT CALLBACK TracerWndProc( HWND, UINT, WPARAM, LPARAM );

private:	// Methods.
	CTraceMaker();
	void LogFile(LPCSTR szData);
	void LogFile(LPCWSTR szData);
	bool InitiateWindow();
	void RegisterWindow(HINSTANCE hInst);
	void PutTexttoWindow(LPCTSTR szData);

private:	// Static Data.	
	static HWND m_hEdit;
	static CTraceMaker *m_psTraceMaker;	

private:	// Data.
	bool m_bWndPerProcess;
	HFONT m_hEditFont;
	TCHAR m_szFile[MAX_PATH];
	//TraceFileSize m_enumSize;
	DWORD	m_MaxFileSizeKb;
	TraceTarget m_enumTarget;
};

// The class 'CTraceFunction' is for internal use only.
class CTraceFunction
{
	TCHAR m_szFunction[TRACER_MAX_BUFFER];
public:
	CTraceFunction(LPCTSTR szFunction);
	~CTraceFunction();
};

#define TRACER (*(CTraceMaker::GetTracer()))

#ifdef TRACER_ENABLED
	#define TRACE_INIT(FilePath, MaxFileSizeInMB, traceTarget, WndOption) \
			TRACER.Initialize(FilePath, MaxFileSizeInMB, traceTarget, WndOption);
	#define TRACE_FUNCTION_IN(Function) CTraceFunction \
									fnTrace(Function);
	#define TRACER_STREAM(arg) ( TRACER << arg );
#else
	#define TRACE_INIT(FilePath, MaxFileSizeInMB, traceTarget, WndOption)   
	#define TRACE_FUNCTION_IN(arg)    
	#define TRACER_STREAM(arg)  
#endif // TRACER_ENABLED


int WriteLog(unsigned int level, WCHAR* prefix, WCHAR* format, ... );
int WriteLog(unsigned int level, char* prefix, char* format, ... );
int WriteLog(char* format, ... );
int WriteLog(WCHAR* format, ... );



void set_stat(char* str, int i=0, char* s2=NULL, int i2=0 );
void set_stat(WCHAR* str, int i=0, WCHAR* s2=NULL, int i2=0 );
bool get_file_version(LPCTSTR file_name, LPTSTR ver, LPTSTR date);



#endif // _TRACE_MAKER_H
