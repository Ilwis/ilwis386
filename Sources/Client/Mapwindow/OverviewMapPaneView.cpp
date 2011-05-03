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
// OverviewMapPaneView.cpp: implementation of the OverviewMapPaneView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\OverviewMapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\DragRectTool.h"
#include "Headers\constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(OverviewMapPaneView, SimpleMapPaneView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SELECTAREA, OnSelectArea)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



OverviewMapPaneView::OverviewMapPaneView()
{
  ShowScrollBars(false);
}

OverviewMapPaneView::~OverviewMapPaneView()
{
}

BOOL OverviewMapPaneView::PreCreateWindow(CREATESTRUCT& cs)
{
  if (!SimpleMapPaneView::PreCreateWindow(cs))
    return FALSE;
  cs.style &= ~WS_HSCROLL & ~WS_VSCROLL;
  return TRUE;
}

void OverviewMapPaneView::OnInitialUpdate()
{
	SimpleMapPaneView::OnInitialUpdate();

  _rScale = 1;
  iXpos = iXmin;
  iYpos = iYmin;
  
  fStarting = false;
  setScrollBars();
}

void OverviewMapPaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
  SimpleMapPaneView::OnUpdate(pSender, lHint, pHint);
  if (6 == lHint)
    return;
	if (lHint == 5 && 0 != dcView) // setdirty report from mappaneview
		Invalidate(); 
  else
    UpdateSelf();
}

void OverviewMapPaneView::OnSize(UINT nType, int cx, int cy)
{
  ZoomableView::OnSize(nType, cx, cy);
  if (0 == cx || 0 == cy)
		return;
  UpdateSelf();
}

void OverviewMapPaneView::UpdateSelf()
{
	if ( hasWMSDrawer() ) {
		ShowWindow(SW_HIDE);
		return;
	} else
		ShowWindow(SW_SHOW);

  iXpos = iXmin;
  iYpos = iYmin;
  iXfalseOffset = 0;
  iYfalseOffset = 0;

	MinMax mm = mmBounds();
  int iWidth = mm.width();
  int iHeight = mm.height();
	double rScaleX = double(dim.width()) / iWidth;
	double rScaleY = double(dim.height()) / iHeight;
	// choose the smallest of the scales as the correct one
	double rScale = min(rScaleX, rScaleY);

	// change to 'Ilwis scale'
	_rScale = rScale >= 1.0 ? rScale : -1.0/rScale;

  CalcFalseOffsets();
  setScrollBars();
  SetDirty();
}

void OverviewMapPaneView::OnDraw(CDC* cdc)
{
  SimpleMapPaneView::OnDraw(cdc);
  MapPaneView* mpv = GetDocument()->mpvGetView();
	MinMax mm = mpv->mmVisibleMapArea();
	CRect rect = rctPos(mm);

  CGdiObject* penOld = cdc->SelectStockObject(WHITE_PEN);
  if (rect.Width() > 4 && rect.Height() > 4) 
  {
    CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);
    cdc->Rectangle(&rect);
    cdc->SelectStockObject(BLACK_PEN);
    rect.DeflateRect(1,1);
    cdc->Rectangle(&rect);
    cdc->SelectObject(brOld);
  }
  else 
  {
    CRect rectWnd;
    GetClientRect(&rectWnd);
    CPoint pt1, pt2;
    // vertical line
    pt1.x = (rect.left + rect.right) / 2;
    pt1.y = rectWnd.top;
    pt2 = pt1;
    pt2.y = rectWnd.bottom;
    cdc->MoveTo(pt1);
    cdc->LineTo(pt2);
    pt1.x += 1;
    pt2.x += 1;
    cdc->SelectStockObject(BLACK_PEN);
    cdc->MoveTo(pt1);
    cdc->LineTo(pt2);

    cdc->SelectStockObject(WHITE_PEN);
    pt1.x = rectWnd.left;
    pt1.y = (rect.top + rect.bottom) / 2;
    pt2 = pt1;
    pt2.x = rectWnd.right;
    cdc->MoveTo(pt1);
    cdc->LineTo(pt2);
    pt1.y += 1;
    pt2.y += 1;
    cdc->SelectStockObject(BLACK_PEN);
    cdc->MoveTo(pt1);
    cdc->LineTo(pt2);
  }
  cdc->SelectObject(penOld);
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void OverviewMapPaneView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men;
	men.CreatePopupMenu();
  add(ID_SELECTAREA);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void OverviewMapPaneView::OnSelectArea()
{
	OnNoTool();
  MapPaneView* mpv = GetDocument()->mpvGetView();
  tools[ID_ZOOMIN] = new AreaSelector(this, this, (NotifyRectProc)&OverviewMapPaneView::AreaSelected); 
  tools[ID_ZOOMIN]->SetCursor(zCursor("ZoomToolCursor"));
	iActiveTool = ID_ZOOMIN;
}

void OverviewMapPaneView::AreaSelected(CRect rect)
{
  OnNoTool();
  MapPaneView* mpv = GetDocument()->mpvGetView();
	rect.NormalizeRect();
  if (rect.Width() < 3 && rect.Height() < 3)
    return;
  MinMax mmWish = mmRect(rect);
  mpv->ShowArea(mmWish);
}

inline int onlypos(int iVal) { return iVal > 0 ? iVal : 0; }

void OverviewMapPaneView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnNoTool();
  MapPaneView* mpv = GetDocument()->mpvGetView();
	MinMax mm = mpv->mmVisibleMapArea();
  MinMax mmBnds = mmBounds();
	CRect rect = rctPos(mm);
	CRect rectBnds = rctPos(mmBnds);

  int iDiff = 0;
  iDiff += onlypos(rect.left - rectBnds.left);
  iDiff += onlypos(rectBnds.right - rect.right);
  iDiff += onlypos(rect.top - rectBnds.top);
  iDiff += onlypos(rectBnds.bottom - rect.bottom);
  
  if (!rect.PtInRect(point) || iDiff < 10)
  {
  	tools[ID_ZOOMIN] = new AreaSelector(this, this, (NotifyRectProc)&OverviewMapPaneView::AreaSelected);
  	tools[ID_ZOOMIN]->SetCursor(zCursor("ZoomToolCursor"));
  	iActiveTool = ID_ZOOMIN;
  }
  else 
  {
    tools[ID_SELECTAREA] = new DragRectTool(this, this, (NotifyRectProc)&OverviewMapPaneView::AreaSelected, rect);
	iActiveTool = ID_SELECTAREA;
  }
  tools[iActiveTool]->OnLButtonDown(nFlags, point);
}

bool OverviewMapPaneView::hasWMSDrawer() {
	/*MapCompositionDoc* mcd = (MapCompositionDoc*)GetDocument();
	for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) {
		WMSMapDrawer *drw = dynamic_cast<WMSMapDrawer *>((*iter));
		return drw != NULL;
	}*/
	return false;
}

