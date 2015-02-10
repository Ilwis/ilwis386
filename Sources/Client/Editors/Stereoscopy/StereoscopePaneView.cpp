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
// StereoscopePaneView.cpp: implementation of the StereoscopePaneView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\StereoscopePaneView.h"
#include "Client\Editors\Stereoscopy\StereoscopeWindow.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Editor.h"
#include "Headers\constant.h"

IMPLEMENT_DYNCREATE(StereoscopePaneView, MapPaneView)

BEGIN_MESSAGE_MAP(StereoscopePaneView, MapPaneView)
	//{{AFX_MSG_MAP(StereoscopePaneView)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCPAINT()
	ON_COMMAND(ID_NONEEDIT, OnNoneEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StereoscopePaneView::StereoscopePaneView()
: spvSiblingPane(0)
, fLeft(true)
, swParent(0)
, fCoupledVertScroll(true)
, fActive(false)
{

}

StereoscopePaneView::~StereoscopePaneView()
{

}

void StereoscopePaneView::OnInitialUpdate()
{
	MapPaneView::OnInitialUpdate();
	ShowScrollBar(SB_BOTH, FALSE);
}

void StereoscopePaneView::SetSiblingPane(StereoscopePaneView * spv)
{
	spvSiblingPane = spv;
	swParent = dynamic_cast<StereoscopeWindow*>(fwParent());
}

void StereoscopePaneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar && !fActive)
		swParent->SetActiveView(this);
	MapPaneView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void StereoscopePaneView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// The vertical scrollbar must scroll uncoupled; the CSplitterWnd vertical scrollbar is "shared" and already scrolls both panes
	// If we don't do this, the vertical scrollbar does not behave correctly and one of the two panes scrolls at twice the speed.
	fCoupledVertScroll = false;
	MapPaneView::OnVScroll(nSBCode, nPos, pScrollBar);
	fCoupledVertScroll = true;
}

void StereoscopePaneView::OnEntireMap()
{
	MapPaneView::OnEntireMap();
	if (fActive && spvSiblingPane)
		spvSiblingPane->OnEntireMap();
}

void StereoscopePaneView::AreaSelected(CRect rect)
{
	MapPaneView::AreaSelected(rect);
	if (fActive && spvSiblingPane) {
		spvSiblingPane->AreaSelected(rect);
		MapCompositionDoc* mcd = spvSiblingPane->GetDocument();
		if ( mcd ) {
			CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
			SetXPosSibling(cbZoom);
			mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		}
		spvSiblingPane->setScrollBars();
		spvSiblingPane->OnDraw(0);
	}
}

void StereoscopePaneView::ZoomOutAreaSelected(CRect rect)
{
	MapPaneView::ZoomOutAreaSelected(rect);
	if (fActive && spvSiblingPane) {
		spvSiblingPane->ZoomOutAreaSelected(rect);
		MapCompositionDoc* mcd = spvSiblingPane->GetDocument();
		if ( mcd ) {
			CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
			SetXPosSibling(cbZoom);
			mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		}
		spvSiblingPane->setScrollBars();
		spvSiblingPane->OnDraw(0);
	}
}

void StereoscopePaneView::SetXPosSibling(CoordBounds & cbZoom2)
{
	MapCompositionDoc* mcd1 = GetDocument();
	MapCompositionDoc* mcd2 = spvSiblingPane->GetDocument();
	if ( mcd1 && mcd2 ) {
		CoordBounds cbZoom1 = mcd1->rootDrawer->getCoordBoundsZoom();
		CoordBounds cbMap1 = mcd1->rootDrawer->getMapCoordBounds();
		double frac1 = (cbZoom1.cMin.x - cbMap1.cMin.x) / cbMap1.width();
		CoordBounds cbMap2 = mcd2->rootDrawer->getMapCoordBounds();
		CRect rect;
		spvSiblingPane->GetClientRect(&rect);
		double width2 = cbZoom2.width();
		cbZoom2.cMin.x = cbMap2.cMin.x + cbMap2.width() * frac1 + width2 * (fLeft ? swParent->rXoffsetDelta() : -swParent->rXoffsetDelta()) / (double)rect.Width();
		cbZoom2.cMax.x = cbZoom2.cMin.x + width2;
		spvSiblingPane->RecenterZoomHorz(cbZoom2, cbMap2);
	}
}

void StereoscopePaneView::PanMoveY(int y, CoordBounds & cbZoom)
{
	MapCompositionDoc* mcd = spvSiblingPane->GetDocument();
	if ( mcd ) {
		CoordBounds cbMap = mcd->rootDrawer->getMapCoordBounds();
		CRect rect;
		spvSiblingPane->GetClientRect(&rect);
		double deltay = cbZoom.height() * y / (double)rect.Height();
		cbZoom.cMin.y += deltay;
		cbZoom.cMax.y += deltay;
		spvSiblingPane->RecenterZoomVert(cbZoom, cbMap);
	}
}

void StereoscopePaneView::PanMove(CPoint pt)
{
	MapPaneView::PanMove(pt);
	if (!swParent->fXoffsetLocked()) {
		if (fLeft)
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() + pt.x);
		else
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() - pt.x);
	}
	if (fActive && spvSiblingPane) {
		MapCompositionDoc* mcd = spvSiblingPane->GetDocument();
		if ( mcd ) {
			CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
			PanMoveY(pt.y, cbZoom);
			SetXPosSibling(cbZoom);
			mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
			spvSiblingPane->setScrollBars();
			spvSiblingPane->OnDraw(0);
		}
	}
}

int StereoscopePaneView::vertPixMove(long iDiff, bool fPreScroll)
{
	MapPaneView::vertPixMove(iDiff, fPreScroll);
	if (fActive && fCoupledVertScroll && spvSiblingPane)
		spvSiblingPane->vertPixMove(iDiff, fPreScroll);
	return 0;	
}

int StereoscopePaneView::horzPixMove(long iDiff, bool fPreScroll)
{
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(GetDocument());
	CoordBounds cbZoom1 = mcd->rootDrawer->getCoordBoundsZoom();
	MapPaneView::horzPixMove(iDiff, fPreScroll);
	CoordBounds cbZoom2 = mcd->rootDrawer->getCoordBoundsZoom();
	if (!swParent->fXoffsetLocked()) {
		CRect rect;
		GetClientRect(&rect);
		double delta = (cbZoom1.cMin.x - cbZoom2.cMin.x) * (double)rect.Width() / cbZoom1.width();
		if (fLeft)
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() + delta);
		else
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() - delta);
	}
	if (fActive && swParent->fXoffsetLocked() && spvSiblingPane) {
		MapCompositionDoc* mcd = spvSiblingPane->GetDocument();
		if ( mcd ) {
			CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
			SetXPosSibling(cbZoom);
			mcd->rootDrawer->setCoordBoundsZoom(cbZoom);
		}
		spvSiblingPane->setScrollBars();
		spvSiblingPane->OnDraw(0);
	}
	return 0;
}

void StereoscopePaneView::SetLeft(bool b)
{
	fLeft = b;
}

void StereoscopePaneView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	fActive = (TRUE == bActivate); // translate BOOL to bool
	GetParent()->Invalidate();
}

void StereoscopePaneView::OnNcPaint() 
{
	// Default OnNcPaint (incl. scrollbars)
	CView::OnNcPaint();

	// get window DC that is clipped to the non-client area
	CWindowDC dc(this);

	CRect rcClient, rcBar;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	GetWindowRect(rcBar);
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());

	// client area and scrollbars is not our bussiness :)
	CRect rcVert (rcClient);
	rcVert.InflateRect(0, GetSystemMetrics(SM_CYHSCROLL));
	dc.ExcludeClipRect(rcVert);
	CRect rcHorz (rcClient);
	rcHorz.InflateRect(GetSystemMetrics(SM_CXVSCROLL), 0);
	dc.ExcludeClipRect(rcHorz);

	COLORREF clrActive = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF clrInactive = GetSysColor(COLOR_SCROLLBAR);
	// Now draw the active or inactive square anyway: the default OnNcPaint doesn't draw
	// unnecessarily, and there is no such thing as NcInvalidate(), so if you only draw
	// when you're active, you won't get cleaned up often enough.
	// COLOR_SCROLLBAR is as close as we can get to the default color of that square
	dc.FillSolidRect(rcBar, fActive?clrActive:clrInactive);

	ReleaseDC(&dc);
}

void StereoscopePaneView::OnNoneEdit()
{
	SetFocus(); // be sure that edit fields get a WM_KILLFOCUS
  delete edit;
  edit = 0;
	StereoscopeWindow* dw = dynamic_cast<StereoscopeWindow*>(dwParent());
	if (0 != dw)
	{
		dw->SetAcceleratorTable();
		dw->bbDataWindow.LoadButtons("stereoscopeGen.but");
		dw->RecalcLayout();
	}
	UpdateFrame();
	GetParentFrame()->RecalcLayout();
  GetDocument()->UpdateAllViews(0);
}
