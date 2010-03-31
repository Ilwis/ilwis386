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
// Drawer.cpp: implementation of the Drawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\syscolor.h"
#include "Engine\Table\Rec.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(Drawer, BaseDrawer)
	//{{AFX_MSG_MAP(Drawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Drawer::Drawer(MapCompositionDoc* mapcd)
: mcd(mapcd)
{
  fAct = fNew = true;
  fSelectPossible = fSelectable = false;
  fDelete = false;
  fDeletePossible = true;
	fScaleLimits = false;
	riScaleLimits = RangeInt(100,1e6);
	iTranquilizer = 0;
	riTranquilizer = RangeInt(0,1);

  fTransparency = false;
  iTransparence = 50;
}

Drawer::Drawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: mcd(mapcd)
{
  fNew = false;
  bool f = true;
  view->ReadElement(sSection, "Show", f);
  fAct = f;
  fSelectPossible = false;
  fSelectable = true;
  view->ReadElement(sSection, "Info", fSelectable);
  fDelete = false;
  fDeletePossible = true;
	fScaleLimits = false;
  view->ReadElement(sSection, "Use Scale Limits", fScaleLimits);
	riScaleLimits = RangeInt(100,1e6);
  view->ReadElement(sSection, "Scale Limits", riScaleLimits);
	iTranquilizer = 0;
	riTranquilizer = RangeInt(0,1);
  fTransparency = view->ReadElement(sSection, "Transparency", iTransparence) > 0;
}

Drawer::~Drawer()
{
// empty
}

void Drawer::WriteLayer(MapView& view, const char* sSection)
{
  bool f = fAct;
  view->WriteElement(sSection, "Show", f);
  view->WriteElement(sSection, "Info", fSelectable);
  view->WriteElement(sSection, "Use Scale Limits", fScaleLimits);
  view->WriteElement(sSection, "Scale Limits", riScaleLimits);
  if (fTransparency)
    view->WriteElement(sSection, "Transparency", iTransparence);
  else
    view->WriteElement(sSection, "Transparency", (char*)0);
}

int Drawer::Configure(bool)
{
  String sTitle;
  sTitle  = sName();
  sTitle &= SDCErrError;
  MessageBox(0, SDCErrNoCnfFrm.sVal(), sTitle.sVal(),
	       MB_OK | MB_ICONEXCLAMATION);
  return 0;
}

int Drawer::Setup()
{
  return 0;
}

String Drawer::sName()
{
  return "-";
}

String Drawer::sInfo(const Coord&)
{
  return String("");
}

void Drawer::Edit()
{
	MapPaneView* mpv = mcd->mpvGetView();
	if (mpv)
		mpv->EditNamedLayer(obj()->fnObj);
}

void Drawer::Prop()
{
	if (!fProperty())
		return;
	IlwWinApp()->Execute(String("prop %S", obj()->fnObj.sFullNameQuoted()));
}


Ilwis::Record Drawer::rec(const Coord&)
{
  return Ilwis::Record();
}


FormBaseDialog* Drawer::wEditRpr(CWnd* wPar, int iRaw)
{
  return 0;
}

FormBaseDialog* Drawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
  return 0;
}

/*
long Tokenizer::iVal()
{
  Token tok = tokGet();
  if (tok == "-")
    return -iVal();
  else
    return (long)tok.sVal();
}

double Tokenizer::rVal()
{
  Token tok = tokGet();
  if (tok == "-")
    return -rVal();
  else
    return (double)tok.sVal();
}
*/

/*
Drawer* drDrawer(MapPane* pane, MapView& view, const char* sSection) 
{
  String sType;
  view->ReadElement(sSection, "Type", sType);
  if ("MapDrawer" == sType)
    return new MapDrawer(pane, view, sSection);
  else if ("MapListDrawer" == sType)
    return new MapListDrawer(pane, view, sSection);
  else if ("MapListColorCompDrawer" == sType)
    return new MapListColorCompDrawer(pane, view, sSection);
  else if ("SegmentMapDrawer" == sType)
    return new SegmentMapDrawer(pane, view, sSection);
  else if ("PolygonMapDrawer" == sType)
    return new PolygonMapDrawer(pane, view, sSection);
  else if ("GridDrawer" == sType)
    return new GridDrawer(pane, view, sSection);
  else if ("Grid3DDrawer" == sType)
    return new Grid3DDrawer(pane, view, sSection);
  else if ("PointDrawer" == sType)
    return new PointDrawer(pane, view, sSection);
  else if ("AnnotationTextDrawer" == sType)
    return new AnnotationTextDrawer(pane, view, sSection);
  else if ("MetafileDrawer" == sType)
    return new MetafileDrawer(pane, view, sSection);
  else if ("BitmapDrawer" == sType)
    return new BitmapDrawer(pane, view, sSection);
  else if ("ScaleBarDrawer" == sType)
    return new ScaleBarDrawer(pane, view, sSection);
  else if ("NorthDrawer" == sType)
    return new NorthDrawer(pane, view, sSection);
  else if ("TitleDrawer" == sType)
    return new TitleDrawer(pane, view, sSection);
  else if ("LegendDrawer" == sType)
//  return new LegendDrawer(pane, view, sSection);
    return LegendBaseDrawer::drLegend(pane, view, sSection);
  else if ("BoxDrawer" == sType)
    return new BoxDrawer(pane, view, sSection);
  else if ("GraticuleDrawer"==sType )
    return new GraticuleDrawer(pane, view, sSection);
  return 0;
}
*/

bool Drawer::fAttributes()
{
  return false;
}

bool Drawer::fRepresentation()
{
  return false;
}

bool Drawer::fEditable()
{
  return false;
}

bool Drawer::fProperty()
{
  return false;
}

bool Drawer::fEditDomain()
{
  return false;
}

void Drawer::EditDomain()
{
  MessageBox(0, SDCErrImpossibleDomAction.sVal(), SDCErrDrawerError.sVal(), 
	       MB_OK | MB_ICONEXCLAMATION);
}

void Drawer::EditRepresentation()
{
  MessageBox(0, SDCErrImpossibleRprAction.sVal(), SDCErrDrawerError.sVal(), 
	       MB_OK | MB_ICONEXCLAMATION);
}

void Drawer::ExecuteAction(const Coord&)
{
}

#define STEP 25

void Drawer::CalcBounds(const GeoRef& grf, const MinMax& mm, LatLon& llMin, LatLon& llMax)
{
  // GeoRef3D is very slow in RowCol2Coord so use boundaries of dtm instead.
  ///GeoRef3D* g3d = mppn->georef->pg3d();
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
  llMin = LatLon(100,200);
  llMax = LatLon(-100,-200);
  long dR = mmGr.height() / STEP;
  long dC = mmGr.width() / STEP;
	if (dR <= 0 || dC <= 0)
		return;
  CoordSystem cs = gr->cs();
  RowCol rc;
  int i, j;				 
  for (i = 0, rc.Row = mmGr.MinRow(); i <= STEP; rc.Row += dR, ++i) {
    for (j = 0, rc.Col = mmGr.MinCol(); j <= STEP; rc.Col += dC, ++j) {
      Coord crd = gr->cConv(rc);
      if (crd.fUndef())
        continue;
      LatLon ll = cs->llConv(crd);
      if (ll.fUndef())
        continue;
      if (ll.Lat < llMin.Lat)
        llMin.Lat = ll.Lat;
      if (ll.Lon < llMin.Lon)
        llMin.Lon = ll.Lon;
      if (ll.Lat > llMax.Lat)
        llMax.Lat = ll.Lat;
      if (ll.Lon > llMax.Lon)
        llMax.Lon = ll.Lon;
    }  
  }  
}

void Drawer::CalcBounds(const CoordSystem& cs, const LatLon& llMin, const LatLon& llMax, CoordBounds& cb)
{
  cb = CoordBounds();
  long dLat = (long)((llMax.Lat - llMin.Lat) / STEP);
  long dLon = (long)((llMax.Lon - llMin.Lon) / STEP);
	if (dLat <= 0 || dLon <= 0)
		return;
  LatLon ll;
  int i, j;
  for (i = 0, ll = llMin; i <= STEP; ll.Lat += dLat, ++i) {
    for (j = 0, ll.Lon = llMin.Lon; j <= STEP; ll.Lon += dLon, ++j) {
      Coord crd = cs->cConv(ll);
      if (crd.fUndef())
        continue;
      cb += crd;  
    }
  }  
}

IlwisObject Drawer::obj() const
{
  return IlwisObject::objInvalid();
}

bool Drawer::fDependsOn(const Drawer*) const
{
  return false;
}

/*
-- Wim 21/7/99: Nice idea, but does not work
CBitmap* Drawer::bmMenu(CDC* cdc)
{
	if (0 == (HBITMAP)bm) {
		bm.CreateBitmap(16,16,1,4,0);
		//bm.CreateCompatibleBitmap(cdc,16,16);
		CDC cdcBm;
		cdcBm.CreateCompatibleDC(cdc);
		CBitmap* bmOld= cdcBm.SelectObject(&bm);
		Color cMenu = SysColor(COLOR_MENU);
		CBrush br(cMenu);
		CPen pen(PS_SOLID,1,cMenu);
		CBrush* brOld = cdcBm.SelectObject(&br);
		CPen* penOld = cdcBm.SelectObject(&pen);
		cdcBm.Rectangle(0,0,16,16);
		cdcBm.DrawIcon(0,0,icon());
		cdcBm.SelectObject(bmOld);
		cdcBm.SelectObject(brOld);
		cdcBm.SelectObject(penOld);
	}
	return &bm;
}
*/


DrawerForm::DrawerForm(Drawer* dr, const String& sTitle)
	: FormWithDest(dr->mcd->wndGetActiveView(), sTitle)
	, drw(dr)
{
	fbs |= fbsTOPMOST;
}

void DrawerForm::AskScaleLimits()
{
	CheckBox* cbScale = new CheckBox(root, SDCUiScaleLimits, &drw->fScaleLimits);
	new FieldRangeInt(cbScale, "", &drw->riScaleLimits, ValueRange(1,1e9));
  CheckBox* cbTransparence = new CheckBox(root, SDCUiTransparent, &drw->fTransparency);
	cbTransparence->Align(cbScale, AL_UNDER);
  new FieldInt(cbTransparence, "", &drw->iTransparence, RangeInt(1,99), true);
	FieldBlank* fb = new FieldBlank(root, 0);
	fb->Align(cbTransparence, AL_UNDER);
}

bool Drawer::fLegendUseful() const
{
	return false;
}

void Drawer::DrawLegendRect(CDC*, CRect, int iRaw) const
{
}

void Drawer::DrawValueLegendRect(CDC*, CRect, double rValue) const 	
{
}


