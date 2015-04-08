#if !defined(AFX_PPNUMEDIT_H__5B576841_5DBF_11D6_98A4_00C026A7402A__INCLUDED_)
#define AFX_PPNUMEDIT_H__5B576841_5DBF_11D6_98A4_00C026A7402A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPNumEdit.h : header file
//
#define UDM_PPNUMEDIT				(WM_USER + 102)

#define UDM_PPNUMEDIT_CANCEL		(UDM_PPNUMEDIT)
#define UDM_PPNUMEDIT_ENTER  		(UDM_PPNUMEDIT + 1)
#define UDM_PPNUMEDIT_MOVE  		(UDM_PPNUMEDIT + 2)
#define UDM_PPNUMEDIT_HOTKEY   		(UDM_PPNUMEDIT + 3)

// This structure sent to PPNumEdit parent in a WM_NOTIFY message
//##ModelId=3EACE5D90200
typedef struct tagNM_PPNUM_EDIT {
    NMHDR hdr;
	UINT  iEvent;
    UINT  iValue;
} NM_PPNUM_EDIT;

/////////////////////////////////////////////////////////////////////////////
// CPPNumEdit window

//##ModelId=3EACE5D90319
class CPPNumEdit : public CEdit
{
// Construction
public:
	enum {	PPNUM_VALUE_DEC = 0, //Type value
			PPNUM_VALUE_HEX,
			PPNUM_VALUE_BIN,
			PPNUM_VALUE_OCT,
			PPNUM_VALUE_ASCII
		};
	enum {	PPNUM_VALUE_CUSTOM = 0, //Size value
			PPNUM_VALUE_BYTE,
			PPNUM_VALUE_WORD,
		};
	enum {	PPNUM_COLOR_VALID_FG = 0, //Index of the colours
			PPNUM_COLOR_VALID_BK,
			PPNUM_COLOR_NOT_VALID_FG,
			PPNUM_COLOR_NOT_VALID_BK
		};

	enum {	PPNUMEDIT_CANCEL_DATA = 0, //The events
			PPNUMEDIT_ENTER_DATA,

			PPNUMEDIT_MOVE_FIRST_DATA,
			PPNUMEDIT_MOVE_END_DATA,
			PPNUMEDIT_MOVE_BEGIN_LINE,
			PPNUMEDIT_MOVE_END_LINE,
			PPNUMEDIT_MOVE_LEFT,
			PPNUMEDIT_MOVE_RIGHT,
			PPNUMEDIT_MOVE_UP,
			PPNUMEDIT_MOVE_DOWN,
			PPNUMEDIT_MOVE_NEXT_FIELD,
			PPNUMEDIT_MOVE_PREV_FIELD,
			PPNUMEDIT_MOVE_PAGE_UP,
			PPNUMEDIT_MOVE_PAGE_DOWN,

			PPNUMEDIT_HOTKEY_HEX,
			PPNUMEDIT_HOTKEY_DEC,
			PPNUMEDIT_HOTKEY_BIN,
			PPNUMEDIT_HOTKEY_OCT,
			PPNUMEDIT_HOTKEY_ASCII,
			
			PPNUMEDIT_MAX_EVENTS
		};

	//##ModelId=3EACE5D9032D
		CPPNumEdit();


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPPNumEdit)
	public:
	//##ModelId=3EACE5D9032E
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//##ModelId=3EACE5D90337
	virtual BOOL DestroyWindow();
	protected:
	//##ModelId=3EACE5D90339
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3EACE5D90342
	BOOL	m_bNotificate; //User wants be notificate

	//##ModelId=3EACE5D9034B
	void	SetValue(UINT nValue, UINT nIndexValue = PPNUM_VALUE_HEX, UINT nTypeValue = PPNUM_VALUE_BYTE, UINT nMin = 0, UINT nMax = 0xFF); //Set Edited Value
	//##ModelId=3EACE5D90355
	UINT	GetValue(); //Gets Current Value

	//##ModelId=3EACE5D90356
	BOOL	IsValidate(); //Test validate of data
	//##ModelId=3EACE5D90357
	BOOL    IsChanged(); //Test changed of data

	//##ModelId=3EACE5D90358
	void	SetDefaultColors(BOOL bRedraw = TRUE); //Sets default colors
	//##ModelId=3EACE5D90360
	void	SetColor(UINT nIndex, COLORREF crColor, BOOL bRedraw = TRUE); //Sets colors of the control
	//##ModelId=3EACE5D90369
	virtual ~CPPNumEdit();

	// Generated message map functions
protected:
	//##ModelId=3EACE5D9036B
	CString m_strText; //the string of the current value
	//##ModelId=3EACE5D90373
	CString m_strOriginal; //the original string of the current value
	//##ModelId=3EACE5D9037D
	UINT m_nValue; //Edited value
	//##ModelId=3EACE5D9037E
	UINT m_nIndexValue; //The index of the value (address, dec, hex, bin, oct, ascii)
	//##ModelId=3EACE5D90387
	UINT m_nMaxLimit; //The max limit of the value
	//##ModelId=3EACE5D90391
	UINT m_nMinLimit; //The min limit of the value
	//##ModelId=3EACE5D9039B
	UINT m_nCharsInValue; //How much the chars of the string value;

	//##ModelId=3EACE5D9039C
	BOOL m_bValidValue;

	//##ModelId=3EACE5D903A5
	COLORREF m_crValidBk; //Color of the background
	//##ModelId=3EACE5D903AF
	COLORREF m_crValidFg; //Color of the foreground
	//##ModelId=3EACE5D903B9
	COLORREF m_crNotValidFg;
	//##ModelId=3EACE5D903BA
	COLORREF m_crNotValidBk;

	//##ModelId=3EACE5D903CE
	CBrush m_brValidBk;
	//##ModelId=3EACE5D903D8
	CBrush m_brNotValidBk; //Color background

	//##ModelId=3EACE5D903E1
	LRESULT SendNotify(UINT uNotifyCode);
	//##ModelId=3EACE5DA0003
	BOOL GetNotify();

	//{{AFX_MSG(CPPNumEdit)
	//##ModelId=3EACE5DA0004
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//##ModelId=3EACE5DA000F
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//##ModelId=3EACE5DA0021
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//##ModelId=3EACE5DA002C
	afx_msg void OnUpdate();
	//##ModelId=3EACE5DA0035
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPNUMEDIT_H__5B576841_5DBF_11D6_98A4_00C026A7402A__INCLUDED_)
