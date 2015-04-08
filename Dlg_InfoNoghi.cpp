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
#include "Dlg_InfoNoghi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_InfoNoghi dialog


CDlg_InfoNoghi::CDlg_InfoNoghi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_InfoNoghi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_InfoNoghi)
	m_cbValue = FALSE;
	m_LabelInfCaption = _T("");
	m_Label2 = _T("");
	//}}AFX_DATA_INIT
}


void CDlg_InfoNoghi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_InfoNoghi)
	DDX_Control(pDX, IDC_CHECK1, m_ChBoxCaption);
	DDX_Check(pDX, IDC_CHECK1, m_cbValue);
	DDX_Text(pDX, IDC_LABEL_INF, m_LabelInfCaption);
	DDX_Text(pDX, IDC_CHECK1, m_Label2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_InfoNoghi, CDialog)
	//{{AFX_MSG_MAP(CDlg_InfoNoghi)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_LABEL2, &CDlg_InfoNoghi::OnStnClickedLabel2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_InfoNoghi message handlers

BOOL CDlg_InfoNoghi::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	SendDlgItemMessage(IDC_STATIC2, STM_SETICON , (WPARAM) (HICON) ::LoadIcon( NULL, IDI_INFORMATION) , 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_InfoNoghi::OnStnClickedLabel2()
{
	// TODO: Add your control notification handler code here
}
