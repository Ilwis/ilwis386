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
// ZoomableView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\ZoomableView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GrcWMS.h"
#include "Client\Mapwindow\PanTool.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Positioner.h"
#include "Headers\constant.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Engine\SpatialReference\prj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ZoomableView

IMPLEMENT_DYNCREATE(ZoomableView, CView)

BEGIN_MESSAGE_MAP(ZoomableView, CView)
	//{{AFX_MSG_MAP(ZoomableView)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_REDRAW, OnReDraw)
	ON_COMMAND(ID_NORMAL, OnNoTool)
	ON_UPDATE_COMMAND_UI(ID_NORMAL, OnUpdateNoTool)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ZoomableView::ZoomableView()
	: iXpos(0), iYpos(0)
	, iXfalseOffset(0), iYfalseOffset(0)
	, iXpage(0), iYpage(0)
	, iXsize(0), iYsize(0)
	, iXmin(0), iXmax(0)
	, iYmin(0), iYmax(0)
{
	_rScale = rUNDEF;
	fAdjustSize = false;
	fScrollBarsVisible = true;
	iActiveTool = 0;
	xOld = yOld = iUNDEF;
	beginMovePoint = CPoint(iUNDEF, iUNDEF);
	mode = cNone;
}

ZoomableView::~ZoomableView()
{
}


void ZoomableView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	_rScale = 1;
  MinMax mm = mmBounds();
  iXpos = iXmin = mm.MinCol();
  iYpos = iYmin = mm.MinRow();
	iXmax = mm.MaxCol();
	iYmax = mm.MaxRow();
  iXsize = mm.width();
	iYsize = mm.height();

	// first place scroll bars
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 10;
	si.nPage = 1;
	si.nPos = 1;
	beginMovePoint.x = iUNDEF;
	if ( fScrollBarsVisible )
	{
	  SetScrollInfo(SB_VERT, &si);
		SetScrollInfo(SB_HORZ, &si);
	}		
	// now set them on the correct position
	setScrollBars();
}

void ZoomableView::OnDraw(CDC* cdc)
{
	ASSERT("ZoomableView::OnDraw() not implemented!");
}

MinMax ZoomableView::mmBounds()
{
	ASSERT("ZoomableView::mmBounds() not implemented!");
	return MinMax();
}

void ZoomableView::SetDirty()
{
	Invalidate();
}

FrameWindow* ZoomableView::fwParent()
{
	CFrameWnd* fw = GetTopLevelFrame();
	return dynamic_cast<FrameWindow*>(fw);
}

DataWindow* ZoomableView::dwParent()
{
	CFrameWnd* fw = GetTopLevelFrame();
	return dynamic_cast<DataWindow*>(fw);
}


BOOL ZoomableView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CPoint point((DWORD)lParam);
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	switch (message) {
		case WM_MOUSEMOVE:
			if (mcd->rootDrawer->is3D()) {
				if ((wParam & MK_CONTROL) || (iActiveTool == ID_PANAREA))
					movePoint(point, message);
			} else {
				if (tools.size() > 0) 
					tools.OnMouseMove(wParam, point);
			}
			break;
		case WM_LBUTTONDBLCLK:
			if (tools.size() > 0) tools.OnLButtonDblClk(wParam, point);
			break;
		case WM_LBUTTONDOWN:
			if (mcd->rootDrawer->is3D()) {
				if ((wParam & MK_CONTROL) || (iActiveTool == ID_PANAREA))
					moveEyePoint(point,message);
			} else {
				if (tools.size() > 0)
					tools.OnLButtonDown(wParam, point);
			}
			break;
		case WM_LBUTTONUP:
			if (mcd->rootDrawer->is3D()) {
				if ((mode != cNone) || (iActiveTool == ID_PANAREA))
					moveEyePoint(point,message);
			} else {
				if (tools.size() > 0)
					tools.OnLButtonUp(wParam, point);
			}
			break;
		case WM_RBUTTONDBLCLK:
			if (tools.size() > 0) tools.OnRButtonDblClk(wParam, point);
			break;
		case WM_RBUTTONDOWN:
			if (mcd->rootDrawer->is3D()) {
				if ((wParam & MK_CONTROL) || (iActiveTool == ID_PANAREA))
					moveViewPoint(point, message);
			} else {
				if (tools.size() > 0) 
					tools.OnRButtonDown(wParam, point);
			}
			break;
		case WM_RBUTTONUP:
			if (mcd->rootDrawer->is3D()) {
				if ((mode != cNone) || (iActiveTool == ID_PANAREA)) {
					moveViewPoint(point, message);
					return TRUE; // prevent context-menu !!
				}
			} else {
				if (tools.size() > 0)
					tools.OnRButtonUp(wParam, point);
			}
			break;
	}
	return CView::OnWndMsg(message, wParam, lParam, pResult);
}

void ZoomableView::movePoint(const CPoint& pnt, UINT message) {
	if ( mode == cPan)
		moveViewPoint(pnt, message);
	if ( mode == cZoom)
		moveEyePoint(pnt, message);

}
void ZoomableView::moveViewPoint(const CPoint& pnt, UINT message) {
	MapCompositionDoc *doc = (MapCompositionDoc *)GetDocument();
	if ( message == WM_RBUTTONDOWN) {
		beginMovePoint = pnt;
		mode = cPan;
		SetCapture();
	}
	else if ( message == WM_RBUTTONUP){
		beginMovePoint = CPoint(iUNDEF,iUNDEF);
		mode = cNone;
		ReleaseCapture();
	} else if (message == WM_MOUSEMOVE && beginMovePoint.x != iUNDEF) {
		CRect rct;
		GetClientRect(rct);
		double deltax = beginMovePoint.x - pnt.x;
		double deltay = beginMovePoint.y - pnt.y;
		if ( deltax == 0 && deltay == 0)
			return;
		deltax = deltax / rct.Width();
		deltay = deltay / rct.Height();
		CoordBounds cb = doc->rootDrawer->getCoordBoundsZoom();
		double shiftx = cb.width() * deltax;
		double shifty = cb.height() * deltay;
		/*
		Coord eyePoint = doc->rootDrawer->getEyePoint();
		eyePoint.x += shiftx;
		eyePoint.z += shifty;
		doc->rootDrawer->setEyePoint(eyePoint);
		Coord viewPoint = doc->rootDrawer->getViewPoint();
		viewPoint.x += shiftx;
		viewPoint.z += shifty;
		doc->rootDrawer->setViewPoint(viewPoint);
		*/
		double tx,ty,tz;
		doc->rootDrawer->getTranslate(tx,ty,tz);
		tx -= shiftx;
		ty += shifty;
		tz += shifty; // intentionally use shifty twice, the resulting interactvity "feels" more natural
		doc->rootDrawer->setTranslate(tx,ty,tz);

		
		beginMovePoint = pnt;
		Invalidate();
	}
}

void ZoomableView::moveEyePoint(const CPoint& pnt, UINT message) {
	MapCompositionDoc *doc = (MapCompositionDoc *)GetDocument();
	if ( message == WM_LBUTTONDOWN) {
		beginMovePoint = pnt;
		mode = cZoom;
		SetCapture();
	}
	else if ( message == WM_LBUTTONUP){
		beginMovePoint = CPoint(iUNDEF,iUNDEF);
		mode =  cNone;
		ReleaseCapture();
	} else if (message == WM_MOUSEMOVE && beginMovePoint.x != iUNDEF) {
		double deltax = beginMovePoint.x - pnt.x;
		double deltay = beginMovePoint.y - pnt.y;
		if ( deltax == 0 && deltay == 0)
			return;
		double roll = deltax/10.0;
		double yaw = deltay/10.0;
		double rx,ry,rz;
		doc->rootDrawer->getRotationAngles(rx,ry,rz);
		rx += roll;
		ry += yaw;
		doc->rootDrawer->setRotationAngles(rx,ry,rz);

		beginMovePoint = pnt;
		Invalidate();
	}
}


MinMax ZoomableView::mmRect(zRect r)   // Windows coordinates -> internal RowCol
{
  MinMax mm;
  mm.rcMin = rcPos(r.topLeft());
  mm.rcMax = rcPos(r.bottomRight());
  return mm;
}

double ZoomableView::rScale() const
{
  return _rScale;
}

RowCol ZoomableView::rcPos(zPoint p) const // Windows coordinates -> internal RowCol
{
  RowCol rc;
  rc.Row = iYpos + scale(p.y) - iYfalseOffset;
	rc.Col = iXpos + scale(p.x) - iXfalseOffset;
  return rc;
}

zPoint ZoomableView::pntPos(RowCol rc) const	// internal RowCol -> Windows coordinates
{
	if (rc.fUndef())
		return zPoint(shUNDEF, shUNDEF);
	else
	{
		zPoint p;
		long tmp = scale(rc.Col - iXpos + iXfalseOffset, true);
		if (tmp > SHRT_MAX) tmp = SHRT_MAX;
		if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
		p.x = tmp;
		tmp = scale(rc.Row - iYpos + iYfalseOffset, true);
		if (tmp > SHRT_MAX) tmp = SHRT_MAX;
		if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
		p.y = tmp;
		return p;
	}
}

long ZoomableView::scale(long x, bool fInv) const // Windows coordinates -> internal RowCol
{
  if (rScale() == 0)
    return x;
  if (fInv)
    if (rScale() > 0)
      return (long)(x * rScale());
    else
      return (long)(x / -rScale());
  else
    if (rScale() > 0)
      return (long)(x / rScale());
    else
      return (long)(x * -rScale());
}

double ZoomableView::scale(double x, bool fInv) const // Windows coordinates -> internal RowCol
{
  if (rScale() == 0)
    return x;
  if (fInv)
    if (rScale() > 0)
      return x * rScale();
    else
      return x / -rScale();
  else
    if (rScale() > 0)
      return x / rScale();
    else
      return x * -rScale();
}

void ZoomableView::CalcMax()
{
	iXpage = scale(dim.width());
	iYpage = scale(dim.height());
	double rTmp;
	MinMax mm = mmBounds();
	iXmin = mm.MinCol();
	iYmin = mm.MinRow();
	rTmp = mm.width() - iXpage;
	if (rScale() < 0) {
		rTmp /= -rScale();
		rTmp *= -rScale();
	}
	iXmax = iXmin + (long)rTmp;
	rTmp = mm.height() - iYpage;
	if (rScale() < 0) {
		rTmp /= -rScale();
		rTmp *= -rScale();
	}
	iYmax = iYmin + (long)rTmp;
}

void ZoomableView::CalcFalseOffsets()
{
  iXfalseOffset = (scale(dim.width()) - iXsize) / 2;
	if (iXfalseOffset < 0)
		iXfalseOffset = 0;
  iYfalseOffset = (scale(dim.height()) - iYsize) / 2;
	if (iYfalseOffset < 0)
		iYfalseOffset = 0;
}

void ZoomableView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		if ( cy == 0 || cx == 0)
			return;
		RowCol rc(cy, cx);
		dim = zDimension(cx, cy);
		mcd->rootDrawer->setViewPort(rc);
		SetDirty();
		
	}
}

void ZoomableView::AreaSelected(CRect rect)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		CRect rectWindow;
		GetClientRect(&rectWindow);
		Coord c1,c2;
		if ( rect.Width() == 0 || rect.Height() == 0) { // case of clicking on the map in zoom mode
			double posx = cbZoom.cMin.x + cbZoom.width() * rect.left / (double)rectWindow.Width(); // determine click point
			double posy = cbZoom.cMax.y - cbZoom.height() * rect.top / (double)rectWindow.Height();
			CoordBounds cb = cbZoom; // == cbView ? cbMap : cbZoom;
			double w = cb.width() / (2.0 * 1.41); // determine new window size
			double h = cb.height() / (2.0 * 1.41);
			c1.x = posx - w; // determine new bounds
			c1.y = posy - h;
			c2.x = posx + w;
			c2.y = posy + h;
		} else {
			c1.x = cbZoom.cMin.x + cbZoom.width() * rect.left / (double)rectWindow.Width(); // determine zoom rectangle in GL coordinates
			c1.y = cbZoom.cMax.y - cbZoom.height() * rect.top / (double)rectWindow.Height();
			c2.x = cbZoom.cMin.x + cbZoom.width() * rect.right / (double)rectWindow.Width();
			c2.y = cbZoom.cMax.y - cbZoom.height() * rect.bottom / (double)rectWindow.Height();
		}
		c1.z = c2.z = 0;

		cbZoom = CoordBounds (c1,c2);
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		RecenterZoomHorz(cbZoom, cbMap);
		RecenterZoomVert(cbZoom, cbMap);
		mcd->rootDrawer->setCoordBoundsZoom(cbZoom);

		setScrollBars();
		OnDraw(0);
	}
}

void ZoomableView::RecenterZoomHorz(CoordBounds & cbZoom, const CoordBounds & cbMap)
{
	double zwidth = cbZoom.width();
	if (zwidth > cbMap.width()) {
		double delta = (zwidth - cbMap.width()) / 2.0;
		cbZoom.cMin.x = cbMap.cMin.x - delta;
		cbZoom.cMax.x = cbMap.cMax.x + delta;
	} else {
		if ( cbZoom.cMax.x > cbMap.cMax.x) {
			cbZoom.cMax.x = cbMap.cMax.x;
			cbZoom.cMin.x = cbZoom.cMax.x - zwidth;
		}
		if ( cbZoom.cMin.x < cbMap.cMin.x) {
			cbZoom.cMin.x = cbMap.cMin.x;
			cbZoom.cMax.x = cbZoom.cMin.x + zwidth;
		}
	}
}

void ZoomableView::RecenterZoomVert(CoordBounds & cbZoom, const CoordBounds & cbMap)
{
	double zheight = cbZoom.height();
	if (zheight > cbMap.height()) {
		double delta = (zheight - cbMap.height()) / 2.0;
		cbZoom.cMin.y = cbMap.cMin.y - delta;
		cbZoom.cMax.y = cbMap.cMax.y + delta;
	} else {
		if ( cbZoom.cMax.y > cbMap.cMax.y) {
			cbZoom.cMax.y = cbMap.cMax.y;
			cbZoom.cMin.y = cbZoom.cMax.y - zheight;
		}
		if ( cbZoom.cMin.y < cbMap.cMin.y) {
			cbZoom.cMin.y = cbMap.cMin.y;
			cbZoom.cMax.y = cbZoom.cMin.y + zheight;
		}
	}
}

void ZoomableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (tools.size() > 0 && VK_ESCAPE == nChar) {
		tools.OnEscape();
		return;
	}
  bool fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
  switch (nChar) {
		// scrolling
    case VK_PRIOR:
      if (fCtrl)
        vertTop();
      else
        vertPageMove(-1);
      return;
    case VK_NEXT:      
      if (fCtrl)
        vertBottom();
      else
        vertPageMove(1);
      return;
    case VK_HOME:  
      if (fCtrl)
        horzLeft();
      else
        horzPageMove(-1);
      return;
    case VK_END:  
      if (fCtrl)
        horzRight();
      else
        horzPageMove(1);
      return;
  }
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void ZoomableView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (nSBCode)
	{
	case SB_TOP:
		horzLeft();
		break;
	case SB_BOTTOM:
		horzRight();
		break;
	case SB_LINEUP:
		horzLineMove(-1);
		break;
	case SB_LINEDOWN:
		horzLineMove(1);
		break;
	case SB_PAGEUP:
		horzPageMove(-1);
		break;
	case SB_PAGEDOWN:
		horzPageMove(1);
		break;
	case SB_THUMBTRACK:
		horzThumbPos(nPos);
		break;
	}
}

void ZoomableView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch (nSBCode)
	{
	case SB_TOP:
		vertTop();
		break;
	case SB_BOTTOM:
		vertBottom();
		break;
	case SB_LINEUP:
		vertLineMove(1);
		break;
	case SB_LINEDOWN:
		vertLineMove(-1);
		break;
	case SB_PAGEUP:
		vertPageMove(1);
		break;
	case SB_PAGEDOWN:
		vertPageMove(-1);
		break;
	case SB_THUMBTRACK:
		vertThumbPos(nPos);
		break;
	}
}

BOOL ZoomableView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) 
{
	bool fControl = nFlags & MK_CONTROL ? true : false;
	bool fShift = nFlags & MK_SHIFT ? true : false;
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	fControl = fControl || (mcd->rootDrawer->is3D() && iActiveTool == ID_PANAREA);
	
	if (!fControl)
		if (!fShift)
			vertPageMove(zDelta > 0 ? -1 : 1);
		else
			horzPageMove(zDelta > 0 ? -1 : 1);
	else
		if (!fShift) {
			ScreenToClient(&point);
			if ( zDelta > 0)
				ZoomInPnt(point);
			else
				ZoomOutPnt(point);
		}
	
	return TRUE;
}

int ZoomableView::vertLineMove(int iMove)
{
	SCROLLINFO si;
	GetScrollInfo(SB_VERT,&si);
	long delta = int(si.nPage) * iMove / 100;
	return vertPixMove(delta);
}

int ZoomableView::vertPageMove(int iMove)
{
	SCROLLINFO si;
	GetScrollInfo(SB_VERT,&si);
    return vertPixMove(si.nPage * iMove);
}

int ZoomableView::vertTop()
{
  return vertPixMove(iYmin - iYpos);
}

int ZoomableView::vertBottom()
{
  return vertPixMove(iYsize - iYmin + iYpos);
}

int ZoomableView::vertThumbPos(int nPos)
{
  SCROLLINFO si;
  GetScrollInfo(SB_VERT,&si);
  return vertPixMove(si.nPos - nPos);
}

int ZoomableView::horzLineMove(int iMove)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ,&si);
	long delta = int(si.nPage) * iMove / 100;
    return horzPixMove(delta);
}

int ZoomableView::horzPageMove(int iMove)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ,&si);
    return horzPixMove(si.nPage * iMove);
}

int ZoomableView::horzLeft()
{
  return horzPixMove(iXmin - iXpos);
}

int ZoomableView::horzRight()
{
  return horzPixMove(iXsize - iXmin + iXpos);
}

int ZoomableView::horzThumbPos(int nPos)
{
  SCROLLINFO si;
  GetScrollInfo(SB_HORZ,&si);
  return horzPixMove(nPos - int(si.nPos));
}

int ZoomableView::vertPixMove(long iDiff, bool fPreScroll)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		double deltay = cbMap.height() * iDiff / SCROLL_SIZE;
		cbZoom.cMin.y += deltay;
		cbZoom.cMax.y += deltay;
		RecenterZoomVert(cbZoom, cbMap);
		mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		setScrollBars();
		OnDraw(0);
	}

  return 0;
}

int ZoomableView::horzPixMove(long iDiff, bool fPreScroll)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		double deltax = cbMap.width() * iDiff / SCROLL_SIZE;
		cbZoom.cMin.x += deltax;
		cbZoom.cMax.x += deltax;
		RecenterZoomHorz(cbZoom, cbMap);
		mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		setScrollBars();
		OnDraw(0);
	}
	return 0;
}

void ZoomableView::setScrollBars() 
{

	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
	
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;

		double xfrac = abs(cbMap.MinX() - cbZoom.MinX())/cbMap.width();
		double xpages = cbMap.width() / cbZoom.width();
		si.nMin = 0;
		si.nMax = xpages > 1.0 ? SCROLL_SIZE : 0;
		si.nPage = SCROLL_SIZE / xpages;
		si.nPos = xfrac * SCROLL_SIZE;
		SetScrollInfo(SB_HORZ, &si);

		double yfrac = abs(cbMap.MaxY() - cbZoom.MaxY())/cbMap.height();
		double ypages = cbMap.height() / cbZoom.height();
		si.nMin = 0;
		si.nMax = ypages > 1.0 ? SCROLL_SIZE : 0;
		si.nPage = SCROLL_SIZE / ypages;
		si.nPos = yfrac * SCROLL_SIZE;
		SetScrollInfo(SB_VERT, &si);

	}
}


zDimension ZoomableView::dimPos(RowCol rc) // internal RowCol -> Windows coordinates
{
  zDimension dim;
  long tmp;
  tmp = scale(rc.Col, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  dim.width() = tmp;
  tmp = scale(rc.Row, true);
  if (tmp > SHRT_MAX) tmp = SHRT_MAX;
  if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
  dim.height() = tmp;
  return dim;
}

zDimension ZoomableView::dimPos(MinMax mm) // internal RowCol -> Windows coordinates
{
  RowCol rc;
  rc.Row = mm.height();
  if (rc.Row <= 0)
    rc.Row = 1;
  rc.Col = mm.width();
  if (rc.Col <= 0)
    rc.Col = 1;
  return dimPos(rc);
}

zRect ZoomableView::rctPos(MinMax mm) // internal RowCol -> Windows coordinates
{
	zRect rect;
	rect.BottomRight() = pntPos(mm.rcMin);
	rect.TopLeft() = pntPos(mm.rcMax);

	if (rect.left() > rect.right())
		swap(rect.left(), rect.right());
	if (rect.top() > rect.bottom())
		swap(rect.top(), rect.bottom());

	return rect;
}
 
void ZoomableView::Pnt2RowCol(zPoint p, double& rRow, double &rCol)
{
  double rCorr = 0.5;
  if (_rScale > 1) 
    rCorr = 1.0 / _rScale;
	rRow = iYpos + scale((double) p.y) + rCorr - iYfalseOffset;
	rCol = iXpos + scale((double) p.x) + rCorr - iXfalseOffset;
}

zPoint ZoomableView::pntPos(double rRow, double rCol) const
{
	if ((rRow == rUNDEF) || (rCol == rUNDEF))
		return zPoint(shUNDEF, shUNDEF);
	else
	{
		double rCorr = 0.5;
		if (_rScale > 1) 
			rCorr = 1.0 / _rScale;
		rRow -= rCorr;
		rCol -= rCorr;
		zPoint p;
		long tmp;
		tmp = scale(rCol - iXpos + iXfalseOffset, true);
		if (tmp > SHRT_MAX) tmp = SHRT_MAX;
		if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
		p.x = tmp;
		tmp = scale(rRow - iYpos + iYfalseOffset, true);
		if (tmp > SHRT_MAX) tmp = SHRT_MAX;
		if (tmp < -SHRT_MAX) tmp = -SHRT_MAX;
		p.y = tmp;
		return p;
	}
}
 
void ZoomableView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
	bool fMapOpen = false;
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	if ( !mcd->fIsEmpty())			
		fMapOpen = true;
	bool zoomedIn = (mcd->rootDrawer->getMapCoordBounds().width() > mcd->rootDrawer->getCoordBoundsZoom().width()) || 
				     (mcd->rootDrawer->getMapCoordBounds().height() > mcd->rootDrawer->getCoordBoundsZoom().height());
	pCmdUI->Enable(fMapOpen && zoomedIn && !mcd->rootDrawer->is3D());
	if (tools.size() == 0)
		iActiveTool = 0;
	pCmdUI->SetRadio(ID_ZOOMOUT == iActiveTool);
}

void ZoomableView::ZoomInPnt(zPoint p)
{

	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	double zoom3D = mcd->rootDrawer->getZoom3D();
	zoom3D /= 1.1;
	mcd->rootDrawer->setZoom3D(zoom3D);
	mcd->mpvGetView()->Invalidate();

}

void ZoomableView::ZoomOutPnt(zPoint p)
{
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	double zoom3D = mcd->rootDrawer->getZoom3D();
	zoom3D *= 1.1;
	mcd->rootDrawer->setZoom3D(zoom3D);
	mcd->mpvGetView()->Invalidate();
}

void ZoomableView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	bool fMapOpen = false;
	MapCompositionDoc *doc = (MapCompositionDoc *)GetDocument();
	if ( !doc->fIsEmpty())			
		fMapOpen = true;
	bool fTooSmall;
 	fTooSmall = false; // scale(dim.width()) < 5 || scale(dim.height()) < 5;
	pCmdUI->Enable(fMapOpen && !fTooSmall && !doc->rootDrawer->is3D());
	pCmdUI->SetRadio(ID_ZOOMIN == iActiveTool);
}

void ZoomableView::OnNoTool()
{
	//noTool(ID_ZOOMIN);
	//noTool(ID_ZOOMOUT);
	noTool();
}

void ZoomableView::noTool(int iTool, bool force) {
	if ( iTool == 0) {
		iActiveTool = 0;
		::SetCursor(LoadCursor(0,IDC_ARROW));
		tools.reset(force);
	} else {
		map<int, MapPaneViewTool *>::iterator cur = tools.find(iTool);
		if ( cur != tools.end()) {
			iActiveTool = 0;
			::SetCursor(LoadCursor(0,IDC_ARROW));
			(*cur).second->Stop();
			MapPaneViewTool *tool = (*cur).second;
			tools.erase(cur);
			if ( tool->stayResident() == false  || force) {
				delete tool;
			}
		}
	}
}


void ZoomableView::OnUpdateNoTool(CCmdUI* pCmdUI)
{
	if (tools.size() == 0)
		iActiveTool = 0;
	pCmdUI->SetRadio(0 == iActiveTool);
}

void ZoomableView::OnZoomIn()
{
	if (iActiveTool == ID_ZOOMIN) {
		noTool(ID_ZOOMIN);
		return;
	}
	noTool();
	if (HIWORD(AfxGetThreadState()->m_lastSentMsg.wParam) == 1)
	{
		zPoint p(dim.width()/2,dim.height()/2);
		ZoomInPnt(p);
	}
	else
		OnSelectArea();
	iActiveTool = ID_ZOOMIN;
}

void ZoomableView::OnZoomOut()
{
	if (iActiveTool == ID_ZOOMOUT) {
		noTool(ID_ZOOMOUT);
		return;
	}
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
	CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
	double rFactor = min(1.41, max(cbMap.width() / cbZoom.width(), cbMap.height() / cbZoom.height()));
	cbZoom *= rFactor;
	RecenterZoomHorz(cbZoom, cbMap);
	RecenterZoomVert(cbZoom, cbMap);
	mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
	setScrollBars();
	OnDraw(0);
	
}

bool ZoomableView::addTool(MapPaneViewTool *tool, int id) {
	map<int, MapPaneViewTool *>::iterator cur = tools.find(id);
	if ( cur == tools.end()){
		tools[id] = tool;
		return true;
	}
	return false;
}

void ZoomableView::changeStateTool(int id, bool isActive) {
	map<int, MapPaneViewTool *>::iterator cur = tools.find(id);
	if ( cur != tools.end()) {
		if ( isActive) {
			iActiveTool= id;
			tools[id]->SetCursor(zCursor(tools[id]->cursorName().c_str()));
		} else {
			noTool(id);
		}
	}
}

void ZoomableView::OnSelectArea()
{
	noTool(ID_ZOOMIN);

	AreaSelector *as;
	if (fAdjustSize)
		as = new AreaSelector(this, this, (NotifyRectProc)&ZoomableView::AreaSelected);
	else 
		as = new AreaSelector(this, this, (NotifyRectProc)&ZoomableView::AreaSelected, dim);
	tools[ID_ZOOMIN] = as;
	as->SetCursor(zCursor("ZoomToolCursor"));
	as->setActive(true);
	iActiveTool = ID_ZOOMIN;
}

void ZoomableView::selectArea(CCmdTarget *target, NotifyRectProc proc, const String& cursor, const Color& clr, bool keepDimensions)
{
	noTool(ID_ZOOMIN);
	AreaSelector *as;
	if (fAdjustSize)
		as = new AreaSelector(this, target, proc, clr);
	else 
		as = new AreaSelector(this, target, proc, dim, clr);
	as->setKeepDimensions(keepDimensions);
	tools[ID_ZOOMIN] = as;
	as->SetCursor(zCursor(cursor.c_str()));
	as->setActive(true);
	iActiveTool = ID_ZOOMIN;
}

void ZoomableView::OnPanArea()
{
	if (iActiveTool == ID_PANAREA) {
		noTool(ID_PANAREA);
		return;
	}
	//noTool(ID_PANAREA);
	noTool();
	tools[ID_PANAREA ] = new PanTool(this, this, (NotifyMoveProc)&ZoomableView::PanMove);
	iActiveTool = ID_PANAREA;
}

void ZoomableView::PanMove(CPoint pt)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	if ( mcd) {
		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		CRect rect;
		GetClientRect(&rect);
		double deltax = -cbZoom.width() * pt.x / (double)rect.Width();
		double deltay = cbZoom.height() * pt.y / (double)rect.Height();
		cbZoom.cMin.x += deltax;
		cbZoom.cMax.x += deltax;
		cbZoom.cMin.y += deltay;
		cbZoom.cMax.y += deltay;
		RecenterZoomHorz(cbZoom, cbMap);
		RecenterZoomVert(cbZoom, cbMap);
		mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		setScrollBars();
		OnDraw(0);
	}
}

void ZoomableView::OnUpdatePanArea(CCmdUI* pCmdUI)
{
	bool fMapOpen = false;
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	if ( !mcd->fIsEmpty())			
		fMapOpen = true;
	bool zoomedIn = (mcd->rootDrawer->getMapCoordBounds().width() > mcd->rootDrawer->getCoordBoundsZoom().width()) || 
			     (mcd->rootDrawer->getMapCoordBounds().height() > mcd->rootDrawer->getCoordBoundsZoom().height());
	pCmdUI->Enable(fMapOpen && (zoomedIn || mcd->rootDrawer->is3D()));
	if (tools.size() == 0)
		iActiveTool = 0;

	pCmdUI->SetRadio(ID_PANAREA == iActiveTool);
}

void ZoomableView::ShowArea(const MinMax& mmWish)
{
	zDimension dimWish = dimPos(mmWish);

	if (fAdjustSize && !GetParentFrame()->IsZoomed()) {
		if (abs(_rScale) < 1)
			_rScale = -1/_rScale;
		if (_rScale < 0)
			_rScale = -1/_rScale;
		double rChangeWidth = (double)dim.width() / dimWish.width();
		double rChangeHeight = (double)dim.height() / dimWish.height();
		_rScale *= sqrt(rChangeWidth * rChangeHeight);
		if (abs(_rScale) < 1)
			_rScale = -1/_rScale;
		dimWish = dimPos(mmWish);
		iXpos = mmWish.MinCol();
		iYpos = mmWish.MinRow();
		zDimension dimResize;
		CRect rct;
		GetClientRect(rct);
		dimResize.width() = dimWish.width() - rct.Width();
		dimResize.height() = dimWish.height() - rct.Height();
		FrameWindow* fw = fwParent();
		if (fw) 
			fw->resize(dimResize);
	}
	else {
		double rOldScale = _rScale;
		if (abs(_rScale) < 1)
			_rScale = -1/_rScale;
		if (_rScale < 0)
			_rScale = -1/_rScale;
		_rScale *= (double)dim.width() / dimWish.width();
		if (abs(_rScale) < 1)
			_rScale = -1/_rScale;
		long iX = (mmWish.MinCol() + mmWish.MaxCol()) / 2;
		long iY = (mmWish.MinRow() + mmWish.MaxRow()) / 2;
		long iWidth = scale(dim.width());
		long iHeight = scale(dim.height());
		if (iWidth <= 2 || iHeight <= 2) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			_rScale = rOldScale;
			return;
		}
		iXpos = iX - iWidth / 2;
		iYpos = iY - iHeight / 2;
	}
	CalcFalseOffsets();
	setScrollBars();
	SetDirty();
	bool fTooSmall = scale(dim.width()) < 5 || scale(dim.height()) < 5;
	if (fTooSmall && ID_ZOOMIN == iActiveTool)
		noTool();
}

void ZoomableView::ShowArea(double rScale, long iX, long iY)
{
	_rScale = rScale;
	iXpos = iX;
	iYpos = iY;
	CalcFalseOffsets();
	setScrollBars();
	SetDirty();
}

void ZoomableView::ZoomOutAreaSelected(CRect rect)
{
  if (rect.Width() < 3 && rect.Height() < 3)
    ZoomOutPnt(rect.TopLeft());
  else if (rect.Width() < 3 || rect.Height() < 3)
    return;
  else {
		CRect rct;
		rct.top = - rect.top;
		rct.bottom = dim.cy + (dim.cy - rect.bottom);
		rct.left = - rect.left;
		rct.right = dim.cx + (dim.cx - rect.right);
		AreaSelected(rct);
	}
	bool fTooSmall;
  fTooSmall= scale(iXsize,true) < 5 || scale(iYsize,true) < 5;
	if (fTooSmall && ID_ZOOMOUT == iActiveTool)
		noTool();
}

void ZoomableView::OnReDraw()
{
	SetDirty();
}


/////////////////////////////////////////////////////////////////////////////
// ZoomableView message handlers

BOOL ZoomableView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (HTCLIENT == nHitTest) {
		if (tools.size() > 0 && iActiveTool > 0 && tools[iActiveTool]->OnSetCursor())
			return TRUE;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

BOOL ZoomableView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

BOOL ZoomableView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CWnd* wnd = GetParent();
	if (0 == dynamic_cast<CFrameWnd*>(wnd)) {
		if (wnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

bool ZoomableView::fShowScrollBars() const
{
	return fScrollBarsVisible;
}

void ZoomableView::ShowScrollBars(bool fYesNo) 
{
	fScrollBarsVisible = fYesNo;
}

MinMax ZoomableView::mmVisibleMapArea() const
{
 return MinMax(RowCol(iYpos - iYfalseOffset, iXpos - iXfalseOffset ), RowCol(iYpos + iYpage - iYfalseOffset, iXpos + iXpage - iXfalseOffset));
}

//------------------------------------------------
MapTools::~MapTools() {
	reset();
}

void MapTools::reset(bool force) {
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		(*cur).second->Stop();
		if ( !(*cur).second->stayResident() || force)
			delete (*cur).second;
	}
	clear();
}

bool MapTools::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	bool result = false;
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			result |= (*cur).second->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	return result;
}

void MapTools::OnMouseMove(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnMouseMove(nFlags, point);
	}
}

void MapTools::OnLButtonDblClk(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnLButtonDblClk(nFlags, point);
	}
}

void MapTools::OnLButtonDown(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnLButtonDown(nFlags, point);
	}
}

void MapTools::OnLButtonUp(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnLButtonUp(nFlags, point);
	}

}
void MapTools::OnRButtonDblClk(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnRButtonDblClk(nFlags, point);
	}
}

void MapTools::OnRButtonDown(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnRButtonDown(nFlags, point);
	}
}

void MapTools::OnRButtonUp(UINT nFlags, CPoint point, int state){
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnRButtonUp(nFlags, point);
	}
}

BOOL MapTools::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	BOOL routed = FALSE;
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive()) {
			routed |= (*cur).second->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}
	}
	return routed;
}

void MapTools::OnEscape() {
	for(map<int, MapPaneViewTool *>::iterator cur = begin(); cur != end(); ++cur) {
		if ( (*cur).second->isActive())
			(*cur).second->OnEscape();
	}
}