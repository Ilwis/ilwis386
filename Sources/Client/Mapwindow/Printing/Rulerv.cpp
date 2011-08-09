// ruler.cpp : implementation file
//
 
#include "Headers\toolspch.h"
#include "rulerV.h"
#include "TextBmpView.h"
#include "TextBmpDoc.h"
//#include "strings.h"
#include <memory.h>


#define MIN_DIST_PIX_NUM 8 
#define MIN_DIST_PIX_DIV 5

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define WIDTH 20
#define V_RULERBARWIDTH 17


BEGIN_MESSAGE_MAP(CRulerBarV, CControlBar)
	//{{AFX_MSG_MAP(CRulerBarV)
 	ON_WM_SYSCOLORCHANGE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	// Global help commands
END_MESSAGE_MAP()

CRulerBarV::CRulerBarV(BOOL b3DExt)  
{
	m_bDeferInProgress = FALSE;
     
	 
	//m_unit.m_nTPU = 0;
	m_nVScroll = 0;

	LOGFONT lf;

 	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	VERIFY(GetObject(hFont, sizeof(LOGFONT), &lf));
	

  
	lf.lfEscapement = 900; 
	lf.lfHeight = -12;
	lf.lfWidth = 0;
	//Assume that this font is avialable in most systems
	strcpy((LPSTR)&(lf.lfFaceName), _T("Times New Roman"));

	VERIFY(fnt.CreateFontIndirect(&lf));

	
	m_cxLeftBorder = 3;
	m_cxRightBorder = 5;


	DWORD dwVersion = ::GetVersion();
	BYTE m_bWin4 = (BYTE)dwVersion >= 4;
	if (m_bWin4)
		m_bDraw3DExt = FALSE;
	else
		m_bDraw3DExt = b3DExt;

	m_cyTopBorder = 0;
	m_cyBottomBorder = 6;
	
	CDC m_dcScreen; 
	m_dcScreen.Attach(::GetDC(NULL));
	m_logy = m_dcScreen.GetDeviceCaps(LOGPIXELSY);
	if (m_dcScreen.m_hDC != NULL)
		::ReleaseDC(NULL, m_dcScreen.Detach());

	
	m_Mapmode=MM_TWIPS;

	CreateGDIObjects();
}

CRulerBarV::~CRulerBarV()
{
 
}

void CRulerBarV::CreateGDIObjects()
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

void CRulerBarV::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
	ASSERT_VALID(this);
	//Get the page size and see if changed -- from document
	//get margins and tabs and see if changed -- from view
 		
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
		if (m_cyTopBorder != pt.y)
		{
			m_cyTopBorder = pt.y;
			Invalidate();
		}
		int nScroll = pView->GetScrollPos(SB_VERT);
		if (nScroll != m_nVScroll)
		{
			m_nVScroll = nScroll;
			Invalidate();
		}
 
	
}


CSize CRulerBarV::GetBaseUnits()
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

 
BOOL CRulerBarV::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
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

CSize CRulerBarV::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	
 	bHorz=FALSE;
	CSize m_size = CControlBar::CalcFixedLayout(bStretch, bHorz);

    CRect rectSize;
    rectSize.SetRectEmpty();
    CalcInsideRect(rectSize, TRUE);       // will be negative size
    	 
	m_size.cx = V_RULERBARWIDTH - rectSize.Width();
	 
	return m_size;
	
	
	
}

 
 
void CRulerBarV::Update(CSize PaperSizePixel, const CRect& MarginsRect, CSize PaperSizeLog)
{
	if ((PaperSizePixel != m_PaperSizePixel) || (MarginsRect != m_MarginRectPixel) || (PaperSizeLog != m_PaperSizeLog))
	{
		m_PaperSizePixel = PaperSizePixel;
		m_MarginRectPixel = MarginsRect;
		m_PaperSizeLog = PaperSizeLog;
    	Invalidate();
	}
	 
 
}

 
 
void CRulerBarV::DoPaint(CDC* pDC)
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

 
void CRulerBarV::DrawFace(CDC& dc)
{
	
	int MarginStart= m_MarginRectPixel.top ;
	int nPageHeight = m_PaperSizePixel.cy-m_MarginRectPixel.bottom ;
	int nPageEdge = m_PaperSizePixel.cy;

 
	dc.SaveDC();

	dc.SelectObject(&penBtnShadow);

	//Draw Right Gray Area
	dc.MoveTo(0,0);
	dc.LineTo(0,nPageEdge - 1);
	dc.LineTo( WIDTH - 2,nPageEdge - 1);
	dc.LineTo( WIDTH - 2,nPageHeight - 1);
	dc.LineTo(1,nPageHeight - 1);
	dc.LineTo(1,nPageHeight);
	dc.LineTo( WIDTH - 2,nPageHeight);


	//Draw Left Gray Area
	dc.MoveTo( 0,0 );
	dc.LineTo( WIDTH-2,0);
	dc.LineTo( WIDTH - 2,MarginStart +1 );
	dc.LineTo( 1, MarginStart +1);
	dc.LineTo( 1, MarginStart);
	dc.LineTo( WIDTH - 2 ,MarginStart);


	 
		 
	dc.SelectObject(&penBtnHighLight);

	//Draw Right
	dc.MoveTo( WIDTH - 1, nPageHeight);
	dc.LineTo( WIDTH - 1 ,nPageEdge);
	dc.MoveTo( WIDTH - 3, nPageHeight + 1);
	dc.LineTo( 1, nPageHeight + 1);
	dc.LineTo( 1, nPageEdge - 1);

	

	
	//Draw Left
	dc.MoveTo( WIDTH - 1 ,MarginStart);
	dc.LineTo(WIDTH -1, 0);

 	 
	dc.MoveTo(1, MarginStart - 1);
	dc.LineTo(1, 1);
	dc.LineTo( WIDTH - 3, 1);

	 
 
	
	
	dc.SelectObject(&penWindow);
	dc.MoveTo( WIDTH - 1, MarginStart);
	dc.LineTo( WIDTH - 1,nPageHeight);

	dc.SelectObject(&penBtnFace);
	dc.MoveTo( WIDTH - 2, MarginStart + 1);
	dc.LineTo( WIDTH - 2, nPageHeight - 1);
	
	dc.SelectObject(&penWindowFrame);
	dc.MoveTo( WIDTH - 2 ,MarginStart);
	dc.LineTo(1, MarginStart);
	dc.LineTo(1, nPageHeight - 1);
	
	dc.FillRect(CRect(2, MarginStart+1,  WIDTH - 2, nPageHeight - 1), &brushWindow);
	dc.FillRect(CRect(2, nPageHeight + 2,  WIDTH - 2 ,nPageEdge - 1), &brushBtnFace);
	dc.FillRect(CRect(2, 2,  WIDTH - 2, MarginStart - 1), &brushBtnFace);

 
	dc.RestoreDC(-1);
}

void CRulerBarV::DrawTickMarks(CDC& dc)
{
	dc.SaveDC();

	dc.SelectObject(&penWindowText);
	dc.SelectObject(&fnt);
	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkMode(TRANSPARENT);

 
	DrawDiv(dc, 144, 1440, 2);
	DrawDiv(dc, 720, 1440, 5);
	DrawNumbers(dc, 1440, 1440);
	
	dc.RestoreDC(-1);
}

void CRulerBarV::DrawNumbers(CDC& dc, int nInc, int nTPU)
{

	int MarginStart= m_MarginRectPixel.top ;
	int nPageHeight = m_PaperSizePixel.cy-m_MarginRectPixel.bottom ;
	int nPageEdge = m_PaperSizePixel.cy;

 	 		 
	TCHAR buf[10];

	long nLog, nPixel, nLen;

	//Don't draw if the space in between is too small
	nPixel =  MulDiv(nInc, m_PaperSizePixel.cy, m_PaperSizeLog.cy);
	if (nPixel>MIN_DIST_PIX_NUM) {


	for (nLog = nInc; nLog < m_PaperSizeLog.cy; nLog += nInc)
	{
	
		nPixel =  MulDiv(nLog, m_PaperSizePixel.cy, m_PaperSizeLog.cy);


		if ((nPixel == MarginStart) || (nPixel == nPageHeight))
			continue;

		 
		wsprintf(buf, _T("%d"), nLog/nTPU);
		nLen = lstrlen(buf);
		CSize sz = dc.GetTextExtent(buf, nLen);

		 

		dc.ExtTextOut(WIDTH/2 - sz.cy/2, nPixel + sz.cx/2, 0, NULL, buf, nLen, NULL);
 
	}
	
	}

  
}

 
void CRulerBarV::DrawDiv(CDC& dc, int nInc, int nLargeDiv, int nLength)
{

	int MarginStart= m_MarginRectPixel.top ;
	int nPageHeight = m_PaperSizePixel.cy-m_MarginRectPixel.bottom ;
	int nPageEdge = m_PaperSizePixel.cy;

 	 		 
	long nLog, nPixel;

	//Don't draw if the space in between is too small
	nPixel =  MulDiv(nInc, m_PaperSizePixel.cy, m_PaperSizeLog.cy);
	if (nPixel>MIN_DIST_PIX_DIV) {

	for (nLog = nInc; nLog < m_PaperSizeLog.cy; nLog += nInc)
	{
		

		if (nLog%nLargeDiv == 0)
			continue;
		
		nPixel =  MulDiv(nLog, m_PaperSizePixel.cy, m_PaperSizeLog.cy);

		if ((nPixel == MarginStart) || (nPixel == nPageHeight))
			continue;

		dc.MoveTo(WIDTH/2 - nLength/2,nPixel);
		dc.LineTo(WIDTH/2 - nLength/2 + nLength, nPixel );
	 
	}

	}
	 
	 	
}


 
void CRulerBarV::OnSysColorChange()
{
	CControlBar::OnSysColorChange();
	CreateGDIObjects();
	Invalidate();	
}

void CRulerBarV::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
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

void CRulerBarV::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CControlBar::OnShowWindow(bShow, nStatus);
	m_bDeferInProgress = FALSE;	
}

void CRulerBarV::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	m_bDeferInProgress = FALSE;	
}

 