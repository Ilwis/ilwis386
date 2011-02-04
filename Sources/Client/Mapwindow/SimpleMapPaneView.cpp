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
// SimpleMapPaneView.cpp: implementation of the SimpleMapPaneView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoBar.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\Editors\Map\BaseMapEditor.h"
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Client\Editors\Editor.h"
#include "Client\Mapwindow\DistanceMeasurer.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Engine\SpatialReference\csdirect.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mapwind.hs"
#include "Engine\Domain\Dmvalue.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MapPaneView

IMPLEMENT_DYNCREATE(SimpleMapPaneView, ZoomableView)

BEGIN_MESSAGE_MAP(SimpleMapPaneView, ZoomableView)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_MEASUREDIST, OnMeasureDist)
	ON_UPDATE_COMMAND_UI(ID_MEASUREDIST, OnUpdateMeasureDist)
END_MESSAGE_MAP()

void SimpleMapPaneView::MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}

const int RepresentationClass::iSIZE_FACTOR=3; // MapWindow to Layout ratio; never make this 0; currently only int ratio supported

SimpleMapPaneView::SimpleMapPaneView()
: info(0), edit(0), dcView(0), cwcsButtonDown(Coord()), fwPar(0), bmView(0), hBmOld(0), pDC(0), drawThread(0), fStopDrawThread(false), fDrawRequest(false), fResizing(false)
{
	fDirty = false;
	fRedrawing = false;
	fDrawStop = false;
	fStarting = true; // prevent too early start
	fDrawAlsoWhenLoading = false;

	hmodMsimg32 = LoadLibrary("msimg32.dll");
	if (hmodMsimg32 <= 0)
	{
		hmodMsimg32 = 0;
		alphablendfunc = 0;
	}
	else {
		alphablendfunc = (AlphaBlendFunc)GetProcAddress(hmodMsimg32, "AlphaBlend");
	}
}

SimpleMapPaneView::~SimpleMapPaneView()
{
	if (drawThread) {
		fStopDrawThread = true;
		drawThread->ResumeThread();
		csThread.Lock(); // wait here til thread exits
		csThread.Unlock();
	}

	while (fRedrawing) {
		fDrawStop = true;
		Sleep(10);
	}
	delete info;
	delete edit;
	delete pib;
	if (dcView)
	{
		// First attempt to clean up GDI and memory .. according to the "official" way
		CBitmap * bm = dcView->SelectObject(CBitmap::FromHandle(hBmOld));
		if (bm) // If a bitmap popped out, clean it up !! don't clean up selected objs !!
		{
			bmView->DeleteObject();
			delete bmView;
			bmView = 0; // intentional, for test below
		}
		dcView->DeleteDC();
		delete dcView;
	}
	// Note: When SimpleMapPaneView was named MapPaneView, and under Windows 98, the code above
	// seemed to work well. Now (SimpleMapPaneView and Windows 2000) the behavior seems different,
	// so either Windows 2000 garbage-collects the GDI and the SelectObject above is not needed,
	// or the CDC is gone, which means the SelectObject should happen earlier at a moment that
	// the CDC still exists, or the TempGDIObject bmOld is invalid and SelectObject doesn't work.
	// Hence here (and also at RedrawInThread) the 2nd attempt to finalize cleaning up GDI and memory
	if (bmView) // means code above failed, so we stil have some cleanup to do
	{
		bmView->DeleteObject(); // encapsulating dc is gone, so this should be ok now
		delete bmView;
	}
	FreeLibrary(hmodMsimg32);
}

MapCompositionDoc* SimpleMapPaneView::GetDocument()
{
	return (MapCompositionDoc*)m_pDocument;
}

void SimpleMapPaneView::OnInitialUpdate()
{
	if (0 == fwPar) {
		CFrameWnd* fw = GetTopLevelFrame();
		fwPar = dynamic_cast<FrameWindow*>(fw);
		NewDrawer *drw = GetDocument()->rootDrawer->getDrawer(0);
		MapStatusBar *sbar = dynamic_cast<MapStatusBar*>(fwPar->status);
		sbar->SetActiveDrawer(drw);
	}
	ZoomableView::OnInitialUpdate();
	_rScale = rUNDEF; // derived class has to set rScale in some way on a valid value
	info = new InfoLine(this);
}

Coord SimpleMapPaneView::crdPnt(zPoint pnt)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//Coord crd;
	//double rRow, rCol;
	//Pnt2RowCol(pnt, rRow, rCol);
	//try {
	//	GetDocument()->georef->RowCol2Coord(rRow,rCol,crd);
	//}
	//catch (...) {
	//	crd = Coord();
	//}  
	//return crd;
}

zPoint SimpleMapPaneView::pntPos(Coord crd)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//try {
	//	double rRow, rCol;
	//	GetDocument()->georef->Coord2RowCol(crd, rRow, rCol);
	//	return pntPos(rRow, rCol);
	//}
	//catch (...) {
	//	return zPoint(shUNDEF, shUNDEF);
	//}  
}

BOOL SimpleMapPaneView::OnEraseBkgnd(CDC* cdc)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		/*	CRect rct;
		GetClientRect(&rct);
		mcd->rootDrawer->getDrawerContext()->setViewPort(RowCol(rct.Height(), rct.Width()));
		mcd->rootDrawer->prepare(NewDrawer::ptALL);*/
		//mcd->rootDrawer->draw();
		//SwapBuffers(cdc->m_hDC);
	}

	return TRUE;
}

void SimpleMapPaneView::OnDraw(CDC* cdc)
{
	fStarting  = false;
	if (!pDC) {
		pDC = GetDC();
		MapCompositionDoc* mcd = GetDocument();
		PreparationParameters pp(NewDrawer::ptINITOPENGL, pDC);
		mcd->rootDrawer->prepare(&pp);
	}
	RequestRedraw();
}

void SimpleMapPaneView::RequestRedraw()
{
	fDrawRequest = true;
	if (!drawThread)
		drawThread = AfxBeginThread(SimpleMapPaneView::DrawInThread, (LPVOID)this); 
	else
		drawThread->ResumeThread();
}

UINT SimpleMapPaneView::DrawInThread(LPVOID lp)
{
	SimpleMapPaneView* mpv = (SimpleMapPaneView*)lp;
	MapCompositionDoc* mcd = mpv->GetDocument();
	mpv->csThread.Lock();
	try {
		while (!mpv->fStopDrawThread) {
			//clock_t start = clock();

			mcd->rootDrawer->getDrawerContext()->TakeContext();
			while (mpv->fDrawRequest)
				mpv->Draw();
			mcd->rootDrawer->getDrawerContext()->ReleaseContext();

			//clock_t end = clock();
			//double total =  1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
			//TRACE(String("drawn DrawInThread in %2.2f milliseconds;\n", total).scVal());
			//TRACE("------\n");

			if (!mpv->fStopDrawThread)
				mpv->drawThread->SuspendThread(); // wait here, and dont consume CPU time either
		
		}
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	mpv->fStopDrawThread = false;
	mpv->drawThread = 0;
	mpv->csThread.Unlock();
	return FALSE;
}

void SimpleMapPaneView::Draw()
{
	fDrawRequest = false;
	MapCompositionDoc* mcd = GetDocument();
	mcd->rootDrawer->draw();
	csResizing.Lock();
	if (fResizing) {
		fResizing = false;
		fDrawRequest = true;
	} else
		mcd->rootDrawer->getDrawerContext()->swapBuffers();
	csResizing.Unlock();
}

void SimpleMapPaneView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	csResizing.Lock();
	fResizing = true;
	csResizing.Unlock();
}

void SimpleMapPaneView::OnMeasureDist()
{
	OnNoTool();
	tools[ID_MEASUREDIST] = new DistanceMeasurer(this);
	iActiveTool = ID_MEASUREDIST;
}

void SimpleMapPaneView::OnUpdateMeasureDist(CCmdUI* pCmdUI)
{
	//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//bool fGeoRefNone = true; // pessimistic
	//GeoRef gr (GetDocument()->georef);
	//if (gr.fValid()) // then we can recalculate the boolean
	//	fGeoRefNone = gr->fGeoRefNone();
	//pCmdUI->Enable(!fGeoRefNone);
	//if (0 == as)
	//	iActiveTool = 0;
	//pCmdUI->SetRadio(ID_MEASUREDIST == iActiveTool);
}

void SimpleMapPaneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (tools.size() > 0 && VK_ESCAPE == nChar) {
		tools.OnEscape();
		return;
	}
	if (edit && edit->OnKeyDown(nChar, nRepCnt, nFlags))
		return;
	bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	switch (nChar) {
		// mouse movement
	case VK_LEFT:
		if (fCtrl)
			MoveMouse(-10, 0);
		else  
			MoveMouse(-1, 0);
		return;
	case VK_UP:
		if (fCtrl)
			MoveMouse(0, -10);
		else  
			MoveMouse(0, -1);
		return;
	case VK_RIGHT:
		if (fCtrl)
			MoveMouse(10, 0);
		else  
			MoveMouse(1, 0);
		return;
	case VK_DOWN:
		if (fCtrl)
			MoveMouse(0, 10);
		else  
			MoveMouse(0, 1);
		return;
	}
	ZoomableView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SimpleMapPaneView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	if (edit && edit->OnMouseMove(nFlags, point)) 
		return;
	MapCompositionDoc* mcd = GetDocument();

	bool fOutside = false; //!rctBounds.PtInRect(point);

	Coord c;
	//Pnt2RowCol(point, rRow, rCol);
	bool fValid = true;
	c = mcd->rootDrawer->screenToWorld(RowCol(point.y,point.x));
	MapStatusBar* msb = 0;
	CoordSystem csy = mcd->rootDrawer->getCoordinateSystem();
	if (fwPar) 
		msb = dynamic_cast<MapStatusBar*>(fwPar->status);

	if (msb && !fOutside) {
		//if (mcd->fShowRowCol) {
			//RowCol rc((long)floor(1+rRow), (long)floor(1+rCol));
			//msb->ShowRowCol(rc);
		//}
		if (c.fUndef())
			fValid = false; // fNone = true;  
		//if (fNone) {
		//	msb->ShowCoord(SMWRem__NoCoords);
		//	msb->ShowLatLon(LatLon());
		//}
		//else if (!fValid) {
		//	msb->ShowCoord(SMWRem__NoCoordsCalculated);
		//	msb->ShowLatLon(LatLon());
		//}
		else {    
			msb->ShowCoord(csy->sValue(c));
			if (csy->pcsDirect()) {
				CoordSystemDirect *pcsd = csy->pcsDirect();
				CoordSystemPtr * pcs = pcsd->csOther.ptr();
				Coord crd = pcsd->cInverseConv(pcs,c);
				msb->ShowRelCoord(pcsd->csOther->sValue(crd));
			}
			if (csy->fCoord2LatLon()) {
				LatLon ll = csy->llConv(c);
				msb->ShowLatLon(ll);
			}
			else
				msb->ShowLatLon(LatLon());
		}  
	}

	if (0 == tools.size() ==0 && (MK_LBUTTON & nFlags)) {
		zRect rect;
		GetClientRect(&rect);
		if (!rect.PtInRect(point)) {
			info->text(point,"");
			int xInc = 0;
			int yInc = 0;
			if (point.x < rect.left()) {
				int iDiff = point.x - rect.left();
				horzPixMove(iDiff);
				xInc = 1;
			}
			if (point.x > rect.right()) {
				int iDiff = point.x - rect.right();
				horzPixMove(iDiff);
				xInc = -1;
			}
			if (point.y < rect.top()) {
				int iDiff = point.y - rect.top();
				vertPixMove(iDiff);
				yInc = 1;
			}
			if (point.y > rect.bottom()) {
				int iDiff = point.y - rect.bottom();
				vertPixMove(iDiff);
				yInc = -1;
			}
			MoveMouse(xInc, yInc);
			UpdateWindow();
		}
		else {
			bool fHide = true;
			if (fValid && !fOutside){
				String s = mcd->rootDrawer->getInfo(c);
				if (s != "") {
					info->text(point,s);
					info->ShowWindow(SW_SHOW);
				}
			}
		}
	}

	if (fValid && !fOutside) {
		CoordWithCoordSystem cwcs(c, mcd->rootDrawer->getCoordinateSystem());
		int state = cmMOUSEMOVE;
		if ( iActiveTool == ID_ZOOMIN)
			state |= cmZOOMIN;
		IlwWinApp()->SendUpdateCoordMessages(state, &cwcs);
	}
	CView::OnMouseMove(nFlags, point);
}

void SimpleMapPaneView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	MapCompositionDoc* mcd = GetDocument();
	RowCol rc(point.y, point.x);
	CPoint pnt(rc.Col, rc.Row);
	Coord c = mcd->rootDrawer->screenToWorld(rc);
	cwcsButtonDown = CoordWithCoordSystem(c, mcd->rootDrawer->getCoordinateSystem());
	if (edit && edit->OnLButtonDown(nFlags, point)) {
		IlwWinApp()->SendUpdateCoordMessages(cmMOUSECLICK, &cwcsButtonDown);
		return;
	}
	if (c.fUndef()) return;
	if ( tools.size() == 0) {
		SetCapture();
		String s = mcd->rootDrawer->getInfo(c);
		if (s != "") {
			info->text(point,s);
			info->ShowWindow(SW_SHOW);
		}
	}

	//mcd->mpvGetView()->Invalidate();
	int state = cmMOUSECLICK;
	if ( iActiveTool == ID_ZOOMIN)
			state |= cmZOOMIN;
	IlwWinApp()->SendUpdateCoordMessages(state, &cwcsButtonDown);
}

void SimpleMapPaneView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	MapCompositionDoc* mcd = GetDocument();
	info->ShowWindow(SW_HIDE);
	ReleaseCapture();
	if (edit && edit->OnLButtonUp(nFlags, point)) {
		return;
	}
	mcd->mpvGetView()->Invalidate();
}

BOOL SimpleMapPaneView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (edit && edit->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return ZoomableView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL SimpleMapPaneView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (HTCLIENT == nHitTest) {
		if (tools.size() > 0 && iActiveTool > 0 && tools[iActiveTool]->OnSetCursor())
			return TRUE;
		else if (edit && edit->OnSetCursor())
			return TRUE;
		else if (fRedrawing) {
			SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
			return TRUE;
		}
	}
	return ZoomableView::OnSetCursor(pWnd, nHitTest, message);
}

UINT SimpleMapPaneView::RedrawPaneInThread(LPVOID lp)
{
	SimpleMapPaneView* mpv = (SimpleMapPaneView*)lp;
	try {
		while (mpv->fDirty) {
		//	mpv->RedrawInThread();
		}
		// invalidate only after finished redrawing
		if (mpv->m_hWnd != 0)
			mpv->Invalidate();
		// be sure to set the cursor by moving the mouse
		MoveMouse(-1, 0);
		MoveMouse(1, 0);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	return FALSE;
}

void SimpleMapPaneView::SetDirty()
{
	if (fStarting)
		return;

	fDirty = true;
	if (fRedrawing)
		fDrawStop = true;
	if (!fRedrawing) {
		fRedrawing = true;
		OnDraw(0);
		fRedrawing = false;
	}
	fDrawAlsoWhenLoading = false;
}

void SimpleMapPaneView::SetDirtySilent()
{
	if (fStarting)
		return;

	fDirty = true;
	if (!fRedrawing) {
		fRedrawing = true;
		AfxBeginThread(SimpleMapPaneView::RedrawPaneInThread, (LPVOID)this); 
	}
	fDrawAlsoWhenLoading = true;
}

double SimpleMapPaneView::rScaleShow()
{
	MapCompositionDoc* mcd = GetDocument();
	//if (0 == mcd || !mcd->georef.fValid())
	//	return rUNDEF;
	//double rPixSize = mcd->georef->rPixSize();
	//if (rUNDEF == rPixSize)
	//	return rUNDEF;
	//CoordSystemLatLon* csll = mcd->georef->cs()->pcsLatLon();
 // if (0 != csll)
 //   rPixSize *= M_PI / 180 * csll->ell.a;
	//if (rScale() > 0)
	//	rPixSize /= rScale();
	//else if (rScale() < 0)
	//	rPixSize *= -rScale();

	//CClientDC dc(this);
	//int iPixPerInchY = dc.GetDeviceCaps(LOGPIXELSY);
 // double rPixPermm = iPixPerInchY / 25.4;         // (pixel per mm)

	//rPixSize *= rPixPermm;
	//rPixSize *= 1000; // 1000 mm per m
	//return rPixSize;
	return rUNDEF;
}

BaseMapEditor* SimpleMapPaneView::editGet() 
{
	return edit;
}

void SimpleMapPaneView::createEditor(ComplexDrawer *drw) {
	BaseMap bmp;
	bmp.SetPointer(((AbstractMapDrawer *)drw->getParentDrawer())->getBaseMap());
	IObjectType type = IOTYPE(bmp->fnObj);
	if ( edit){
		pib->pixview->GetDocument()->setEditFeature(0);
		delete edit;
		edit = 0;
	}
	if ( type == IlwisObject::iotPOINTMAP) {
		edit = ILWISAPP->getMEditor("PointSetEditor","ilwis38",GetDocument(), bmp);
		edit->init(drw, pib->pixview->GetDocument());

	}
	if ( type == IlwisObject::iotSEGMENTMAP) {
		edit = ILWISAPP->getMEditor("LineSetEditor","ilwis38",GetDocument(), bmp);
		edit->init(drw, pib->pixview->GetDocument());

	}
}

void SimpleMapPaneView::createPixInfoBar() {
	if ( !pib && GetDocument()->pixInfoDoc) {
		CFrameWnd* fw = GetTopLevelFrame();
		MapWindow *parent = dynamic_cast<MapWindow*>(fw);
		pib = new PixelInfoBar();
		pib->Create(parent);
		pib->SetWindowText(TR("Pixel Info").scVal());
		pib->EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
		GetDocument()->pixInfoDoc->AddView(pib->pixview);
		parent->RecalcLayout();
		CRect rct;
		parent->ltb.GetWindowRect(&rct);
		rct.OffsetRect(0,1);
		parent->DockControlBar(pib, AFX_IDW_DOCKBAR_LEFT,&rct);

	}
}

#ifdef _DEBUG
void SimpleMapPaneView::AssertValid() const
{
	// prevent errors with OLE
	// CView::AssertValid();
}

void SimpleMapPaneView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
