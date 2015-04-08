#pragma once


// CDlg_CheckDsk dialog

class CDlg_CheckDsk : public CDialog
{
	DECLARE_DYNAMIC(CDlg_CheckDsk)

public:
	CDlg_CheckDsk(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_CheckDsk();

// Dialog Data
	enum { IDD = IDD_CHK_DSK };
	LARGE_INTEGER _disk_size;
	int disk_data_offset_in_mb;

	int m_FixErrors, m_SectorScan;
	HANDLE _hImageFilehandle; // disk continer file handle;
	BOOL _image_readonly;
	WCHAR _filesystem[10];
	BOOL _re_formated;
	PVOID pDiskPartitionClass;	///< if not NULL - read from %physicaldrive1%  device by pDiskPartitionClass
	BOOL encrypted;				///<	 TRUE this is encrypted partition.	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
public:
//	afx_msg void OnBnClickedCheck1();
public:
	afx_msg void OnBnClickedOk2();
};
