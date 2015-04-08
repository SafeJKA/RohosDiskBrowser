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
** Contact info@rohos.com http://rohos.com
**/


#include "stdafx.h"
#include "DiskBrowserApp.h"
#include "Dlg_DiskInfo.h"
#include "Dlg_HexView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_DiskInfo dialog

CDlg_DiskInfo::CDlg_DiskInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_DiskInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_DiskInfo)
	m_FileName =_T("");
	m_Algorithm = _T("");
	m_DiskSize = _T("");
	m_EncKey = _T("");
	m_iv = _T("");
	m_Letter = _T("");
	m_chbx_readonly = FALSE;
	m_ed8_text = _T("");
	//}}AFX_DATA_INIT
}


void CDlg_DiskInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_DiskInfo)
	DDX_Text(pDX, IDC_EDIT1, m_FileName);
	DDX_Text(pDX, IDC_EDIT2, m_Algorithm);
	DDX_Text(pDX, IDC_EDIT6, m_DiskSize);
	DDX_Text(pDX, IDC_EDIT4, m_EncKey);
	DDX_Text(pDX, IDC_EDIT5, m_iv);
	DDX_Text(pDX, IDC_EDIT3, m_Letter);
	DDX_Text(pDX, IDC_EDIT7, m_FreeSpace);
	DDX_Check(pDX, IDC_CHECK1, m_chbx_readonly);
	DDX_Text(pDX, IDC_EDIT8, m_ed8_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_DiskInfo, CDialog)
	//{{AFX_MSG_MAP(CDlg_DiskInfo)
	//}}AFX_MSG_MAP
//	ON_EN_CHANGE(IDC_EDIT3, &CDlg_DiskInfo::OnEnChangeEdit3)
ON_BN_CLICKED(IDC_BUTTON1, &CDlg_DiskInfo::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_DiskInfo message handlers

void CDlg_DiskInfo::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CDlg_DiskInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText( LS(IDS_CREATE_DISK2_DLG) );
	
	
	//
	SetDlgItemText(IDC_LABEL_FILENAME, LS(IDS_DISK_FOLDER) );
	SetDlgItemText(IDC_EDIT8, LS(IDS_MOUNT_AS_READONLY ) );

	SetDlgItemText(IDC_LABEL_ALGO, LS(IDS_ENCYPT_ALGORITM) );
	SetDlgItemText(IDC_LABEL_DISKNUM, LS(IDS_DRIVE_LETER) );
	SetDlgItemText(IDC_DSIZE, LS(IDS_DISK_SIZE_MB) );
	SetDlgItemText(IDCANCEL, LS(IDS_CANCEL) );
	
	// TODO: Add extra initialization here	

	m_FileName = pKeyInfo->FileName; 
   m_Algorithm = pKeyInfo->AlgorithmName;

   m_Letter.Format("%c:\\ (%s)", pKeyInfo->DiskNumber + 'A', _format);   

   //m_FreeSpace.Format();

	ULARGE_INTEGER ui;
	
	ui.LowPart = pKeyInfo->DiskSizeLow;
	ui.HighPart = pKeyInfo->DiskSizeHigh;

	TCHAR out[100];
	TCHAR str[500]={0}, str1[10];

	StrFormatByteSize64( ui.QuadPart,  out, 100);      
	StrFormatByteSize64( _FSys._disk_total_space.QuadPart,  str, 100);      
	m_DiskSize.Format("%s (%s)", out, str); 
	StrFormatByteSize64(  _FSys._disk_free_space.QuadPart, out, 100 );
	m_FreeSpace = out;

	


	for (int i=0; i<=64; i++){
		sprintf(str1, "0x%X, ", pKeyInfo->IV[i]);
		strcat(str, str1);
	}
     
	m_iv=str; 

     strcpy(str,"");
	for (int i=0; i<=32; i++){
		sprintf(str1, "0x%X, ", pKeyInfo->Key[i]);
		strcat(str, str1);
	}
	
	m_EncKey=str;
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CDlg_DiskInfo::OnEnChangeEdit3()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}

void CDlg_DiskInfo::OnBnClickedButton1()
{
	CDlg_HexView dlg;

	//_tcscpy(Dinf._image_file_name,
	_tcscpy(dlg._image_file_name,   _image_file_name);

	dlg.DoModal();
	
}
