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
, fCoupledScroll(true)
, fActive(false)
{

}

StereoscopePaneView::~StereoscopePaneView()
{

}

void StereoscopePaneView::SetSiblingPane(StereoscopePaneView * spv)
{
	spvSiblingPane = spv;
	swParent = dynamic_cast<StereoscopeWindow*>(fwParent());
}

void StereoscopePaneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	fCoupledScroll = swParent->fXoffsetLocked();
	MapCompositionDoc* mcd = GetDocument();
	CoordBounds cbBefore (mcd->rootDrawer->getCoordBoundsZoom()); // sample the position before scroll
	MapPaneView::OnHScroll(nSBCode, nPos, pScrollBar);
	CoordBounds cbAfter (mcd->rootDrawer->getCoordBoundsZoom()); // sample the position after scroll
	// The difference should give the "delta" of the XoffsetDelta.
	// "photos move away from eachother" results in an increase of the minmax when left, and
	// a decrease of the minmax in the right pane. Record it as such.
	if (!fCoupledScroll && swParent)
		if (fLeft) // we're in left pane
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() + cbAfter.cMin.x - cbBefore.cMin.x);
		else // we're in right pane
			swParent->SetXoffsetDelta(swParent->rXoffsetDelta() + cbBefore.cMin.x - cbAfter.cMin.x);
	
	fCoupledScroll = true;
}

void StereoscopePaneView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// We scroll vertically uncoupled; the CSplitterWnd vertical scrollbar is "shared"
	// and already scrolls coupled for us
	// If we don't do this, the vertical scrollbar does not behave correctly when the
	// left pane is active.
	fCoupledScroll = false;
	MapPaneView::OnVScroll(nSBCode, nPos, pScrollBar); // calls SetDirty; scroll uncoupled
	fCoupledScroll = true;
}

void StereoscopePaneView::RequestRedraw()
{
	MapPaneView::RequestRedraw(); // call parent implementation

	if (swParent)
	{
		bool fIAmMaster = swParent->fRequestMasterLock();
		if (fIAmMaster)
		{
			// Check on fCoupledScroll: don't pass "ShowAreaInSlave" when scrolling uncoupled
			// This would not give a wrong iXoffsetDelta, but would result in incorrect display
			// Also check for rUNDEF scale to avoid calling ShowArea in the slave when this would be nonsense
			if (fCoupledScroll && spvSiblingPane)// && spvSiblingPane->rScale()!=rUNDEF)
			{
				double rXposDelta = 0;
				if (fLeft) // we're in left pane
					rXposDelta = -swParent->rXoffsetDelta();
				else // we're in right pane
					rXposDelta = +swParent->rXoffsetDelta();
				CoordBounds cb (GetDocument()->rootDrawer->getCoordBoundsZoom());
				cb.cMin += Coord(rXposDelta, 0);
				cb.cMax += Coord(rXposDelta, 0);
				spvSiblingPane->GetDocument()->rootDrawer->setCoordBoundsZoom(cb);
				spvSiblingPane->setScrollBars();
				spvSiblingPane->RequestRedraw();
			}
			swParent->ReleaseMasterLock();
		}
	}
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
