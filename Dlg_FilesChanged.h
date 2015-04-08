#if !defined(AFX_DLG_FILESCHANGED_H__0BCEB43E_3AE4_40A8_AB04_8875ECE8D055__INCLUDED_)
#define AFX_DLG_FILESCHANGED_H__0BCEB43E_3AE4_40A8_AB04_8875ECE8D055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_FilesChanged.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_FilesChanged dialog

class CDlg_FilesChanged : public CDialog
{
// Construction
public:
	CString _ChagedFileName;


	CDlg_FilesChanged(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_FilesChanged)
	enum { IDD = IDD_DLG_F_CHANGED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_FilesChanged)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_FilesChanged)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_FILESCHANGED_H__0BCEB43E_3AE4_40A8_AB04_8875ECE8D055__INCLUDED_)
