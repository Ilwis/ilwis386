#pragma once

// custom window styles
#define FMC_MULTISELECT			0x0040L
#define FMC_NOHIGHLIGHTTODAY	0x0080L		
#define FMC_TODAYBUTTON			0x0100L	
#define FMC_NONEBUTTON			0x0200L	
#define FMC_AUTOSETTINGS		0x0400L
#define FMC_NO3DBORDER			0x0800L
#define FMC_NOSHOWNONMONTHDAYS	0x1000L


// I usually don't use preprocessor macros like this
// but it makes sense here
#define FMC_MAX_SIZE_NONE		CSize(-2,-2)
#define FMC_MAX_SIZE_PARENT		CSize(-1,-1)

// default font settings
#define FMC_DEFAULT_FONT			"Tahoma"
#define FMC_DEFAULT_FONT_SIZE		8
#define FMC_DEFAULT_FONT_SIZE_640	7
#define FMC_DEFAULT_FONT_SIZE_800	8
#define FMC_DEFAULT_FONT_SIZE_1024	10
#define FMC_DEFAULT_FONT_SIZE_1280	12
#define FMC_DEFAULT_MIN_FONT_SIZE	5										

// used to disable Maximum selection day rules
#define FMC_NO_MAX_SEL_DAYS			-1

// callback function (definition) for the IsSpecialDate
// function
typedef BOOL (CALLBACK* funcSPECIALDATE)(COleDateTime&);


// forward declration for list popup
class CalendarCtrlListCtrl;
/////////////////////////////////////////////////////////////////////////////
// CalendarCtrl window

class CalendarCtrlFontInfo
{
public:
	virtual void CreateFont(CDC* pDC);
	CalendarCtrlFontInfo() {m_pFont = NULL;}
	~CalendarCtrlFontInfo() {if (m_pFont) delete m_pFont;}

	CString m_strFontName;
	int m_iFontSize;
	BOOL m_bBold;
	BOOL m_bItalic;
	BOOL m_bUnderline;
	COLORREF m_cColor;
	CFont*	m_pFont;
};

class CalendarCtrlFontHotSpot
{
public:
	CalendarCtrlFontHotSpot() {};
	~CalendarCtrlFontHotSpot() {};

	CRect			m_rect;
	COleDateTime	m_dt;
};

class CalendarCtrlCell
{
public:
	CalendarCtrlCell();
	~CalendarCtrlCell();

	CRect m_rectHeader;
	int	  m_iRow;
	int	  m_iCol;

	CRect m_rect;
	
	CalendarCtrlFontHotSpot* m_parHotSpots;
};

class CalendarCtrl : public CWnd, public BaseZapp
{
// Construction
public:
	CalendarCtrl();

// Attributes
public:

// Operations
public:
	BOOL IsDateSelected(COleDateTime& dt);
	void AutoConfigure();
	void AutoSize();
	int ComputeTodayNoneHeight();
	CSize ComputeTotalSize();
	CSize ComputeSize();
	CSize Compute3DBorderSize();
	void FireNoneButton();
	void FireNotifyDblClick();
	void FireNotifyClick();
	void FireTodayButton();
	void ScrollRight(int iCount = 1);
	void ScrollLeft(int iCount = 1);
	void ClearSelections();
	CalendarCtrlFontHotSpot* HitTest(POINT& pt);
	CalendarCtrlCell* HeaderHitTest(POINT& point);

	COleDateTime GetDate();
	void SetDate(COleDateTime dt);

	void GetDateSel(COleDateTime& dtBegin, COleDateTime& dtEnd);
	void SetDateSel(COleDateTime dtBegin, COleDateTime dtEnd);

	void SetRowsAndColumns(int iRows, int iCols);
	int GetRows() {return m_iRows;}
	int GetCols() {return m_iCols;}

	void SetShowNonMonthDays(BOOL bValue) {m_bShowNonMonthDays = bValue;}
	BOOL GetShowNonMonthDays() {return m_bShowNonMonthDays;}

	void SetDefaultMinFontSize(int iValue);
	int GetDefaultMinFontSize() {return m_iDefaultMinFontSize;}

	void SetDefaultFont(LPCTSTR lpszValue);
	CString GetDefaultFont() {return m_strDefaultFontName;}

	void SetMonthName(int iMonth, LPCTSTR lpszName);
	CString GetMonthName(int iMonth);

	void SetDayOfWeekName(int iDayOfWeek, LPCTSTR lpszName);
	CString GetDayOfWeekName(int iDayOfWeek);

	void SetFirstDayOfWeek(int iDayOfWeek);
	int GetFirstDayOfWeek();

	void SetCurrentMonthAndYear(int iMonth, int iYear);
	int GetCurrentMonth() {return m_iCurrentMonth;}
	int GetCurrentYear() {return m_iCurrentYear;}

	void SetMultiSelect(BOOL bValue) {m_bMultiSel = bValue;}
	BOOL IsMultiSelect() {return m_bMultiSel;}

	void SetBackColor(COLORREF cColor) {m_cBackColor = cColor;}
	COLORREF GetBackColor() {return m_cBackColor;}

	void SetHighlightToday(BOOL bValue) {m_bHighlightToday = bValue;}	
	BOOL GetHighlightToday() {return m_bHighlightToday;}
	
	void SetShowTodayButton(BOOL bValue) {m_bShowTodayButton = bValue;}
	BOOL GetShowTodayButton() {return m_bShowTodayButton;}

	void SetShowNoneButton(BOOL bValue) {m_bShowNoneButton = bValue;}
	BOOL GetShowNoneButton() {return m_bShowNoneButton;}

	void SetNonMonthDayColor(COLORREF cColor) {m_cNonMonthDayColor = cColor;}
	COLORREF GetNonMonthDayColor(COLORREF cColor) {m_cNonMonthDayColor = cColor;}

	void SetShow3DBorder(BOOL bValue) {m_bShow3dBorder = bValue;}
	BOOL GetShow3DBorder() {return m_bShow3dBorder;}

	void SetMaxSize(SIZE size);
	CSize GetMaxSize() {return m_szMaxSize;}

	void SetUseAutoSettings(BOOL bValue) {m_bUseAutoSettings = bValue;}
	BOOL GetUseAutoSettings() {return m_bUseAutoSettings;}

	void SetMaxSelDays(int iValue) {m_iMaxSelDays = iValue;}
	int GetMaxSelDays() {return m_iMaxSelDays;}

	void SetSpecialDaysCallback(funcSPECIALDATE pValue);
	funcSPECIALDATE GetSpecialDaysCallback() {return m_pfuncCallback;}

	void SetHeaderFont(LPCTSTR lpszFont, int iSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cColor);
	CalendarCtrlFontInfo& GetHeaderFont() {return m_HeaderFontInfo;}

	void SetDaysOfWeekFont(LPCTSTR lpszFont, int iSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cColor);
	CalendarCtrlFontInfo& GetDaysOfWeekFont() {return m_DaysOfWeekFontInfo;}

	void SetSpecialDaysFont(LPCTSTR lpszFont, int iSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cColor);
	CalendarCtrlFontInfo& GetSpecialDaysFont() {return m_SpecialDaysFontInfo;}

	void SetDaysFont(LPCTSTR lpszFont, int iSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cColor);
	CalendarCtrlFontInfo& GetDaysFont() {return m_DaysFontInfo;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CalendarCtrl)
	public:
	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CalendarCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CalendarCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDevModeChange(LPTSTR lpDeviceName);
	afx_msg void OnFontChange();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnTimeChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int								m_iCurrentMonth;
	int								m_iCurrentYear;
	CalendarCtrlFontInfo	m_HeaderFontInfo;
	CalendarCtrlFontInfo	m_DaysOfWeekFontInfo;
	CalendarCtrlFontInfo	m_DaysFontInfo;
	CalendarCtrlFontInfo	m_SpecialDaysFontInfo;
	COLORREF						m_cBackColor;
	BOOL							m_bHighlightToday;
	BOOL							m_bShowTodayButton;
	BOOL							m_bShowNoneButton;
	COLORREF						m_cNonMonthDayColor;
	CString							m_arMonthNames[12];
	CString							m_arDaysOfWeekNames[7];
	int								m_iFirstDayOfWeek;
	int								m_iRows;
	int								m_iCols;
	BOOL							m_bMultiSel;
	BOOL							m_bShow3dBorder;
	BOOL							m_bUseAutoSettings;
	CSize							m_szMaxSize;
	CString							m_strDefaultFontName;
	int								m_iDefaultMinFontSize;
	int								m_iMaxSelDays;
	BOOL							m_bShowNonMonthDays;
	funcSPECIALDATE					m_pfuncCallback;

	BOOL							m_bTracking;
	COleDateTime					m_dtTrackingStart;
	BOOL							m_bScrollTracking;
	BOOL							m_bTodayTracking;
	BOOL							m_bTodayDown;
	BOOL							m_bNoneTracking;
	BOOL							m_bNoneDown;
	BOOL							m_bHeaderTracking;
	CalendarCtrlListCtrl*		m_pHeaderList;
	UINT							m_iHeaderTimerID;
	CalendarCtrlCell*		m_pHeaderCell;

	CRect							m_rectScrollLeft;
	CRect							m_rectScrollRight;
	CRect							m_rectToday;
	CRect							m_rectNone;

protected:
	BOOL IsSpecialDate(COleDateTime& dt);
	void AllocateCells();
	void SetCellHeaderPosition(int iMonthRow, int iMonthCol, RECT rect);
	void SetCellPosition(int iMonthRow, int iMonthCol, RECT rect);
	void SetHotSpot(int iMonthRow, int iMonthCol, int iDayCounter, COleDateTime& dt, RECT rect);
	void ClearHotSpots();
	int DrawTodayButton(CDC& dc, int iY);
	BOOL IsToday(COleDateTime& dt);
	CString CStr(long lValue);
	void CreateFontObjects();
	int DrawHeader(CDC& dc, int iY, int iLeftX, int iRow, int iCol, int iMonth, int iYear);
	int DrawDays(CDC& dc, int iY, int iLeftX, int iRow, int iCol, int iMonth, int iYear);
	int DrawDaysOfWeek(CDC& dc, int iY, int iLeftX, int iRow, int iCol);

	// computed values of importance
	BOOL							m_bFontsCreated;
	BOOL							m_bSizeComputed;
	int								m_iHeaderHeight;
	int								m_iIndividualDayWidth;
	int								m_iDaysOfWeekHeight;
	int								m_iDaysHeight;
	CSize							m_szMonthSize;

	// cells
	COleDateTime					m_dtSelBegin;
	COleDateTime					m_dtSelEnd;
	CalendarCtrlCell*		m_parCells;
	int								m_iCells;
};

class _export CalendarSelect: public FormEntry
{
public:
  ~CalendarSelect();
  CalendarSelect(FormEntry* par);
  void show(int);
  virtual void StoreData() {};
  void setDirty() { calendar->Invalidate(); }
  bool fValid();
  COleDateTime GetDate();
  void SetDate(const ILWIS::Time& date);

private:
	CalendarCtrl *calendar;
	ILWIS::Time tempDate;

	void create();
	void SetFocus() { if(calendar) calendar->SetFocus(); } // overriden
	virtual String sName(int id) { return String();}
};



