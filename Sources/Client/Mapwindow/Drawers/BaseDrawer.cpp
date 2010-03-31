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
// BaseDrawer.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Engine\SpatialReference\GR3D.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(BaseDrawer, CCmdTarget)
	//{{AFX_MSG_MAP(BaseDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BaseDrawer::BaseDrawer()
{
}

BaseDrawer::~BaseDrawer()
{
}

IlwisObject BaseDrawer::obj() const
{
  return IlwisObject::objInvalid();
}

Domain BaseDrawer::dm() const
{
  return Domain();
}

Representation BaseDrawer::rpr() const
{
  return Representation();
}

DomainValueRangeStruct BaseDrawer::dvrs() const
{
  return DomainValueRangeStruct();
}

RangeReal BaseDrawer::rrStretchRange() const
{
  return RangeReal();
}

Color BaseDrawer::clrRaw(long) const
{
  return Color();
}

Color BaseDrawer::clrVal(double) const
{
  return Color();
}

Color BaseDrawer::clrPrimary(int iNr)
{
  switch (iNr%32) {
    case  0: return Color(  0,  0,  0);
    case  1: return Color(255,  0,  0);
    case  2: return Color(255,255,  0);
    case  3: return Color(  0,  0,255);
    case  4: return Color(255,  0,255);
    case  5: return Color(  0,255,255);
    case  6: return Color(  0,255,  0);
    case  7: return Color(128,128,128);
    case  8: return Color(224,224,224);  // was white 255,255,255
    case  9: return Color(128,  0,  0);
    case 10: return Color(128,128,  0);
    case 11: return Color(  0,  0,128);
    case 12: return Color(128,  0,128);
    case 13: return Color(  0,128,128);
    case 14: return Color(  0,128,  0);
    case 15: return Color(255,128,  0);
    case 16: return Color(191,  0,  0);
    case 17: return Color(191,191,  0);
    case 18: return Color(  0,  0,191);
    case 19: return Color(191,  0,191);
    case 20: return Color(  0,191,191);
    case 21: return Color(  0,191,  0);
    case 22: return Color(191,191,191);
    case 23: return Color(192,220,192);
    case 24: return Color( 63,  0,  0);
    case 25: return Color( 63, 63,  0);
    case 26: return Color(  0,  0, 63);
    case 27: return Color( 63,  0, 63);
    case 28: return Color(  0, 63, 63);
    case 29: return Color(  0, 63,  0);
    case 30: return Color( 63, 63, 63);
    case 31: return Color(127, 63,  0);
  }  
  return Color();
}

zIcon BaseDrawer::icon() const
{
	return zIcon("LogoIcon");
}

String BaseDrawer::sTitle() const
{
	IlwisObject ob = obj();
	if (ob.fValid())
		return ob->sTypeName();
	else
		return "";
}

CoordBounds BaseDrawer::cbRect(Positioner* psn) const
{
  // GeoRef3D is very slow in RowCol2Coord so use boundaries of dtm instead.
  GeoRef3D* g3d = psn->georef()->pg3d();
  if (0 != g3d)
    return g3d->mapDTM->cb();
  CoordBounds cb;
  MinMax mm = psn->mmSize();
  cb += psn->georef()->cConv(mm.rcMin);
  cb += psn->georef()->cConv(mm.rcMax);
  long dRow = mm.height() / 10;
  long dCol = mm.width() / 10;
//  long iRow, iCol;
  int r, c;
  RowCol rc;
  for (rc.Row = mm.rcMin.Row, r = 0; r < 10; rc.Row += dRow, ++r)
    for (rc.Col = mm.rcMin.Col, c = 0; c < 10; rc.Col += dCol, ++c)
      cb += psn->georef()->cConv(rc);
  return cb;
}

