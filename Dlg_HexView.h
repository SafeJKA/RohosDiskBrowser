#pragma once

#include "PPDumpCtrl.h"

// CDlg_HexView dialog

class CDlg_HexView : public CDialog
{
	DECLARE_DYNAMIC(CDlg_HexView)

public:
	CDlg_HexView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_HexView();

	CPPDumpCtrl* m_ctlHexEdit2;	
	BOOL m_bStyle [19];
	void SetNewStyle();

	bool Read_DisplayData(__int64 offset);
	TCHAR _image_file_name[100];
	BYTE buff[15009];

// Dialog Data
	enum { IDD = IDD_HEXVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
public:
	int m_offset;
};
