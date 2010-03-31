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
// LegendItemDrawer.cpp: implementation of the LegendItemDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\LegendLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Editors\Layout\LegendDrawer.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\Mapwindow\Drawers\MapListDrawer.h"
#include "Client\Mapwindow\Drawers\MapListColorCompDrawer.h"
#include "Client\Mapwindow\Drawers\SegmentMapDrawer.h"
#include "Client\Mapwindow\Drawers\PolygonMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Editors\Layout\LegendItemDrawer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LegendItemDrawer::LegendItemDrawer(LegendLayoutItem& item)
: LegendBaseDrawer(item)
{
  iColumns = 1;
  iRowHeight = 10;
	rWidthFact = 1.4;
	fAutoRowHeight = true;
	fAskRowHeight = false;
	iRowExtra = 0;
}

LegendItemDrawer::~LegendItemDrawer()
{
}

void LegendItemDrawer::ReadElements(ElementContainer& ec, const char* sSection)
{
	LegendBaseDrawer::ReadElements(ec, sSection);
 	ObjectInfo::ReadElement(sSection, "Columns", ec, iColumns);
 	ObjectInfo::ReadElement(sSection, "Width Factor", ec, rWidthFact);
	if (rWidthFact < 0.1)
		rWidthFact = 1.4;
	ObjectInfo::ReadElement(sSection, "Row Height", ec, iRowHeight);
	fAutoRowHeight = iRowHeight <= 0;
	iRowExtra = 0;
	ObjectInfo::ReadElement(sSection, "Extra Row Height", ec, iRowExtra);
	if (iRowExtra < 0)
		iRowExtra = 0;
}

void LegendItemDrawer::WriteElements(ElementContainer& ec, const char* sSection)
{
	LegendBaseDrawer::WriteElements(ec, sSection);
 	ObjectInfo::WriteElement(sSection, "Columns", ec, iColumns);
 	ObjectInfo::WriteElement(sSection, "Width Factor", ec, rWidthFact);
	if (fAutoRowHeight)
	 	ObjectInfo::WriteElement(sSection, "Row Height", ec, (char*)0);
	else
	 	ObjectInfo::WriteElement(sSection, "Row Height", ec, iRowHeight);
	ObjectInfo::WriteElement(sSection, "Extra Row Height", ec, iRowExtra);
}

void LegendItemDrawer::OnDraw(CDC* cdc)
{
	if (!fValid())
		return;
  int iNr = iItems();
  int iRows = 1 + (iNr - 1) / iColumns;
	if (fAutoRowHeight)
		iRowHeight = lli.iFontSize();
	int iTotalRowHeight = max(lli.iFontSize(), iRowHeight) + iRowExtra;
	int iDiffY2 = (lli.iFontSize() - iRowHeight) / 2;
	CRect rct = lli.rectPos();
  int iColWidth = (rct.Width() - iTotalRowHeight) / iColumns;
	CPoint pt = rct.TopLeft();
  const CPoint pStart = pt;
  int i = 0;
  for (int c = 0; c < iColumns; ++c) {
    for (int r = 0; i < iNr && r < iRows; ++r, ++i) {
      CPoint p = pStart;
      p.x += c * iColWidth;
      p.y += r * iTotalRowHeight;
      DrawItem(cdc, p, i);
      String sText = sItem(i);
			CString str = sText.scVal();
      p.x += rounding((rWidthFact + 0.5) * iRowHeight);
			p.y -= iDiffY2;
      cdc->TextOut(p.x, p.y, str);
    }
  }
}

void LegendItemDrawer::Setup()
{
	if (!fValid())
		return;

	CWindowDC cdc(CWnd::GetDesktopWindow());
	TextLayoutItem::InitFont init(&lli, &cdc);

	if (fAutoRowHeight)
		iRowHeight = lli.iFontSize();
	int iTotalRowHeight = max(lli.iFontSize(), iRowHeight) + iRowExtra;
	int iColWidth = 0;
  int iNr = iItems();
	for (int i = 0; i < iNr; ++i) 
	{
		String sText = sItem(i);
		CString str = sText.scVal();
		CSize sz = cdc.GetTextExtent(str);
		if (sz.cx > iColWidth)
			iColWidth = sz.cx;
	}
	iColWidth += rounding((rWidthFact + 0.5) * iTotalRowHeight);
	int iTotWidth = iColWidth * iColumns + iRowHeight;
  int iRows = 1 + (iNr - 1) / iColumns;
	int iTotHeight = iTotalRowHeight * iRows;

	MinMax mm = lli.mmPosition();
	mm.MaxCol() = mm.MinCol() + iTotWidth;
	mm.MaxRow() = mm.MinRow() + iTotHeight;
	lli.SetPosition(mm);
}


