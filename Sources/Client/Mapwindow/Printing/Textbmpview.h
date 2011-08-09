// textbmpView.h : interface of the CTextbmpView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTBMPVIEW_H__7D9EB8AD_10D8_11D2_9CDA_64D903C10627__INCLUDED_)
#define AFX_TEXTBMPVIEW_H__7D9EB8AD_10D8_11D2_9CDA_64D903C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//m_Zoomstate values
#define ZOOMFITHEIGHT 0
#define ZOOMFITWIDTH 1
#define ZOOMIN 2

#define ID_VIEW_FITWIDTH                32771
#define ID_VIEW_FITHEIGHT               32772
#define ID_VIEW_ZOOM150                 32781
#define ID_VIEW_ZOOM75                  32782
#define ID_VIEW_ZOOM125                 32783
#define ID_VIEW_ZOOM100                 32784
#define ID_FILE_MARGINS                 32780


class CTextbmpView : public CScrollView
{
protected: // create from serialization only
	CTextbmpView();
	DECLARE_DYNCREATE(CTextbmpView)

// Attributes
public:
	CTextbmpDoc* GetDocument();
	
protected:
	//Basic Variables
	CSize m_PaperSizeLog;  //size of paper in logical units
	CSize m_PaperSizeDev;  //size of paper in device units
	CSize m_PageSizeLog;   //size of printable area in logical units
	CSize m_PageSizeDev;   //size of printable area in device units
	CPoint m_OffsetLog;    //Top-Left point of printable area in logical units
	CPoint m_OffsetDev;    //Top-Left point of printable area in device units
	CRect m_ClientRectLog; //window client rectangle in logical units
	CRect m_ClientRectDev; //window client rectangle in device units
	CSize m_GrayMargin;

	//Intermediate variables built from the basic values
	//Used for defining the paper, printable area and scroll area(paper  + gray margin)
	CRect m_PaperRectLog;
	CRect m_PageRectLog;
	CRect m_ScrollRectLog;
	CSize m_MaxSizeLog;
	long m_ScreenLarger_YD2;
	long m_ScreenLarger_XD2;
	long m_offsetGrayYD2;
	long m_offsetGrayXD2;
	CPoint m_WinOrg;
	

	CRect m_rectUserMarginLog;
	CRect m_MaxRectLog; 
	BOOL m_PaperXLargerThanClientX;
	BOOL m_PaperYLargerThanClientY;

	CPen m_shadowPen;
	CPen m_dashPen;

	int m_ZoomPercent; //recommended between 50% to 150%
	int m_ZoomState;
	int m_mapmode;

	
	POINT m_StoredPt;  //used for storing the mouse-clicked point
	LOGFONT m_logfont;
	void DrawBitmap( CDC *dcSource, CDC *dcDest, HBITMAP hBitmap, const CRect& rect );
	void test(BITMAPINFO& bitmapInfo,LPVOID pBitmapBits) ;
	

// Operations
public:
	int OnSetPageGlobals();
	int SetPageGlobals();
	void SetScreenGlobals(); 
	void SetGlobalRect();
 	void SetDefaultPaper(); 

	void GetPaperData(CRect& margins, CRect& paperrectpix, CSize& papersizepix, CSize& papersizelog); 
	void AdjustMapMode(CDC *pDC, BOOL bShiftOrigin);
	void DocToClient(CRect &rect);
	CDC* CreatePrinterDC();

	void PaperToScreen(CSize* size); 
	void PaperToScreen(CPoint* point); 
	void ScreenToPaper(CSize* size); 
	void ScreenToPaper(CPoint* point); 
	void OnFilePrintPreview();
	void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo,
							  POINT pt, CPreviewView* pView);

	
	virtual void EventPaperChange();
	virtual void EventMarginsChange();  
	void GetPageInfo(CRect* rectMarginsLog, CRect* rectPaperLog, CRect* rectPrintableLog); 
	void DrawHere(CDC* pDC,CDC* pDD, CRect rectPaperLog, CRect rectPrintableLog,CRect rectMarginsLog); 
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextbmpView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextbmpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTextbmpView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnZoomFitWidth();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnZoomFitHeight();
	afx_msg void OnViewZoom100();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMargins();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewZoom150();
	afx_msg void OnUpdateViewZoom150(CCmdUI* pCmdUI);
	afx_msg void OnViewZoom125();
	afx_msg void OnUpdateViewZoom125(CCmdUI* pCmdUI);
	afx_msg void OnViewZoom75();
	afx_msg void OnUpdateViewZoom75(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoom100(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFitheight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFitwidth(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnBarReturn(NMHDR*, LRESULT* );
	DECLARE_MESSAGE_MAP()

 	
};

#ifndef _DEBUG  // debug version in textbmpView.cpp
inline CTextbmpDoc* CTextbmpView::GetDocument()
   { return (CTextbmpDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTBMPVIEW_H__7D9EB8AD_10D8_11D2_9CDA_64D903C10627__INCLUDED_)
