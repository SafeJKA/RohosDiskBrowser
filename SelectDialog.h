/********************************************************************
	created:	2008/07/22
	created:	22:7:2008   10:23
	filename: 	SelectDialog.h
	file base:	SelectDialog
	file ext:	h
	author:		Hojjat Bohlooli - software@tarhafarinin.ir
	
	purpose:	select multiple file and folders together in browse dialog
				free for non commercial uses.
*********************************************************************/
#pragma once

#include <dlgs.h> // for (MULTI)FILEOPENORD

// CSelectDialog
class CSelectDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CSelectDialog)

public:
	CSelectDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
						OFN_EXPLORER & (~OFN_SHOWHELP),
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CSelectDialog();
	
protected:
	virtual void OnInitDone();
	virtual void OnFolderChange();
	virtual BOOL OnFileNameOK();
	static LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	static CString m_strCurrendDirectory;
	static CStringArray m_SelectedItemList;	/*this list includes files and folders
											are selected by user. */
	static WNDPROC m_wndProc;
};
