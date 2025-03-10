/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52�North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\Drawers\ScreenSwapper.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
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
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mapwind.hs"
#include "Engine\Domain\Dmvalue.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\TextDrawer.h"
//#include "Engine\Drawers\MouseClickInfoDrawer.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"
#include "Client\Mapwindow\PanTool.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Engine\Base\System\RegistrySettings.h"

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
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
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
: info(0), edit(0), cwcsButtonDown(Coord()), fwPar(0), pDC(0), drawThread(0), fStopDrawThread(false), fDrawRequest(false), fResizing(false)
{
	fDirty = false;
	fRedrawing = false;
	fDrawStop = false;
	fStarting = true; // prevent too early start
	fDrawAlsoWhenLoading = false;
	IlwisSettings settings("DefaultSettings");
	fSoftwareRendering = settings.fValue("SoftwareRendering", false);

	swapper = new ScreenSwapper();
}

SimpleMapPaneView::~SimpleMapPaneView()
{
	if (drawThread) {
		fStopDrawThread = true;
		drawThread->ResumeThread();
		csThread.Lock(); // wait here til thread exits
		csThread.Unlock();
	}
	delete swapper;
	while (fRedrawing) {
		fDrawStop = true;
		Sleep(10);
	}
	delete info;
	delete edit;
	delete pib;
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
	MapCompositionDoc* mcd = GetDocument();
	Coord crd = mcd->rootDrawer->screenToWorld(RowCol(pnt.y, pnt.x));
	return crd;
}

zPoint SimpleMapPaneView::pntPos(Coord crd)
{
	MapCompositionDoc* mcd = GetDocument();
	RowCol rc = mcd->rootDrawer->WorldToScreen(crd);
	zPoint pnt (rc.Col, mcd->rootDrawer->getViewPort().Row - rc.Row);
	return pnt;
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
		if (fSoftwareRendering)
			pp.contextMode |= DrawerContext::mSOFTWARERENDERER;
		mcd->rootDrawer->prepare(&pp);
	}
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
			while (mpv->fDrawRequest) {
				mpv->getSwapper()->bitmapBufferRedraw(mcd);
				mpv->Draw();
			}
			mcd->rootDrawer->getDrawerContext()->ReleaseContext();

			//clock_t end = clock();
			//double total =  1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
			//TRACE(String("drawn DrawInThread in %2.2f milliseconds;\n", total).c_str());
			//TRACE("------\n");

			if (!mpv->fStopDrawThread)
				mpv->drawThread->SuspendThread(); // wait here, and dont consume CPU time either
		
		}
	}
	catch(ErrorObject& err)
	{
		mcd->rootDrawer->getDrawerContext()->ReleaseContext();
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
	mcd->rootDrawer->draw(mcd->rootDrawer->getCoordBoundsZoom());
	// draw editor
	if (edit && !mcd->rootDrawer->fUsesTopDrawer())
		edit->draw(&fDrawStop);
	csResizing.Lock();
	if (fResizing) {
		fResizing = false;
		fDrawRequest = true;
	} else
		mcd->rootDrawer->getDrawerContext()->swapBuffers();
	csResizing.Unlock();
}

void SimpleMapPaneView::setBitmapRedraw(bool yesno) {
	swapper->setBitmapRedraw(yesno);
}

void SimpleMapPaneView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	csResizing.Lock();
	fResizing = true;
	csResizing.Unlock();
}

void SimpleMapPaneView::OnMeasureDist()
{
	noTool(ID_MEASUREDIST);
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

void SimpleMapPaneView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (tools.OnKeyUp(nChar, nRepCnt, nFlags))
		return;
	ZoomableView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void SimpleMapPaneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (tools.size() > 0 && VK_ESCAPE == nChar) { // route VK_ESCAPE first to tools, then to edit, then to restore-fullscreen
		tools.OnEscape();
	}
	if (edit && edit->OnKeyDown(nChar, nRepCnt, nFlags))
		return;
	if ( VK_ESCAPE == nChar) {
		((MapWindow *)getFrameWindow())->OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	if (tools.OnKeyDown(nChar, nRepCnt, nFlags))
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

void SimpleMapPaneView::ResetStatusBar()
{
	MapStatusBar* msb = 0;
	if (fwPar) {
		msb = dynamic_cast<MapStatusBar*>(fwPar->status);
		if (msb)
			msb->Reset();
	}
}

void SimpleMapPaneView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (mode != cNone)
		return;
	PanTool * panTool = (tools.find(ID_PANAREA) != tools.end()) ? (PanTool*)tools[ID_PANAREA] : 0;
	if (panTool && panTool->panning())
		return;
	//throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	if (edit && edit->OnMouseMove(nFlags, point)) 
		return;
	MapCompositionDoc* mcd = GetDocument();

	bool fOutside = false; //!rctBounds.PtInRect(point);

	//Pnt2RowCol(point, rRow, rCol);
	bool fValid = true;
	Coord cRowCol (mcd->rootDrawer->screenToOpenGL(RowCol(point.y,point.x)));
	if ( cRowCol.fUndef())
		return ;
	Coord c (mcd->rootDrawer->glToWorld(cRowCol));
	MapStatusBar* msb = 0;
	CoordSystem csy = mcd->rootDrawer->getCoordinateSystem();
	if (fwPar) 
		msb = dynamic_cast<MapStatusBar*>(fwPar->status);

	if (msb && !fOutside) {
		//if (mcd->fShowRowCol) {
			//RowCol rc((long)floor(1+rRow), (long)floor(1+rCol));
			//msb->ShowRowCol(rc);
		//}
		GeoRef grf = mcd->rootDrawer->getGeoReference();
		if (grf.fValid()) {
			RowCol rc ((long)floor(1-cRowCol.y), (long)floor(1+cRowCol.x));
			msb->ShowRowCol(rc);
		}
		if (c.fUndef())
			fValid = false; // fNone = true;
		bool fNone = grf.fValid() && grf->fGeoRefNone();
		if (fNone) {
			msb->ShowCoord(TR("No Coordinates"));
			msb->ShowLatLon(LatLon());
		} else if (!fValid) {
			msb->ShowCoord(TR("No Coordinates calculated"));
			msb->ShowLatLon(LatLon());
		} else {
			String v;
			if ( csy->pcsLatLon())
				v =  useMetricCoords ? String("%f, %f", c.x, c.y) : csy->sValue(c); 
			else
				v = csy->sValue(c);
			msb->ShowCoord(v);
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

	if (0 == tools.size()&& (MK_LBUTTON & nFlags)) {
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
				int iDiff = rect.top() - point.y;
				vertPixMove(iDiff);
				yInc = 1;
			}
			if (point.y > rect.bottom()) {
				int iDiff = rect.bottom() - point.y;
				vertPixMove(iDiff);
				yInc = -1;
			}
			MoveMouse(xInc, yInc);
			UpdateWindow();
		}
		else {
			bool fHide = true;
			if (fValid && !fOutside && mode == cNone){
				String s = mcd->rootDrawer->getInfo(c);
				if (s == "")
					s = "?";
				info->text(point,s);
			}
			else
				info->ShowWindow(SW_HIDE);
		}
	}

	if (fValid && !fOutside && mode == cNone) {
		CoordWithCoordSystem cwcs(c, mcd->rootDrawer->getCoordinateSystem());
		int state = cmMOUSEMOVE;
		AreaSelector * selectionTool = (tools.find(ID_ZOOMIN) != tools.end()) ? (AreaSelector*)tools[ID_ZOOMIN] : ((tools.find(ID_ZOOMOUT) != tools.end()) ? (AreaSelector*)tools[ID_ZOOMOUT] : 0);
		if (selectionTool && selectionTool->selecting()) {
			if ( iActiveTool == ID_ZOOMIN)
				state |= cmZOOMIN;
			else if ( iActiveTool == ID_ZOOMOUT)
				state |= cmZOOMOUT;
		}
		IlwWinApp()->SendUpdateCoordMessages(state, &cwcs);
	}

	CView::OnMouseMove(nFlags, point);
}

void SimpleMapPaneView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (mode != cNone)
		return;
	PanTool * panTool = (tools.find(ID_PANAREA) != tools.end()) ? (PanTool*)tools[ID_PANAREA] : 0;
	if (panTool && panTool->panning())
		return;
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
		if (s == "")
			s = "?";
		info->text(point,s);
		info->ShowWindow(SW_SHOW);
	}

	//mcd->mpvGetView()->Invalidate();
	int state = cmMOUSECLICK;
	AreaSelector * selectionTool = (tools.find(ID_ZOOMIN) != tools.end()) ? (AreaSelector*)tools[ID_ZOOMIN] : ((tools.find(ID_ZOOMOUT) != tools.end()) ? (AreaSelector*)tools[ID_ZOOMOUT] : 0);
	if (selectionTool && selectionTool->selecting()) {
		if ( iActiveTool == ID_ZOOMIN)
				state |= cmZOOMIN;
		else if ( iActiveTool == ID_ZOOMOUT)
				state |= cmZOOMOUT;
	}
	IlwWinApp()->SendUpdateCoordMessages(state, &cwcsButtonDown);
}

void SimpleMapPaneView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (mode != cNone)
		return;
	PanTool * panTool = (tools.find(ID_PANAREA) != tools.end()) ? (PanTool*)tools[ID_PANAREA] : 0;
	if (panTool && panTool->panning())
		return;
	MapCompositionDoc* mcd = GetDocument();
	info->ShowWindow(SW_HIDE);
	ReleaseCapture();
	if (edit && edit->OnLButtonUp(nFlags, point)) {
		return;
	}
	mcd->mpvGetView()->Invalidate();
}

void SimpleMapPaneView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (mode != cNone)
		return;
	if (edit)
		edit->OnRButtonDown(nFlags, point);
}

void SimpleMapPaneView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if (mode != cNone)
		return;
	if (edit)
		edit->OnRButtonDblClk(nFlags, point);
}

BOOL SimpleMapPaneView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (edit && edit->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	if ( tools.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
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

Editor* SimpleMapPaneView::editGet() 
{
	return edit;
}

void SimpleMapPaneView::createEditor(ComplexDrawer *drw) {
	BaseMap bmp;
	bmp.SetPointer(((SpatialDataDrawer *)drw->getParentDrawer())->getBaseMap());
	IObjectType type = IOTYPE(bmp->fnObj);
	if ( edit){
		pib->pixview->GetDocument()->setEditFeature(0);
		delete edit;
		edit = 0;
	}
	if ( type == IlwisObject::iotPOINTMAP) {
		//edit = ILWISAPP->getMEditor("PointSetEditor","ilwis38",GetDocument(), bmp);
		//edit->init(drw, pib->pixview->GetDocument());

	}
	if ( type == IlwisObject::iotSEGMENTMAP) {
		//edit = ILWISAPP->getMEditor("LineSetEditor","ilwis38",GetDocument(), bmp);
		//edit->init(drw, pib->pixview->GetDocument());

	}
}

void SimpleMapPaneView::createPixInfoBar() {
	if ( !pib && GetDocument()->pixInfoDoc) {
		CFrameWnd* fw = GetTopLevelFrame();
		MapWindow *parent = dynamic_cast<MapWindow*>(fw);
		pib = new PixelInfoBar();
		pib->Create(parent);
		pib->SetWindowText(TR("Pixel Info").c_str());
		pib->EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);
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
