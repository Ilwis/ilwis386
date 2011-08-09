// ruler.cpp : implementation file
 
#include "Headers\toolspch.h"
#include "Ruler.h"
#include "TextBmpView.h"
#include "TextBmpDoc.h"
#include <memory.h>


#define MIN_DIST_PIX_NUM 8 
#define MIN_DIST_PIX_DIV 5

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define HEIGHT 20
#define H_RULERBARHEIGHT 17
 


BEGIN_MESSAGE_MAP(CRulerBar, CControlBar)
	//{{AFX_MSG_MAP(CRulerBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	// Global help commands
END_MESSAGE_MAP()

CRulerBar::CRulerBar(BOOL b3DExt)  
{
	m_bDeferInProgress = FALSE;
    m_nScroll = 0;

	LOGFONT lf;
	
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	VERIFY(GetObject(hFont, sizeof(LOGFONT), &lf));
	

	lf.lfHeight = -8;
	lf.lfWidth = 0;
	VERIFY(fnt.CreateFontIndirect(&lf));

 
	m_cxLeftBorder = 0;
	DWORD dwVersion = ::GetVersion();
	BYTE m_bWin4 = (BYTE)dwVersion >= 4;
	if (m_bWin4)
		m_bDraw3DExt = FALSE;
	else
		m_bDraw3DExt = b3DExt;

	m_cyTopBorder = 3;
	m_cyBottomBorder = 3;
	

	m_Mapmode=MM_TWIPS;

 	CDC m_dcScreen; 
	m_dcScreen.Attach(::GetDC(NULL));
	m_logx = m_dcScreen.GetDeviceCaps(LOGPIXELSX);
	if (m_dcScreen.m_hDC != NULL)
		::ReleaseDC(NULL, m_dcScreen.Detach());
	

	CreateGDIObjects();
}

CRulerBar::~CRulerBar()
{
 
}

void CRulerBar::CreateGDIObjects()
{
	penFocusLine.DeleteObject();
	penBtnHighLight.DeleteObject();
	penBtnShadow.DeleteObject();
	penWindowFrame.DeleteObject();
	penBtnText.DeleteObject();
	penBtnFace.DeleteObject();
	penWindowText.DeleteObject();
	penWindow.DeleteObject();
	brushWindow.DeleteObject();
	brushBtnFace.DeleteObject();

	penFocusLine.CreatePen(PS_DOT, 1,GetSysColor(COLOR_WINDOWTEXT));
	penBtnHighLight.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHIGHLIGHT));
	penBtnShadow.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
	penWindowFrame.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWFRAME));
	penBtnText.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNTEXT));
	penBtnFace.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNFACE));
	penWindowText.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT));
	penWindow.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW));
	brushWindow.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	brushBtnFace.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
}

void CRulerBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
	ASSERT_VALID(this);
 
		
 		
		CTextbmpView* pView = (CTextbmpView*) GetView();
		ASSERT(pView != NULL);

		CRect margins;
		CRect paperrectpix;
		CSize papersizepix;
		CSize papersizelog; 
		pView->GetPaperData(margins,paperrectpix,papersizepix,papersizelog); 

		Update(papersizepix, margins,papersizelog);
		 
		CPoint pt = paperrectpix.TopLeft();
		pView->ClientToScreen(&pt);
		ScreenToClient(&pt);
		if (m_cxLeftBorder != pt.x)
		{

 			m_cxLeftBorder = pt.x ;
			Invalidate();

		}
		int nScroll = pView->GetScrollPos(SB_HORZ);
		if (nScroll != m_nScroll)
		{
			m_nScroll = nScroll;
			Invalidate();
		}
 
	
}


CSize CRulerBar::GetBaseUnits()
{

	ASSERT(fnt.GetSafeHandle() != NULL);
	
	
	CDC m_dcScreen; 
	m_dcScreen.Attach(::GetDC(NULL));

	CFont* pFont = m_dcScreen.SelectObject(&fnt);
	TEXTMETRIC tm;
	VERIFY(m_dcScreen.GetTextMetrics(&tm) == TRUE);
	m_dcScreen.SelectObject(pFont);
 
	if (m_dcScreen.m_hDC != NULL)
		::ReleaseDC(NULL, m_dcScreen.Detach());
	
	return CSize(tm.tmAveCharWidth, tm.tmHeight);

}


BOOL CRulerBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent

	//dwStyle |= WS_CLIPSIBLINGS;
	// force WS_CLIPSIBLINGS (avoids SetWindowPos bugs)
	m_dwStyle = dwStyle;
	dwStyle |= WS_CLIPSIBLINGS|WS_CHILD|WS_VISIBLE;
	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	LPCTSTR lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_BTNFACE+1), NULL);

	if (!CWnd::Create(lpszClass, NULL, dwStyle, rect, pParentWnd, nID, NULL))
		return FALSE;
	// NOTE: Parent must resize itself for control bar to be resized

 	return TRUE;
}
 

CSize CRulerBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	
	//Use this to return the size of control bar
 	
	ASSERT(bHorz);
	CSize m_size = CControlBar::CalcFixedLayout(bStretch, bHorz);
    CRect rectSize;
    rectSize.SetRectEmpty();
    CalcInsideRect(rectSize, bHorz);       // will be negative size
  	
	m_size.cy = H_RULERBARHEIGHT - rectSize.Height();

 	return m_size;
	
	
}

 
 
void CRulerBar::Update(CSize PaperSizePixel, const CRect& MarginsRect, CSize PaperSizeLog)
{
	if ((PaperSizePixel != m_PaperSizePixel) || (MarginsRect != m_MarginRectPixel) || (PaperSizeLog != m_PaperSizeLog))
	{
		m_PaperSizePixel = PaperSizePixel;
		m_MarginRectPixel = MarginsRect;
		m_PaperSizeLog = PaperSizeLog;
    	Invalidate();
	}
	 
 

}

 
 
void CRulerBar::DoPaint(CDC* pDC)
{
	CControlBar::DoPaint(pDC); // CControlBar::DoPaint -- draws border

 	pDC->SaveDC();
	// offset coordinate system
	CPoint pointOffset(0,0);
	RulerToClient(pointOffset);
	pDC->SetViewportOrg(pointOffset);

	DrawFace(*pDC);
	DrawTickMarks(*pDC);

 	pDC->RestoreDC(-1);
 	
	// Do not call CControlBar::OnPaint() for painting messages
}

 
void CRulerBar::DrawFace(CDC& dc)
{
 	//Assume the margins are sizes, positive
	int MarginStart= m_MarginRectPixel.left ;
	int nPageWidth = m_PaperSizePixel.cx-m_MarginRectPixel.right ;
	int nPageEdge = m_PaperSizePixel.cx;


	dc.SaveDC();

	dc.SelectObject(&penBtnShadow);

	//Draw Right Gray Area
	dc.MoveTo(0,0);
	dc.LineTo(nPageEdge - 1, 0);
	dc.LineTo(nPageEdge - 1, HEIGHT - 2);
	dc.LineTo(nPageWidth - 1, HEIGHT - 2);
	dc.LineTo(nPageWidth - 1, 1);
	dc.LineTo(nPageWidth, 1);
	dc.LineTo(nPageWidth, HEIGHT -2);


	//Draw Left Gray Area
	dc.MoveTo(0,0);
	dc.LineTo(0, HEIGHT-2);
	dc.LineTo(MarginStart +1 , HEIGHT - 2);
	dc.LineTo(MarginStart +1 , 1);
	dc.LineTo(MarginStart , 1);
	dc.LineTo(MarginStart , HEIGHT -2);

		 
	dc.SelectObject(&penBtnHighLight);

	//Draw Right
	dc.MoveTo(nPageWidth, HEIGHT - 1);
	dc.LineTo(nPageEdge, HEIGHT -1);
	dc.MoveTo(nPageWidth + 1, HEIGHT - 3);
	dc.LineTo(nPageWidth + 1, 1);
	dc.LineTo(nPageEdge - 1, 1);

	
	//Draw Left
	dc.MoveTo(MarginStart, HEIGHT - 1);
	dc.LineTo(0, HEIGHT -1);

	  
	dc.MoveTo(MarginStart - 1, 1);
	dc.LineTo(1, 1);
	dc.LineTo(1, HEIGHT - 3);

	dc.SelectObject(&penWindow);
	dc.MoveTo(MarginStart, HEIGHT - 1);
	dc.LineTo(nPageWidth, HEIGHT -1);

	dc.SelectObject(&penBtnFace);
	dc.MoveTo(MarginStart + 1, HEIGHT - 2);
	dc.LineTo(nPageWidth - 1, HEIGHT - 2);
	
	dc.SelectObject(&penWindowFrame);
	dc.MoveTo(MarginStart, HEIGHT - 2);
	dc.LineTo(MarginStart, 1);
	dc.LineTo(nPageWidth - 1, 1);
	
	dc.FillRect(CRect(MarginStart+1, 2, nPageWidth - 1, HEIGHT-2), &brushWindow);
	dc.FillRect(CRect(nPageWidth + 2, 2, nPageEdge - 1, HEIGHT-2), &brushBtnFace);
	dc.FillRect(CRect(2, 2, MarginStart - 1, HEIGHT-2), &brushBtnFace);

 
	dc.RestoreDC(-1);
}

void CRulerBar::DrawTickMarks(CDC& dc)
{
	dc.SaveDC();

	dc.SelectObject(&penWindowText);
	dc.SelectObject(&fnt);
	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkMode(TRANSPARENT);

 
	if (m_Mapmode==MM_TWIPS) {
	
		DrawDiv(dc, 144, 1440, 2);
		DrawDiv(dc, 720, 1440, 5);
		DrawNumbers(dc, 1440, 1440);

	}
	else if (m_Mapmode==MM_LOMETRIC) {

		DrawDiv(dc, 10, 100, 2);
		DrawDiv(dc, 50, 100, 5);
		DrawNumbers(dc, 100, 100);


	
	}
	else if (m_Mapmode==MM_HIENGLISH) {

		DrawDiv(dc, 100, 1000, 2);
		DrawDiv(dc, 500, 1000, 5);
		DrawNumbers(dc, 1000, 1000);


	}
	
	dc.RestoreDC(-1);
}

void CRulerBar::DrawNumbers(CDC& dc, int nInc, int nTPU)
{

 	int MarginStart= m_MarginRectPixel.left ;
	int nPageWidth = m_PaperSizePixel.cx-m_MarginRectPixel.right ;
	int nPageEdge = m_PaperSizePixel.cx;

	 		 
	TCHAR buf[10];

	long nLog, nPixel, nLen;

	//Don't draw if the space in between is too small
	nPixel =  MulDiv(nInc, m_PaperSizePixel.cx, m_PaperSizeLog.cx);
	if (nPixel>MIN_DIST_PIX_NUM) {


	for (nLog = nInc; nLog < m_PaperSizeLog.cx; nLog += nInc)
	{
		
		nPixel =  MulDiv(nLog, m_PaperSizePixel.cx, m_PaperSizeLog.cx);

		if ((nPixel == MarginStart) || (nPixel == nPageWidth))
			continue;

		wsprintf(buf, _T("%d"), nLog/nTPU);
		nLen = lstrlen(buf);
		CSize sz = dc.GetTextExtent(buf, nLen);
		dc.ExtTextOut(nPixel - sz.cx/2, HEIGHT/2 - sz.cy/2, 0, NULL, buf, nLen, NULL);
	}
	
	}

 
}

 
void CRulerBar::DrawDiv(CDC& dc, int nInc, int nLargeDiv, int nLength)
{

 	int MarginStart= m_MarginRectPixel.left ;
	int nPageWidth = m_PaperSizePixel.cx-m_MarginRectPixel.right ;
	int nPageEdge = m_PaperSizePixel.cx;

	 		 
	long nLog, nPixel;

	//Don't draw if the space in between is too small
	nPixel =  MulDiv(nInc, m_PaperSizePixel.cx, m_PaperSizeLog.cx);
	if (nPixel>MIN_DIST_PIX_DIV) {

	for (nLog = nInc; nLog < m_PaperSizeLog.cx; nLog += nInc)
	{
		

		if (nLog%nLargeDiv == 0)
			continue;
		
		nPixel =  MulDiv(nLog, m_PaperSizePixel.cx, m_PaperSizeLog.cx);

		if ((nPixel == MarginStart) || (nPixel == nPageWidth))
			continue;

		dc.MoveTo(nPixel, HEIGHT/2 - nLength/2);
		dc.LineTo(nPixel, HEIGHT/2 - nLength/2 + nLength);
	 
	}

	}
	 
	 	
}

 
void CRulerBar::OnSysColorChange()
{
	CControlBar::OnSysColorChange();
	CreateGDIObjects();
	Invalidate();	
}

void CRulerBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanging(lpwndpos);
	CRect rect;
	GetClientRect(rect);
	int minx = min(rect.Width(), lpwndpos->cx);
	int maxx = max(rect.Width(), lpwndpos->cx);
	rect.SetRect(minx-2, rect.bottom - 6, minx, rect.bottom);
	InvalidateRect(rect);
	rect.SetRect(maxx-2, rect.bottom - 6, maxx, rect.bottom);
	InvalidateRect(rect);
}

void CRulerBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CControlBar::OnShowWindow(bShow, nStatus);
	m_bDeferInProgress = FALSE;	
}

void CRulerBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	m_bDeferInProgress = FALSE;	
}

 
 