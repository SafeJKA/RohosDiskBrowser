// XThemeHelper.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XThemeHelper implements CXThemeHelper, a singleton helper class that
//     wraps the functions of UXTHEME.DLL.
//
// History
//     Version 1.0 - 2005 March 22
//     - Initial public release
//
// Public APIs:
//                  NAME                               DESCRIPTION
//     ------------------------------   -------------------------------------------
//     IsAppThemed()                    Reports whether the current application's 
//                                      user interface displays using visual styles.
//     IsThemeActive()                  Tests if a visual style for the current 
//                                      application is active.
//     IsThemeLibAvailable()            Test whether UXTHEME.DLL (and its functions) 
//                                      are accessible.
//     CloseThemeData()                 Closes the theme data handle.
//     DrawThemeBackground()            Draws the background image defined by the 
//                                      visual style for the specified control part.
//     DrawThemeText()                  Draws text using the color and font 
//                                      defined by the visual style.
//     GetThemeBackgroundContentRect()  Retrieves the size of the content area 
//                                      for the background defined by the visual 
//                                      style.
//     OpenThemeData()                  Opens the theme data for a window and 
//                                      its associated class.
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
#define XTHEMEHELPER_CPP
#include "XThemeHelper.h"

///////////////////////////////////////////////////////////////////////////////
// class static data
PFNCLOSETHEMEDATA					CXThemeHelper::m_CloseThemeData = NULL;
PFNDRAWTHEMEBACKGROUND				CXThemeHelper::m_DrawThemeBackground = NULL;
PFNDRAWTHEMETEXT					CXThemeHelper::m_DrawThemeText = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT	CXThemeHelper::m_GetThemeBackgroundContentRect = NULL;
PFNISAPPTHEMED						CXThemeHelper::m_IsAppThemed = NULL;
PFNISTHEMEACTIVE					CXThemeHelper::m_IsThemeActive = NULL;
PFNOPENTHEMEDATA					CXThemeHelper::m_OpenThemeData = NULL;
HMODULE 							CXThemeHelper::m_hThemeLib = NULL;
BOOL								CXThemeHelper::m_bThemeLibLoaded = FALSE;
int									CXThemeHelper::m_nUseCount = 0;

///////////////////////////////////////////////////////////////////////////////
// ctor
CXThemeHelper::CXThemeHelper()
{
	m_nUseCount++;

	TRACE(_T("in CXThemeHelper: m_nUseCount=%d\n"), m_nUseCount);

	if (!m_bThemeLibLoaded)
	{
		Init();
	}
	else
	{
		TRACE(_T("already initialized\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// Init
void CXThemeHelper::Init()
{
	TRACE(_T("in CXThemeHelper::Init\n"));

	m_hThemeLib = LoadLibrary(_T("UXTHEME.DLL"));

	if (m_hThemeLib)
	{
		m_CloseThemeData                = (PFNCLOSETHEMEDATA)GetProcAddress(m_hThemeLib,
																			"CloseThemeData");
		m_DrawThemeBackground           = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hThemeLib,
																			"DrawThemeBackground");
		m_DrawThemeText                 = (PFNDRAWTHEMETEXT)GetProcAddress(m_hThemeLib,
																			"DrawThemeText");
		m_GetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(m_hThemeLib,
																			"GetThemeBackgroundContentRect");
		m_IsAppThemed                   = (PFNISAPPTHEMED)GetProcAddress(m_hThemeLib,
																			"IsAppThemed");
		m_IsThemeActive                 = (PFNISTHEMEACTIVE)GetProcAddress(m_hThemeLib,
																			"IsThemeActive");
		m_OpenThemeData                 = (PFNOPENTHEMEDATA)GetProcAddress(m_hThemeLib,
																			"OpenThemeData");
		if (m_CloseThemeData &&
			m_DrawThemeBackground &&
			m_DrawThemeText &&
			m_GetThemeBackgroundContentRect &&
			m_IsAppThemed &&
			m_IsThemeActive &&
			m_OpenThemeData)
		{
			TRACE(_T("UXTHEME.DLL loaded ok\n"));
			m_bThemeLibLoaded = TRUE;
		}
		else
		{
			TRACE(_T("ERROR - failed to locate theme library function\n"));
			FreeLibrary(m_hThemeLib);
			m_hThemeLib = NULL;
		}
	}
	else
	{
		TRACE(_T("UXTHEME.DLL not accessible\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXThemeHelper::~CXThemeHelper()
{
	TRACE(_T("in CXThemeHelper::~CXThemeHelper\n"));

	if (m_nUseCount > 0)
		m_nUseCount--;

	if (m_nUseCount == 0)
	{
		TRACE(_T("unloading theme dll\n"));
		if (m_hThemeLib)
			FreeLibrary(m_hThemeLib);
		m_hThemeLib = NULL;
		m_bThemeLibLoaded = FALSE;
	}
	TRACE(_T("m_nUseCount=%d\n"), m_nUseCount);
}

///////////////////////////////////////////////////////////////////////////////
// CloseThemeData
BOOL CXThemeHelper::CloseThemeData(HTHEME hTheme)
{
	TRACE(_T("in CXThemeHelper::CloseThemeData\n"));

	BOOL ok = FALSE;

	if (m_bThemeLibLoaded && hTheme)
	{
		HRESULT hr = m_CloseThemeData(hTheme);
		if (SUCCEEDED(hr))
			ok = TRUE;
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// DrawThemeBackground
BOOL CXThemeHelper::DrawThemeBackground(HTHEME hTheme,
										HDC hdc,
										int iPartId,
										int iStateId,
										const RECT *pRect,
										const RECT *pClipRect)
{
	//TRACE(_T("in CXThemeHelper::DrawThemeBackground\n"));

	BOOL ok = FALSE;

	if (m_bThemeLibLoaded && hTheme)
	{
		HRESULT hr = m_DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect,  pClipRect);
		if (SUCCEEDED(hr))
			ok = TRUE;
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// DrawThemeText
BOOL CXThemeHelper::DrawThemeText(HTHEME hTheme,
								  HDC hdc,
								  int iPartId,
								  int iStateId,
								  LPCTSTR lpszText,
								  DWORD dwTextFlags,
								  DWORD dwTextFlags2,
								  const RECT *pRect)
{
	//TRACE(_T("in CXThemeHelper::DrawThemeText\n"));

	BOOL ok = FALSE;

	if (m_bThemeLibLoaded && hTheme)
	{
		HRESULT hr = S_OK;

#ifdef _UNICODE

		hr = m_DrawThemeText(hTheme, hdc, iPartId, iStateId,
						lpszText, wcslen(lpszText),
						dwTextFlags, dwTextFlags2, pRect);

#else

		int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszText, _tcslen(lpszText)+1, NULL, 0);
		nLen += 2;
		WCHAR * pszWide = new WCHAR[nLen];
		if (pszWide)
		{
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszText, _tcslen(lpszText)+1, pszWide, nLen);
			hr = m_DrawThemeText(hTheme, hdc, iPartId, iStateId,
							pszWide, wcslen(pszWide),
							dwTextFlags, dwTextFlags2, pRect);
			delete pszWide;
		}

#endif

		if (SUCCEEDED(hr))
			ok = TRUE;
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// GetThemeBackgroundContentRect
BOOL CXThemeHelper::GetThemeBackgroundContentRect(HTHEME hTheme,
												  HDC hdc,
												  int iPartId,
												  int iStateId,
												  const RECT *pBoundingRect,
												  RECT *pContentRect)
{
	TRACE(_T("in CXThemeHelper::GetThemeBackgroundContentRect\n"));

	BOOL ok = FALSE;

	if (m_bThemeLibLoaded && hTheme)
	{
		HRESULT hr = m_GetThemeBackgroundContentRect(hTheme, hdc, iPartId, 
							iStateId, pBoundingRect, pContentRect);
		if (SUCCEEDED(hr))
			ok = TRUE;
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// IsAppThemed
BOOL CXThemeHelper::IsAppThemed()
{
	BOOL ok = FALSE;

	if (m_bThemeLibLoaded)
	{
		ok = m_IsAppThemed();
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// IsThemeActive
BOOL CXThemeHelper::IsThemeActive()
{
	BOOL ok = FALSE;

	if (m_bThemeLibLoaded)
	{
		ok = m_IsThemeActive();
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// OpenThemeData
HTHEME CXThemeHelper::OpenThemeData(HWND hWnd, LPCTSTR lpszClassList)
{
	TRACE(_T("in CXThemeHelper::OpenThemeData\n"));

	HTHEME hTheme = NULL;

	if (m_bThemeLibLoaded)
	{

#ifdef _UNICODE

		hTheme = m_OpenThemeData(hWnd, lpszClassList);

#else

		int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszClassList, 
			_tcslen(lpszClassList)+1, NULL, 0);
		nLen += 2;
		WCHAR * pszWide = new WCHAR [nLen];
		if (pszWide)
		{
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszClassList, 
				_tcslen(lpszClassList)+1, pszWide, nLen);
			hTheme = m_OpenThemeData(hWnd, pszWide);
			delete pszWide;
		}

#endif

	}

	TRACE(_T("m_hTheme=%X\n"), hTheme);

	return hTheme;
}

