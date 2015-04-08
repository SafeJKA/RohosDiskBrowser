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


// CDiskBrowserApp.h : header file
//

#if !defined(AFX_TL_NTFSREADERDLG_H__59208DD4_DE0C_403E_B510_7AE2ACC65C2C__INCLUDED_)
#define AFX_TL_NTFSREADERDLG_H__59208DD4_DE0C_403E_B510_7AE2ACC65C2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CDlg_DiskBrowserMain dialog

class CDlg_DiskBrowserMain : public CDialog
{
	/** данные структуры DISK_BLOB_INFO 
	*/
	typedef struct
	{
		DWORD   cbData;
		BYTE    *pbData;
	} DISK_BLOB;

	DISK_BLOB_INFO *pKeyInfo; // disk header decrypted from disk container file (RDI)

// Construction
public:
	void WriteDefaultLangFile();
	CString _cmdline_image;
	TCHAR _OpenedDiskName[500];
	bool _ImageOpened;
	
	int _EvaluationStatus;// Состояние программы
				// 0 = Не установленно
				// 1 = Not registered(using <30 days)
				// 2 = Not Registered(using >30 days) 
				// 3 = Registered
	

	
	        CDlg_DiskBrowserMain(CWnd* pParent = NULL);	// standard constructor

	void    AskForDeleting();
	void    onDiskInfo();	
	void    OnCreateFolder();
	void    OnDeleteFile();
	void    OnAbout();
	void    OnImportFile();
	//bool    ShowNoghiInfoIfNeeded();
	bool    EnoughSpace(__int64 FileSize);
	int     GetLVItemByXY(POINT xyPoint);
	__int64 MyGetDiskFreeSpaceEx(LPCSTR pszDrive);
	LPCTSTR  GetFileNameFromPath(LPCTSTR in);
	CString GetFileNameOfOpenedImage();
	CString GetFileNameFromFullPath(CString in_str);
	CString GetAplicationDirectory(DWORD in_flag);
	CString GetMessage(unsigned long in_msg);

	


// Dialog Data
	//{{AFX_DATA(CDlg_DiskBrowserMain)
	enum { IDD = IDD_TL_NTFSREADER_DIALOG };
	CStatic	m_TreeControl;
	CTreeCtrl			ff_tree;
	CListCtrl			ff_list;
	CComboBox		cbox_FavoritesList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_DiskBrowserMain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool setBlob(LPBYTE pBlob, DWORD blob_size, LPCTSTR passwd);
	BOOL GetFileBlob(LPCTSTR FileName);
	BOOL GetFileBlob_New(LPCTSTR FileName);
	
	BOOL AddDiskBlobToContainer(LPCTSTR containerName, LPBYTE blob1, DWORD blob_size1);
	void AddPossibleDiskImages(CDialog *dlg);
	void OnOpenDisk();
	bool FindBlob_CheckPassword(LPCTSTR containerName, LPCTSTR password);
	static UINT ShowFileDialogThread(LPVOID p);
	CWinThread *pWorkerThread1;
	static TCHAR _lastVirtualFolder_Path[1000]; // path to show in ShowFileDialogThread


	HICON m_hIcon;

	DISK_BLOB blob;
	DISK_BLOB *pDiskBlob;

	BYTE file_blob[5032];		///< блоб диска из файл 
	DWORD file_blob_size;

	CMenu contextMenu2; ///< context menu on empty space;

	TCHAR edited_item_text[MAX_PATH];
	TCHAR _current_path[MAX_PATH*3];

	BOOL _traveler_mode;	///< TRUE - if it runs from f:\ 
	TCHAR _traveler_usb_drive[10]; ///< f:\


	// Generated message map functions
	//{{AFX_MSG(CDlg_DiskBrowserMain)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClickLV(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnToolBarToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnBegindragList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void OnNewDisk(void);
public:
	afx_msg void OnPartitionNew();
public:
	afx_msg void OnUpdateMenu(CCmdUI *pCmdUI);
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
public:
	afx_msg void OnUpdateMenuFile(CCmdUI *pCmdUI);
public:
	afx_msg void OnCheckUpdates();
public:
	int OnOpenAs(void);
public:
	afx_msg void OnAVirtualfolder();
public:
	afx_msg void OnPartitionRepair();
public:
	afx_msg void OnChangePassword();
public:
	afx_msg void OnLvnEndlabeleditList(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnLvnBeginlabeleditList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnFileRename();
public:
	afx_msg void OnNewfolder();
public:
	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	afx_msg void OnOpenAndPreventDataLeak();
public:
	afx_msg void OnOpenandVirtualizeFoldertree();
	void PrepareListView_columns(void);
	

	afx_msg void OnBnClickedButtonAddFavorites();
	afx_msg void OnCbnSelchangePath();
public:
	void _detect_traveler_mode(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TL_NTFSREADERDLG_H__59208DD4_DE0C_403E_B510_7AE2ACC65C2C__INCLUDED_)
