/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
// GraphView.cpp: implementation of the GraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\Framewin.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\GraphLegend.h"
#include <afxpriv.h>
#include "Client\Editors\Utils\GeneralBar.h"
#include "Headers\Hs\Graph.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int iMINSIZE = 50;

IMPLEMENT_DYNCREATE(GraphView, CView)

BEGIN_MESSAGE_MAP(GraphView, CView)
//  ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_GRPH_OPTIONS, OnGraphOptions)
	ON_COMMAND(ID_COPY, OnGraphCopyClipboard)
	ON_COMMAND(ID_GRPH_LEGEND, OnGraphLegend)
	ON_COMMAND(ID_FILE_PRINT,	OnFilePrint)
END_MESSAGE_MAP()


GraphView::GraphView()
: iBorder(15), fnt(0), info(0), fFitOnPage(true), rWidthInCm(10), grdrw(0)
{
	fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
	CDC cdc;
	cdc.CreateCompatibleDC(0);
	CFont* fntOld = cdc.SelectObject(fnt);
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	iCharHeight = tm.tmHeight - 1;
  cdc.SelectObject(fntOld);
}

GraphView::~GraphView()
{
	if (0 != info)
	  delete info;
}

void GraphView::Create(CWnd *wnd)
{
  CView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), wnd, 100, 0);
}

zPoint GraphView::pntPos(double rRow, double rCol) const
{
  zPoint p;
  long tmp;
  tmp = _rScaleX * rCol;
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.x = tmp;
  tmp = _rScaleY * rRow;
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  p.y = tmp;
  return p;

}

void GraphView::Pnt2RowCol(zPoint p, double& rRow, double &rCol)
{
  rRow = p.y / _rScaleY;
  rCol = p.x / _rScaleX;
}

void GraphView::OnSize(UINT nType, int cx, int cy) 
{
	if (0 == cx || 0 == cy)
		return;
	bool fFirst = dim.cx <= 0;
 	if (fFirst) 
		OnGraphFitInWindow();
	OnGraphFitInWindow();
  if (grdrw && grdrw->grleg) 
  {
    // if legend outside window, move it.
    if (cx < grdrw->grleg->rect.left || cy < grdrw->grleg->rect.top)
      grdrw->grleg->rect = CRect(100,100,100,100);
  }
}

void GraphView::OnDraw(CDC* pDC)
{
	if (0 == fnt)
		OnInitialUpdate();
	if (0 != grdrw)
	  grdrw->draw(pDC);
}

MinMax GraphView::mmBounds()
{
	// graph view works with dimension 0..1000
	// these are mapped by ZoomableView to screen coords
	return MinMax(RowCol(0,0), RowCol(1000,1000)); // internal units mapped on 'entire' graph axis
}

bool GraphView::fConfigure()
{
	if (0 != grdrw)
	  return grdrw->fConfigure();
	return false;
}

void GraphView::OnInitialUpdate()
{
	if (0 != grdrw)
	  grdrw->Init();
  info = new InfoLine(this);
  PostMessage(WM_COMMAND, ID_GRPH_FITINWINDOW, 0);
}


#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

void GraphView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_GRPH_OPTIONS);
	// no add because not always possible
	add(ID_COPY);
	add(ID_FILE_PRINT);
  men.AppendMenu(MF_SEPARATOR);
	add(ID_GRPH_FITINWINDOW);
	GeneralBar* gb = dynamic_cast<GeneralBar*>(GetParent());
	if (gb) {
	  men.AppendMenu(MF_SEPARATOR);
		add(ID_ALLOWDOCKING);
		men.CheckMenuItem(ID_ALLOWDOCKING, gb->fAllowDocking() ? MF_CHECKED : MF_UNCHECKED);
	}
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void GraphView::OnGraphOptions()
{
  if (0 == grdrw)
    return;
  if (grdrw->fConfigure())
    Invalidate();
}

void GraphView::OnGraphFitInWindow()
{
  CRect rect;
  GetClientRect(&rect);
  dim = rect.Size();
	dim.cx -= 2 * iBorder;
	dim.cy -= 2 * iBorder;
  
  if (0 != grdrw) {
  	grdrw->CalcAxisSizes();
    dim.cx -= grdrw->iSpaceNeededX();
	  dim.cy -= grdrw->iSpaceNeededY();
	}

	_rScaleX = dim.width() / 1000.0;
	_rScaleY = dim.height() / 1000.0;
	Invalidate();
}

void GraphView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  if (0 == grdrw)
    return;
  GraphDrawer::Area area = grdrw->aPoint(point);
  if (area == GraphDrawer::aLEGEND) 
  {
    CRectTracker tracker;
    tracker.m_rect = grdrw->grleg->rect;
    if (tracker.Track(this,point)) 
    {
      grdrw->grleg->rect = tracker.m_rect;
      GetDocument()->SetModifiedFlag();
      Invalidate();
    }
    return;
  }
  if (area != GraphDrawer::aGRAPH)  
    return;
	String s = grdrw->sText(point);
	SetCapture();
	info->text(point,s);
}

void GraphView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	info->text(point,"");
	ReleaseCapture();
}

void GraphView::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (0 != grdrw)
  	if (MK_LBUTTON & nFlags) {
      GraphDrawer::Area area = grdrw->aPoint(point);
      if (area != GraphDrawer::aGRAPH) 
      {
  	  	info->text(point,"");
      }
      else {
        String s = grdrw->sText(point);
    		SetCapture();
  	  	info->text(point,s);
      }
  	}
}

void GraphView::OnGraphCopyClipboard()
{
	if (!OpenClipboard())
		return;
	EmptyClipboard();

	CRect rect;
	GetClientRect(rect);
	CSize sz = rect.Size();
	CMetaFileDC mdc;
	mdc.Create();
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);
	mdc.SetAttribDC(dc.GetSafeHdc());
	mdc.SetWindowExt(sz);
	mdc.SetWindowOrg(rect.TopLeft());
	OnDraw(&mdc);
	HMETAFILE hMF = mdc.Close();

	HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,sizeof(METAFILEPICT)+1);
	METAFILEPICT* mp = (METAFILEPICT*)GlobalLock(hnd);
	mp->mm = MM_TEXT;
	mp->xExt = sz.cx;
	mp->yExt = -sz.cy;
	mp->hMF = hMF;
	GlobalUnlock(hnd);
	SetClipboardData(CF_METAFILEPICT, hnd);	

	CloseClipboard();
}

void GraphView::OnGraphLegend()
{
	MessageBox("Not yet implemented", "Coming Soon!", MB_OK | MB_ICONEXCLAMATION);
}

BOOL GraphView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL bResult;
	CWinApp* pApp = AfxGetApp();


	// ask our app what the default printer is
	// if there isn't any, punt to MFC so it will generate an error

	if (!pApp->GetPrinterDeviceDefaults(&pInfo->m_pPD->m_pd) ||
		pInfo->m_pPD->m_pd.hDevMode == NULL)
		return DoPreparePrinting(pInfo);

	HGLOBAL	hDevMode = pInfo->m_pPD->m_pd.hDevMode;
	HGLOBAL hDevNames = pInfo->m_pPD->m_pd.hDevNames;

	DEVMODE* pDevMode = (DEVMODE*) ::GlobalLock(hDevMode);
	DEVNAMES* pDevNames = (DEVNAMES*) ::GlobalLock(hDevNames);

	LPCSTR pstrDriverName = ((LPCSTR) pDevNames)+pDevNames->wDriverOffset;
	LPCSTR pstrDeviceName = ((LPCSTR) pDevNames)+pDevNames->wDeviceOffset;
	LPCSTR pstrOutputPort = ((LPCSTR) pDevNames)+pDevNames->wOutputOffset;
	CDC dcPrinter;
	if (dcPrinter.CreateDC(pstrDriverName, pstrDeviceName, pstrOutputPort, NULL))
	{
		int iSelection = fFitOnPage ? 0 : 1; // 0 is Fit ; 1 is define width in cm
		class FormPrintOptions : public FormWithDest
		{
		public:
			FormPrintOptions(CWnd* parent, int* iSel, double* rWidthInCm, CDC* dcPrinter)
				: FormWithDest(parent, TR("Print Options"))
			{
				RadioGroup* rg = new RadioGroup(root, "", iSel);
				new RadioButton(rg, TR("&Fit on Page"));
				RadioButton* rb = new RadioButton(rg, TR("&Width (cm)"));
				int iPrWidth = dcPrinter->GetDeviceCaps(HORZSIZE)/10 - 1.5; // margins
				new FieldReal(rb, "", rWidthInCm, ValueRange(0, iPrWidth, 0.1));
				SetMenHelpTopic("ilwismen\\graph_window_print_graph.htm");
				create();
			}
		};
		FormPrintOptions frm(this, &iSelection, &rWidthInCm, &dcPrinter);
		if (!frm.fOkClicked())
			return FALSE;
		fFitOnPage = iSelection == 0;
		dcPrinter.DeleteDC();

		pInfo->SetMinPage(1);
		pInfo->SetMaxPage(1);
		bResult = DoPreparePrinting(pInfo);
	}
	else
	{
		MessageBox("Could not create printer DC");
		bResult = FALSE;
	}
	::GlobalUnlock(hDevMode);
	::GlobalUnlock(hDevNames);

	return bResult;

}

void GraphView::OnPrint(CDC* cdc, CPrintInfo* pInfo) 
{
	double rResX = cdc->GetDeviceCaps(HORZRES) / double(cdc->GetDeviceCaps(HORZSIZE));
	double rResY = cdc->GetDeviceCaps(VERTRES) / double(cdc->GetDeviceCaps(VERTSIZE));
  int iOffsetX, iOffsetY;
	iOffsetX = (int)(10 * rResX); // 1 cm.
	iOffsetY = (int)(10 * rResY);  // 1 cm.
	cdc->SetMapMode(MM_ISOTROPIC);
	cdc->SetWindowOrg(0,0);
	cdc->SetViewportOrg(iOffsetX, iOffsetY);
	CRect rect;
	GetClientRect(&rect);
  cdc->SetWindowExt(rect.Size());
	if (fFitOnPage) {
		int iPrX = cdc->GetDeviceCaps(HORZRES) - (int)(iOffsetX * 1.5); // half of offset right
		int iPrY = cdc->GetDeviceCaps(VERTRES) - (int)(iOffsetX * 1.5); // half of offset below
  	cdc->SetViewportExt(CSize(iPrX, iPrY));
	}
	else {
		int iPrX = (int)(rResX * 10 * rWidthInCm);
		int iPrY = (int)(rResY * 10 * rWidthInCm * rect.Height() / rect.Width());
   	cdc->SetViewportExt(CSize(iPrX, iPrY));
	}
  OnDraw(cdc);
}

int GraphView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int i = CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
//	GetTopLevelFrame()->SetActiveView(this);
	return i;
}

void GraphView::SaveSettings(const FileName& fn, int iGraphWindow)
{
	// do nothing
}

void GraphView::LoadSettings(const FileName& fn, int iGraphWindow)
{
	// do nothing
}

void GraphView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  GraphDrawer::Area area = grdrw->aPoint(point);
  GraphAxis* gaxis = 0;
  bool fChanged = false;
  switch (area)
  {
    case GraphDrawer::aXAXIS:
      gaxis = ga(GraphAxis::gapX);
      break;
    case GraphDrawer::aYAXISLEFT:
      gaxis = ga(GraphAxis::gapYLeft);
      break;
    case GraphDrawer::aYAXISRIGHT:
      gaxis = ga(GraphAxis::gapYRight);
      break;
    case GraphDrawer::aROSEX:
      gaxis = ga(GraphAxis::gapXRose);
      break;
    case GraphDrawer::aROSEY:
      gaxis = ga(GraphAxis::gapYRose);
      break;
    case GraphDrawer::aTITLE:
      fChanged = grdrw->fConfigureTitle();
      break;
  }
  if (0 != gaxis)
    fChanged = gaxis->fConfig();
  if (fChanged) 
  {
    CDocument* doc = GetDocument();
    doc->SetModifiedFlag();
    doc->UpdateAllViews(0);
  }
}

GraphAxis* GraphView::ga(GraphAxis::GraphAxisPos gp) const
{
  CartesianGraphDrawer* cgd = dynamic_cast<CartesianGraphDrawer*>(grdrw);
  RoseDiagramDrawer* rdd = dynamic_cast<RoseDiagramDrawer*>(grdrw);
  if (0 != cgd) 
  {
    switch (gp)
    {
      case GraphAxis::gapX:
        return cgd->gaxX;
      case GraphAxis::gapYLeft:
        return cgd->gaxYLeft;
      case GraphAxis::gapYRight:
        return cgd->gaxYRight;
      default:
        return 0;
    }
  }  
  if (0 != rdd)
  {
    switch (gp)
    {
      case GraphAxis::gapXRose:
        return rdd->rdaxX;
      case GraphAxis::gapYRose:
        return rdd->rdaxY;
      default:
        return 0;
    }
  }  
  return 0;
}


