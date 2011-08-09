#include "Client\Headers\formelementspch.h"
#include "textbmpDoc.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\ilwis.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "textbmpView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CTextbmpView, CScrollView)

BEGIN_MESSAGE_MAP(CTextbmpView, CScrollView)
	//{{AFX_MSG_MAP(CTextbmpView)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_FITWIDTH, OnZoomFitWidth)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_FITHEIGHT, OnZoomFitHeight)
	ON_COMMAND(ID_VIEW_ZOOM100, OnViewZoom100)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_FILE_MARGINS, OnMargins)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_ZOOM150, OnViewZoom150)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM150, OnUpdateViewZoom150)
	ON_COMMAND(ID_VIEW_ZOOM125, OnViewZoom125)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM125, OnUpdateViewZoom125)
	ON_COMMAND(ID_VIEW_ZOOM75, OnViewZoom75)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM75, OnUpdateViewZoom75)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM100, OnUpdateViewZoom100)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FITHEIGHT, OnUpdateViewFitheight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FITWIDTH, OnUpdateViewFitwidth)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()


//========================================================
//Definitions for compilation

//need enough high res to allows smooth text scrolling and double buffer painting...
#define MAP_MODE MM_TWIPS;   //other choices : MM_HIENGLISH, MM_LOMETRIC
#define XGRAY_MARGIN 1000	 //10 mm :units in HIMETRIC added to bottom of gray page, paper will be offet by half of this amount if paper larger than screen
#define YGRAY_MARGIN 1000	 //10 mm :units in HIMETRIC added to right of gray page, , paper will be offet by half of this amount if paper larger than screen

//=========================================================


/////////////////////////////////////////////////////////////////////////////
// CTextbmpView construction/destruction

CTextbmpView::CTextbmpView()
{
	// TODO: add construction code here
	m_ScreenLarger_YD2=0;
	m_ScreenLarger_XD2=0;
	m_offsetGrayYD2=0;
	m_offsetGrayXD2=0;
	m_WinOrg.x=0;  
	m_WinOrg.y=0;
	m_GrayMargin.cx=0;
	m_GrayMargin.cy=0;
	m_ZoomPercent=100;
	m_ZoomState=ZOOMFITWIDTH;

	m_StoredPt.x=0;
	m_StoredPt.y=0;

	m_mapmode=MAP_MODE;

	//fonts stuff: initialize the m_logfont
 	ZeroMemory(&m_logfont, sizeof(m_logfont));  
	m_logfont.lfHeight = 18;	 
	m_logfont.lfWidth = 0;		 
	strcpy((LPSTR)&(m_logfont.lfFaceName), _T("Times New Roman"));


	//Set margins to one inch 
	if (m_mapmode==MM_TWIPS) {
		m_rectUserMarginLog.top=1440;
		m_rectUserMarginLog.bottom=1440;
		m_rectUserMarginLog.left=1440;
		m_rectUserMarginLog.right=1440;
	} 
	else if (m_mapmode==MM_LOMETRIC) {
		m_rectUserMarginLog.top=254;
		m_rectUserMarginLog.bottom=254;
		m_rectUserMarginLog.left=254;
		m_rectUserMarginLog.right=254;
	} 
	else if (m_mapmode==MM_HIENGLISH) {  

		m_rectUserMarginLog.top=1000;
		m_rectUserMarginLog.bottom=1000;
		m_rectUserMarginLog.left=1000;
		m_rectUserMarginLog.right=1000;

	}

	
}

CTextbmpView::~CTextbmpView()
{
}

BOOL CTextbmpView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTextbmpView drawing



 
void CTextbmpView::OnDraw(CDC* pDC)
{
	CTextbmpDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

 

	//For Double Buffer: Simply Set up the Compatible DC and Bitmap
	//Using whatever mapmode we are currently in. The DocToClient convert the clip rectangle into device units (pixels)
	//The intersectionClipRect is created using current map mode
	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;

	// only paint the rect that needs repainting
	CRect client;
	pDC->GetClipBox(client);
		
	CRect rect = client;
	DocToClient(rect);

	
	CRect rMargins ,rPaper, rPrintable;
	GetPageInfo(&rMargins, &rPaper, &rPrintable);
	
	 
	if (!pDC->IsPrinting())
	{
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
			{
				OnPrepareDC(&dc, NULL);
				pDrawDC = &dc;

				// offset origin more because bitmap is just piece of the whole drawing
				dc.OffsetViewportOrg(-rect.left, -rect.top);
				pOldBitmap = dc.SelectObject(&bitmap);
				dc.SetBrushOrg(rect.left % 8, rect.top % 8);

				// might as well clip to the same rectangle
				dc.IntersectClipRect(client);
			}
		}
	}


 
	
	if (!pDrawDC->IsPrinting()) { //draw the borders
	
		 	
	 		
		//Fill background of client window gray before setting an mapmode...
 		::FillRect(pDrawDC->m_hDC,&client, (HBRUSH)GetStockObject(GRAY_BRUSH)); //background


		if (m_ZoomState==ZOOMFITWIDTH) {
		
			pDrawDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDrawDC->SetWindowExt(m_ScrollRectLog.right,-m_ScrollRectLog.right);
			CSize rSize = pDrawDC->SetViewportExt(m_ClientRectDev.right,m_ClientRectDev.right);

		}
		else if (m_ZoomState==ZOOMFITHEIGHT) {

			long extValue,viewValue;
		 
			extValue=labs(m_ScrollRectLog.bottom);
			viewValue=labs(m_ClientRectDev.bottom);
			pDrawDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDrawDC->SetWindowExt(extValue,-extValue);
			CSize rSize = pDrawDC->SetViewportExt(viewValue,viewValue);

		}
		else if (m_ZoomState==ZOOMIN) {

			
			CSize WinSize = pDrawDC->GetWindowExt();
			CSize ViewSize = pDrawDC->GetViewportExt();

			ViewSize.cx=MulDiv(ViewSize.cx,m_ZoomPercent,100);
			ViewSize.cy=MulDiv(ViewSize.cy,m_ZoomPercent,100);

			 
			pDrawDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDrawDC->SetWindowExt(WinSize);
			CSize rSize = pDrawDC->SetViewportExt(ViewSize);

		}

	

		//Fill physical paper white
		::FillRect(pDrawDC->m_hDC,&m_PaperRectLog, (HBRUSH)GetStockObject(WHITE_BRUSH)); //paper
		

		//Draw a dark outline around the paper
		CPen *oldPen;
		oldPen=pDrawDC->SelectObject(&m_shadowPen);
		pDrawDC->MoveTo(m_PaperRectLog.left-1,m_PaperRectLog.top+1);
		pDrawDC->LineTo(m_PaperRectLog.right+1,m_PaperRectLog.top+1);
		pDrawDC->LineTo(m_PaperRectLog.right+1,m_PaperRectLog.bottom-1);
		pDrawDC->LineTo(m_PaperRectLog.left-1,m_PaperRectLog.bottom-1);
		pDrawDC->LineTo(m_PaperRectLog.left-1,m_PaperRectLog.top+1);
		

		//Draw the printable area
 
		pDrawDC->SelectObject(&m_dashPen);
		pDrawDC->MoveTo(m_PageRectLog.left,m_PageRectLog.top);
		pDrawDC->LineTo(m_PageRectLog.right,m_PageRectLog.top);
		pDrawDC->LineTo(m_PageRectLog.right,m_PageRectLog.bottom);
		pDrawDC->LineTo(m_PageRectLog.left,m_PageRectLog.bottom);
		pDrawDC->LineTo(m_PageRectLog.left,m_PageRectLog.top);


		pDrawDC->MoveTo(rMargins.left,rMargins.top);
		pDrawDC->LineTo(rMargins.right,rMargins.top);
		pDrawDC->LineTo(rMargins.right,rMargins.bottom);
		pDrawDC->LineTo(rMargins.left,rMargins.bottom);
		pDrawDC->LineTo(rMargins.left,rMargins.top);
 		
		pDrawDC->SelectObject(oldPen);
		

		 
		pDrawDC->SetWindowOrg(m_WinOrg);
	

	}
 
	DrawHere(pDrawDC, pDC, rPaper,rPrintable,rMargins); 
	dc.SelectObject(pOldBitmap);

	////BitBlt the Bitmap in MM_TEXT Mode
	//if (pDrawDC != pDC)
	//{
	//	pDC->SetViewportOrg(0, 0);
	//	pDC->SetWindowOrg(0,0);
	//	pDC->SetMapMode(MM_TEXT);
	//	dc.SetViewportOrg(0, 0);
	//	dc.SetWindowOrg(0,0);
	//	dc.SetMapMode(MM_TEXT);
	//	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
	//		&dc, 0, 0, SRCCOPY);
	//	dc.SelectObject(pOldBitmap);
	//}


	

}


void CTextbmpView::DrawHere(CDC* dcDraw, CDC *dcDest, CRect PaperRect, CRect PrintableRect,CRect MarginsRect) 
{

	CRect	rect;
	HDC		hMemDC;

	BITMAPINFO	bitmapInfo;
	LPVOID		pBitmapBits;
	int			nXRes, nYRes;
	BOOL		bSuccess = FALSE;

	rect = MarginsRect;

	nXRes = rect.Width();
	nYRes = rect.Height();

	nXRes = nXRes & ~(sizeof(DWORD)-1);							// aligning width to 4 bytes (sizeof(DWORD)) avoids 
																// pixel garbage at the upper line

	// Initialize the bitmap header information...
	memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth		= nXRes;
	bitmapInfo.bmiHeader.biHeight		= nYRes;
	bitmapInfo.bmiHeader.biPlanes		= 1;
	bitmapInfo.bmiHeader.biBitCount		= 24;
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage	= bitmapInfo.bmiHeader.biWidth * bitmapInfo.bmiHeader.biHeight * 3;
	bitmapInfo.bmiHeader.biXPelsPerMeter = int(72./.0254);	// 72*fac DPI
	bitmapInfo.bmiHeader.biYPelsPerMeter = int(72./.0254);	// 72*fac DPI

	// create DIB
	HDC hTmpDC = ::GetDC(m_hWnd);
	HBITMAP hDib = CreateDIBSection(hTmpDC, &bitmapInfo, DIB_RGB_COLORS, &pBitmapBits, NULL, (DWORD)0);

	// create memory device context
	//CDC *pDC = new CWindowDC(this);
	////pDC->m_bPrinting = TRUE;	// this does the trick in OnDraw: it prevents changing rendering context and swapping buffers
	//if ((hMemDC = CreateCompatibleDC(pDC == NULL ? NULL : pDC->GetSafeHdc())) == NULL)
	//{
	//	DeleteObject(hDib);
	//	return FALSE;
	//}
	HGDIOBJ hOldDib = SelectObject(dcDraw->m_hDC, hDib);

	// Store current context and viewport
	CTextbmpDoc *doc = GetDocument();
	DrawerContext *context = doc->getMapDoc()->rootDrawer->getDrawerContext();
	RowCol viewportOld = doc->getMapDoc()->rootDrawer->getViewPort();

	// Make hMemDC the current OpenGL rendering context.
	DrawerContext * contextMem = new ILWIS::DrawerContext();
	contextMem->initOpenGL(dcDraw->m_hDC, 0, DrawerContext::mDRAWTOBITMAP);
	doc->getMapDoc()->rootDrawer->setDrawerContext(contextMem);
	doc->getMapDoc()->rootDrawer->setViewPort(RowCol(nYRes,nXRes));

	PreparationParameters ppEDITCOPY (ILWIS::NewDrawer::ptOFFSCREENSTART);
	vector<ComplexDrawer*> drawerList;
	int count = doc->getMapDoc()->rootDrawer->getDrawerCount();
	for (int i = 0; i < count; ++i)	{
		ILWIS::ComplexDrawer* spatialDataDrawer = dynamic_cast<ILWIS::ComplexDrawer*>(doc->getMapDoc()->rootDrawer->getDrawer(i));
		if (spatialDataDrawer != 0)	{
			int count2 = spatialDataDrawer->getDrawerCount();
			for (int j = 0; j < count2; ++j) {
				ILWIS::ComplexDrawer* drawer = dynamic_cast<ILWIS::ComplexDrawer*>(spatialDataDrawer->getDrawer(j));
				if (drawer != 0)	{
					drawerList.push_back(drawer); // remember for 2nd loop later on so that we dont need to repeat the dynamic_cast
					drawer->prepare(&ppEDITCOPY);
				}
			}
		}
	}

	contextMem->TakeContext();
	//glClearColor(1,1,1,0.0); // override glClearColor of initOpenGL() to a more logical choice for Copy/Paste
	doc->getMapDoc()->rootDrawer->draw();
	glFinish();	// Finish all OpenGL commands
	contextMem->ReleaseContext();

	PreparationParameters ppEDITCOPYDONE (ILWIS::NewDrawer::ptOFFSCREENEND);
	for (vector<ComplexDrawer*>::iterator it = drawerList.begin(); it != drawerList.end(); ++it)
		(*it)->prepare(&ppEDITCOPYDONE);

	// contextMem is no longer needed
	delete contextMem;

	// Restore original context and viewport
	doc->getMapDoc()->rootDrawer->setDrawerContext(context);
	doc->getMapDoc()->rootDrawer->setViewPort(viewportOld);

	//DrawBitmap(dcDraw, dcDest, hDib, rect);
	test(bitmapInfo, pBitmapBits);

	//if (hOldDib != NULL)
	//	SelectObject(hMemDC, hOldDib);

	// Delete our DIB and device context
	DeleteObject(hDib);
	//DeleteDC(hMemDC);

}

void CTextbmpView::test(BITMAPINFO& bitmapInfo,LPVOID pBitmapBits) {
	if (OpenClipboard())
	{
		HGLOBAL hClipboardCopy = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(BITMAPINFOHEADER) + bitmapInfo.bmiHeader.biSizeImage);
		if (hClipboardCopy != NULL)
		{
			LPVOID lpClipboardCopy, lpBitmapBitsOffset;
			lpClipboardCopy = ::GlobalLock((HGLOBAL) hClipboardCopy);
			lpBitmapBitsOffset = (LPVOID)((BYTE*)lpClipboardCopy + sizeof(BITMAPINFOHEADER));

			memcpy(lpClipboardCopy, &bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
			memcpy(lpBitmapBitsOffset, pBitmapBits, bitmapInfo.bmiHeader.biSizeImage);
			::GlobalUnlock(hClipboardCopy);

			EmptyClipboard();

			if (SetClipboardData(CF_DIB, hClipboardCopy) != NULL)
			{
				//if (nReduceResCount > 0)
				//	pApp->SetStatusBarInfo(POLICY_RESOLUTION_REDUCED, (int)round(fac*72.));
			}
			else
			{
				GlobalFree(hClipboardCopy);
			}
			CloseClipboard();
		}
	}
}


void CTextbmpView::DrawBitmap( CDC *dcSource, CDC *dcDest, HBITMAP hBitmap, const CRect& rect )
{
	if (dcDest != dcSource)
	{
		dcDest->SetViewportOrg(0, 0);
		dcDest->SetWindowOrg(0,0);
		dcDest->SetMapMode(MM_TEXT);
		dcSource->SetViewportOrg(0, 0);
		dcSource->SetWindowOrg(0,0);
		dcSource->SetMapMode(MM_TEXT);
		dcDest->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			dcSource, 0, 0, SRCCOPY);
	}

	// Get logical coordinates
	//BITMAP bm;
	//::GetObject( hBitmap, sizeof( bm ), &bm );
	//CPoint size( bm.bmWidth, bm.bmHeight );
	//pDC->DPtoLP(&size);

	//CPoint org(0,0);
	//pDC->DPtoLP(&org);

	//// Create a memory DC compatible with the destination DC
	//CDC memDC;
	//memDC.CreateCompatibleDC( pDC );
	//memDC.SetMapMode( pDC->GetMapMode() );
	//
	////memDC.SelectObject( &bitmap );
	//HBITMAP hBmOld = (HBITMAP)::SelectObject( memDC.m_hDC, hBitmap );
	//
	//
	////// Select and realize the palette
	////if( hPal && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	////{
	////	SelectPalette( pDC->GetSafeHdc(), hPal, FALSE );
	////	pDC->RealizePalette();
	////}
	//pDC->BitBlt(xDest, yDest, size.x, size.y, &memDC, org.x, org.y, SRCCOPY);

	//::SelectObject( memDC.m_hDC, hBmOld );
}
BOOL CTextbmpView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTextbmpView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTextbmpView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CTextbmpView::OnFilePrintPreview() {

	 
	CMainFrame* mainWnd=(CMainFrame*) AfxGetMainWnd( );

	//Hide Rulers before preview
	(mainWnd->m_wndRulerBarH).ShowWindow(SW_HIDE);
	(mainWnd->m_wndRulerBarV).ShowWindow(SW_HIDE);
	
	CScrollView::OnFilePrintPreview();

	
}

void CTextbmpView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo,
							  POINT pt, CPreviewView* pView) {

	//Restore Rulers after preview	
	CMainFrame* mainWnd=(CMainFrame*) AfxGetMainWnd();
	(mainWnd->m_wndRulerBarH).ShowWindow(SW_SHOW);
	(mainWnd->m_wndRulerBarV).ShowWindow(SW_SHOW);
	

	CScrollView::OnEndPrintPreview(pDC, pInfo,
							  pt, pView); 

}

/////////////////////////////////////////////////////////////////////////////
// CTextbmpView diagnostics

#ifdef _DEBUG
void CTextbmpView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTextbmpView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CTextbmpDoc* CTextbmpView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextbmpDoc)));
	return (CTextbmpDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextbmpView message handlers

int CTextbmpView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//Pen Creation 
	m_shadowPen.CreatePen(PS_SOLID, 3, RGB(64,64,64));
	m_dashPen.CreatePen(PS_DOT, 1, RGB(0,0,0));
 	 

	// TODO: Add your specialized creation code here
	 
	SetPageGlobals();
	 
	 
	return 0;
}


//Set Page Globals
int CTextbmpView::SetPageGlobals() {


	long PageWidthDev=0 ;
	long PageHeightDev=0 ;
	long LOGPInchX=0;
	long LOGPInchY=0;
	long PhyWidth=0;
	long PhyHeight=0;
	long PhyOffsetX=0;
	long PhyOffsetY=0;
	long PrinterType=0;


	CDC *printDC = NULL;
	printDC = CreatePrinterDC();
	if (printDC==NULL) {

			SetDefaultPaper();
			return FALSE;

	}
	
 
	 
	PageWidthDev = printDC->GetDeviceCaps(HORZRES);
    PageHeightDev = printDC->GetDeviceCaps(VERTRES);
    LOGPInchX = printDC->GetDeviceCaps(LOGPIXELSX);
    LOGPInchY= printDC->GetDeviceCaps(LOGPIXELSY);
    PhyWidth = printDC->GetDeviceCaps(PHYSICALWIDTH);
    PhyHeight = printDC->GetDeviceCaps(PHYSICALHEIGHT);
	PhyOffsetX = printDC->GetDeviceCaps(PHYSICALOFFSETX);
    PhyOffsetY = printDC->GetDeviceCaps(PHYSICALOFFSETY);
	PrinterType = printDC->GetDeviceCaps(TECHNOLOGY);
	if ((PrinterType!=DT_PLOTTER) && (PrinterType!=DT_RASPRINTER)) {

		 		 
		SetDefaultPaper();
		return FALSE;
		

	}

 
	CSize Tsize(PhyWidth,PhyHeight);
	printDC->SetMapMode(m_mapmode);
	printDC->DPtoLP(&Tsize);
	m_PaperSizeLog=Tsize;


	m_OffsetDev.x=PhyOffsetX;
	m_OffsetDev.y=PhyOffsetY;
	m_OffsetLog=m_OffsetDev;
	printDC->DPtoLP(&m_OffsetLog);

	 
	m_PageSizeDev.cx=PageWidthDev;
	m_PageSizeDev.cy=PageHeightDev;
	m_PageSizeLog=m_PageSizeDev;
	printDC->DPtoLP(&m_PageSizeLog);

	

	::DeleteDC(printDC->GetSafeHdc());

	return TRUE;

}

int CTextbmpView::OnSetPageGlobals() {

	SetPageGlobals();

	//This is to make sure the gray area will be updated correctly 
	SetScreenGlobals();

	EventPaperChange();

	Invalidate(TRUE);
	
	return TRUE;

}

void CTextbmpView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
 
	
}


void CTextbmpView::SetScreenGlobals() 
{

      
	CClientDC xDC(this);
	
     
	::GetClientRect(this->m_hWnd, &m_ClientRectDev); 
	xDC.SetMapMode(m_mapmode);
	m_ClientRectLog=m_ClientRectDev;
	xDC.DPtoLP(&m_ClientRectLog);

	
	CSize TMargin(XGRAY_MARGIN,YGRAY_MARGIN);
	xDC.HIMETRICtoLP(&TMargin);
	m_GrayMargin=TMargin;
	m_offsetGrayYD2=TMargin.cy/2;
	m_offsetGrayXD2=TMargin.cx/2;
	
 
	//Compare Client Window Size To Paper Size
		
	//First set the m_MaxRectLog to the logical client rectangle
	m_MaxRectLog=m_ClientRectLog;

	
	
	m_PaperXLargerThanClientX=TRUE;
	m_MaxRectLog.right=m_PaperSizeLog.cx;
	m_PaperYLargerThanClientY=TRUE;
	m_MaxRectLog.bottom=m_PaperSizeLog.cy;
	

 
	SetGlobalRect(); 


	CSize Scrollsize;
	if (m_ZoomState==ZOOMIN) { 
	//Set scroll size to equal that of the gray rectangle
	
	 	
		Scrollsize.cx=MulDiv(m_ScrollRectLog.right,m_ZoomPercent,100);
		Scrollsize.cy=MulDiv(m_ScrollRectLog.bottom,m_ZoomPercent,100);

	}
	else if (m_ZoomState==ZOOMFITWIDTH) {

		Scrollsize.cx=m_ClientRectLog.right;
		Scrollsize.cy=MulDiv(m_ScrollRectLog.bottom,m_ClientRectLog.right ,m_ScrollRectLog.right);
 

	}
	else if (m_ZoomState==ZOOMFITHEIGHT) { //Actually means fit height

		Scrollsize.cx=MulDiv(m_ScrollRectLog.right,m_ClientRectLog.bottom ,m_ScrollRectLog.bottom);
 		Scrollsize.cy=m_ClientRectLog.bottom;

	}


	 
	if (Scrollsize.cy<0) Scrollsize.cy=-Scrollsize.cy;

	//TRACE("\nm_MaxRectLog = %d %d\n",m_MaxRectLog.right,m_MaxRectLog.bottom);
	//TRACE("\nScrollSize =%d %d\n",Scrollsize.cx,Scrollsize.cy);
 
	SetScrollSizes(m_mapmode, Scrollsize);
 

}

void CTextbmpView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	 
	
	// TODO: Add your message handler code here
	SetScreenGlobals(); 
	
}
 
void CTextbmpView::OnDestroy() 
{
	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_shadowPen.DeleteObject();
	m_dashPen.DeleteObject();
	 
	
}


void CTextbmpView::SetGlobalRect() 
{
		

 
		//Assume that m_MaxRectLog is properly initialized
		m_ScrollRectLog=m_MaxRectLog;

		if (!m_PaperYLargerThanClientY) { //ScreenLarger

			//Need inversion for m_MaxRectLog
			m_ScrollRectLog.bottom=-m_MaxRectLog.bottom;
			m_PaperRectLog.top=-m_ScreenLarger_YD2;
			m_PaperRectLog.bottom=-m_PaperSizeLog.cy-m_ScreenLarger_YD2;
			m_PageRectLog.top=m_OffsetLog.y-m_ScreenLarger_YD2;; //because it is point...so no need to invert in the logcal coordinates
 			m_PageRectLog.bottom=m_PageRectLog.top-m_PageSizeLog.cy;  //need to invert size for this map mode
	

		}
		else { //Paper Larger
		

			m_ScrollRectLog.bottom=-m_MaxRectLog.bottom-m_GrayMargin.cy;
			m_PaperRectLog.top=-m_offsetGrayYD2;
			m_PaperRectLog.bottom=-m_PaperSizeLog.cy-m_offsetGrayYD2; //because it is size...so must invert
			m_PageRectLog.top=m_OffsetLog.y-m_offsetGrayYD2; //because it is point...so no need to invert in the logcal coordinates
 			m_PageRectLog.bottom=m_PageRectLog.top-m_PageSizeLog.cy;  //need to invert size for this map mode
			

		}


		if (!m_PaperXLargerThanClientX) { //ScreenLarger
			
			m_ScrollRectLog.right=m_MaxRectLog.right;
			m_PaperRectLog.left=m_ScreenLarger_XD2;
			m_PaperRectLog.right=m_PaperSizeLog.cx+m_ScreenLarger_XD2;
			m_PageRectLog.left=m_OffsetLog.x+m_ScreenLarger_XD2;
		 	m_PageRectLog.right=m_PageRectLog.left+m_PageSizeLog.cx;
		
			
		}
		else { //Paper Larger

			m_ScrollRectLog.right=m_MaxRectLog.right+m_GrayMargin.cx;
			m_PaperRectLog.left=m_offsetGrayXD2;
			m_PaperRectLog.right=m_PaperSizeLog.cx+m_offsetGrayXD2; //because it is size...so must invert
			m_PageRectLog.left=m_OffsetLog.x+m_offsetGrayXD2;
		 	m_PageRectLog.right=m_PageRectLog.left+m_PageSizeLog.cx;


			 
		}


  
		m_WinOrg.x=-(m_PageRectLog.left);
		m_WinOrg.y=-(m_PageRectLog.top);
		
  
 
}

void CTextbmpView::OnZoomFitHeight() 
{
	// TODO: Add your command handler code here
	m_ZoomState=ZOOMFITHEIGHT;
	SetScreenGlobals();
	Invalidate(TRUE);

 	
}



//Fit Width
void CTextbmpView::OnZoomFitWidth() 
{
		// TODO: Add your command handler code here

	m_ZoomState=ZOOMFITWIDTH;
	SetScreenGlobals();
	Invalidate(TRUE);	

 	
		 
}

 
 

void CTextbmpView::SetDefaultPaper() 
{
	 

	
	//Set 8.5 inch by 11 inch
	if (m_mapmode==MM_TWIPS) {
	
		m_PaperSizeLog.cx=12244;
		m_PaperSizeLog.cy=15844;
		 
		m_OffsetLog.x=168;
		m_OffsetLog.y=-168;
		 
		m_PageSizeLog.cx=11904;
		m_PageSizeLog.cy=14880;


	}
	else if (m_mapmode==MM_HIENGLISH) {
	
		 
		m_PaperSizeLog.cx=8503;
		m_PaperSizeLog.cy=11003;

 		m_OffsetLog.x=117; 
		m_OffsetLog.y=-117;

 		m_PageSizeLog.cx=8267; 
		m_PageSizeLog.cy=10333;

	}
	else if (m_mapmode==MM_LOMETRIC) {
	
 
		m_PaperSizeLog.cx=2160; 
		m_PaperSizeLog.cy=2795;
 
		m_OffsetLog.x=30;
		m_OffsetLog.y=-30;
 
		m_PageSizeLog.cx=2100;
		m_PageSizeLog.cy=2625;



	}
	
	
}


  
void CTextbmpView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	 


	ScreenToPaper(&point); 
	m_StoredPt=point;
	Invalidate();
	 	
	CScrollView::OnLButtonDown(nFlags, point);
}


void CTextbmpView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CScrollView::OnPrepareDC(pDC, pInfo);
	 
}




  

void CTextbmpView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

}


void CTextbmpView::DocToClient(CRect &rect)
{
 	
  
	CClientDC drawdc(this);
	OnPrepareDC(&drawdc,NULL);
	drawdc.LPtoDP(rect);
	rect.NormalizeRect();

	
}
 

 
inline int roundleast(int n)
{
	int mod = n%10;
	n -= mod;
	if (mod >= 5)
		n += 10;
	else if (mod <= -5)
		n -= 10;
	return n;
}

static void RoundRect(LPRECT r1)
{
	r1->left = roundleast(r1->left);
	r1->right = roundleast(r1->right);
	r1->top = roundleast(r1->top);
	r1->bottom = roundleast(r1->bottom);
}

static void MulDivRect(LPRECT r1, LPRECT r2, int num, int div)
{
	r1->left = MulDiv(r2->left, num, div);
	r1->top = MulDiv(r2->top, num, div);
	r1->right = MulDiv(r2->right, num, div);
	r1->bottom = MulDiv(r2->bottom, num, div);
}


void CTextbmpView::OnMargins() 
{

	
	// TODO: Add your command handler code here
	CPageSetupDialog dlg;
	PAGESETUPDLG& psd = dlg.m_psd;
	psd.Flags |= PSD_MARGINS |	PSD_INTHOUSANDTHSOFINCHES | PSD_DISABLEORIENTATION | PSD_DISABLEPAPER | PSD_DISABLEPRINTER;
	int nUnitsPerInch = 1000;

	//convert m_rectUserMarginLog in logical units to thousandths of inches
	if (m_mapmode==MM_TWIPS) 
		MulDivRect(&psd.rtMargin, m_rectUserMarginLog, nUnitsPerInch, 1440);
	else if (m_mapmode==MM_LOMETRIC)
		MulDivRect(&psd.rtMargin, m_rectUserMarginLog, nUnitsPerInch, 254);
	else if (m_mapmode==MM_HIENGLISH)    
		MulDivRect(&psd.rtMargin, m_rectUserMarginLog, nUnitsPerInch, 1000);
			
	
	RoundRect(&psd.rtMargin);
	
	PRINTDLG pd;
	LPDEVNAMES ptrdevnames_pd;
	LPDEVMODE  ptrdevmode_pd;
	LPDEVNAMES ptrdevnames_psd;  
	LPDEVMODE  ptrdevmode_psd; 
	
	if (AfxGetApp()->GetPrinterDeviceDefaults(&pd)) {


		//Copy default settings and put them into the page setup dialog
		ptrdevnames_pd = (LPDEVNAMES) GlobalLock(pd.hDevNames);
		ptrdevmode_pd =  (LPDEVMODE) GlobalLock(pd.hDevMode);
		 
		if (psd.hDevNames) GlobalFree(psd.hDevNames);
		if (psd.hDevMode) GlobalFree(psd.hDevMode);

		psd.hDevNames = GlobalAlloc(GPTR, GlobalSize (pd.hDevNames));
		psd.hDevMode =  GlobalAlloc(GPTR, GlobalSize (pd.hDevMode));
		ptrdevnames_psd = (LPDEVNAMES) GlobalLock(psd.hDevNames);
		ptrdevmode_psd =  (LPDEVMODE) GlobalLock(psd.hDevMode);

		memcpy(ptrdevnames_psd,ptrdevnames_pd, (size_t) GlobalSize (pd.hDevNames));
		memcpy(ptrdevmode_psd,ptrdevmode_pd, (size_t) GlobalSize (pd.hDevMode));

		GlobalUnlock(ptrdevmode_pd); 
		GlobalUnlock(ptrdevmode_psd);
		GlobalUnlock(ptrdevnames_pd);
		GlobalUnlock(ptrdevnames_psd);

	}
	else {
	

		psd.hDevNames = NULL; 
		psd.hDevMode = NULL;  

	}


	if (dlg.DoModal() == IDOK)
	{
		RoundRect(&psd.rtMargin);


		//convert psd.rtMargin in thousandths of inches to logical units 
		if (m_mapmode==MM_TWIPS) 
			MulDivRect(m_rectUserMarginLog, &psd.rtMargin, 1440, nUnitsPerInch);
		else if (m_mapmode==MM_LOMETRIC)
			MulDivRect(m_rectUserMarginLog, &psd.rtMargin, 254, nUnitsPerInch);
		else if (m_mapmode==MM_HIENGLISH)  
			MulDivRect(m_rectUserMarginLog, &psd.rtMargin, 1000, nUnitsPerInch);


		EventMarginsChange();
			
		 
	}

	 
	
}


void  CTextbmpView::GetPaperData(CRect& margins, CRect& paperrectpix, CSize& papersizepix, CSize& papersizelog) 
{


	CClientDC drawdc(this);
	
	AdjustMapMode(&drawdc,FALSE);

	//Margins Data
	CRect MarginRectPixel;
	MarginRectPixel=m_rectUserMarginLog;
	drawdc.LPtoDP(&MarginRectPixel);

	MarginRectPixel.top=abs(MarginRectPixel.top);
	MarginRectPixel.bottom=abs(MarginRectPixel.bottom);
	MarginRectPixel.left=abs(MarginRectPixel.left);
	MarginRectPixel.right=abs(MarginRectPixel.right);
	
	margins=MarginRectPixel;



	//PaperRect Data
	CRect PaperRectPixel;

	PaperRectPixel=m_PaperRectLog;
	drawdc.LPtoDP(&PaperRectPixel);
	 
	paperrectpix=PaperRectPixel;


	//PaperSize in screen pixels
	CSize PaperSizePixel;

	PaperSizePixel=m_PaperSizeLog;
	drawdc.LPtoDP(&PaperSizePixel);

	papersizepix=PaperSizePixel;
	

	//Logical PaperSize Data
	papersizelog=m_PaperSizeLog;



}
	

void CTextbmpView::AdjustMapMode(CDC *pDC, BOOL bShiftOrigin)
{


	pDC->SetMapMode(m_mapmode);
	
	if (m_ZoomState==ZOOMFITWIDTH) {
		
			pDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDC->SetWindowExt(m_ScrollRectLog.right,-m_ScrollRectLog.right);
			CSize rSize = pDC->SetViewportExt(m_ClientRectDev.right,m_ClientRectDev.right);

	}
	else if (m_ZoomState==ZOOMFITHEIGHT) {

			long extValue,viewValue;
 
			extValue=labs(m_ScrollRectLog.bottom);
			viewValue=labs(m_ClientRectDev.bottom);
			pDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDC->SetWindowExt(extValue,-extValue);
			CSize rSize = pDC->SetViewportExt(viewValue,viewValue);

	}
	else if (m_ZoomState==ZOOMIN) {
		
			
			CSize WinSize = pDC->GetWindowExt();
			CSize ViewSize = pDC->GetViewportExt();

			ViewSize.cx=MulDiv(ViewSize.cx,m_ZoomPercent,100);
			ViewSize.cy=MulDiv(ViewSize.cy,m_ZoomPercent,100);

			pDC->SetMapMode(MM_ISOTROPIC); //gives WYSIWYG
			CSize vSize = pDC->SetWindowExt(WinSize);
			CSize rSize = pDC->SetViewportExt(ViewSize);

	}

	
	if (bShiftOrigin) pDC->SetWindowOrg(m_WinOrg);

	

}

BOOL CTextbmpView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	
	//return CScrollView::OnEraseBkgnd(pDC);
}


CDC* CTextbmpView::CreatePrinterDC()
{
   PRINTDLG PrtDlg;
   HDC      hDC;
   
   if (!AfxGetApp()->GetPrinterDeviceDefaults(&PrtDlg))
   {
       
		return NULL;

   } 
   else
   {
      CPrintDialog dlg(FALSE); 

      dlg.m_pd.hDevMode = PrtDlg.hDevMode;
      dlg.m_pd.hDevNames = PrtDlg.hDevNames;
    
      hDC = dlg.CreatePrinterDC();
   }
      
   CDC* pDC = CDC::FromHandle(hDC);
   pDC->m_bPrinting = TRUE;              
   return pDC;        
}



//Not yet tested...
void CTextbmpView::PaperToScreen(CSize* size) 
{

	CClientDC drawdc(this);
	OnPrepareDC(&drawdc);
	AdjustMapMode(&drawdc,TRUE);
 
	drawdc.LPtoDP(size);


}

void CTextbmpView::PaperToScreen(CPoint* point) 
{

	CClientDC drawdc(this);
	OnPrepareDC(&drawdc);
	AdjustMapMode(&drawdc,TRUE);
 
	drawdc.LPtoDP(point,1);





}

void CTextbmpView::ScreenToPaper(CSize* size) 
{

	CClientDC drawdc(this);
	OnPrepareDC(&drawdc);
	AdjustMapMode(&drawdc,TRUE);
 
	drawdc.DPtoLP(size);


}

void CTextbmpView::ScreenToPaper(CPoint* point) 
{

	CClientDC drawdc(this);
	OnPrepareDC(&drawdc);
	AdjustMapMode(&drawdc,TRUE);
 
	drawdc.DPtoLP(point,1);
	
	 
}
 


void CTextbmpView::OnViewZoom150() 
{
	// TODO: Add your command handler code here
	m_ZoomPercent=150;
	m_ZoomState=ZOOMIN;
	SetScreenGlobals();
	Invalidate(TRUE);

	
}

void CTextbmpView::OnUpdateViewZoom150(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck((m_ZoomState==ZOOMIN) && (m_ZoomPercent==150));
	
}

void CTextbmpView::OnViewZoom125() 
{
	// TODO: Add your command handler code here
	m_ZoomPercent=125;
	m_ZoomState=ZOOMIN;
	SetScreenGlobals();
	Invalidate(TRUE);

	
}

void CTextbmpView::OnUpdateViewZoom125(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck((m_ZoomState==ZOOMIN) && (m_ZoomPercent==125));
	
	
}

void CTextbmpView::OnViewZoom75() 
{
	// TODO: Add your command handler code here
	m_ZoomPercent=75;
	m_ZoomState=ZOOMIN;
	SetScreenGlobals();
	Invalidate(TRUE);

	
}

void CTextbmpView::OnUpdateViewZoom75(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck((m_ZoomState==ZOOMIN) && (m_ZoomPercent==75));
	
}

void CTextbmpView::OnUpdateViewZoom100(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck((m_ZoomState==ZOOMIN) && (m_ZoomPercent==100));
	
}


void CTextbmpView::OnViewZoom100() 
{
	// TODO: Add your command handler code here
	m_ZoomPercent=100;
	m_ZoomState=ZOOMIN;
	SetScreenGlobals();
	Invalidate(TRUE);
	
}

void CTextbmpView::OnUpdateViewFitheight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_ZoomState==ZOOMFITHEIGHT);
	
}

void CTextbmpView::OnUpdateViewFitwidth(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_ZoomState==ZOOMFITWIDTH);
	
}


void CTextbmpView::EventPaperChange() 
{

	CRect rMargins, rPaper, rPrintable;
	GetPageInfo(&rMargins, &rPaper, &rPrintable);


	TRACE("\nrMargins top=%d, left=%d, bottom=%d, right=%d",rMargins.top,rMargins.left,rMargins.bottom,rMargins.right);
	TRACE("\nrPaper top=%d, left=%d, bottom=%d, right=%d",rPaper.top,rPaper.left,rPaper.bottom,rPaper.right);
	TRACE("\nrPrintable top=%d, left=%d, bottom=%d, right=%d",rPrintable.top,rPrintable.left,rPrintable.bottom,rPrintable.right);
	 

	//This event is called after the print setup dialog has been displayed
	//Todo: add code here 
	 




}
	

void CTextbmpView::EventMarginsChange()
{
	
	CRect rMargins ,rPaper, rPrintable;
	GetPageInfo(&rMargins, &rPaper, &rPrintable);
	 

	//This event is called after the margins setup dialog has been displayed and the user clicked OK
	//Todo: add code here 
	 


}


void CTextbmpView::GetPageInfo(CRect* rectMarginsLog, CRect* rectPaperLog, CRect* rectPrintableLog)
{
	int orgx,orgy;
	orgx=m_PageRectLog.left;
	orgy=m_PageRectLog.top;
		
	rectPaperLog->top=m_PaperRectLog.top-orgy;
	rectPaperLog->bottom=m_PaperRectLog.bottom-orgy;
	rectPaperLog->left=m_PaperRectLog.left-orgx;
	rectPaperLog->right=m_PaperRectLog.right-orgx;

	rectPrintableLog->top=m_PageRectLog.top-orgy;
	rectPrintableLog->bottom=m_PageRectLog.bottom-orgy;
	rectPrintableLog->left=m_PageRectLog.left-orgx;
	rectPrintableLog->right=m_PageRectLog.right-orgx;

	rectMarginsLog->top=rectPaperLog->top-m_rectUserMarginLog.top;
	rectMarginsLog->bottom=rectPaperLog->bottom+m_rectUserMarginLog.bottom;
	rectMarginsLog->left=rectPaperLog->left+m_rectUserMarginLog.left;
	rectMarginsLog->right=rectPaperLog->right-m_rectUserMarginLog.right;

}
	