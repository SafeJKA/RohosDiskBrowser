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
** Contact info@rohos.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#if !defined(AFX_TL_NTFSREADER_H__74F5EF7E_B86E_436A_92D1_749F68B4A3CF__INCLUDED_)
#define AFX_TL_NTFSREADER_H__74F5EF7E_B86E_436A_92D1_749F68B4A3CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDiskBrowserApp:
// See CDiskBrowserApp.cpp for the implementation of this class
//

class CDiskBrowserApp : public CWinApp
{
public:
	CDiskBrowserApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTL_NTFSREADERApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTL_NTFSREADERApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TL_NTFSREADER_H__74F5EF7E_B86E_436A_92D1_749F68B4A3CF__INCLUDED_)
