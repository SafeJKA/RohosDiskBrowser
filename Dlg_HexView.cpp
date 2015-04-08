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
#include "Dlg_HexView.h"


void decrypt(char *buf,u64 pos,DWORD sz);
void encrypt(char *buf,u64 pos,DWORD sz);
char init_crypto(char *key,char *iv);

extern char IV[64];
extern char Key[32];

// CDlg_HexView dialog

IMPLEMENT_DYNAMIC(CDlg_HexView, CDialog)

CDlg_HexView::CDlg_HexView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_HexView::IDD, pParent)
	, m_offset(0)
{
		
	m_bStyle [0] = TRUE; //Address in Control Bar
	m_bStyle [1] = TRUE; //Hex in Control Bar
	m_bStyle [2] = TRUE; //Dec in Control Bar
	m_bStyle [3] = TRUE; //Bin in Control Bar
	m_bStyle [4] = FALSE;//Oct in Control Bar
	m_bStyle [5] = TRUE; //Ascii in Control Bar

	m_bStyle [6] = TRUE; //Address in Field Bar
	m_bStyle [7] = TRUE; //Hex in Field Bar
	m_bStyle [8] = FALSE;//Dec in Field Bar
	m_bStyle [9] = FALSE; //Bin in Field Bar
	m_bStyle [10] = FALSE;//Oct in Field Bar
	m_bStyle [11] = TRUE; //Ascii in Field Bar

	m_bStyle [12] = TRUE; //Field's Name
	m_bStyle [13] = FALSE;//Word Data
	m_bStyle [14] = FALSE; //Separators
	m_bStyle [15] = FALSE;//Read Only
	m_bStyle [16] = TRUE;//Selecting Data
	m_bStyle [17] = FALSE; //Low to High Byte
	m_bStyle [18] = TRUE; //Track Mouse


}

CDlg_HexView::~CDlg_HexView()
{
}

void CDlg_HexView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_HexView, CDialog)
	ON_BN_CLICKED(IDOK, &CDlg_HexView::OnBnClickedOk)
END_MESSAGE_MAP()

void CDlg_HexView::SetNewStyle()
{
	UINT nNewStyle = 0;
	UINT nStyle[] = {
		PPDUMP_BAR_ADDRESS,
		PPDUMP_BAR_HEX,
		PPDUMP_BAR_DEC,
		PPDUMP_BAR_BIN,
		PPDUMP_BAR_OCT,
		PPDUMP_BAR_ASCII,
		PPDUMP_FIELD_ADDRESS,
		PPDUMP_FIELD_HEX,
		PPDUMP_FIELD_DEC,
		PPDUMP_FIELD_BIN,
		PPDUMP_FIELD_OCT,
		PPDUMP_FIELD_ASCII,
		PPDUMP_NAMED_FIELDS,
		PPDUMP_WORD_DATA,
		PPDUMP_SEPARATOR_LINES,
		PPDUMP_READ_ONLY,
		PPDUMP_SELECTING_DATA,
		PPDUMP_DATA_LOW_HIGH,
		PPDUMP_TRACK_MOUSE_MOVE
					};
	for (int i = 0; i < 19; i++)
	{
		if (m_bStyle [i])
			nNewStyle |= nStyle [i];
	}
	m_ctlHexEdit2->SetStyles(nNewStyle, TRUE);
}


// CDlg_HexView message handlers

BOOL CDlg_HexView::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlHexEdit2 = new CPPDumpCtrl;
	if ( !m_ctlHexEdit2 ) return true;
	CRect rect;                         // Создайте окно Gridctrl
	GetClientRect(rect);

	rect.bottom -= 60;		// оставим Заголовок для надписей
	m_ctlHexEdit2->Create(rect, this, 100);
	SetNewStyle();

	Read_DisplayData(0);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

extern CFsys _FSys;// Для работы с файловой системой

bool CDlg_HexView::Read_DisplayData(__int64 offset)
{
	DWORD dwRead =0;


	LARGE_INTEGER liOffset;
	liOffset.QuadPart=0;	
	if ( 0 == _FSys.ReadDiskData( buff, 512 * 20, liOffset ) )
	{
		AfxMessageBox( "Error reading file" );
		return false;
	}

	m_ctlHexEdit2->SetPointerData(512*20, buff, NULL, TRUE); 

	return true;


}
void CDlg_HexView::OnBnClickedOk()
{
	m_offset = GetDlgItemInt(IDC_EDIT1);

	if (m_offset)
	{
		LARGE_INTEGER liOffset;
		liOffset.QuadPart = m_offset * 512;	
		if ( 0 == _FSys.ReadDiskData( buff, 512 * 20, liOffset ) )
		{
			AfxMessageBox( "Error reading file" );
			return;
		}
		m_ctlHexEdit2->SetPointerData(512*20, buff, NULL, TRUE); 
	}

	if (m_offset == 0 )
		OnOK();
}
