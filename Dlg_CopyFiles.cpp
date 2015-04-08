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


#include "stdafx.h"
#include "DiskBrowserApp.h"
#include "Dlg_CopyFiles.h"

#include "pub.h"
#include "stdlib.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 
#pragma comment(lib,"comctl32.lib") 

// CDlg_CopyFiles dialog

ULARGE_INTEGER CDlg_CopyFiles::liCurrentPosition = {0, 0};
ULARGE_INTEGER CDlg_CopyFiles::liCurrentFileSize = {0, 0};
bool CDlg_CopyFiles::_terminate = 0;
HANDLE CDlg_CopyFiles::_hWaitHandle=0;

IMPLEMENT_DYNAMIC(CDlg_CopyFiles, CDialog)

CDlg_CopyFiles::CDlg_CopyFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CopyFiles::IDD, pParent)
	, test(0)
{
	_terminate =0;

}

CDlg_CopyFiles::~CDlg_CopyFiles()
{
}

void CDlg_CopyFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_CopyFiles, CDialog)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_CPY_NAME, &CDlg_CopyFiles::OnStnClickedCpyName)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_CPY_PRG, &CDlg_CopyFiles::OnNMCustomdrawCpyPrg)
ON_BN_CLICKED(IDC_BUTTON1, &CDlg_CopyFiles::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CDlg_CopyFiles::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlg_CopyFiles message handlers

BOOL CDlg_CopyFiles::OnInitDialog()
{
	CDialog::OnInitDialog();

	HWND hwnd = ::GetDlgItem(m_hWnd, IDC_CPY_PRG);
	long cs = ::GetWindowLong(hwnd, GWL_STYLE);
	//::SetWindowLong(hwnd, GWL_STYLE, cs | /*PBS_MARQUEE*/ 0x08);

	SetWindowLongPtr(hwnd,GWL_STYLE,cs |0x08); 

	liCurrentPosition.QuadPart = 0;

	//if ( !
	//SendDlgItemMessage(IDC_PROGRESS1, PBM_SETRANGE , 1, (LPARAM) MAKELPARAM (0, 10) );
	//SendDlgItemMessage(IDC_PROGRESS1, PBM_SETPOS , 9, 0 );
	::SendMessage(hwnd , /*PBM_SETMARQUEE*/(WM_USER+10) , (WPARAM)true, (LPARAM) 40 ) ;
	//	) 

	GetPodNogamiPath(_curr_folder, false ); 

	if (_filesListIn.GetCount() > 0 )
	{
		CWinThread *pWorkerThread1 = AfxBeginThread( ImportThreadProc, this,   THREAD_PRIORITY_NORMAL, 0, 0 );
		_hWaitHandle = pWorkerThread1->m_hThread;

		_currentFile = _filesListIn[0];
		SetDlgItemText(IDC_CPY_NAME, _currentFile);

		SetTimer(1, 300, NULL); // 		

		return TRUE;
	}

	if (_filesListOut.GetCount() > 0 )
	{
		CWinThread *pWorkerThread1 = AfxBeginThread( ExportThreadProc, this,   THREAD_PRIORITY_NORMAL, 0, 0 );
		_hWaitHandle = pWorkerThread1->m_hThread;

		_currentFile = _filesListOut[0];
		SetDlgItemText(IDC_CPY_NAME, _currentFile);

		SetTimer(1, 300, NULL); // 
	}

	if (_filesListVirtualize.GetCount() > 0 )
	{
		//CWinThread *pWorkerThread1 = AfxBeginThread( ExportThreadProc, this,   THREAD_PRIORITY_NORMAL, 0, 0 );
		//_hWaitHandle = pWorkerThread1->m_hThread;

		_currentFile = _filesListVirtualize[0];
		SetDlgItemText(IDC_CPY_NAME, _currentFile);

		ExportThreadProc((void*)this);		

		SetTimer(1, 300, NULL); // 
	}


	if (_filesListIn.GetCount() == 0 &&  _filesListOut.GetCount() ==0 && _filesListVirtualize.GetCount() ==0)
	{
		SetDlgItemText(IDC_CPY_NAME, "No files ...");
	}
	


	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// path - were to save in disk image, 
// filePath - file path to import
bool CDlg_CopyFiles::importFile(CFsys* fs, LPCTSTR path, LPCTSTR filePath)
{
	TCHAR str1[500];
	strcpy(str1, path);
	strcat(str1, "/");

	_currentFile = filePath;	

	CFileStatus st;
	CFile::GetStatus( filePath , st);
	//st.

	TCHAR fname[300], fext[50];
	_splitpath(filePath, 0, 0, fname, fext);

	/*LPCTSTR fname = strrchr(filePath, '\\');
	if (fname)
	{
		fname++;
		strcat(str1, fname);
	}*/
	strcat(str1, fname);
	strcat(str1, fext);

	//::MessageBox(0, filePath, "1", 0);
	//::MessageBox(0, str1, "2", 0);
	liCurrentPosition.QuadPart =0;

	if( fs->LoadFile(filePath, str1,(DLGPROC)CallBack_Proc)==-1)
	{
		WriteLog("Error import %s %s", filePath, str1);
		_filesWithError.Add(filePath);
		return false;
	}

	liCurrentPosition.QuadPart =0;

	return true;

}

bool CDlg_CopyFiles::importFolder(CFsys* fs, LPCTSTR path, LPCTSTR folderPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;	
	
	TCHAR path1[500];
	TCHAR in_path1[500];

	// create folder name 

	LPCTSTR fname = _tcsrchr(folderPath, '\\');
	if (fname)
	{
		fname += _tclen( fname);
		if ( fs->CreateDirectoryA(path, fname) == 0)
		{
			WriteLog("Error creating %s", fname);
			_filesWithError.Add(folderPath);
			return false;
		}

		_tcscpy(in_path1, path);
		_tcscat(in_path1, "/");					
		_tcscat(in_path1, fname);					
		
	} else 
		return false;
	
	_tcscpy(path1, folderPath);
	_tcscat(path1, "\\*.*");		
	
	hFind = FindFirstFile(path1, &FindFileData);	
	if (hFind == INVALID_HANDLE_VALUE) 		
		return false;	    
	
	do {		

		if ( !strcmp(FindFileData.cFileName, ".")  || !strcmp(FindFileData.cFileName, "..") )
			continue;

		_tcscpy(path1, folderPath);
		_tcscat(path1, "\\");		
		_tcscat(path1, FindFileData.cFileName);		

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)		
			importFolder(fs, in_path1, path1);			
		 else
			importFile(fs, in_path1, path1);				
		
	} while ( FindNextFile(hFind, &FindFileData) );

	FindClose(hFind);

	return true;
	
}

/*
int cntFolder(LPCTSTR folderPath, int files_cnt, int if_zero_stop );
{
	if ( --if_zero_stop == 0)
		if_zero_stop 


}*/


UINT CDlg_CopyFiles::ImportThreadProc(LPVOID p)
{
	CDlg_CopyFiles* dlg = (CDlg_CopyFiles*)p;
	CFileStatus st;

	// посчитать сколько файлов.

	/*int cntFiles =0;
	for (int i =0; i<dlg->_filesListIn.GetSize(); i++ )
	{		
		CFile::GetStatus( dlg->_filesListIn[i] , st);

		if ( st.m_attribute & CFile::directory)
			cntFiles += cntFolder(dlg->_filesListIn[i], 3 );
		else
			cntFiles += 1;

	}*/


	for (int i =0; i<dlg->_filesListIn.GetSize(); i++ )
	{
		if (dlg->_terminate)
			break;
		
		CFile::GetStatus( dlg->_filesListIn[i] , st);

		if ( st.m_attribute & CFile::directory)
			dlg->importFolder(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListIn[i] );
		else
			dlg->importFile(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListIn[i] );

	}

	
	dlg->_hWaitHandle = 0;
	AfxEndThread(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// path - were to save, 
// filePath - file path in disk image 
bool CDlg_CopyFiles::exportFile(CFsys* fs, LPCTSTR path, LPCTSTR filePath)
{
	TCHAR str1[500];
	strcpy(str1, path);
	strcat(str1, "\\");

	_currentFile = filePath;	

	LPCTSTR fname = _tcsrchr(filePath, PATH_SEP);
	if (fname)
		fname++;
	else 
		fname = filePath;
			
	strcat(str1, fname);	

	liCurrentPosition.QuadPart =0;

	if ( fs->SaveFile(filePath, str1, 0, 0, (DLGPROC)CallBack_Proc) != 0 )
	{
		WriteLog("Error export %s", filePath);
		_filesWithError.Add(filePath);
		return false;
	}

	liCurrentPosition.QuadPart =0;

	return true;
}

bool CDlg_CopyFiles::exportFolder(CFsys* fs, LPCTSTR path, LPCTSTR folderPath)
{
	TCHAR path1[500];
	TCHAR in_path1[500];

	// create folder name 
	LPCTSTR fname = strrchr(folderPath, PATH_SEP);
	if (!fname)
		fname = folderPath;
	else 	
		fname++;

	_tcscpy(in_path1, path);
	_tcscat(in_path1, "\\");					
	_tcscat(in_path1, fname);

	if ( CreateDirectory(in_path1, NULL) == 0)
	{
		WriteLog("Error creating %s", in_path1);
		_filesWithError.Add(folderPath);
		return false;
	}

	FS_FF ff={0};
	fs->BrowsePath(folderPath, &ff);

	// exporting Folders
	for(int i=0;i<ff.fd_c;i++)
	{
		_tcscpy(path1, folderPath);
		_tcscat(path1, PATH_SEP1);		
		_tcscat(path1, ff.fld[i].ans_name);		

		//if ( fs->isDirectory(path1) )
		exportFolder(fs,in_path1, path1);
		//else	exportFile(fs, in_path1, path1);

	}


	// exporting Files
	for(int i=0;i<ff.fl_c;i++)
	{
		_tcscpy(path1, folderPath);
		_tcscat(path1, PATH_SEP1);		
		_tcscat(path1, ff.fle[i].ans_name);		

		//if ( fs->isDirectory(path1) )
		//exportFolder(fs,in_path1, path1);
		//else	
		exportFile(fs, in_path1, path1);

	}


	return true;
}

/** virtualize _filesListVirtualize
*/
UINT CDlg_CopyFiles::ExportThreadProc(LPVOID p)
{
	CDlg_CopyFiles* dlg = (CDlg_CopyFiles*)p;
	CFileStatus st;

	// virtualize file list command
	if (dlg->_filesListVirtualize.GetSize())
	{
		TCHAR filename[500] = "";
		TCHAR virtFileName[500] = "";
		TCHAR virtPath[500] = "";
		
		for (int i =0; i<dlg->_filesListVirtualize.GetSize(); i++ )
		{
			if (dlg->_terminate)
				break;

			dlg->_currentFile = (LPCTSTR)dlg->_filesListVirtualize[i];	

			/*bool isDir = dlg->_fsys->isDirectory((char*) (LPCTSTR)dlg->_filesListVirtualize[i]);			

			if ( isDir)
			{
				//dlg->exportFolder(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListVirtualize[i] );
			}
			else */

			{
				//dlg->exportFile(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListVirtualize[i] );

				strcpy(virtFileName, "c:\\");
				virtFileName[0] = dlg->_curr_folder[0];
				strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));
				strcat(virtFileName, "\\");

				LPCTSTR spath = dlg->_filesListVirtualize[i];

				LPCTSTR spath2 = spath; //_tcsrchr(spath, PATH_SEP);
				if (spath2[0]==PATH_SEP)
					spath2++;
				//else
				//	filename  = spath;	
				
				strcat(virtFileName, spath2);
				//AfxMessageBox(virtFileName);

				switch (_FSys.isDirectory(spath))
				{
				case 1: // folder
					if ( !dlg->_fsys->CreateVirtualFolder(spath) )
						dlg->_filesWithError.Add(spath);
					break;
				case 0: // file
					if ( !dlg->_fsys->CreateVirtualFile(spath, virtFileName) )
						dlg->_filesWithError.Add(spath);
					break;
				}

				
			}

		}

		dlg->_hWaitHandle = 0;
		dlg->_filesListVirtualize.RemoveAll(); 

		return TRUE;
	}


	// export command

	for (int i =0; i<dlg->_filesListOut.GetSize(); i++ )
	{
		if (dlg->_terminate)
			break;
		
		bool isDir = dlg->_fsys->isDirectory((char*) (LPCTSTR)dlg->_filesListOut[i]);

		if ( isDir)
			dlg->exportFolder(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListOut[i] );
		else
			dlg->exportFile(dlg->_fsys, (LPCTSTR)dlg->_path, dlg->_filesListOut[i] );

	}


	dlg->_hWaitHandle = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CopyFiles::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	KillTimer(1);
	SetTimer(1, 400, NULL);

	if ( _terminate == 0)
	{
		SetDlgItemText(IDC_CPY_NAME, _currentFile);
		SendDlgItemMessage(IDC_CPY_PRG, PBM_SETSTEP, 1, 0);

		if ( liCurrentPosition.QuadPart )
		{
			TCHAR str[50];
			StrFormatByteSize64( liCurrentPosition.QuadPart,  str, 40);      
			SetDlgItemText(IDC_STATIC1, str);
		}

	}

	switch( WaitForSingleObject( _hWaitHandle, 0 ) ) // INFINITE
	{
		case WAIT_OBJECT_0:
		case WAIT_FAILED:
			//WriteLog("CDlg_Wait::OnTimer, object signaled");
			_hWaitHandle = 0;
			KillTimer(1);			

			HWND hwnd = ::GetDlgItem(m_hWnd, IDC_CPY_PRG);
			::SendMessage(hwnd , /*PBM_SETMARQUEE*/(WM_USER+10) , (WPARAM)false, (LPARAM) 40 ) ;
			SendDlgItemMessage(IDC_CPY_PRG, PBM_SETSTEP, 0, 0);

			if (_filesWithError.GetSize() )
			{
				GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_SHOW);
				SetDlgItemText(IDC_CPY_NAME, "An error occured with some files. Click Report... or Close.");
				return;
			}
			OnOK();
			return;
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlg_CopyFiles::OnStnClickedCpyName()
{
	// TODO: Add your control notification handler code here
}

//void CDlg_CopyFiles::OnNMCustomdrawCpyPrg(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CDlg_CopyFiles::OnBnClickedButton1()
{
	if (_hWaitHandle)
	{
		_terminate =1;
		SetDlgItemText(IDC_CPY_NAME, "Please wait...");
	}
	else 
		EndDialog(IDOK);
	
	// TODO: Add your control notification handler code here
}

void CDlg_CopyFiles::OnBnClickedButton2()
{
	CString s1;
	for (int i =0; i<_filesWithError.GetSize(); i++)
	{
		s1 += _filesWithError[i]; 
		s1 += "\n";

		if (i>15)
		{
			s1 += "\n ... \n";
			break;
		}
	}

	AfxMessageBox(s1);

	// TODO: Add your control notification handler code here
}

/** called by Cfsys::

wparam PLARGE_INTEGER - total size fo the processed item
lParam PLARGE_INTEGER - current posiiton of the item

*/
int CALLBACK CDlg_CopyFiles::CallBack_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	liCurrentPosition.QuadPart = ((PLARGE_INTEGER)lParam)->QuadPart;
	liCurrentFileSize.QuadPart = ((PLARGE_INTEGER)wParam)->QuadPart;

	if (_terminate)
	{
		_hWaitHandle =0;
		return 1;
	}

	return 0;

	
}

