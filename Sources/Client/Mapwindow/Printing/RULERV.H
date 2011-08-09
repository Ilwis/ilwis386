// riched.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __RULER_V__
#define __RULER_V__

class CTextbmpView;
class CTextbmpDoc;
class CRulerBarV;

 
/////////////////////////////////////////////////////////////////////////////
// CRulerBarV

class CRulerBarV : public CControlBar
{
// Construction
public:
	CRulerBarV(BOOL b3DExt = TRUE);
	~CRulerBarV();

// Operations
public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
protected:
	void CRulerBarV::Update(CSize PaperSizePixel, const CRect& MarginsRect, CSize PaperSizeLog);

// Attributes
public:
	BOOL m_bDeferInProgress;
	BOOL m_bDraw3DExt;
	CFont fnt;
	CSize GetBaseUnits();
	CSize m_sizePaper;
	CRect m_rectMargin;
 	int m_logy;
	int m_nLinePos;
	int m_nVScroll; // in pixels
	int m_Mapmode;


	CSize m_PaperSizePixel;
	CSize m_PaperSizeLog;
	CRect m_MarginRectPixel;

	CPen penFocusLine;
	CPen penBtnHighLight;
	CPen penBtnShadow;
	CPen penWindowFrame;
	CPen penBtnText;
	CPen penBtnFace;
	CPen penWindowText;
	CPen penWindow;
	CBrush brushWindow;
	CBrush brushBtnFace;

// Implementation
public:
	virtual void DoPaint(CDC* pDC);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	void ClientToRuler(CPoint& pt) {pt.Offset(-m_cxLeftBorder, -m_cyTopBorder+m_nVScroll);}
	void ClientToRuler(CRect& rect) {rect.OffsetRect(-m_cxLeftBorder, -m_cyTopBorder+m_nVScroll);}
	void RulerToClient(CPoint& pt) {pt.Offset(m_cxLeftBorder, m_cyTopBorder-m_nVScroll);}
	void RulerToClient(CRect& rect) {rect.OffsetRect(m_cxLeftBorder, m_cyTopBorder-m_nVScroll);}

	//int XTwipsToClient(int nT) {return MulDiv(nT, m_logy, 1440) + m_cyTopBorder - m_nVScroll;}
	//int XClientToTwips(int nC) {return MulDiv(nC - m_cyTopBorder + m_nVScroll, 1440, m_logy);}

	//int XTwipsToRuler(int nT) {return MulDiv(nT, m_logy, 1440);}
	//int XRulerToTwips(int nR) {return MulDiv(nR, 1440, m_logy);}

	int XLogToClient(int nT) {return MulDiv(nT, m_PaperSizePixel.cy, m_PaperSizeLog.cy) + m_cxLeftBorder - m_nVScroll;}
	int XClientToLog(int nC) {return MulDiv(nC - m_cxLeftBorder + m_nVScroll, m_PaperSizeLog.cy, m_PaperSizePixel.cy);}

	int XLogToRuler(int nT) {return MulDiv(nT, m_PaperSizePixel.cy, m_PaperSizeLog.cy);}
	int XRulerToLog(int nR) {return MulDiv(nR, m_PaperSizeLog.cy, m_PaperSizePixel.cy);}
 

	int XRulerToClient(int nR) {return nR + m_cyTopBorder - m_nVScroll;}
	int XClientToRuler(int nC) {return nC - m_cyTopBorder + m_nVScroll;}

protected:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	void CreateGDIObjects();
	void DrawFace(CDC& dc);
	void DrawTickMarks(CDC& dC);
	void DrawNumbers(CDC& dc, int nInc, int nTPU);
	void DrawDiv(CDC& dc, int nInc, int nLargeDiv, int nLength);
    CView* GetView()
    {
		ASSERT(GetParent() != NULL);
		return ((CFrameWnd*)GetParent())->GetActiveView();
	}
	CDocument* GetDocument() { return GetView()->GetDocument(); }

 
	// Generated message map functions
	//{{AFX_MSG(CRulerBarV)
 	afx_msg void OnSysColorChange();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	//afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
		 
};

 
#endif
