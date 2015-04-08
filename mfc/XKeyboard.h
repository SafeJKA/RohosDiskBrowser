// XKeyboard.h  Version 1.0
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

#ifndef  XKEYBOARD_H 
#define  XKEYBOARD_H 

#include "XButtonXP.h"

///////////////////////////////////////////////////////////////////////////////
// CXKeyboard dialog

class CXKeyboard : public CDialog
{
// Construction
public:
	CXKeyboard(int xPos = -1,
			   int yPos = -1,
			   LPCTSTR lpszTitle = _T("Keyboard"), 
			   LPCTSTR lpszShowText = _T("Show text"), 
			   CWnd* pParent = NULL);

// Dialog Data
protected:
	//{{AFX_DATA(CXKeyboard)
	//enum { IDD = IDD_XKEYBOARD };	// use string form
	CButton			m_btnSpacebar;
	CButton			m_chkShowText;
	CEdit			m_edtPassword;
	CString			m_strPassword;
	CXButtonXP		m_btnRShift;
	CXButtonXP		m_btnLShift;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXKeyboard)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:

	enum XKEYBOARD_CASE
	{
		Upper,
		Lower,
		UpperOrLower
	};

	enum SPECIAL_CHARACTERS
	{
		Allow,
		DontAllow,
		RequireSpecial,
		RequireSpecialOrNumeric
	};

	void	DrawPasswChars();

	CString GetPassword()					{ return m_strPassword; }
	void SetPassword(LPCTSTR lpszPassword);

	BOOL GetShowText()						{ return m_bShowText; }
	void SetShowText(BOOL bShowText)		{ m_bShowText = bShowText; }

	BOOL GetShowSpacebar()					{ return m_bShowSpacebar; }
	void SetShowSpacebar(BOOL bShow);

	SPECIAL_CHARACTERS GetSpecial()					{ return m_eSpecial; }
	void SetSpecial(SPECIAL_CHARACTERS eSpecial)	{ m_eSpecial = eSpecial; }

	int GetMinLength()						{ return m_nMinLength; }
	void SetMinLength(int nMinLength)		{ m_nMinLength = nMinLength; }

	int GetMaxLength()						{ return m_nMaxLength; }
	void SetMaxLength(int nMaxLength)		{ m_nMaxLength = nMaxLength; }	// 0 = no maximum

	XKEYBOARD_CASE GetCase()				{ return m_eCase; }
	void SetCase(XKEYBOARD_CASE eCase);

	void SetMRUPasswords(CStringArray& saPasswords)
	{
		for (int i = 0; i < saPasswords.GetSize(); i++)
		{
			CString s = saPasswords[i];
			if (!s.IsEmpty())
				m_saPasswords.Add(s);
		}
	}
	void ClearMRUPasswords()				{ m_saPasswords.RemoveAll(); }

	int GetTimeout()						{ return m_nTimeout; }
	void SetTimeout(int nTimeout)			{ m_nTimeout = nTimeout; }

	BOOL GetReadOnly()						{ return m_bReadOnly; }
	void SetReadOnly(BOOL bReadOnly)		{ m_bReadOnly = bReadOnly; }


// virtual functions
public:
	virtual BOOL CheckSpecial(LPCTSTR lpszPassword, LPCTSTR lpszSpecial);
	virtual BOOL CheckMRUPasswords(LPCTSTR lpszPassword, CStringArray& saPasswords);

// Implementation
protected:
	int					m_xPos, m_yPos;
	int					m_nMinLength;
	int					m_nMaxLength;
	int					m_nTimeout;			// in seconds; 0 = no timeout
	BOOL				m_bShift;
	BOOL				m_bShowText;
	BOOL				m_bShowSpacebar;
	BOOL				m_bReadOnly;
	SPECIAL_CHARACTERS	m_eSpecial;
	XKEYBOARD_CASE		m_eCase;
	CString				m_strTitle;
	CString				m_strShowText;
	CStringArray		m_saPasswords;


	void SetButtons(BOOL bShift);
	void ShrinkDialog(CRect& rectDialog);

	// Generated message map functions
	//{{AFX_MSG(CXKeyboard)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnShowText();
	afx_msg void OnLshift();
	afx_msg void OnRshift();
	afx_msg void OnSpacebar();
	afx_msg void OnUpdatePassword();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar( UINT nChar, UINT nRepCnt,  UINT nFlags );
	//}}AFX_MSG
	afx_msg void OnKey(UINT nID);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // XKEYBOARD_H 

