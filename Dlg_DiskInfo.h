#if !defined(AFX_DLG_DISKINFO_H__C26AC2A0_17F2_491B_81E3_DF2BE19295F3__INCLUDED_)
#define AFX_DLG_DISKINFO_H__C26AC2A0_17F2_491B_81E3_DF2BE19295F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_DiskInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_DiskInfo dialog

class CDlg_DiskInfo : public CDialog
{
// Construction
public:
	CDlg_DiskInfo(CWnd* pParent = NULL);   // standard constructor
	DISK_BLOB_INFO *pKeyInfo;

// Dialog Data
	//{{AFX_DATA(CDlg_DiskInfo)
	enum { IDD = IDD_DLG_PARTINFO2 };
	CString	m_FileName;
	CString	m_Algorithm;
	CString	m_DiskSize;
	CString	m_EncKey;
	CString	m_iv;
	CString	m_Letter;
	CString	m_FreeSpace;
	BOOL	m_chbx_readonly;
	CString	m_ed8_text;
	//}}AFX_DATA

	CString _format;

	TCHAR _image_file_name[500];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_DiskInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_DiskInfo)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnEnChangeEdit3();
public:
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_DISKINFO_H__C26AC2A0_17F2_491B_81E3_DF2BE19295F3__INCLUDED_)
