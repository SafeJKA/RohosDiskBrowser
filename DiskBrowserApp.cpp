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
** Contact info@rohos.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "pub.h"
#include "DiskBrowserApp.h"
#include "DiskBrowserDlg.h"

#include "common1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// b

BEGIN_MESSAGE_MAP(CDiskBrowserApp, CWinApp)
	//{{AFX_MSG_MAP(CDiskBrowserApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskBrowserApp construction

CDiskBrowserApp::CDiskBrowserApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDiskBrowserApp object

CDiskBrowserApp theApp;
HINSTANCE h_lang;
CFsys _FSys;// Для работы с файловой системой


/////////////////////////////////////////////////////////////////////////////
// CDiskBrowserApp initialization

BOOL CDiskBrowserApp::InitInstance()
{
	TCHAR path[600], rohospath[700];
	GetPodNogamiPath(path, false );

	TCHAR *new_app_name = new char[100];
	strcpy(new_app_name, LS(IDS_ROHOS_DISK_BROWSER) );
	m_pszAppName = new_app_name;	
	// создать Log файл 
	/*ReadReg(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Rohos"), _T("RohosPath"), path, 400 );
	if ( 0 == _tcslen(path) ) 
		_tcscpy(path, TEXT("rohos_dv.log") );
	else {		
		_tcscat(path, TEXT("rohos_dv.log") );
	}*/

	
	ReadReg(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Rohos"), _T("RohosPath"), rohospath, 500);
	//if ( StrStrI(path, rohospath)  ) 
	{ // creates log only if we run from c:\\program files\\rohos

		//_tcscat(path, TEXT("rohos_dv.log") );
		//TRACE_INIT( path, 300/*Kb*/, FILE_ONLY, true);
	}

	//WriteLog("00");	

	AfxEnableControlContainer();

	//m_pszAppName = new TCHAR[200];
	//_tcscpy((LPTSTR)m_pszAppName, "Rohos Disk Viewer");

	//WriteLog("11");	

	h_lang = m_hInstance;

	InitCommonControls();
	
	h_acl = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	CDlg_DiskBrowserMain dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CDiskBrowserApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	
	
	if(code < 0)
		CWinApp::ProcessMessageFilter(code, lpMsg);

	if(m_wnd && h_acl)
	{
		if(::TranslateAccelerator(m_wnd->m_hWnd, h_acl, lpMsg))
			return(TRUE);
	}

	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
