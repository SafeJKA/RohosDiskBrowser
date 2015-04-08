#if !defined(AFX_DLG_PASSWORD_H__3199052C_131C_4EB8_9AD3_0016B1A9457F__INCLUDED_)
#define AFX_DLG_PASSWORD_H__3199052C_131C_4EB8_9AD3_0016B1A9457F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Password.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Password dialog

class CDlg_Password : public CDialog
{
// Construction
public:
	void AddPossibleDiskImage(LPCTSTR imagepath);
	void AddPossibleDiskImageAtBegin(LPCTSTR imagepath);
	CDlg_Password(CWnd* pParent = NULL);   // standard constructor

	CStringArray _possible_disk_images_arr;	///<the list of the disk container files that we could turn on.

	CString	m_FullPath;

// Dialog Data
	//{{AFX_DATA(CDlg_Password)
	enum { IDD = IDD_DLG_PASSW };
	CString	m_password;	
	CString	m_lbEnterPassword;
	CString	m_lbImagePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Password)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CFileDialog *myFileDialog;
	CString m_vk_passw; ///<	 virtual keyboard passwrod here

	// Generated message map functions
	//{{AFX_MSG(CDlg_Password)
	virtual void OnOK();
	afx_msg void OnVirtualKey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeCombo1();
public:
	afx_msg void OnBnClickedBrowseFile();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PASSWORD_H__3199052C_131C_4EB8_9AD3_0016B1A9457F__INCLUDED_)
