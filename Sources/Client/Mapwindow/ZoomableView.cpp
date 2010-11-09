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
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SelectionRectangle.h"
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
	: as(0)
	, iXpos(0), iYpos(0)
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
	zoomf = 0.0;
	beginMovePoint = CPoint(iUNDEF, iUNDEF);
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
	switch (message) {
		case WM_MOUSEMOVE:
			if (as) 
				as->OnMouseMove(wParam, point);
			else if (wParam & MK_CONTROL) moveEyePoint(point, message);
//			return CView::OnWndMsg(message, wParam, lParam, pResult);
			//return TRUE;
			break;
		case WM_LBUTTONDBLCLK:
			if (as) as->OnLButtonDblClk(wParam, point);
			return FALSE;
		case WM_LBUTTONDOWN:
			if (as) as->OnLButtonDown(wParam, point);
			else if (wParam & MK_CONTROL) moveEyePoint(point,message);
			break;
		case WM_LBUTTONUP:
			if (as) as->OnLButtonUp(wParam, point);
			else if (wParam & MK_CONTROL) moveEyePoint(point,message);
			break;
		case WM_RBUTTONDBLCLK:
			if (as) as->OnRButtonDblClk(wParam, point);
			return FALSE;
		case WM_RBUTTONDOWN:
			if (as) as->OnRButtonDown(wParam, point);
			return FALSE;
		case WM_RBUTTONUP:
			if (as) as->OnRButtonUp(wParam, point);
			return FALSE;
	}
	return CView::OnWndMsg(message, wParam, lParam, pResult);
}

void ZoomableView::moveEyePoint(const CPoint& pnt, UINT message) {
	if ( message == WM_LBUTTONDOWN) {
		beginMovePoint = pnt;
	}
	else if ( message == WM_LBUTTONUP){
		beginMovePoint = CPoint(iUNDEF,iUNDEF);
		MapCompositionDoc *doc = (MapCompositionDoc *)GetDocument();
		Coord eyePoint = doc->rootDrawer->getEyePoint();
	} else if (message == WM_MOUSEMOVE && beginMovePoint.x != iUNDEF) {
		double deltax = beginMovePoint.x - pnt.x;
		double deltay = beginMovePoint.y - pnt.y;
		if ( deltax == 0 && deltay == 0)
			return;
		double roll = deltax / 50.0;
		double yaw = deltay / 50.0;

		MapCompositionDoc *doc = (MapCompositionDoc *)GetDocument();
		Coord eyePoint = doc->rootDrawer->getEyePoint();
		Coord viewPoint = doc->rootDrawer->getViewPoint();
		double deltaXEyeView = eyePoint.x - viewPoint.x;
		double deltaYEyeView = eyePoint.y - viewPoint.y;
		double deltaZEyeView = eyePoint.z - viewPoint.z;
		double distance = sqrt(deltaXEyeView * deltaXEyeView + deltaYEyeView * deltaYEyeView + deltaZEyeView * deltaZEyeView);
		double theta = M_PI_2 - atan2( deltaZEyeView ,sqrt( deltaXEyeView*deltaXEyeView + deltaYEyeView * deltaYEyeView));
		double phi = atan2(deltaYEyeView,deltaXEyeView);
		theta += yaw;
		phi += roll;
		double x = distance * cos(phi) * sin(theta);
		double y = distance * sin(theta) * sin(phi);
		double z = distance * cos(theta);
		double xe =  x + viewPoint.x;
		double ye = y + viewPoint.y;
		double ze = z + viewPoint.z;
	
		Coord newEye(xe,ye,ze);
		doc->rootDrawer->setEyePoint(newEye);
		beginMovePoint = pnt;
		doc->mpvGetView()->Invalidate();
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
		mcd->rootDrawer->setZoom(rect);
		setScrollBars();
		OnDraw(0);
	}
}

void ZoomableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (as && VK_ESCAPE == nChar) {
		as->OnEscape();
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
		vertLineMove(-1);
		break;
	case SB_LINEDOWN:
		vertLineMove(1);
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
	pCmdUI->Enable(fMapOpen && zoomedIn);
	if (0 == as)
		iActiveTool = 0;
	pCmdUI->SetRadio(ID_ZOOMOUT == iActiveTool);
}

void ZoomableView::ZoomInPnt(zPoint p)
{

	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	Coord eyePoint = mcd->rootDrawer->getEyePoint();
	Coord viewPoint = mcd->rootDrawer->getViewPoint();
	double deltaXEyeView = eyePoint.x - viewPoint.x;
	double deltaYEyeView = eyePoint.y - viewPoint.y;
	double deltaZEyeView = eyePoint.z - viewPoint.z;
	Coord newEyePoint(eyePoint.x - deltaXEyeView/10.0,
		eyePoint.y - deltaYEyeView/10.0,
		eyePoint.z - deltaZEyeView/10.0);
	mcd->rootDrawer->setEyePoint(newEyePoint);
	mcd->mpvGetView()->Invalidate();

}

void ZoomableView::ZoomOutPnt(zPoint p)
{
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	Coord eyePoint = mcd->rootDrawer->getEyePoint();
	Coord viewPoint = mcd->rootDrawer->getViewPoint();
	double deltaXEyeView = eyePoint.x - viewPoint.x;
	double deltaYEyeView = eyePoint.y - viewPoint.y;
	double deltaZEyeView = eyePoint.z - viewPoint.z;
	Coord newEyePoint(eyePoint.x + deltaXEyeView/10.0,
		eyePoint.y + deltaYEyeView/10.0,
		eyePoint.z + deltaZEyeView/10.0);
	mcd->rootDrawer->setEyePoint(newEyePoint);
	mcd->mpvGetView()->Invalidate();
}

void ZoomableView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	bool fMapOpen = false;
	IlwisDocument *doc = (IlwisDocument *)GetDocument();
	if ( !doc->fIsEmpty())			
		fMapOpen = true;
	bool fTooSmall;
 	fTooSmall = false; // scale(dim.width()) < 5 || scale(dim.height()) < 5;
	pCmdUI->Enable(fMapOpen && !fTooSmall);
	if (0 == as)
		iActiveTool = 0;
	pCmdUI->SetRadio(ID_ZOOMIN == iActiveTool);
}

void ZoomableView::OnNoTool()
{
	iActiveTool = 0;
	if (as) 
		as->Stop();
}

void ZoomableView::OnUpdateNoTool(CCmdUI* pCmdUI)
{
	if (0 == as)
		iActiveTool = 0;
	pCmdUI->SetRadio(0 == iActiveTool);
}

void ZoomableView::OnZoomIn()
{
	if (iActiveTool == ID_ZOOMIN) {
		OnNoTool();
		return;
	}
	OnNoTool();
	if (HIWORD(AfxGetThreadState()->m_lastSentMsg.wParam) == 1)
	{
		zPoint p(dim.width()/2,dim.height()/2);
		ZoomInPnt(p);
	}
	else
		OnSelectArea();
}

void ZoomableView::OnZoomOut()
{
	if (iActiveTool == ID_ZOOMOUT) {
		OnNoTool();
		return;
	}
	OnNoTool();
	MapCompositionDoc *mcd = (MapCompositionDoc *)GetDocument();
	CoordBounds cb = mcd->rootDrawer->getCoordBoundsZoom();
	cb *= 1.41;
	mcd->rootDrawer->setCoordBoundsZoom(cb);
	setScrollBars();
	OnDraw(0);
	
	iActiveTool = ID_ZOOMOUT;
}

void ZoomableView::OnSelectArea()
{
	OnNoTool();

	if (fAdjustSize)
		as = new AreaSelector(this, this, (NotifyRectProc)&ZoomableView::AreaSelected);
	else 
		as = new AreaSelector(this, this, (NotifyRectProc)&ZoomableView::AreaSelected, dim);
	as->SetCursor(zCursor("ZoomToolCursor"));
	iActiveTool = ID_ZOOMIN;
}

void ZoomableView::OnPanArea()
{
	if (iActiveTool == ID_PANAREA) {
		OnNoTool();
		return;
	}
	OnNoTool();
	CRect rect;
	GetClientRect(&rect);
	as = new PanTool(this, this, (NotifyMoveProc)&ZoomableView::PanMove, rect);
	iActiveTool = ID_PANAREA;
}

void ZoomableView::PanMove(CPoint pt)
{
	horzPixMove(-scale(pt.x),false);
	vertPixMove(-scale(pt.y),false);
	SetDirty();
}

void ZoomableView::OnUpdatePanArea(CCmdUI* pCmdUI)
{
	bool fMapOpen = false;
	IlwisDocument *doc = (IlwisDocument *)GetDocument();
	if ( !doc->fIsEmpty())			
		fMapOpen = true;
	bool fPanPossible = iXpage < iXsize || iYpage < iYsize;
	pCmdUI->Enable(fMapOpen && fPanPossible);
	if (0 == as)
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
		OnNoTool();
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
		OnNoTool();
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
		if (as && as->OnSetCursor())
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

