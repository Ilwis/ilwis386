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
// GraticuleDrawer.cpp: implementation of the GraticuleDrawer class.
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
#include "Client\Mapwindow\Drawers\GraticuleDrawer.h"
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
#include "Client\FormElements\fentdms.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int iSTEP = 25;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GraticuleDrawer::GraticuleDrawer(MapCompositionDoc* mapcd)
: Drawer(mapcd)
{
	LatLon llMin, llMax;
	CalcBounds(mcd->georef, mcd->mmBounds(), llMin, llMax);
  rDist = (llMax.Lat - llMin.Lat) / 7;
  if (rDist > 30)
    rDist = 30;
  else if (rDist > 0.9)
    rDist = rRound(rDist);
  else {
    rDist *= 60;  // work in minutes
    if (rDist > 0.25)   // if larger than 15 seconds
      rDist = rRound(rDist);
    else {
      rDist *= 60;  // work in seconds
      if (rDist > 0.2)
        rDist = rRound(rDist);
      else
        rDist = 1;
      rDist /= 60;
    }  
    rDist /= 60;
  }  
	col = SysColor(COLOR_WINDOWTEXT);
  ldt = ldtSingle;
}

GraticuleDrawer::GraticuleDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: Drawer(mapcd, view, sSection)
{
  rDist = view->rReadElement(sSection, "Distance");
  col = view->iReadElement(sSection, "Color");
  String s = "Single";
  view->ReadElement(sSection, "Line Style", s);
  ldt = Line::ldtConv(s);
}

GraticuleDrawer::~GraticuleDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

void GraticuleDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "GraticuleDrawer");
  Drawer::WriteLayer(view, sSection);
	String sInterVal("%0.12f", rDist);	
  view->WriteElement(sSection, "Distance", sInterVal);
  view->WriteElement(sSection, "Color", (long)col);
  view->WriteElement(sSection, "Line Style", Line::sConv(ldt));
}

int GraticuleDrawer::draw(CDC* cdc, zRect, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (!fAct) return 0;
	CoordSystem cs = psn->georef()->cs();
  zPoint p1,p2;
  String sVal;
  LatLon ll, llMin, llMax;

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
  CalcBounds(mcd->georef, mm, llMin, llMax);
	llMin.Lat -= rDist;
	llMin.Lon -= rDist;
	llMax.Lat += rDist;
	llMax.Lon += rDist;
	cdc->SetBkMode(TRANSPARENT);

	int iCntLat = (int)((llMax.Lat - llMin.Lat) / rDist);
	int iCntLon = (int)((llMax.Lon - llMin.Lon) / rDist);
	riTranquilizer = RangeInt(1,iCntLat+iCntLon);
	iTranquilizer = 0;

	LatLon ll1, ll2;
	Coord c1, c2;
	ll1.Lat = llMin.Lat; // southernmost points on meridians
	ll2.Lat = llMax.Lat; // northernmost points on meridians
  for (double x = ceil(llMin.Lon / rDist) * rDist; x < llMax.Lon; x += rDist)
  {
		++iTranquilizer;
		if (*fDrawStop)
			break;
		ll1.Lon = ll2.Lon = x; //fixed longitude
		if (!(ll1.fUndef() || ll2.fUndef()))
			DrawMeridian(cdc, psn, ll1, ll2);
	}

	ll1.Lon = llMin.Lon;
	ll2.Lon = llMax.Lon;
  for (double y = ceil(llMin.Lat / rDist) * rDist; y < llMax.Lat; y += rDist)
  {
		++iTranquilizer;
		if (*fDrawStop)
			break;
		ll1.Lat = ll2.Lat = y;//fixed latitude
		if (!(ll1.fUndef() || ll2.fUndef()))
			DrawParallel(cdc, psn, ll1, ll2);
	}
	cdc->SelectObject(penOld);
  return 1;
}

String GraticuleDrawer::sName()
{
  return SDCNameGraticule;
}


class GraticuleForm: public DrawerForm
{
public:
  GraticuleForm(GraticuleDrawer *gdr) 
	: DrawerForm(gdr, SDCTitleGraticule)
	{
		iImg = IlwWinApp()->iImage(".grat");

		new FieldDMS(root, SDCUiGraticuleDistance, &gdr->rDist);
	  new FieldLineType(root, SDCUiLineType, &gdr->ldt);
		new FieldColor(root, SDCUiColor, &gdr->col);

		AskScaleLimits();

	  SetMenHelpTopic(htpCnfGraticule);
		create();
	}
private:
  GraticuleDrawer *gd;
};

int GraticuleDrawer::Configure(bool fShow)
{
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
	GraticuleForm frm(this);
  bool fOk = frm.fOkClicked();
	if (fOk)
		fAct = true;
  Setup();
	return fOk;
}


void GraticuleDrawer::DrawMeridian(CDC* cdc, Positioner* psn, LatLon ll1, LatLon ll2)
{
	CoordSystem cs = psn->georef()->cs();
	Coord crd;
	LatLon llLatStep, llLatNext;
	CRect rect = psn->rectSize();
	int iSteps = max(rect.Width(), rect.Height());
	llLatStep.Lon = 0;
	llLatNext.Lon = ll1.Lon;
	llLatStep.Lat = (ll2.Lat - ll1.Lat) / iSteps;
	zPoint p(shUNDEF, shUNDEF);
	zPoint pOld;				// used to detect graticule interruptions
	int iDx, iDy, iDold = 0; // successive x,y moves of pen
	bool fPointOk= true;
	bool fLineRestarts = false; //true after each interruption
	crd = cs->cConv(ll1);
	if (crd != crdUNDEF) 
		p = psn->pntPos(crd);
	if (p.x == shUNDEF || p.y == shUNDEF)
		fPointOk = false;
	else {
		fPointOk = true;
		cdc->MoveTo(p);
		pOld = p;
	}
	llLatNext = ll1;
	for (int i = 0; i < iSteps; ++i) {
		llLatNext += llLatStep;
		crd = cs->cConv(llLatNext);
		if (crd == crdUNDEF) continue;

		pOld = p;
		p = psn->pntPos(crd);
		fPointOk = !(p.x == shUNDEF || p.y == shUNDEF);
		if (!fPointOk)
			continue;

		iDx = p.x - pOld.x;
		iDy = p.y - pOld.y;
		int iD = abs(iDx) + abs(iDy);
		if (iD > 10 * iDold + 2)
			fLineRestarts = true;
		else
			fLineRestarts = false;
		iDold = iD; 
		if (fLineRestarts) 
			cdc->MoveTo(p);
		else 
			cdc->LineTo(p);
	}
}
 
void GraticuleDrawer::DrawParallel(CDC* cdc, Positioner* psn, LatLon ll1, LatLon ll2)
{
	CoordSystem cs = psn->georef()->cs();
	Coord crd;
	LatLon llLonStep, llLonNext;
	CRect rect = psn->rectSize();
	int iSteps = max(rect.Width(), rect.Height());
	llLonStep.Lat = 0;
	llLonNext.Lat = ll1.Lat; 
	llLonStep.Lon = (ll2.Lon - ll1.Lon) / iSteps;
	zPoint p(shUNDEF, shUNDEF);
	zPoint pOld;				// used to detect graticule interruptions
	int iDx, iDy, iDold = 0; // successive x,y moves of pen
	bool fPointOk = true;
	bool fLineRestarts = false; //true after each interruption
	crd = cs->cConv(ll1);
	if (crd != crdUNDEF) 
		p = psn->pntPos(crd);
	if (p.x == shUNDEF || p.y == shUNDEF)
		fPointOk = false;
	else {
		fPointOk = true;
		cdc->MoveTo(p);
		pOld = p;
	}
	llLonNext = ll1;
	for (int i = 0; i < iSteps; ++i) {
		llLonNext += llLonStep;
		crd = cs->cConv(llLonNext);
		if (crd == crdUNDEF) continue;

		pOld = p;
		p = psn->pntPos(crd);
		fPointOk = !(p.x == shUNDEF || p.y == shUNDEF);
		if (!fPointOk)
			continue;

		iDx = p.x - pOld.x;
		iDy = p.y - pOld.y;
		int iD = abs(iDx) + abs(iDy);
		if (iD > 10 * iDold + 2)
			fLineRestarts = true;
		else
			fLineRestarts = false;
		iDold = iD; 
		if (fLineRestarts)
			cdc->MoveTo(p);
		else 
			cdc->LineTo(p);
	}
}

int GraticuleDrawer::Setup()
{
  if (rDist <= 0)
    rDist = 1.0 / 3600;
  return 0;
}
