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
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Base\Framewin.h"
#include "Client\ilwis.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Client\Editors\Editor.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\DistanceMeasurer.h"
#include "Client\Mapwindow\MapStatusBar.h"
#include "Engine\SpatialReference\csdirect.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mapwind.hs"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h" // for a dynamic_cast
#include "Engine\Map\txtann.h" // for the next include to work
#include "Client\Mapwindow\Drawers\AnnotationTextDrawer.h" // for a dynamic_cast
#include "Client\Mapwindow\Drawers\Grid3DDrawer.h" // for a dynamic_cast
#include "Client\Mapwindow\Drawers\WMSMapDrawer.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
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
	ON_COMMAND(ID_MEASUREDIST, OnMeasureDist)
	ON_UPDATE_COMMAND_UI(ID_MEASUREDIST, OnUpdateMeasureDist)
END_MESSAGE_MAP()

void SimpleMapPaneView::MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}

const int RepresentationClass::iSIZE_FACTOR=3; // MapWindow to Layout ratio; never make this 0; currently only int ratio supported

SimpleMapPaneView::SimpleMapPaneView()
: info(0), edit(0), dcView(0), cwcsButtonDown(Coord()), fwPar(0), bmView(0), hBmOld(0)
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
	while (fRedrawing) {
		fDrawStop = true;
		Sleep(10);
	}
	delete info;
	delete edit;
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
	}
	ZoomableView::OnInitialUpdate();
	_rScale = rUNDEF; // derived class has to set rScale in some way on a valid value
  info = new InfoLine(this);
}

MinMax SimpleMapPaneView::mmBounds()
{
  return GetDocument()->mmBounds();
}

void SimpleMapPaneView::Coord2RowCol(Coord crd, double& rRow, double &rCol)
{
  try {
    GetDocument()->georef->Coord2RowCol(crd,rRow,rCol);
  }
  catch (...) {
	}
}

Coord SimpleMapPaneView::cConv(RowCol rc)
{
  try {
    return GetDocument()->georef->cConv(rc);
  }
  catch (...) {
    return Coord();
  }  
}

RowCol SimpleMapPaneView::rcConv(Coord crd)
{
  try {
    return GetDocument()->georef->rcConv(crd);
  }
  catch (...) {
		return RowCol();
	}
}

Coord SimpleMapPaneView::crdPnt(zPoint pnt)
{
  Coord crd;
  double rRow, rCol;
  Pnt2RowCol(pnt, rRow, rCol);
  try {
    GetDocument()->georef->RowCol2Coord(rRow,rCol,crd);
  }
  catch (...) {
    crd = Coord();
  }  
  return crd;
}

zPoint SimpleMapPaneView::pntPos(Coord crd)
{
  try {
		double rRow, rCol;
    GetDocument()->georef->Coord2RowCol(crd, rRow, rCol);
		return pntPos(rRow, rCol);
  }
  catch (...) {
    return zPoint(shUNDEF, shUNDEF);
  }  
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

void SimpleMapPaneView::InitOpenGL(HDC hDC) {

}

void SimpleMapPaneView::OnDraw(CDC* cdc)
{
	fStarting  = false;
	CDC *dc = cdc == 0 ? GetDC() : cdc;
	MapCompositionDoc* mcd = GetDocument();
	PreparationParameters pp(NewDrawer::ptINITOPENGL, dc);
	mcd->rootDrawer->prepare(&pp);
	mcd->rootDrawer->getDrawerContext()->TakeContext(true);
	mcd->rootDrawer->draw();
	SwapBuffers(dc->m_hDC);
	mcd->rootDrawer->getDrawerContext()->ReleaseContext();
}

void SimpleMapPaneView::OnMeasureDist()
{
	OnNoTool();
	as = new DistanceMeasurer(this);
	iActiveTool = ID_MEASUREDIST;
}

void SimpleMapPaneView::OnUpdateMeasureDist(CCmdUI* pCmdUI)
{
	bool fGeoRefNone = true; // pessimistic
	GeoRef gr (GetDocument()->georef);
	if (gr.fValid()) // then we can recalculate the boolean
		fGeoRefNone = gr->fGeoRefNone();
	pCmdUI->Enable(!fGeoRefNone);
	if (0 == as)
		iActiveTool = 0;
	pCmdUI->SetRadio(ID_MEASUREDIST == iActiveTool);
}

void SimpleMapPaneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (as && VK_ESCAPE == nChar) {
		as->OnEscape();
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
	if (edit && edit->OnMouseMove(nFlags, point)) 
		return;
	MapCompositionDoc* mcd = GetDocument();

	GeoRef georef = mcd->georef;
  if (!georef.fValid()) 
		return;

	CRect rctBounds = rctPos(mcd->mmBounds());
	bool fOutside = !rctBounds.PtInRect(point);

  bool fNone = georef->fGeoRefNone();
  double rRow, rCol;
  Coord c;
  Pnt2RowCol(point, rRow, rCol);
  bool fValid = true;
  try {
    georef->RowCol2Coord(rRow,rCol,c);
  }
  catch (...) {
    fValid = false;
  }  
	MapStatusBar* msb = 0;
	if (fwPar) 
		msb = dynamic_cast<MapStatusBar*>(fwPar->status);

	if (msb && !fOutside) {
		if (mcd->fShowRowCol) {
			RowCol rc((long)floor(1+rRow), (long)floor(1+rCol));
			msb->ShowRowCol(rc);
		}
		if (c.fUndef())
			fValid = false; // fNone = true;  
		if (fNone) {
			msb->ShowCoord(SMWRem__NoCoords);
			msb->ShowLatLon(LatLon());
		}
		else if (!fValid) {
			msb->ShowCoord(SMWRem__NoCoordsCalculated);
			msb->ShowLatLon(LatLon());
		}
		else {    
			msb->ShowCoord(georef->cs()->sValue(c));
			if (georef->cs()->pcsDirect()) {
				CoordSystemDirect *pcsd = georef->cs()->pcsDirect();
				CoordSystemPtr * pcs = pcsd->csOther.ptr();
				Coord crd = pcsd->cInverseConv(pcs,c);
				msb->ShowRelCoord(pcsd->csOther->sValue(crd));
			}
			if (georef->cs()->fCoord2LatLon()) {
				LatLon ll = georef->cs()->llConv(c);
				msb->ShowLatLon(ll);
			}
			else
				msb->ShowLatLon(LatLon());
		}  
	}
	if (0 == as && (MK_LBUTTON & nFlags)) {
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
			if (fValid && !fOutside)
				for (list<Drawer*>::reverse_iterator iter = mcd->dl.rbegin(); iter != mcd->dl.rend(); ++iter)
				{
					Drawer* dr = *iter;
					if (dr->fSelectable) 
					{
            Coord crd = c;
            BaseMapDrawer* bmd = dynamic_cast<BaseMapDrawer*>(dr);
            if (bmd && bmd->basemap()->cs() != mcd->georef->cs())
            {
              crd = bmd->basemap()->cs()->cConv(mcd->georef->cs(), c);
            }
						String s = dr->sInfo(crd);
						if (s != "") {
              DomainValue* dv = dr->dm()->pdv();
              if (0 != dv && dv->fUnit())
                s = String("%S %S", s, dv->sUnit());
							info->text(point,s);
							fHide = false;
							break;
						}
					}
				}
      if (fHide)
				info->text(point,"");
    }
  }
	if (!fNone && fValid && !fOutside) {
		CoordWithCoordSystem cwcs(c, georef->cs());
		IlwWinApp()->SendUpdateCoordMessages(cmMOUSEMOVE, &cwcs);
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
	SetCapture();
	ILWIS::MouseClickInfoDrawer *mid = (ILWIS::MouseClickInfoDrawer *)(mcd->rootDrawer->getDrawer("MouseClickInfoDrawer"));
	if ( mid) {
		mid->setActivePoint(c);
		ILWIS::PreparationParameters pp(ILWIS::NewDrawer::ptALL,0);
		mid->prepare(&pp);
		mcd->mpvGetView()->Invalidate();
	}
	IlwWinApp()->SendUpdateCoordMessages(cmMOUSECLICK, &cwcsButtonDown);
}

void SimpleMapPaneView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	MapCompositionDoc* mcd = GetDocument();
	ILWIS::MouseClickInfoDrawer *mid = (ILWIS::MouseClickInfoDrawer *)mcd->rootDrawer->getDrawer("MouseClickInfoDrawer");
	if (mid) {
		mid->setActivePoint(Coord());
		mcd->mpvGetView()->Invalidate();
	}
	ReleaseCapture();
	if (edit && edit->OnLButtonUp(nFlags, point)) {
		return;
	}
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
		if (as && as->OnSetCursor())
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
		while (mpv->fDirty) 
			mpv->RedrawInThread();
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

void SimpleMapPaneView::RedrawInThread()
{
	if ( true) {
		MapCompositionDoc* mcd = GetDocument();
	//	(*(mcd->drawers.begin()))->prepare();
	//	(*(mcd->drawers.begin()))->draw();
		return;
	}

	fRedrawing = true;
	CWaitCursor curWait;
	fDirty = false;
	fDrawStop = false;
	MapCompositionDoc* mcd = GetDocument();
	MapStatusBar* msb = 0;
	if (fwPar) 
		msb = dynamic_cast<MapStatusBar*>(fwPar->status);
	CRect rect;
	GetClientRect(rect);
	MinMax mm = mmRect(rect);
	double rSc = rScale();
	BitmapPositioner psnBm(rSc,mm,mcd->georef);

	CClientDC cdc(this);
	CDC* dcBm = new CDC; // will be cleaned up at next cycle or at destructor
	CBitmap * bm = new CBitmap; // will be cleaned up at next cycle or at destructor
	bm->CreateCompatibleBitmap(&cdc,rect.Width(),rect.Height()); // will be released at next cycle or at destructor
	dcBm->CreateCompatibleDC(&cdc); // will be released at next cycle or at destructor
	CBitmap * bmToRestore = dcBm->SelectObject(bm); // NOTE: bmToRestore belongs to dcBm

	Color colBackground = mcd->colBackground;
	//  if (0 != alphablendfunc)
	//    colBackground.alpha() = 255;

	CPen penRect(PS_SOLID,1,colBackground);
	CBrush brRect(colBackground);
	CPen* penOld = dcBm->SelectObject(&penRect);
	CBrush* brOld = dcBm->SelectObject(&brRect);
	CRect rctBounds = rctPos(mcd->mmBounds());
	dcBm->Rectangle(rect);
	dcBm->SelectObject(penOld);
	dcBm->SelectObject(brOld);

	int iState = mcd->iState();
	for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		if (fDrawStop)
			break;
		if (iState != mcd->iState()) 
			break;
		Drawer* dr = *iter;
		if (msb)
			msb->SetActiveDrawer(dr);
		if (iState != mcd->iState()) 
			break;
		if (dr->fScaleLimits) {
			double rScale = rounding(rScaleShow());
			if (rScale < dr->riScaleLimits.iLo())
				continue;
			if (rScale >= dr->riScaleLimits.iHi())
				continue;
		}
		if (0 != edit && dr == edit->dr())
			continue;
		if (dr->fTransparency && 0 != alphablendfunc)
		{
			CBitmap bmLayer;
			bmLayer.CreateCompatibleBitmap(&cdc,rect.Width(),rect.Height());
			CDC dcLayer;
			dcLayer.CreateCompatibleDC(&cdc);
			dcLayer.SelectObject(&bmLayer);

			CPen penRect(PS_SOLID,1,Color(0));
			CBrush brRect(Color(0));
			CPen* penOld = dcLayer.SelectObject(&penRect);
			CBrush* brOld = dcLayer.SelectObject(&brRect);
			dcLayer.Rectangle(rect);
			dcLayer.SelectObject(penOld);
			dcLayer.SelectObject(brOld);

			dr->draw(&dcLayer, rect, &psnBm, &fDrawStop);
			if (fDrawStop)
				break;
			BLENDFUNCTION blendfunc;
			blendfunc.BlendOp = AC_SRC_OVER;
			blendfunc.BlendFlags = 0;
			blendfunc.SourceConstantAlpha = (byte)(255 * (100 - dr->iTransparence)/100.0);
			blendfunc.AlphaFormat = 1;

			alphablendfunc(*dcBm, 0,0, rect.Width(), rect.Height(), 
				dcLayer,0,0,rect.Width(),rect.Height(),blendfunc);
		}
		else
		{
			if ((dynamic_cast<MapDrawer*>(dr) != 0) || (dynamic_cast<AnnotationTextDrawer*>(dr) != 0) || (dynamic_cast<Grid3DDrawer*>(dr) != 0)) {
				WMSMapDrawer *wmsdr = dynamic_cast<WMSMapDrawer*>(dr);
				bool overview = wmsdr == 0 || dynamic_cast<OverviewMapPaneView *>(this) == 0;
				if ( overview)
					dr->draw(dcBm, rect, &psnBm, &fDrawStop);
			}
			else
			{
				// special handling for non raster-layers - reduce thickness

				int iOldMapMode = dcBm->SetMapMode(MM_ANISOTROPIC);
				SetGraphicsMode(dcBm->GetSafeHdc(), GM_ADVANCED); // otherwise fonts can't be scaled and rotated at the same time!
				CSize szOldWindowExt = dcBm->ScaleWindowExt(RepresentationClass::iSIZE_FACTOR, 1, RepresentationClass::iSIZE_FACTOR, 1);

				// only influence thicknesses - not positions, so alter the positioner
				if (rSc < 0) // scale correction for historical reasons - negative scale number had special meaning
					rSc = -1 / rSc;
				BitmapPositioner psnBmScaled(rSc * RepresentationClass::iSIZE_FACTOR, mm, mcd->georef);

				// now draw
				dr->draw(dcBm, rect, &psnBmScaled, &fDrawStop);
				// then restore window properties
				dcBm->SetWindowExt(szOldWindowExt);
				dcBm->SetMapMode(iOldMapMode);
			}
		}
	}
	// Now we need to do {dcBm->SelectObject(bmToRestore); bm->DeleteObject(); delete bm;}
	// This would release and free bm, but it is too early.
	// I don't remember exactly why, but either the map disappears from the display, or
	// at a slideshow there is a black bitmap shown between slides

	// The solution is to do it at the next redraw cycle or at the destructor
	// For this we need to maintain pointers to the old CDC, bitmap and popped bitmap
	// (resp. dcView, bmView and bmOld) while the new CDC, bitmab and popped bitmap
	// (resp. dcBm, bm and bmToRestore) are being created

	// Hence the code below and in the destructor

	if (msb && !fDrawStop)
		msb->SetActiveDrawer(0);
	if (iState != mcd->iState()) 
	{
		// First attempt to clean up GDI and memory: according to the "official" way
		CBitmap * bmOut = dcBm->SelectObject(bmToRestore); // To return the bm of the previous redraw
		// we keep bmOut for fail-safety.. it is a TempGDIObject equal to our bm
		if (bmOut) // If the bm appeared, clean it up !! don't clean up objs still selected!!
		{
			bm->DeleteObject();
			delete bm;
			bm = 0;
		}
		dcBm->DeleteDC();
		delete dcBm;
		// one more attempt to clean up GDI and memory
		if (bm)
		{
			bm->DeleteObject(); // encapsulating dc is gone, so this should be ok now
			delete bm;
		}
		fDirty = true;
		return;
	}
	csDcView.Lock();
	if (dcView)
	{
		// First attempt to clean up GDI and memory: according to the "official" way
		CBitmap * bmOut = dcView->SelectObject(CBitmap::FromHandle(hBmOld)); // To return the bm of the previous redraw
		// we keep bmOut for fail-safety.. it is a TempGDIObject equal to our bmView
		if (bmOut) // If the bm appeared, clean it up !! don't clean up objs still selected!!
		{
			bmView->DeleteObject();
			delete bmView;
			bmView = 0;
		}
		dcView->DeleteDC();
		delete dcView;
	}
	// one more attempt to clean up GDI and memory
	if (bmView)
	{
		bmView->DeleteObject(); // encapsulating dc is gone, so this should be ok now
		delete bmView;
	}
	dcView = dcBm;			 // bmToRestore belongs to dcBm, just like bmOld belongs to dcView
	bmView = bm;				 // bmView and dcView must be cleaned up at next round or destructor
	hBmOld = (HBITMAP)bmToRestore->GetSafeHandle(); // hBmOld helps freeing our bitmap from CDC with SelectObject
	csDcView.Unlock();
	fRedrawing = false;
}

// returns 1:.... value
double SimpleMapPaneView::rScaleShow()
{
	MapCompositionDoc* mcd = GetDocument();
	if (0 == mcd || !mcd->georef.fValid())
		return rUNDEF;
	double rPixSize = mcd->georef->rPixSize();
	if (rUNDEF == rPixSize)
		return rUNDEF;
	CoordSystemLatLon* csll = mcd->georef->cs()->pcsLatLon();
  if (0 != csll)
    rPixSize *= M_PI / 180 * csll->ell.a;
	if (rScale() > 0)
		rPixSize /= rScale();
	else if (rScale() < 0)
		rPixSize *= -rScale();

	CClientDC dc(this);
	int iPixPerInchY = dc.GetDeviceCaps(LOGPIXELSY);
  double rPixPermm = iPixPerInchY / 25.4;         // (pixel per mm)

	rPixSize *= rPixPermm;
	rPixSize *= 1000; // 1000 mm per m
	return rPixSize;
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

const Editor* SimpleMapPaneView::editGet() const
{
  return edit;
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
