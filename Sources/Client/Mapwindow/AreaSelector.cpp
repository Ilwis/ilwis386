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
// AreaSelector.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\AreaSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AreaSelector

using namespace ILWIS;

AreaSelector::AreaSelector(ZoomableView* mappaneview, CCmdTarget* cmdTarget, NotifyRectProc notProc, const Color& _clr)
: MapPaneViewTool(mappaneview)
, cmt(cmdTarget)
, np(notProc)
, fDown(false)
, fKeepDimensions(false)
, clr(_clr)
{
	SetCursor(zCursor("AreaSelCursor"));
	needsMouseFocus = true;
}

AreaSelector::AreaSelector(ZoomableView* mappaneview, CCmdTarget* cmdTarget, NotifyRectProc notProc, zDimension dimension, const Color& _clr)
: MapPaneViewTool(mappaneview)
, cmt(cmdTarget)
, np(notProc)
, fDown(false)
, dim(dimension)
, fKeepDimensions(true)
, selectionDrawer(0)
,clr(_clr)
{
	SetCursor(zCursor("AreaSelCursor"));
	needsMouseFocus = true;
}

AreaSelector::~AreaSelector()
{
	if (fDown)
		DrawRect();
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(mpv->GetDocument());
	mcd->rootDrawer->setSelectionDrawer(0);
	delete selectionDrawer;
}

/////////////////////////////////////////////////////////////////////////////
// AreaSelector message handlers

void AreaSelector::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (fDown) {
		pEnd = point;
		DrawRect();
	}
}

void AreaSelector::OnLButtonDown(UINT nFlags, CPoint point) 
{
	pStart = point;
	pEnd = pStart;
	fDown = true;

	if ( selectionDrawer == NULL)  {
		MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(mpv->GetDocument());
		ILWIS::DrawerParameters sp(mcd->rootDrawer, mcd->rootDrawer);
		selectionDrawer = (ILWIS::SelectionRectangle *)NewDrawer::getDrawer("SelectionRectangle", "Ilwis38", &sp);
		mcd->rootDrawer->setSelectionDrawer(selectionDrawer);
		//mcd->rootDrawer->setBitmapRedraw(true);
		mcd->mpvGetView()->setBitmapRedraw(true);
		selectionDrawer->setColor(clr);
	}
	mpv->SetCapture();
	DrawRect();
}

void AreaSelector::OnLButtonUp(UINT nFlags, CPoint point) 
{
	DrawRect();
	fDown = false;
    pEnd = point;

	if (selectionDrawer != 0) {
		MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(mpv->GetDocument());
		mcd->rootDrawer->setSelectionDrawer(0);
		delete selectionDrawer;
		selectionDrawer = NULL;
		mcd->mpvGetView()->setBitmapRedraw(false);
	}

	(cmt->*np)(rect());
	ReleaseCapture();
}

CRect AreaSelector::rect() const
{
  int x1, y1, x2, y2;
  x1 = pStart.x;
  x2 = pEnd.x;
  y1 = pStart.y;
  y2 = pEnd.y;
	if (fKeepDimensions) {
		int dx = x2 - x1;
		int dy = y2 - y1;
		double rFactX = abs(double(dx) / dim.cx);
		double rFactY = abs(double(dy) / dim.cy);
		double rFact = double(dim.cx) / dim.cy;
		if (rFactX > rFactY) 
			y2 = y1 + (int)(abs(dx) * sign(dy) / rFact);
		else
			x2 = x1 + (int)(abs(dy) * sign(dx) * rFact);
	}
  if (abs(x1-x2) < 3) x2 = x1;
  if (abs(y1-y2) < 3) y2 = y1;
  CRect rect(x1,y1,x2,y2);
  rect.NormalizeRect();

 
  return rect;
}

void AreaSelector::DrawRect()
{
	if ( selectionDrawer) {
		selectionDrawer->calcWorldCoordinates(rect());
		mpv->Invalidate();
	}
}
