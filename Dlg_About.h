#if !defined(AFX_DLG_ABOUT_H__9F708F89_BE02_4826_9F6C_E0C9FE7FDA34__INCLUDED_)
#define AFX_DLG_ABOUT_H__9F708F89_BE02_4826_9F6C_E0C9FE7FDA34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_About.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_About dialog

class CDlg_About : public CDialog
{
// Construction
public:
	CDlg_About(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_About)
	enum { IDD = IDD_DLG_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_About)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_About)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_ABOUT_H__9F708F89_BE02_4826_9F6C_E0C9FE7FDA34__INCLUDED_)
