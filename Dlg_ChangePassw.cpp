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
#include "Dlg_ChangePassw.h"


// CDlg_ChangePassw dialog

IMPLEMENT_DYNAMIC(CDlg_ChangePassw, CDialog)

CDlg_ChangePassw::CDlg_ChangePassw(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ChangePassw::IDD, pParent)
{

}

CDlg_ChangePassw::~CDlg_ChangePassw()
{
}

void CDlg_ChangePassw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_ChangePassw, CDialog)
	ON_BN_CLICKED(IDOK, &CDlg_ChangePassw::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlg_ChangePassw message handlers

bool CDlg_ChangePassw::VerifyPassword(LPBYTE pBlob, DWORD blob_size, LPCTSTR passwd)
{
	
	BYTE buff[10000];
	DWORD  buff_size = blob_size;
	memcpy(buff, pBlob, blob_size);
	

	if (blob_size ==0 && _tcslen(passwd)==0 ) // opening non-encrypted , plain disk from YSB drive
		return false;

	if (blob_size ==0 ) // opening encrypted drive, (but without Admin rights )
		return false;

	BYTE key[KEY_SIZE+16] = "";
	BYTE iv[IV_SIZE+16] = "";
		
	memset( iv, 0, IV_SIZE );
	memset( key, 0, KEY_SIZE );

	DeriveKeyFromPassw_v2(passwd, key);	
	if ( !DecryptBuffer( buff, &buff_size, key, iv ) ) 
	{		
		//encryption_mode==1
		DeriveKeyFromPassw(passwd, key);			
		memcpy(buff, pBlob, blob_size);
		if ( !DecryptBuffer( buff, &buff_size, key, iv ) ) 
		{		
		
			return false;		
		}
	}
	return true;
}

void CDlg_ChangePassw::OnBnClickedOk()
{
		GetDlgItemText(IDC_EDIT1,	_passw, 90);
	GetDlgItemText(IDC_EDIT2,	_new_passw, 90);
	GetDlgItemText(IDC_EDIT3,	_passw_confirmation, 90);

	
	if (VerifyPassword(file_blob, file_blob_size, _passw ) == false)
	{
		AfxMessageBox( LS(IDS_WRONG_DISK_PASSW) );
		return;
	}

	if ( _tcscmp(_new_passw, _passw_confirmation) )
	{
		AfxMessageBox( LS(IDS_PASW_NOT_MATCH) );
		return;
	}

	OnOK();

}

BOOL CDlg_ChangePassw::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( LS(IDS_DISK_PASSW_CHANGE_DLG) );
	SetDlgItemText(IDC_LABEL_ENTER_PASS, LS(IDS_ENTER_DISK_PASSW) );
	SetDlgItemText(IDC_LABEL_ENTER_PASS2, LS(IDS_L_NEW_PASSW) );
	SetDlgItemText(IDC_LABEL_ENTER_PASS3, LS(IDS_L_CONFIRM_PASSW) );


	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
