// XKeyboard.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XKeyboard implements CXKeyboard, a dialog class that displays a keyboard
//     on the screen.  The typical use for an on-screen keyboard is to accept
//     passwords or other sensitive user input, where the possibility of a
//     keylogger might exist.
//
// History
//     Version 1.0 - 2005 April 4
//     - Initial public release
//
// Public APIs:
//          NAME                              DESCRIPTION
//     ------------------   -------------------------------------------------
//     GetPassword()        Returns the password
//     SetPassword()        Sets the edit control with the password
//     GetShowText()        Returns state of Show Text checkbox
//     SetShowText()        Sets state of Show Text checkbox
//     GetShowSpacebar()    Returns TRUE if spacebar is visible
//     SetShowSpacebar()    Sets visibility of spacebar (TRUE = visible)
//     GetSpecial()         Returns enum value of special character setting
//     SetSpecial()         Sets special character enum value
//     GetMinLength()       Returns minimum length of a password
//     SetMinLength()       Sets minimum length of password
//     GetMaxLength()       Returns minimum length of a password
//     SetMaxLength()       Sets minimum length of password
//     GetCase()            Returns enum value of case setting
//     SetCase()            Sets case enum value
//     SetMRUPasswords()    Sets MRU password list
//     ClearMRUPasswords()  Clears MRU password list
//     GetTimeout()         Returns timeout value in seconds
//     SetTimeout()         Sets timeout value in seconds; 0 = no timeout
//     GetReadOnly()        Returns read-only state of edit control
//     SetReadOnly()        Sets read-only state of edit control (TRUE = read-only)
//
//     CheckSpecial()       Virtual function that returns TRUE if password
//                          contains a special character
//     CheckMRUPasswords()  Virtual function that returns TRUE if password
//                          is in MRU list
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "XKeyboardTest.h"
#include "XKeyboard.h"
#include "XKeyboardRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define NBUTTONS			47		// just to sanity-check the button arrays (see below)
#define TIMER_TIMEOUT		1
#define TIMER_SHIFT_CHECK	2

///////////////////////////////////////////////////////////////////////////////
// character arrays for keyboard keys and special keys - all characters
// are in same row and relative position as actual keyboard

static TCHAR * unshifted =
{
	_T("`1234567890-=")
	_T("qwertyuiop[]\\")
	_T("asdfghjkl;'")
	_T("zxcvbnm,./")
};

static TCHAR * shifted =
{
	_T("~!@#$%^&*()_+")
	_T("QWERTYUIOP{}|")
	_T("ASDFGHJKL:\"")
	_T("ZXCVBNM<>?")
};

static TCHAR * special = 
{
	_T("`-=")				// unshifted
	_T("[]\\")
	_T(";'")
	_T(",./")
	_T("~!@#$%^&*()_+")		// shifted
	_T("{}|")
	_T(":\"")
	_T("<>?")
};

///////////////////////////////////////////////////////////////////////////////
// _AfxFailMinMaxWithFormat - display message if string size is out of range
// (from DLGDATA.CPP)
static void _AfxFailMinMaxWithFormat(CDataExchange* pDX,
									 long minVal, 
									 long maxVal, 
									 LPCTSTR lpszFormat, 
									 UINT nIDPrompt)
	// error string must have '%1' and '%2' strings for min and max values
{
	ASSERT(lpszFormat != NULL);

	if (!pDX->m_bSaveAndValidate)
	{
		TRACE(_T("Warning: initial dialog data is out of range.\n"));
		return;     // don't stop now
	}
	TCHAR szMin[32];
	TCHAR szMax[32];
	wsprintf(szMin, lpszFormat, minVal);
	wsprintf(szMax, lpszFormat, maxVal);
	CString prompt = _T("");
	AfxFormatString2(prompt, nIDPrompt, szMin, szMax);
	AfxMessageBox(prompt, MB_ICONEXCLAMATION, nIDPrompt);
	prompt.Empty(); // exception prep
	pDX->Fail();
}

///////////////////////////////////////////////////////////////////////////////
// DDV_MinMaxString - DDV function to check size of string
/*static void DDV_MinMaxString(CDataExchange* pDX, 
							 CString const& value, 
							 int nMin, 
							 int nMax)
{
	int nLen = value.GetLength();
	TRACE(_T("in DDV_MinMaxString:  nMin=%d  nMax=%d  len=%d  m_bSaveAndValidate=%d\n"), 
		nMin, nMax, nLen, pDX->m_bSaveAndValidate);
	ASSERT(nMin >= 1);
	if (pDX->m_bSaveAndValidate && 
		((nLen < nMin) || 
		((nMax > 0) && (nLen > nMax))))
	{
		if (nMin == 1)
			_AfxFailMinMaxWithFormat(pDX, (long)nMin, (long)nMax, _T("%u"),
				IDS_XKEYBOARD_STRING_RANGE1);
		else
			_AfxFailMinMaxWithFormat(pDX, (long)nMin, (long)nMax, _T("%u"),
				IDS_XKEYBOARD_STRING_RANGE);
	}
	else if (pDX->m_hWndLastControl != NULL && pDX->m_bEditLastControl)
	{
		// limit the control max-chars automatically
		::SendMessage(pDX->m_hWndLastControl, EM_LIMITTEXT, nMax, 0);
	}
}*/

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CXKeyboard, CDialog)
	//{{AFX_MSG_MAP(CXKeyboard)
	ON_BN_CLICKED(IDC_KBD_SHOW_TEXT, OnShowText)
	ON_BN_CLICKED(IDC_KBD_LSHIFT, OnLshift)
	ON_BN_CLICKED(IDC_KBD_RSHIFT, OnRshift)
	ON_BN_CLICKED(IDC_KBD_SPACEBAR, OnSpacebar)
	//ON_EN_UPDATE(IDC_KBD_PASSWORD, OnUpdatePassword)
	ON_WM_TIMER()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_KBD_01, IDC_KBD_47, OnKey)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXKeyboard::CXKeyboard(int xPos /*= -1*/,		// for initial position
					   int yPos /*= -1*/,
					   LPCTSTR lpszTitle /*= _T("Keyboard")*/, 
					   LPCTSTR lpszShowText /*= _T("Show text")*/, 
					   CWnd* pParent /*=NULL*/)
	: CDialog(_T("IDD_XKEYBOARD"), pParent)
{
	//{{AFX_DATA_INIT(CXKeyboard)
	m_strPassword   = _T("");
	//}}AFX_DATA_INIT
	m_xPos          = xPos;
	m_yPos          = yPos;
	m_nMinLength    = 1;
	m_nMaxLength    = 0;				// 0 = no maximum length
	m_nTimeout      = 0;
	m_bShift        = FALSE;
	m_bShowText     = FALSE;
	m_bShowSpacebar = TRUE;
	m_bReadOnly     = FALSE;
	m_eSpecial      = Allow;
	m_eCase         = UpperOrLower;
	m_strTitle      = lpszTitle;
	m_strShowText   = lpszShowText;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
//		DDX_Text(pDX,    IDC_KBD_PASSWORD,   m_strPassword);	// removed 

// DDV_MinMaxString(pDX, m_strPassword, m_nMinLength, m_nMaxLength);
void CXKeyboard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXKeyboard)
	DDX_Control(pDX, IDC_KBD_SPACEBAR,   m_btnSpacebar);
	DDX_Control(pDX, IDC_KBD_SHOW_TEXT,  m_chkShowText);
	//DDX_Control(pDX, IDC_KBD_PASSWORD,   m_edtPassword);
	DDX_Control(pDX, IDC_KBD_RSHIFT,     m_btnRShift);
	DDX_Control(pDX, IDC_KBD_LSHIFT,     m_btnLShift);
	
	//}}AFX_DATA_MAP
	
}

LPCTSTR LS(UINT id);
#include "strings.h"

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXKeyboard::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(_tcslen(unshifted) == NBUTTONS);
	ASSERT(_tcslen(shifted) == NBUTTONS);

	CRect rectDialog;
	GetWindowRect(&rectDialog);

	// shrink the dialog if there is no spacebar
	if (!m_bShowSpacebar)
		ShrinkDialog(rectDialog);

	// set up buttons
	SetButtons(m_bShift);
	m_btnRShift.SetToggle(TRUE);
	m_btnLShift.SetToggle(TRUE);

	SetDlgItemText(IDCANCEL, LS(IDS_CANCEL) );
	SetDlgItemText(IDC_KBD_SHOW_TEXT, LS(IDS_VIEW) );

	// set control text
	//m_chkShowText.SetWindowText(m_strShowText);
	//m_edtPassword.SetWindowText(m_strPassword);
	int n = m_strPassword.GetLength();
	//m_edtPassword.SetSel(n, n);

	m_chkShowText.SetCheck(m_bShowText);
	//m_edtPassword.SetPasswordChar(_T('*'));

	/*if (m_eCase == Upper)
		m_edtPassword.ModifyStyle(0, ES_UPPERCASE);
	else if (m_eCase == Lower)
		m_edtPassword.ModifyStyle(0, ES_LOWERCASE);*/

	m_bShowText = !m_bShowText;
	OnShowText();

	// position dialog if requested
	if ((m_xPos != -1) || (m_yPos != -1))
	{
		MoveWindow(m_xPos, m_yPos, rectDialog.Width(), rectDialog.Height());
	}

	CString strCaption = _T("");
	if (m_nTimeout > 0)
	{
		strCaption.Format(IDS_XKEYBOARD_TIMEOUT, m_strTitle, m_nTimeout);
		SetTimer(TIMER_TIMEOUT, 1000, NULL);
	}
	else
	{
		strCaption = m_strTitle;
	}
	SetWindowText(strCaption);

	SetTimer(TIMER_SHIFT_CHECK, 50, NULL);

	/*m_edtPassword.SetReadOnly(m_bReadOnly);
	if (!m_bReadOnly)		// set focus only if its not read-only
	{
		m_edtPassword.SetFocus();
		return FALSE;
	}*/

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
// ShrinkDialog
void CXKeyboard::ShrinkDialog(CRect& rectDialog)
{
	CRect rectSpacebar;
	m_btnSpacebar.GetWindowRect(&rectSpacebar);
	ScreenToClient(&rectSpacebar);

	UINT aID[] = { IDC_KBD_GROUPBOX, IDC_KBD_PASSWORD, IDC_KBD_SHOW_TEXT, IDOK, IDCANCEL };

	// move controls up, since there is no spacebar
	int nDeltaY = 0;
	for (int i = 0; i < _countof(aID); i++)
	{
		CRect rect;
		GetDlgItem(aID[i])->GetWindowRect(&rect);
		ScreenToClient(&rect);

		if (nDeltaY == 0)
			nDeltaY = rect.top - rectSpacebar.top + 6;

		rect.top -= nDeltaY;
		rect.bottom -= nDeltaY;

		GetDlgItem(aID[i])->MoveWindow(&rect);
	}

	rectDialog.bottom -= nDeltaY;
	MoveWindow(&rectDialog);

	m_btnSpacebar.ShowWindow(SW_HIDE);
	m_btnSpacebar.EnableWindow(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// SetPassword
void CXKeyboard::SetPassword(LPCTSTR lpszPassword)
{
	ASSERT(lpszPassword);

	m_strPassword = lpszPassword;

	if (m_eCase == Upper)
		m_strPassword.MakeUpper();
	else if (m_eCase == Lower)
		m_strPassword.MakeLower();

	if (!m_bShowSpacebar)
		m_strPassword.Replace(_T(" "), _T(""));
}

///////////////////////////////////////////////////////////////////////////////
// SetCase
void CXKeyboard::SetCase(XKEYBOARD_CASE eCase)
{
	m_eCase = eCase;
	SetPassword(m_strPassword);
}

///////////////////////////////////////////////////////////////////////////////
// SetShowSpacebar
void CXKeyboard::SetShowSpacebar(BOOL bShow)
{
	m_bShowSpacebar = bShow; 
	SetPassword(m_strPassword);
}

///////////////////////////////////////////////////////////////////////////////
// SetButtons - set character displayed on keyboard button
void CXKeyboard::SetButtons(BOOL bShift)
{
	CString str = _T("");
	TCHAR c;
	for (int i = 0; i < NBUTTONS; i++)
	{
		c = bShift ? shifted[i] : unshifted[i];
		if ((m_eSpecial == DontAllow) && !_istalpha(c) && !_istdigit(c))
			c = _T(' ');
		str = c;
		if (c == _T('&'))			// double & so it will be displayed
			str += c;
		UINT nID = IDC_KBD_01 + i;
		GetDlgItem(nID)->SetWindowText(str);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CheckSpecial - returns TRUE if there is a special character in password
BOOL CXKeyboard::CheckSpecial(LPCTSTR lpszPassword, LPCTSTR lpszSpecial)
{
	ASSERT(lpszPassword);
	ASSERT(lpszSpecial);

	if (_tcspbrk(lpszPassword, lpszSpecial) != NULL)
	{
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// CheckMRUPasswords - returns TRUE if match found in MRU list
BOOL CXKeyboard::CheckMRUPasswords(LPCTSTR lpszPassword, CStringArray& saPasswords)
{
	ASSERT(lpszPassword);

	BOOL ok = FALSE;

	int nMRU = saPasswords.GetSize();
	int nPasswordLen = _tcslen(lpszPassword);

	for (int i = 0; i < nMRU; i++)
	{
		CString s = saPasswords[i];
		if (s.IsEmpty())
			continue;
		int nLen = s.GetLength();
		if (nPasswordLen < nLen)
			nLen = nPasswordLen;
		if (_tcsnicmp(lpszPassword, s, nLen) == 0)
		{
			ok = TRUE;
			break;
		}
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// OnOK
void CXKeyboard::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	/*
	BOOL bNonSpaceSeen = FALSE;
	for (int i = 0; i < m_strPassword.GetLength(); i++)
	{
		if (m_strPassword[i] == _T(' '))
			continue;
		bNonSpaceSeen = TRUE;
		break;
	}

	if (!bNonSpaceSeen)
	{
		AfxMessageBox(IDS_XKEYBOARD_NON_SPACE);
		return;
	}

	if (!m_bShowSpacebar)
	{
		m_strPassword.Replace(_T(" "), _T(""));
	}

	if (m_eSpecial == DontAllow)
	{
		if (CheckSpecial(m_strPassword, special))
		{
			AfxMessageBox(IDS_XKEYBOARD_ALPHA_NUMERIC);
			return;
		}
	}

	if (m_eSpecial == RequireSpecial)
	{
		if (!CheckSpecial(m_strPassword, special))
		{
			AfxMessageBox(IDS_XKEYBOARD_SPECIAL);
			return;
		}
	}

	if (m_eSpecial == RequireSpecialOrNumeric)
	{
		if (!CheckSpecial(m_strPassword, special) &&
			(_tcspbrk(m_strPassword, _T("0123456789")) == NULL))
		{
			AfxMessageBox(IDS_XKEYBOARD_SPECIAL_DIGIT);
			return;
		}
	}

	if (m_eCase == Upper)
		m_strPassword.MakeUpper();
	else if (m_eCase == Lower)
		m_strPassword.MakeLower();

	if (CheckMRUPasswords(m_strPassword, m_saPasswords))
	{
		AfxMessageBox(IDS_XKEYBOARD_SIMILAR_MRU);
		return;
	}

	UpdateData(FALSE);*/

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// OnShowText - show or hide the plain text
void CXKeyboard::OnShowText() 
{
	m_bShowText = !m_bShowText;
	DrawPasswChars();
	/*if (m_bShowText)
	{
		m_edtPassword.ModifyStyle(ES_PASSWORD, 0);
		m_edtPassword.SetPasswordChar(0);
	}
	else
	{
		m_edtPassword.ModifyStyle(0, ES_PASSWORD);
		m_edtPassword.SetPasswordChar(_T('*'));
	}

	m_edtPassword.RedrawWindow();*/
}

///////////////////////////////////////////////////////////////////////////////
// OnLshift
void CXKeyboard::OnLshift() 
{
	m_bShift = !m_bShift;
	m_btnRShift.SetToggleState(m_bShift);
	SetButtons(m_bShift);
}

///////////////////////////////////////////////////////////////////////////////
// OnRshift
void CXKeyboard::OnRshift() 
{
	m_bShift = !m_bShift;
	m_btnLShift.SetToggleState(m_bShift);
	SetButtons(m_bShift);
}

///////////////////////////////////////////////////////////////////////////////
// OnSpacebar
void CXKeyboard::OnSpacebar() 
{
	//CString strPassword = _T("");
	//m_edtPassword.GetWindowText(strPassword);

	m_strPassword += _T(' ');

	//m_edtPassword.SetWindowText(strPassword);
}

void	CXKeyboard :: DrawPasswChars()
{
	CDC * dc = GetDC();

	TCHAR hiddenStr[100];
	memset(hiddenStr, 0, 99);
	if (!m_bShowText)
		memset(hiddenStr, '*', m_strPassword.GetLength() );
	else 
		strcpy(hiddenStr, m_strPassword);

	CRect r;
	r.SetRect(25,300, 286, 144+33);

	CFont m_Font; //.DeleteObject();   
    m_Font.CreateFont(21,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"MS Shell Dlg 2"); 
	
	CFont *oldFont = dc->SelectObject(&m_Font);
	dc->DrawText(hiddenStr, _tcslen(hiddenStr), r, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	dc->SelectObject(oldFont); 

	m_Font.DeleteObject();   

	ReleaseDC(dc);

}

///////////////////////////////////////////////////////////////////////////////
// OnKey
void CXKeyboard::OnKey(UINT nID) 
{
	CString strPassword = _T("");
	//m_edtPassword.GetWindowText(strPassword);

	TCHAR c = m_bShift ? shifted[nID-IDC_KBD_01] : unshifted[nID-IDC_KBD_01];

	if ((m_eSpecial == Allow) || 
		(m_eSpecial == RequireSpecial) || 
		_istalpha(c) || _istdigit(c))
	{
		if ((m_strPassword.GetLength() < m_nMaxLength) || (m_nMaxLength == 0))
			m_strPassword += c;
	}

	DrawPasswChars();

	//m_edtPassword.SetWindowText(strPassword);
}

///////////////////////////////////////////////////////////////////////////////
// OnUpdatePassword - called when the user types (!) into the edit control
void CXKeyboard::OnUpdatePassword() 
{
	CString strPassword = _T("");
	/*if (!m_bShowSpacebar)
	{
		m_edtPassword.GetWindowText(strPassword);
		if (strPassword.Find(_T(' ')) >= 0)
		{
			strPassword.Replace(_T(" "), _T(""));
			m_edtPassword.SetWindowText(strPassword);
			int n = strPassword.GetLength();
			m_edtPassword.SetSel(n, n);
		}
	}

	if (m_eSpecial == DontAllow)
	{
		m_edtPassword.GetWindowText(strPassword);
		int index = strPassword.FindOneOf(special);
		if (index != -1)
		{
			// discard special character
			strPassword.Delete(index);
			m_edtPassword.SetWindowText(strPassword);
			int n = strPassword.GetLength();
			m_edtPassword.SetSel(n, n);
		}
	}*/
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXKeyboard::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_TIMEOUT)
	{
		if (--m_nTimeout <= 0)
		{
			KillTimer(nIDEvent);
			EndDialog(IDABORT);
		}
		else
		{
			CString strCaption = _T("");
			strCaption.Format(IDS_XKEYBOARD_TIMEOUT, m_strTitle, m_nTimeout);
			SetWindowText(strCaption);
		}
	}
	else if (nIDEvent == TIMER_SHIFT_CHECK)
	{
		static int bOldState = FALSE;
		SHORT nState = GetAsyncKeyState(VK_SHIFT);

		BOOL bToggle = FALSE;

		if ((nState < 0) && !bOldState && !m_bShift)
			bToggle = TRUE;
		else if ((nState >= 0) && bOldState && m_bShift)
			bToggle = TRUE;

		if (bToggle)
		{
			m_bShift = !m_bShift;
			m_btnLShift.SetToggleState(m_bShift);
			m_btnRShift.SetToggleState(m_bShift);
			SetButtons(m_bShift);
		}

		if (nState < 0)
			bOldState = TRUE;	// real shift key is down
		else
			bOldState = FALSE;	// real shift key is up
	}

	CDialog::OnTimer(nIDEvent);
}

void CXKeyboard::OnChar(
   UINT nChar,
   UINT nRepCnt,
   UINT nFlags 
)
{
	if ((m_strPassword.GetLength() < m_nMaxLength) || (m_nMaxLength == 0))

	m_strPassword += (TCHAR)nChar;
	DrawPasswChars();

}

BOOL CXKeyboard::PreTranslateMessage(MSG* pMsg) 
{

	if (pMsg->message == WM_CHAR )
	{
		OnChar(pMsg->wParam, 0, 0);
	}

	return CDialog::PreTranslateMessage(pMsg);
}