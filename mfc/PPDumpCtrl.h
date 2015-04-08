#if !defined(AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_)
#define AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPDumpCtrl.h : header file
//
#include "PPNumEdit.h"
#include <afxtempl.h>

#define PPDUMPCTRL_CLASSNAME    _T("MFCDumpCtrl")  // Window class name


#define UDM_PPDUMPCTRL_FIRST		 (WM_USER + 100)
#define UDM_PPDUMPCTRL_CHANGE_DATA	 (UDM_PPDUMPCTRL_FIRST) //User was changed the data
#define UDM_PPDUMPCTRL_BEGIN_ADDR	 (UDM_PPDUMPCTRL_FIRST + 1) //User was changed the first address on the screen
#define UDM_PPDUMPCTRL_MENU_CALLBACK (UDM_PPDUMPCTRL_FIRST + 2) //User want displaying the menu
#define UDM_PPDUMPCTRL_SELECTION	 (UDM_PPDUMPCTRL_FIRST + 3) //User want displaying the menu

//Flags of the style

//The available fields of the data
#define PPDUMP_FIELD_ADDRESS	0x00000001	// Address field is exist
#define PPDUMP_FIELD_HEX		0x00000002	// DEC field is exist
#define PPDUMP_FIELD_DEC    	0x00000004	// HEX field is exist
#define PPDUMP_FIELD_BIN    	0x00000008	// BIN field is exist
#define PPDUMP_FIELD_OCT		0x00000010	// OCT field is exist
#define PPDUMP_FIELD_ASCII  	0x00000020	// ASCII field is exist
//------------------------------------------------------------
#define PPDUMP_FIELD_ALL		0x0000003F	// All fields is exist

//The available fields of the control bar
#define PPDUMP_BAR_ADDRESS		0x00000100	// Address field on the bar is exist
#define PPDUMP_BAR_HEX			0x00000200	// DEC field on the bar is exist
#define PPDUMP_BAR_DEC    		0x00000400	// HEX field on the bar is exist
#define PPDUMP_BAR_BIN    		0x00000800	// BIN field on the bar is exist
#define PPDUMP_BAR_OCT			0x00001000	// OCT field on the bar is exist
#define PPDUMP_BAR_ASCII  		0x00002000	// ASCII field on the bar is exist
//------------------------------------------------------------
#define PPDUMP_BAR_ALL			0x00003F00	// The control bar have all fields 

//The additional styles
#define PPDUMP_SEPARATOR_LINES	0x00010000	// The separators is drawn
#define PPDUMP_WORD_DATA      	0x00020000	// 16-bit data
#define PPDUMP_NAMED_FIELDS   	0x00040000	// The names of the fields of the data is drawn
#define PPDUMP_READ_ONLY      	0x00080000	// Data for read only
#define PPDUMP_SELECTING_DATA  	0x00100000	// Enable selecting data
#define PPDUMP_DATA_LOW_HIGH	0x00200000  // The direction of the data in word data (High->Low or Low->High)
#define PPDUMP_TRACK_MOUSE_MOVE	0x00400000  // Enables draw the track rectangle around the mouse

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
//##ModelId=3EACE5DA0049
typedef struct tagNM_PPDUMP_CTRL {
    NMHDR hdr;
	int   iAddress;
    UINT  iValue;
} NM_PPDUMP_CTRL;

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
//##ModelId=3EACE5DA0071
typedef struct tagNM_PPDUMP_SEL {
    NMHDR hdr;
	int   iFirstAddr;
    int   iLastAddr;
} NM_PPDUMP_SEL;

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
//##ModelId=3EACE5DA0099
typedef struct tagNM_PPDUMP_MENU {
    NMHDR hdr;
	HMENU hMenu;
    int   iArea;
	int   iAddress;
} NM_PPDUMP_MENU;

/////////////////////////////////////////////////////////////////////////////
// CPPDumpCtrl window

//##ModelId=3EACE5E1007B
class CPPDumpCtrl : public CWnd
{
// Construction
public:
	enum {	PPDUMP_BAR_AREA_ADDRESS = 0, //The index of the fields
			PPDUMP_BAR_AREA_HEX,
			PPDUMP_BAR_AREA_DEC,
			PPDUMP_BAR_AREA_BIN,
			PPDUMP_BAR_AREA_OCT,
			PPDUMP_BAR_AREA_ASCII,

			PPDUMP_BAR_MAX_AREAS
		};

	enum {	PPDUMP_COLOR_DATA_FG = 0, //The index of the colors
			PPDUMP_COLOR_DATA_BK,
			PPDUMP_COLOR_DATA_CHANGE_FG,
			PPDUMP_COLOR_EDIT_FG,
			PPDUMP_COLOR_EDIT_BK,
			PPDUMP_COLOR_EDIT_ERR_FG,
			PPDUMP_COLOR_EDIT_ERR_BK,
			PPDUMP_COLOR_CARET_BK,
			PPDUMP_COLOR_ADDRESS_FG,
			PPDUMP_COLOR_SEPARATORS,
			PPDUMP_COLOR_TEXT_HEADER,
			PPDUMP_COLOR_MOUSE_TRACK,

			PPDUMP_COLOR_MAX
		};

	//##ModelId=3EACE5E10090
	CPPDumpCtrl();

protected:
		enum {	PPDUMP_CANCEL_DATA = 0, //The events
				PPDUMP_ENTER_DATA,

				PPDUMP_MOVE_FIRST_DATA,
				PPDUMP_MOVE_LAST_DATA,
				PPDUMP_MOVE_BEGIN_LINE,
				PPDUMP_MOVE_END_LINE,
				PPDUMP_MOVE_LEFT,
				PPDUMP_MOVE_RIGHT,
				PPDUMP_MOVE_UP,
				PPDUMP_MOVE_DOWN,
				PPDUMP_MOVE_NEXT_FIELD,
				PPDUMP_MOVE_PREV_FIELD,
				PPDUMP_MOVE_PAGE_UP,
				PPDUMP_MOVE_PAGE_DOWN,

				PPDUMP_HOTKEY_HEX,
				PPDUMP_HOTKEY_DEC,
				PPDUMP_HOTKEY_BIN,
				PPDUMP_HOTKEY_OCT,
				PPDUMP_HOTKEY_ASCII,
				
				PPDUMP_MAX_EVENTS
		};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPPDumpCtrl)
	public:
	//##ModelId=3EACE5E10091
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3EACE5E1009B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//##ModelId=3EACE5E100A4
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3EACE5E100AD
	virtual ~CPPDumpCtrl();

	// Generated message map functions
protected:
	//##ModelId=3EACE5E100B7
	COLORREF    m_crColor [PPDUMP_COLOR_MAX]; //The colors
	//##ModelId=3EACE5E100C1
	COLORREF    m_crDisableFg; //Color for disable foreground
	//##ModelId=3EACE5E100C2
	COLORREF	m_crDisableBk; //Color for disable background
	
	//##ModelId=3EACE5E100CB
	UINT		m_nStyle; //Current style of the control
	
	//##ModelId=3EACE5E100D5
	LPBYTE		m_pNewData;	//Pointer to the new array
	//##ModelId=3EACE5E100E0
	LPBYTE		m_pOldData;	//Pointer to the old array
	
	//##ModelId=3EACE5E100E1
	int			m_nRealLengthData; //Real lenght of the data of the array
	//##ModelId=3EACE5E100EA
	int			m_nLengthData; //Work lenght of the data array
	//##ModelId=3EACE5E100FE
	int			m_nOffsetAddress; //Offset viewing address
	//##ModelId=3EACE5E100FF
	int			m_nDataInLines; //how much data in the line
	//##ModelId=3EACE5E10108
	int			m_nMaxDataOnScreen; //how much data may be on the screen

	//##ModelId=3EACE5E10112
	int			m_nBeginAddress;  //The first address on the screen
	//##ModelId=3EACE5E1011C
	int			m_nEditedAddress; //The address of the editing data
	//##ModelId=3EACE5E1011D
	int			m_nCurrentAddr;   //The address under cursor
	//##ModelId=3EACE5E10126
	int         m_nCaretAddrBegin;  //The current address of the caret
	//##ModelId=3EACE5E10130
	int         m_nCaretAddrEnd;  //The current address of the caret
	//##ModelId=3EACE5E1013A
	int         m_nCaretAddrFirst; //The address which started selection data
	//##ModelId=3EACE5E10144
	int			m_nAddressToolTip; //The address which tooltip show last
	
	//##ModelId=3EACE5E10145
	int         m_nEditedArea; //The area where data is editing
	//##ModelId=3EACE5E101B2
	int			m_nCurArea; //The area under cursor
	
	//##ModelId=3EACE5E101BC
	TCHAR		m_chSpecCharView; //The char which will change special characters (0 - 31) 
	//##ModelId=3EACE5E101BD
	BOOL        m_bMouseOverCtrl; //the mouse over control
	//##ModelId=3EACE5E101D0
	CString		m_sFormatToolTip; //the format string for tooltip

	//##ModelId=3EACE5E101D1
	BOOL		m_bFocused; //The control was focused
	//##ModelId=3EACE5E101DA
	BOOL		m_bPressedLButton;

	//##ModelId=3EACE5E1020D
	CFont	m_font;	//font
	//##ModelId=3EACE5E10220
	int		m_nWidthFont; //Average width of the characters
	//##ModelId=3EACE5E1022A
	int		m_nHeightFont; //Height one line

	//##ModelId=3EACE5E10234
	BYTE	m_nCharsData [PPDUMP_BAR_MAX_AREAS]; //how much chars in the every data fields

	//##ModelId=3EACE5E1023E
	CRect	m_rLastTrackRect; //last track rect of the cursor if exist, else EMPTY
	//##ModelId=3EACE5E1023F
	CRect   m_rBarArea [PPDUMP_BAR_MAX_AREAS + 1]; //The rect of the fields in the control bar
	//##ModelId=3EACE5E10248
	CRect   m_rFieldArea [PPDUMP_BAR_MAX_AREAS + 1]; //The rect of the data fields

	//##ModelId=3EACE5E1025D
	CScrollBar   m_pVScroll;    // The object of the vertical scrollbar
	//##ModelId=3EACE5E10267
	CPPNumEdit * m_pEdit;     // Editor of the Value
	//##ModelId=3EACE5E1027B
	CToolTipCtrl m_pToolTip;	// Tooltip

	//##ModelId=3EACE5E10284
	HMENU		m_hMenu;			// Handle to associated menu
	//##ModelId=3EACE5E1028E
	HWND	    m_hParentWnd; // Handle to window for notification about change data

	//procedures
	//##ModelId=3EACE5E102A2
	void  SetCharsInData(); //Sets much chars in the data of the fields
	//##ModelId=3EACE5E102A3
	DWORD GetDataFromCurrentAddress(DWORD nAddress, BOOL bNewData = TRUE, int nDir = 0, int nByte = 0); //Gets the data from the current address
	//##ModelId=3EACE5E102B7
	int   CalculateControlBar(CRect rect); //Calculates the rect of the data on the control bar

	//##ModelId=3EACE5E102C1
	void VerticalSplitClientArea(CDC * pDC, CRect rect); 
	//##ModelId=3EACE5E102D4
	int  DrawControlBar(CDC * pDC, CRect rect, BOOL bDisable = FALSE); //Draws the control bar
	//##ModelId=3EACE5E102DF
	int  DrawStatusBar(CDC * pDC, CRect rect, BOOL bDisable = FALSE); //Draws the status bar
	//##ModelId=3EACE5E102F3
	void UpdateControlBar(CDC * pDC = NULL, BOOL bDisable = FALSE, int nNewAddr = -1); //Update the control bar
	//##ModelId=3EACE5E10306
	void RecalculateWorkData(DWORD nNewStyle); //Recalculate work length data array

	//##ModelId=3EACE5E10310
	void DrawAddressField(CDC * pDC, BOOL bDisable); //Draws the address field
	//##ModelId=3EACE5E1031B
	void DrawHexField(CDC * pDC, BOOL bDisable); //Draws the address field
	//##ModelId=3EACE5E10326
	void DrawDecField(CDC * pDC, BOOL bDisable); //Draws the address field
	//##ModelId=3EACE5E10338
	void DrawBinField(CDC * pDC, BOOL bDisable); //Draws the address field
	//##ModelId=3EACE5E10343
	void DrawOctField(CDC * pDC, BOOL bDisable); //Draws the address field
	//##ModelId=3EACE5E10356
	void DrawASCIIField(CDC * pDC, BOOL bDisable); //Draws the address field
	
	//-------------------------------------------------------------------------
	//##ModelId=3EACE5E10360
	int DrawAddressValue(CDC * pDC, CRect rect, UINT nAddress, BOOL bBkgnd = FALSE, BOOL bDisable = FALSE); //Draws Address 
	//##ModelId=3EACE5E10375
	int DrawHexValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Hex Value
	//##ModelId=3EACE5E10394
	int DrawDecValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Dec Value
	//##ModelId=3EACE5E103B2
	int DrawOctValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Oct Value
	//##ModelId=3EACE5E103CF
	int DrawBinValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Bin Value
	//##ModelId=3EACE5E20005
	int DrawAsciiValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draw ASCII

	//##ModelId=3EACE5E20023
	int DrawStringValue(CDC * pDC, CRect rect, CString str, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws the string of the value
	
	//-------------------------------------------------------------------------
	//##ModelId=3EACE5E20041
	BOOL TrackDataField(UINT nIndex, CPoint point); //Track the data of the field
	//##ModelId=3EACE5E2004C
	BOOL TrackDataAsciiField(CPoint point); //Track the data of the ascii field

	//##ModelId=3EACE5E20056
	BOOL CompleteEditValue(UINT nValue, BOOL bDelete = TRUE);
	//##ModelId=3EACE5E20069
	void SetEditedValue(int nAddr = -1, int nArea = -1);

	//##ModelId=3EACE5E20073
	int  GetNextField(int nArea);
	//##ModelId=3EACE5E2007D
	int  GetPrevField(int nArea);
	//##ModelId=3EACE5E20087
	BOOL IsExistField(int nArea);

	//##ModelId=3EACE5E20091
	CRect GetRectAddress(int nAddress, int nArea); //Gets the rect of the data 
	//##ModelId=3EACE5E2009C
	int   GetMaxDataOnScreen(); //How much data on the screen
	//##ModelId=3EACE5E200A5
	int   GetAddressForControlBar(); //Gets address of the data in the control bar
	//##ModelId=3EACE5E200AF
	BOOL  KillEdit(); //If exist edit the kill him

	//##ModelId=3EACE5E200B0
	LRESULT SendNotify(UINT uNotifyCode, UINT nAddress, UINT nValue); //Send notify to the parent
	//##ModelId=3EACE5E200C3
	BOOL    HandleHotkeys(UINT uNotifyCode, BOOL bFromEdit = FALSE, UINT nValue = 0); //Handled the pressed hotkeys
	
	//##ModelId=3EACE5E200EB
	void TrackMouseMove(BOOL bDrawn = FALSE, CDC * pDC = NULL); //Draws the mouse track
	//##ModelId=3EACE5E200F5
	void MoveCaretAddress(int nIndex, BOOL bEdited = FALSE, UINT nValue = 0); //Moving the caret
	//##ModelId=3EACE5E20113
	BOOL SetVisibleAddress (int nAddress);

	//##ModelId=3EACE5E2011D
	void    InitToolTip();
	//##ModelId=3EACE5E20127
	CString GetToolTipString(int nAddress);
	//##ModelId=3EACE5E20131
	CString FormatingString(int nIndex, int nValue, int nLength = 0);
	//##ModelId=3EACE5E20145
	int     GetDataUnderCursor(CPoint pt, LPINT nAddress);

	//##ModelId=3EACE5E20163
	BOOL RegisterWindowClass();
	//##ModelId=3EACE5E20164
	BOOL Initialise();

public:
	//Create
	//##ModelId=3EACE5E2016D
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	//Functions for the fonts
	//##ModelId=3EACE5E20182
	BOOL  SetFont(CFont & font, BOOL bRedraw = TRUE); //set font
	//##ModelId=3EACE5E20195
	BOOL  SetFont(LOGFONT & lf, BOOL bRedraw = TRUE); //set font
	//##ModelId=3EACE5E2019F
	BOOL  SetFont(LPCTSTR lpszFaceName, int nSizePoints = 8,
									BOOL bUnderline = FALSE, BOOL bBold = FALSE,
									BOOL bStrikeOut = FALSE, BOOL bItalic = FALSE, BOOL bRedraw = TRUE); //set font
	//##ModelId=3EACE5E201BD
	void  SetDefaultFont(BOOL bRedraw = TRUE); //set default fonts

	//Functions for the colors
	//##ModelId=3EACE5E201BF
	COLORREF SetColor(int nIndex, COLORREF crColor, BOOL bRedraw = TRUE); //Change color
	//##ModelId=3EACE5E201D1
	COLORREF GetColor(int nIndex); //Gets color
	//##ModelId=3EACE5E201DB
	void	 SetDefaultColors(BOOL bRedraw = TRUE); //Sets default colors

	//Functions for the data arrays
	//##ModelId=3EACE5E201DD
	void  SetPointerData(DWORD nLength, LPBYTE pNewData, LPBYTE pOldData = NULL, BOOL bRedraw = TRUE); 
	//##ModelId=3EACE5E201F0
	void  SetOffsetViewAddress(int nAddress = 0, BOOL bRedraw = TRUE); //Set o
	
	//##ModelId=3EACE5E201FA
	void  SetBeginAddress(int nAddress = 0, BOOL bRedraw = TRUE); //Sets the begin address
	//##ModelId=3EACE5E20205
	int   GetBeginAddress(); //Gets the begin address 

	//Functions for the styles
	//##ModelId=3EACE5E2020D
	void  SetStyles(DWORD nStyle, BOOL bRedraw = TRUE); //Set New Style
	//##ModelId=3EACE5E20217
	void  ModifyStyles(DWORD nAddStyle, DWORD nRemoveStyle, BOOL bRedraw = TRUE); //Modify styles
	//##ModelId=3EACE5E20223
	DWORD GetStyles(); //Get current Styles
	//##ModelId=3EACE5E2022B
	void  SetDefaultStyles(BOOL bRedraw = TRUE); //Set default styles

	//##ModelId=3EACE5E2022D
	void  SetSpecialCharView(TCHAR chSymbol = NULL, BOOL bRedraw = TRUE);

	//##ModelId=3EACE5E2023F
	void  SetReadOnly(BOOL bReadOnly = TRUE);
	//##ModelId=3EACE5E20241
	BOOL  IsReadOnly();
	
	//##ModelId=3EACE5E20249
	void  SetSelectRange(int nBegin = 0, int nEnd = -1, BOOL bVisible = TRUE); //Sets the address of the caret
	//##ModelId=3EACE5E20254
	void  GetSelectRange(LPINT nBegin, LPINT nEnd); //Gets the address of the caret
	//##ModelId=3EACE5E2025E
	BOOL  IsAddressSelected(int nAddress);	
	//##ModelId=3EACE5E20268
	void  EnableSelect(BOOL bEnable = TRUE);
	//##ModelId=3EACE5E20272
	BOOL  IsEnableSelect();
	
	//##ModelId=3EACE5E20273
	BOOL  SetMenu(UINT nMenu, BOOL bRedraw = TRUE);

	//##ModelId=3EACE5E2027C
	void  SetNotify(HWND hWnd);
	//##ModelId=3EACE5E20286
	void  SetNotify(BOOL bNotify = TRUE);
	//##ModelId=3EACE5E20288
	BOOL  GetNotify(); //Is enabled notification

	//##ModelId=3EACE5E20290
	void  SetTooltipText(int nText, BOOL bActivate = TRUE);
	//##ModelId=3EACE5E2029A
	void  SetTooltipText(CString sFormatTip, BOOL bActivate = TRUE);
	//##ModelId=3EACE5E202A4
	void  ActivateTooltip(BOOL bActivate = TRUE);
	//##ModelId=3EACE5E202A6
	CToolTipCtrl * GetTooltip();

	//##ModelId=3EACE5E202AE
	void  SetTrackMouseMove(BOOL bTrack = TRUE, BOOL bRedraw = TRUE);
	//##ModelId=3EACE5E202B8
	BOOL  IsTrackMouseMove();

	//{{AFX_MSG(CPPDumpCtrl)
	//##ModelId=3EACE5E202B9
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//##ModelId=3EACE5E202C2
	afx_msg void OnPaint();
	//##ModelId=3EACE5E202CC
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//##ModelId=3EACE5E202E0
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//##ModelId=3EACE5E202EA
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//##ModelId=3EACE5E202F4
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//##ModelId=3EACE5E202FF
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//##ModelId=3EACE5E20312
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//##ModelId=3EACE5E20326
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//##ModelId=3EACE5E2033A
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//##ModelId=3EACE5E20344
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//##ModelId=3EACE5E20358
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//##ModelId=3EACE5E2036C
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=3EACE5E20380
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	//##ModelId=3EACE5E2038A
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//##ModelId=3EACE5E20394
    afx_msg void NotifyEditEnter(NMHDR * pNMHDR, LRESULT * result);
	//##ModelId=3EACE5E203A8
	afx_msg void NotifyEditCancel(NMHDR * pNMHDR, LRESULT * result);
	//##ModelId=3EACE5E203B2
	afx_msg void NotifyEditMove(NMHDR * pNMHDR, LRESULT * result);
	//##ModelId=3EACE5E203BD
	afx_msg void NotifyEditHotKeys(NMHDR * pNMHDR, LRESULT * result);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_)
