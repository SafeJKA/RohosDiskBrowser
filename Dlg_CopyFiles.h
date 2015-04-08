#pragma once

#include "pub.h"

// CDlg_CopyFiles dialog

class CDlg_CopyFiles : public CDialog
{
	DECLARE_DYNAMIC(CDlg_CopyFiles)

	static UINT ImportThreadProc(LPVOID p);
	static UINT ExportThreadProc(LPVOID p);
	static ULARGE_INTEGER  liCurrentPosition;
	static ULARGE_INTEGER  liCurrentFileSize;

	bool importFile(CFsys* fs, LPCTSTR path, LPCTSTR filePath);
	bool importFolder(CFsys* fs, LPCTSTR path, LPCTSTR folderPath);

	bool exportFile(CFsys* fs, LPCTSTR path, LPCTSTR filePath);
	bool exportFolder(CFsys* fs, LPCTSTR path, LPCTSTR folderPath);

	static bool _terminate; // = 1 if need to stop thread
	TCHAR _curr_folder[500];

public:
	CDlg_CopyFiles(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_CopyFiles();

	CFsys *_fsys; // mounted file system

	CStringArray _filesListIn; // files to be imported into _fsys
	CStringArray _filesListOut; // files to be exported from _fsys
	CStringArray _filesListVirtualize; // files to be virtualized
	CString _path; // Import/Export path

	CStringArray _filesWithError; // files to be exported from _fsys

	CString _currentFile;

	static HANDLE _hWaitHandle;
	

// Dialog Data
	enum { IDD = IDD_COPY };

	static int CALLBACK CallBack_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnStnClickedCpyName();
public:
//	afx_msg void OnNMCustomdrawCpyPrg(NMHDR *pNMHDR, LRESULT *pResult);
public:
	// //
	int test;
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton2();
};
