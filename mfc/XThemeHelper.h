// XThemeHelper.h  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This software is released into the public domain.  You are free to use
// it in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this
// software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef  XTHEMEHELPER_H
#define  XTHEMEHELPER_H

#include "uxtheme.h"
#include "tmschema.h"


///////////////////////////////////////////////////////////////////////////////
//
// typedefs for uxtheme functions
//
typedef HRESULT	(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT	(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc,
								int iPartId, int iStateId,
								const RECT *pRect,  const RECT *pClipRect);
typedef HRESULT	(__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc,
								int iPartId, int iStateId, LPCWSTR pszText,
								int iCharCount, DWORD dwTextFlags,
								DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT	(__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,
								HDC hdc, int iPartId, int iStateId,
								const RECT *pBoundingRect,
								RECT *pContentRect);
typedef BOOL	(__stdcall *PFNISAPPTHEMED)();
typedef BOOL	(__stdcall *PFNISTHEMEACTIVE)();
typedef HTHEME	(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd,
								LPCWSTR pszClassList);


///////////////////////////////////////////////////////////////////////////////
//
// CXThemeHelper class definition
//
class CXThemeHelper
{
// Construction
public:
	CXThemeHelper();
	virtual ~CXThemeHelper();

// Attributes
public:
	BOOL IsAppThemed();
	BOOL IsThemeActive();
	BOOL IsThemeLibAvailable() { return m_bThemeLibLoaded; }

// Operations
public:
	BOOL	CloseThemeData(HTHEME hTheme);
	BOOL	DrawThemeBackground(HTHEME hTheme,
								HDC hdc,
								int iPartId,
								int iStateId,
								const RECT *pRect,
								const RECT *pClipRect);
	BOOL	DrawThemeText(HTHEME hTheme,
						  HDC hdc,
						  int iPartId,
						  int iStateId,
						  LPCTSTR lpszText,
						  DWORD dwTextFlags,
						  DWORD dwTextFlags2,
						  const RECT *pRect);
	BOOL	GetThemeBackgroundContentRect(HTHEME hTheme,
										  HDC hdc,
										  int iPartId,
										  int iStateId,
										  const RECT *pBoundingRect,
										  RECT *pContentRect);
	HTHEME	OpenThemeData(HWND hWnd, LPCTSTR lpszClassList);

// Implementation
private:
	static PFNCLOSETHEMEDATA				m_CloseThemeData;
	static PFNDRAWTHEMEBACKGROUND			m_DrawThemeBackground;
	static PFNDRAWTHEMETEXT					m_DrawThemeText;
	static PFNGETTHEMEBACKGROUNDCONTENTRECT	m_GetThemeBackgroundContentRect;
	static PFNISAPPTHEMED					m_IsAppThemed;
	static PFNISTHEMEACTIVE					m_IsThemeActive;
	static PFNOPENTHEMEDATA					m_OpenThemeData;

	static HMODULE							m_hThemeLib;
	static BOOL								m_bThemeLibLoaded;

	static int								m_nUseCount;

	void	Init();
};

///////////////////////////////////////////////////////////////////////////////
//
// CXThemeHelper instance
//
#ifndef XTHEMEHELPER_CPP
// include an instance in each file;  the namespace insures uniqueness
namespace { CXThemeHelper ThemeHelper; }
#endif

#endif // XTHEMEHELPER_H
