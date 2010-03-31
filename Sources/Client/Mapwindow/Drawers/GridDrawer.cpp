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
// GridDrawer.cpp: implementation of the GridDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\GridDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\FormElements\fldfontn.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcolor.h"
#include "Headers\Hs\Drwforms.hs"
#include "Engine\Base\Round.h"
#include "Engine\SpatialReference\GR3D.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int iSTEP = 25;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GridDrawer::GridDrawer(MapCompositionDoc* mapcd)
: Drawer(mapcd)
{
  Coord cMin, cMax;
  CalcBounds(mcd->georef, mcd->mmBounds(), cMin, cMax);
	rDist = rRound((cMax.x - cMin.x) / 7);
	col = SysColor(COLOR_WINDOWTEXT);
  ldt = ldtSingle;
}

GridDrawer::GridDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: Drawer(mapcd, view, sSection)
{
  rDist = view->rReadElement(sSection, "Distance");
  col = view->iReadElement(sSection, "Color");
  String s = "Single";
  view->ReadElement(sSection, "Line Style", s);
  ldt = Line::ldtConv(s);
}

GridDrawer::~GridDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

void GridDrawer::WriteLayer(MapView& view, const char* sSection)
{
  Drawer::WriteLayer(view, sSection);
  view->WriteElement(sSection, "Type", "GridDrawer");
  view->WriteElement(sSection, "Distance", rDist);
  view->WriteElement(sSection, "Color", (long)col);
  view->WriteElement(sSection, "Line Style", Line::sConv(ldt));
}


int GridDrawer::draw(CDC* cdc, zRect, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct) return 0;
  zPoint p1,p2;
  String sVal;
  Coord c, cMin, cMax;

//  MinMax MB = mcd->mmBounds();
  MinMax mm = psn->mmSize();
//  zRect re(psn->pntPos(MB.rcMin), psn->pntPos(MB.rcMax));
      
  Color clr;
  if ((long)col == -1)
      clr = SysColor(COLOR_WINDOWTEXT);
  else
      clr = col;
	CPen pen(Line::psConv(ldt),1,clr);
	CPen* penOld = cdc->SelectObject(&pen);
  CalcBounds(mcd->georef, mm, cMin, cMax);
  bool fLinear = mcd->georef->fLinear();

	cdc->SetBkMode(TRANSPARENT);

	int iCntX = (long)((cMax.x - cMin.x) / rDist);
	int iCntY = (long)((cMax.y - cMin.y) / rDist);
	riTranquilizer = RangeInt(1,iCntX+iCntY);
	iTranquilizer = 0;

	Coord c1, c2;
	c1.y = cMin.y;
	c2.y = cMax.y;
  for (double x = ceil(cMin.x / rDist) * rDist; x < cMax.x ; x += rDist)
  {
		++iTranquilizer;
		if (*fDrawStop)
			break;
		c1.x = c2.x = x;
		if (fLinear)
			DrawLinearLine(cdc, psn, c1, c2);
		else
			DrawCurvedLine(cdc, psn, c1, c2);
	}

	c1.x = cMin.x;
	c2.x = cMax.x;
  for (double y = ceil(cMin.y / rDist) * rDist; y < cMax.y ; y += rDist)
  {
		++iTranquilizer;
		if (*fDrawStop)
			break;
		c1.y = c2.y = y;
		if (fLinear)
			DrawLinearLine(cdc, psn, c1, c2);
		else
			DrawCurvedLine(cdc, psn, c1, c2);
	}
	cdc->SelectObject(penOld);
  return 1;
}

String GridDrawer::sName()
{
  return SDCNameGrid;
}


class GridForm: public DrawerForm
{
public:
  GridForm(GridDrawer *gdr) 
	: DrawerForm(gdr, SDCTitleGrid)
	{
		iImg = IlwWinApp()->iImage(".grid");

	  new FieldReal(root, SDCUiGridDistance, &gdr->rDist, ValueRange(0.0,1e10,0.001));
	  new FieldLineType(root, SDCUiLineType, &gdr->ldt);
	  new FieldColor(root, SDCUiColor, &gdr->col);

		AskScaleLimits();

	  SetMenHelpTopic(htpCnfGrid);
		create();
	}
private:
  GridDrawer *gd;
};

int GridDrawer::Configure(bool fShow)
{
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
	GridForm frm(this);
  bool fOk = frm.fOkClicked();
	if (fOk)
		fAct = true;
  Setup();
	return fOk;
}

void GridDrawer::DrawLinearLine(CDC* cdc, Positioner* psn, Coord c1, Coord c2)
{
	zPoint p1, p2;
	p1 = psn->pntPos(c1);
	p2 = psn->pntPos(c2);
	cdc->MoveTo(p1);
	cdc->LineTo(p2);
}

void GridDrawer::DrawCurvedLine(CDC* cdc, Positioner* psn, Coord c1, Coord c2)
{
	Coord cStep, crd;
	CRect rect = psn->rectSize();
	int iSteps = max(rect.Width(), rect.Height());
	cStep.x = (c2.x - c1.x) / iSteps;
	cStep.y = (c2.y - c1.y) / iSteps;
	zPoint p;
	bool fPointOk;
	p = psn->pntPos(c1);
  if (p.x == shUNDEF || p.y == shUNDEF)
		fPointOk = false;
	else {
		fPointOk = true;
		cdc->MoveTo(p);
	}
	crd = c1;
	for (int i = 0; i < iSteps; ++i) {
		crd += cStep;
		p = psn->pntPos(crd);
	  if (p.x == shUNDEF || p.y == shUNDEF)
			fPointOk = false;
		else if (fPointOk) 
			cdc->LineTo(p);
		else {
			fPointOk = true;
			cdc->MoveTo(p);
		}
	}
}

void GridDrawer::CalcBounds(const GeoRef& grf, const MinMax& mm, Coord& cMin, Coord& cMax)
{
  GeoRef gr;
  MinMax mmGr;
  if (0 != grf->pg3d()) {
    gr = grf->pg3d()->mapDTM->gr();
    mmGr = MinMax(RowCol(0.0,0.0), gr->rcSize());
  }
  else {
    gr = grf;
    mmGr = mm;
  }
  long dR = mmGr.height() / iSTEP;
  long dC = mmGr.width() / iSTEP;
  CoordSystem cs = gr->cs();
  double rRow, rCol;
  Coord c;

  CoordBounds cb;
  rCol = mmGr.MinCol();
  rRow = mmGr.MinRow();
  gr->RowCol2Coord(rRow, rCol, c);
  cb += c;
  rCol = mmGr.MaxCol();
  rRow = mmGr.MaxRow();
  gr->RowCol2Coord(rRow, rCol, c);
  cb += c;
  
	if (dR < 1) 
		dR = 1;
	if (dC < 1)
		dC = 1;

	for (rRow = mmGr.MinRow();; rRow += dR) 
	{
		bool fBreak = false;
		if (rRow > mmGr.MaxRow()) {
			rRow = mmGr.MaxRow();
			fBreak = true;
		}
		for (rCol = mmGr.MinCol();; rCol += dC)
		{
			bool fBreak = false;
			if (rCol > mmGr.MaxCol()) {
				rCol = mmGr.MaxCol();
				fBreak = true;
			}
			gr->RowCol2Coord(rRow, rCol, c);
			cb += c;
			if (fBreak)
				break;
		}          
		if (fBreak)
			break;
	}    
  cMin = cb.cMin;
  cMax = cb.cMax;
}

int GridDrawer::Setup()
{
  if (rDist <= 0)
    rDist = 1.0;
  return 0;
}
