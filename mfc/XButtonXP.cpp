// XButtonXP.cpp  Version 1.1
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XButtonXP implements CXButtonXP, an ownerdraw button that is XP
//     theme-aware and may be used as a pushbutton or a toggle button.
//     An icon and/or text may be displayed on the button.  Please see
//     my article at www.codeproject.com.
//
//     This code is based partially on Ewan Ward's article
//     "Native Win32 Theme aware Owner-draw Controls without MFC"
//     http://www.codeproject.com/buttonctrl/nativewin32xpthemes.asp
//
// History
//     Version 1.1 - 2005 March 30
//     - Fixed bug in drawing toggle button, reported by Dominik Reichl.
//     - Added SetToggleState()
//     - Removed #include "XButtonXPTest.h"
//
//     Version 1.0 - 2005 March 22
//     - Initial public release
//
// Public APIs:
//          NAME                              DESCRIPTION
//     ------------------   -------------------------------------------------
//     GetToggle()          Returns TRUE if button is a toggle button
//     SetToggle()          Sets button to toggle state (TRUE) or to
//                          pushbutton state (FALSE)
//     GetToggleState()     Returns TRUE if button is toggled (down)
//     SetToggleState()     Set toggle state to pushed (TRUE) or
//                          unpushed (FALSE)
//     EnableTheming()      Enables theming (TRUE)
//     IsThemed()           Returns TRUE if button is themed;  this is more
//                          than just EnableTheming() setting - see code
//     SetIcon()            Set icon for display on button
//     SetIconAlignment()   Set icon alignment
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
#include "XButtonXP.h"
#include "XThemeHelper.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (WINVER < 0x0500)
#error WINVER must be defined in stdafx.h:  #define WINVER 0x0500
#endif

///////////////////////////////////////////////////////////////////////////////
// various defines that are needed
#ifndef ODS_NOFOCUSRECT
#define ODS_NOFOCUSRECT     0x0200		// from WinUser.h
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))
#endif

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED     0x031A
#endif

///////////////////////////////////////////////////////////////////////////////
// Message map
BEGIN_MESSAGE_MAP(CXButtonXP, COddButton)
	//{{AFX_MSG_MAP(CXButtonXP)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXButtonXP::CXButtonXP()
{
	m_hTheme           = NULL;
	m_bMouseOverButton = FALSE;
	m_bFirstTime       = TRUE;
	m_hIcon            = NULL;
	m_eIconAlignment   = LEFT;
	m_bIsToggle        = FALSE;
	m_bToggled         = FALSE;
	m_bLButtonDown     = FALSE;
	m_bSent            = FALSE;
	m_bEnableTheming   = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXButtonXP::~CXButtonXP()
{
	if (m_hTheme)
		ThemeHelper.CloseThemeData(m_hTheme);
	m_hTheme = NULL;

	if (m_hIcon)
		DestroyIcon(m_hIcon);
	m_hIcon = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// IsThemed
BOOL CXButtonXP::IsThemed()
{
	BOOL bAppThemed = ThemeHelper.IsAppThemed();	// user can disable theming
													// for app by right-clicking
													// on exe, then clicking on
													// Properties | Compatibility |
													// Disable visual themes
	return (m_hTheme && m_bEnableTheming && bAppThemed);
}

///////////////////////////////////////////////////////////////////////////////
// SetIcon - from resource id
void CXButtonXP::SetIcon(UINT nIDResource, ICON_ALIGNMENT ia /*= LEFT*/)
{
	TRACE(_T("in CXButtonXP::SetIcon UINT\n"));

	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(nIDResource),
								IMAGE_ICON,
								0,
								0,
								0);

	SetIcon(hIcon, ia);

	TRACE(_T("m_hIcon=%X\n"), m_hIcon);
}

///////////////////////////////////////////////////////////////////////////////
// SetIcon - from HICON
void CXButtonXP::SetIcon(HICON hIcon, ICON_ALIGNMENT ia /*= LEFT*/)
{
	TRACE(_T("in CXButtonXP::SetIcon HICON\n"));

	if (m_hIcon)
		DestroyIcon(m_hIcon);

	m_hIcon = hIcon;
	TRACE(_T("m_hIcon=%X\n"), m_hIcon);

	m_eIconAlignment = ia;

	RedrawWindow();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// PRIVATE FUNCTIONS & MESSAGE HANDLERS
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PrepareImageRect
void CXButtonXP::PrepareImageRect(BOOL bHasText,
								  RECT& rectItem,
								  RECT& rectText,
								  BOOL bIsPressed,
								  DWORD dwWidth,
								  DWORD dwHeight,
								  RECT& rectImage)
{
	CopyRect(&rectImage, &rectItem);

	if (bHasText)
	{
		// image must be placed just inside the focus rect
		int nTextWidth = rectText.right - rectText.left;
		if (m_eIconAlignment == RIGHT)
		{
			rectText.right = nTextWidth - dwWidth - dwWidth/2;
			rectText.left  = dwWidth/2;
			rectImage.left = rectItem.right - dwWidth - dwWidth/2;
		}
		else	// icon is on the left
		{
			rectText.right = nTextWidth - dwWidth/2;
			rectText.left  = 2 * dwWidth;
			rectImage.left = dwWidth/2;
		}
	}
	else	// no text
	{
		// center image horizontally
		int nImageWidth = rectImage.right - rectImage.left;
		rectImage.left += ((nImageWidth - dwWidth)/2);
	}

	// center image vertically
	int nImageHeight = rectImage.bottom - rectImage.top;
	rectImage.top += ((nImageHeight - dwHeight)/2);

	// if button is pressed then press image also
	if (bIsPressed && !IsThemed())
		OffsetRect(&rectImage, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////
// DrawIcon
void CXButtonXP::DrawIcon(CDC *pDC,
						  BOOL bHasText,
						  RECT& rectItem,		// from LPDRAWITEMSTRUCT
						  RECT& rectText,
						  BOOL bIsPressed,
						  BOOL bIsDisabled)
{
	if (m_hIcon)
	{
		// first get size of icon

		DWORD dwWidth = 32;		// assume 32x32
		DWORD dwHeight = 32;
		ICONINFO iconinfo;

		if (GetIconInfo(m_hIcon, &iconinfo))
		{
			CBitmap* pBitmap = CBitmap::FromHandle(iconinfo.hbmColor);
			if (pBitmap)
			{
				BITMAP bm;
				pBitmap->GetBitmap(&bm);
				dwWidth = bm.bmWidth;
				dwHeight = bm.bmHeight;
			}
		}

		RECT rectImage;

		PrepareImageRect(bHasText, rectItem, rectText, bIsPressed, dwWidth, dwHeight, rectImage);

		pDC->DrawState(CPoint(rectImage.left, rectImage.top),
					   CSize(rectImage.right - rectImage.left, rectImage.bottom - rectImage.top),
					   m_hIcon,
					   (bIsDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_ICON,
					   (CBrush *) NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CXButtonXP::OnMouseMove(UINT nFlags, CPoint point)
{
	// check to see if mouse is over button

	CPoint pt(point);
	ClientToScreen(&pt);

	CRect rect;
	GetWindowRect(&rect);

	BOOL bMouseOverButton = m_bMouseOverButton;

	if (rect.PtInRect(pt))
		m_bMouseOverButton = TRUE;
	else
		m_bMouseOverButton = FALSE;

	if (bMouseOverButton != m_bMouseOverButton)
		InvalidateRect(NULL);

	//TRACE(_T("in CXButtonXP::OnMouseMove:  m_bMouseOverButton=%d\n"), m_bMouseOverButton);

	COddButton::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd - to prevent flicker
BOOL CXButtonXP::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DrawItem
void CXButtonXP::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (m_bFirstTime)
	{
		// open theme for Button

		m_bFirstTime = FALSE;

		if (ThemeHelper.IsThemeLibAvailable())
		{
			m_hTheme = ThemeHelper.OpenThemeData(m_hWnd, _T("Button"));
		}
	}

	CDC *pDC = CDC::FromHandle(lpDIS->hDC);

	CMemDC memDC(pDC);

	CFont *pFont = GetParent()->GetFont();

	if (pFont)
		memDC.SelectObject(pFont);

	// button state

	BOOL bIsPressed     = lpDIS->itemState & ODS_SELECTED;

	if (m_bIsToggle)
	{
		if (m_bLButtonDown && !m_bMouseOverButton)
		{
			// left mb is down outside button
			bIsPressed = !m_bToggled;
		}
		else
		{
			bIsPressed = m_bToggled;
		}
	}
	BOOL bIsFocused     = lpDIS->itemState & ODS_FOCUS;
	BOOL bIsDisabled    = lpDIS->itemState & ODS_DISABLED;
	BOOL bDrawFocusRect = !(lpDIS->itemState & ODS_NOFOCUSRECT);

	RECT itemRect = lpDIS->rcItem;

	memDC.SetBkMode(TRANSPARENT);

	DWORD part = BP_PUSHBUTTON;

	if (IsThemed())
	{
		DWORD state = bIsPressed ? PBS_PRESSED : PBS_NORMAL;

		if (state == PBS_NORMAL)
		{
			if (bIsFocused)
				state = PBS_DEFAULTED;
			if (m_bMouseOverButton)
				state = PBS_HOT;
		}
		ThemeHelper.DrawThemeBackground(m_hTheme, memDC.m_hDC, part, state, &itemRect, NULL);
	}
	else
	{
		if (bIsFocused)
		{
			if (!m_bIsToggle || !bIsPressed)
			{
				CBrush brush(RGB(0,0,0));
				memDC.FrameRect(&itemRect, &brush);
			}
			InflateRect(&itemRect, -1, -1);
		}

		if (m_bIsToggle)
		{
			// draw the traditional pushbutton edge using DrawEdge
			if (bIsPressed)
			{
				memDC.DrawEdge(&itemRect, EDGE_SUNKEN, BF_RECT | BF_MIDDLE | BF_SOFT);
			}
			else
			{
				memDC.DrawEdge(&itemRect, EDGE_RAISED, BF_RECT | BF_MIDDLE | BF_SOFT);
			}
		}
		else
		{
			CBrush brBackground(GetSysColor(COLOR_BTNFACE));

			memDC.FillRect(&itemRect, &brBackground);

			// draw pressed button
			if (bIsPressed)
			{
				CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
				memDC.FrameRect(&itemRect, &brBtnShadow);
			}
			else	// ...else draw non pressed button
			{
				UINT uState = DFCS_BUTTONPUSH |
					  (m_bMouseOverButton ? DFCS_HOT : 0) |
					  ((bIsPressed) ? DFCS_PUSHED : 0);

				memDC.DrawFrameControl(&itemRect, DFC_BUTTON, uState);
			}
		}
	}

	// get the button's title
	CString strTitle = _T("");
	GetWindowText(strTitle);
	BOOL bHasText = !strTitle.IsEmpty();

	UINT uTextAlignment = DT_LEFT;
	if (m_hIcon == NULL)
		uTextAlignment = DT_CENTER;
	uTextAlignment |= DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK;

	RECT captionRect = lpDIS->rcItem;

	// draw the icon
	DrawIcon(&memDC, bHasText, lpDIS->rcItem, captionRect, bIsPressed, bIsDisabled);

	// write the button title (if any)
	if (bHasText)
	{
		// draw the button's title -
		// if button is pressed then "press" title also
		if (bIsPressed && !IsThemed())
			OffsetRect(&captionRect, 1, 1);

		// center text
		RECT centerRect = captionRect;
		if (m_hIcon)
			memDC.DrawText(strTitle, -1, &captionRect, uTextAlignment | DT_CALCRECT);

		LONG captionRectHeight = captionRect.bottom - captionRect.top;
		LONG centerRectHeight  = centerRect.bottom  - centerRect.top;
		OffsetRect(&captionRect, 0, (centerRectHeight - captionRectHeight)/2);

		if (IsThemed())
		{
			ThemeHelper.DrawThemeText(m_hTheme, memDC.m_hDC, part, PBS_NORMAL,
							strTitle, uTextAlignment, 0, &captionRect);
		}
		else
		{
			memDC.SetBkMode(TRANSPARENT);

			if (bIsDisabled)
			{
				OffsetRect(&captionRect, 1, 1);
				memDC.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
				memDC.DrawText(strTitle, -1, &captionRect, uTextAlignment);
				OffsetRect(&captionRect, -1, -1);
				memDC.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
				memDC.DrawText(strTitle, -1, &captionRect, uTextAlignment);
			}
			else
			{
				memDC.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
				memDC.SetBkColor(::GetSysColor(COLOR_BTNFACE));
				memDC.DrawText(strTitle, -1, &captionRect, uTextAlignment);
			}
		}
	}

	// draw the focus rect
	if (bIsFocused && bDrawFocusRect)
	{
		RECT focusRect = itemRect;
		InflateRect(&focusRect, -3, -3);
		memDC.DrawFocusRect(&focusRect);
	}
}

///////////////////////////////////////////////////////////////////////////////
// DefWindowProc
LRESULT CXButtonXP::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// catch WM_KEYDOWN so we can avoid intefering with TAB key
		case WM_GETDLGCODE:
		{
			if (lParam)
			{
				MSG *pMsg = (MSG *) lParam;
				if (pMsg && (pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB))
				{
					// don't interfere with tab processing
					return 0;
				}
			}
			return DLGC_WANTALLKEYS;	// we want all keys except TAB key
		}
		break;

		// handle RETURN and SPACE - simulate a button click
		case WM_CHAR:
		{
			if ((wParam == VK_SPACE) || (wParam == VK_RETURN))
			{
				if (m_bIsToggle)
				{
					m_bToggled = !m_bToggled;
					InvalidateRect(NULL);
				}
				else
				{
					SendMessage(BM_SETSTATE, 1);
					m_bSent = TRUE;
				}

				if (wParam == VK_RETURN)
				{
					CWnd* pParent = GetParent();
					if (pParent != NULL)
					{
						::SendMessage(pParent->m_hWnd, WM_COMMAND,
							(BN_CLICKED<<16) | this->GetDlgCtrlID(),
							(LPARAM)this->m_hWnd);
					}
				}
			}
		}
		break;

		case WM_LBUTTONUP:
		{
			m_bLButtonDown = FALSE;

			if (m_bSent)
			{
				SendMessage(BM_SETSTATE, 0);
				m_bSent = FALSE;

				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					::SendMessage(pParent->m_hWnd, WM_COMMAND,
						(BN_CLICKED<<16) | this->GetDlgCtrlID(),
						(LPARAM)this->m_hWnd);
				}
			}

			if (m_bIsToggle)
			{
				// if button is toggled and mouse button is released
				// outside of button, ignore the mouse click and restore
				// button state
				CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				ClientToScreen(&point);
				CRect rectButton;
				GetWindowRect(&rectButton);

				if (!rectButton.PtInRect(point))
				{
					m_bToggled = !m_bToggled;
					InvalidateRect(NULL);
				}
			}
		}
		break;

		case WM_LBUTTONDOWN:
		{
			m_bLButtonDown = TRUE;
			if (m_bIsToggle)
			{
				m_bToggled = !m_bToggled;
				InvalidateRect(NULL);
			}
		}
		break;

		case WM_LBUTTONDBLCLK:
		{
			if (m_bIsToggle)
			{
				// for toggle buttons, treat doubleclick as singleclick
				SendMessage(BM_SETSTATE, m_bToggled);
			}
			else
			{
				SendMessage(BM_SETSTATE, 1);
				m_bSent = TRUE;
			}
			return 0;
		}

		case WM_THEMECHANGED:
		{
			if (IsThemed())
			{
				if (m_hTheme)
				{
					// when user changes themes, close current theme and re-open
					ThemeHelper.CloseThemeData(m_hTheme);
					m_hTheme = NULL;
					m_bFirstTime = TRUE;
				}
			}
		}
		break;
	}

	return COddButton::DefWindowProc(message, wParam, lParam);
}
