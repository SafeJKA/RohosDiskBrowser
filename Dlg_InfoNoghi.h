#if !defined(AFX_DLG_INFONOGHI_H__F10A09FF_B478_4FE7_A715_43499F6F4647__INCLUDED_)
#define AFX_DLG_INFONOGHI_H__F10A09FF_B478_4FE7_A715_43499F6F4647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_InfoNoghi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_InfoNoghi dialog

class CDlg_InfoNoghi : public CDialog
{
// Construction
public:
	CDlg_InfoNoghi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_InfoNoghi)
	enum { IDD = IDD_INFO_NOGHI };
	CButton	m_ChBoxCaption;
	BOOL	m_cbValue;
	CString	m_LabelInfCaption;
	CString	m_Label2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_InfoNoghi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_InfoNoghi)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedLabel2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_INFONOGHI_H__F10A09FF_B478_4FE7_A715_43499F6F4647__INCLUDED_)
