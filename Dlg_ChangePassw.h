#pragma once


// CDlg_ChangePassw dialog

class CDlg_ChangePassw : public CDialog
{
	DECLARE_DYNAMIC(CDlg_ChangePassw)

public:
	CDlg_ChangePassw(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_ChangePassw();

// Dialog Data
	enum { IDD = IDD_CHANGE_PASSW1 };


	BYTE file_blob[5032];		///< блоб диска из файл для прверки пароля.
	DWORD file_blob_size;

	TCHAR _passw[100];
	TCHAR _new_passw[100];
	TCHAR _passw_confirmation[100];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();


	bool VerifyPassword(LPBYTE pBlob, DWORD blob_size, LPCTSTR passwd);
public:
	virtual BOOL OnInitDialog();
};
