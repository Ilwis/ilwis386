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
// HistogramGraphView.cpp: implementation of the HistogramGraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Table\tblview.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Headers\messages.h"
#include "Headers\constant.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

IMPLEMENT_DYNCREATE(HistogramGraphView, GraphPaneView)

BEGIN_MESSAGE_MAP(HistogramGraphView, GraphPaneView)
  ON_MESSAGE(ILWM_VIEWSETTINGS, OnViewSettings)
  	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

HistogramGraphView::HistogramGraphView() : drawer(0), mcd(0), moveMode(false), tool(0), color(RGB(25,0,0)), spread(0.01)
{
}

HistogramGraphView::HistogramGraphView(SpatialDataDrawer *drw, MapCompositionDoc *doc, DrawerTool *_tool) : drawer(drw), mcd(doc), moveMode(false), tool(_tool),color(RGB(255,0,0)), spread(0.01)
{
}

HistogramGraphView::~HistogramGraphView()
{
	if (tool)
		tool->update();
}

void HistogramGraphView::OnInitialUpdate()
{
  HistogramDoc* hd = dynamic_cast<HistogramDoc*>(CView::GetDocument());
  if (hd) {
    hd->RemoveView(this);
    hd->hgd.AddView(this);
  }
	GraphPaneView::OnInitialUpdate();
}

LRESULT HistogramGraphView::OnViewSettings(WPARAM wP, LPARAM lP)
{
	if (wP == SAVESETTINGS) 
  {
    GraphDoc* doc = GetDocument();
    doc->RemoveView(this);
	}
	return TRUE;
}

void HistogramGraphView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GraphPaneView::OnLButtonDown(nFlags, point);
	moveMode = true;
	setRasterSelection(point);


}

void HistogramGraphView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GraphPaneView::OnLButtonUp(nFlags, point);
	info->text(point, "");
	moveMode = false;
}

void HistogramGraphView::OnMouseMove(UINT nFlags, CPoint point) 
{
  GraphPaneView::OnMouseMove(nFlags, point);
  setRasterSelection(point);

}

void HistogramGraphView::setThresholdRange() {
	if (!drawer)
		return;
	RangeReal rr;
	if ( drawer->getType() == "AnimationDrawer") {
		MapListPtr *mlptr = static_cast<MapListPtr *>(drawer->getObject());
		rr = mlptr->getRange();
	}else {
		rr = drawer->getBaseMap()->rrMinMax();
	}
	drawer->setTresholdColor(color);
	drawer->setTresholdRange(RangeReal(lastValue - rr.rWidth()*spread, lastValue + rr.rWidth()*spread),false);

	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drawer->prepareChildDrawers(&pp);
	mcd->mpvGetView()->Invalidate();
}

void HistogramGraphView::setRasterSelection(CPoint point) {
	if ( drawer && moveMode) {
		String s = grdrw->sText(point);
		String head = s.sHead(",");
		//String tail = s.sTail(",").sTrimSpaces();
		double v = head.rVal();
		if ( v == rUNDEF)
			return;
		if ( drawer->getBaseMap()) {
			lastValue = v;
			setThresholdRange();

		}

	}
}

void HistogramGraphView::setTresholdColor(const Color& clr) {
	color = clr;
}

Color HistogramGraphView::getTresholdColor() const{
	return color;
}

void HistogramGraphView::setSpread(double v) {
	spread = v;
}

double HistogramGraphView::getSpread() const{
	return spread;
}

void HistogramGraphView::OnDestroy() {
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 0);
}
