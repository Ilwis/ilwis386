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
/*
// $Log: /ILWIS 3.0/PointMap/PNTSUB.cpp $
 * 
 * 4     4/04/00 5:49p Hendrikse
 * added a better  String (more digits up arcseconds) in case of LatLons
 * in the
 * SubMap lower and upper limits
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:22a Wind
 * adpated to use of quoted file names
*/
// Revision 1.3  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.2  1997/08/05 10:38:21  Wim
// sSyntax() corrected
//
/* PointMapSubMap
   Copyright Ilwis System Development ITC
   june 1996, by Jelle Wind
	Last change:  WK    5 Aug 97   12:38 pm
*/                                                                      

#include "PointApplications\PNTSUB.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\point.hs"


IlwisObjectPtr * createPointMapSubMap(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapSubMap::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapSubMap(fn, (PointMapPtr &)ptr);
}

const char* PointMapSubMap::sSyntax() {
  return "PointMapSubMap(pntmap,crdminx,crdminy,crdmaxx,crdmaxy)";
}

PointMapSubMap* PointMapSubMap::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as(5);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  CoordBounds cb(Coord(as[1].rVal(), as[2].rVal()), Coord(as[3].rVal(), as[4].rVal()));
  return new PointMapSubMap(fn, p, pmp, cb);
}

PointMapSubMap::PointMapSubMap(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PointMapSubMap", "PointMap", pmp);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmp.ptr());
}

PointMapSubMap::PointMapSubMap(const FileName& fn, PointMapPtr& p, const PointMap& pm, 
                                   const CoordBounds& cb)
: PointMapVirtual(fn, p, pm->cs(),cb,pm->dvrs()), pmp(pm)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (pmp->fTblAttSelf())
    SetAttributeTable(pmp->tblAtt());
}

void PointMapSubMap::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapSubMap");
  WriteElement("PointMapSubMap", "PointMap", pmp);
}

PointMapSubMap::~PointMapSubMap()
{
}

String PointMapSubMap::sExpression() const
{
  if ( 0 != pmp->cs()->pcsLatLon())  
	return String("PointMapSubMap(%S,%.7f,%.7f,%.7f,%.7f)", pmp->sNameQuoted(false, fnObj.sPath()),
                                    cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
  else
	return String("PointMapSubMap(%S,%g,%g,%g,%g)", pmp->sNameQuoted(false, fnObj.sPath()),
                                    cb().MinX(), cb().MinY(), cb().MaxX(), cb().MaxY());
}


void PointMapSubMap::Init()
{
  htpFreeze = "ilwisapp\\submap_of_point_map_algorithm.htm";
  sFreezeTitle = "PointMapSubMap";
}

bool PointMapSubMap::fFreezing()
{
  trq.SetText(String(SPNTTextSelectPoints_S.scVal(), sName(true, fnObj.sPath())));
  for (long i=1; i <= pmp->iFeatures(); ++i ) {
    if (trq.fUpdate(i, pmp->iFeatures()))
      return false;
    Coord crd = pmp->cValue(i);
    if (cb().fContains(crd))
      if (fUseReals())
        pms->iAddVal(crd, pmp->rValue(i));
      else
        pms->iAddRaw(crd, pmp->iRaw(i));
  }  
  trq.fUpdate(pmp->iFeatures(), pmp->iFeatures());
  _iPoints = pms->iPnt();
  return true;
}




