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
/* MapCross
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
	Last change:  JEL   9 May 97    3:24 pm
*/
#define MAPCROSS_C
#include "Applications\Raster\MAPCROSS.H"
#include "Engine\Table\Col.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapCross(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCross::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCross(fn, (MapPtr &)ptr);
}

const char* MapCross::sSyntax() {
  return "MapCross(map1,map2,crosstable)";
}


MapCross* MapCross::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(3);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map map1(as[0], fn.sPath());
  Map map2(as[1], fn.sPath());
  bool fIncompGeoRef = false;
  if (map1->gr()->fGeoRefNone() && map2->gr()->fGeoRefNone())
    fIncompGeoRef = map1->rcSize() != map2->rcSize();
  else
    fIncompGeoRef = map1->gr() != map2->gr();
  if (fIncompGeoRef)
    throw ErrorIncompatibleGeorefs(map1->gr()->sName(true, fn.sPath()),
                          map2->gr()->sName(true, fn.sPath()), fn, errMapCross);
  return new MapCross(fn, p, map1, map2, as[2]);
}

MapCross::MapCross(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapCross", "Map2", map2);
  ReadElement("MapCross", "CrossTable", sCrossTbl);
  fNeedFreeze = true;
  sFreezeTitle = "MapCross";
  htpFreeze = htpMapCrossT;
  objdep.Add(map2.ptr());
}

MapCross::MapCross(const FileName& fn, MapPtr& p, const Map& map1, const Map& _map2, const String& sCrossTable)
: MapFromMap(fn, p, map1), map2(_map2), sCrossTbl(sCrossTable)
{
/*  if (mp->dm()->pdsrt() == 0)
    throw ErrorClassOrIDDomain(mp->dm()->sName(true, fnObj.sPath()), fnObj);
  if (mp->gr() != map2->gr())
    throw ErrorSameGeoref(mp->gr()->sName(true, fnObj.sPath()),
                          map2->gr()->sName(true, fnObj.sPath()), fnObj);*/
  fNeedFreeze = true;
  objdep.Add(map2.ptr());
  Table tblCross;
  try {                        
    tblCross = Table(sCrossTbl);
    SetDomainValueRangeStruct(tblCross->dm()); 
  }
  catch (const ErrorObject&) {
    Store();
    ptr.Store(); // TableCross checks for existence of MapCross
    FileName fnTblCross(sCrossTbl, fnObj);
    fnTblCross.sExt = ".tbt";
    tblCross = Table(fnTblCross, String("TableCross(%S,%S,%S)", mp->sName(true, fnObj.sPath()), map2->sName(true, fnObj.sPath()), 
                 sName(true, fnObj.sPath())));
    SetDomainValueRangeStruct(tblCross->dm()); 
    SetAttributeTable(tblCross);
    ptr.Store(); // store domain info
  }
  sFreezeTitle = "MapCross";
  htpFreeze = htpMapCrossT;
/*  Prevent cyclic definition -> stack overflow !!
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  FileName fnTblCross(sCrossTbl, fnObj);
  fnTblCross.sExt = ".tbt";
  //Table tblCross(fnTblCross, String("TableCross(%S,%S,%S)", mp->sName(true, fnObj.sPath()), map2->sName(true, fnObj.sPath()), 
  //               sName(true, fnObj.sPath())));
*/                 
}

void MapCross::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapCross");
  WriteElement("MapCross", "Map2", map2->sNameQuoted(true, fnObj.sPath()));
  WriteElement("MapCross", "CrossTable", sCrossTbl);
}

MapCross::~MapCross()
{
}

bool MapCross::fFreezing()
{
  FileName fnTblCross(sCrossTbl, fnObj);
  fnTblCross.sExt = ".tbt";
  Table tblCross(fnTblCross);
  tblCross->DeleteCalc();
  tblCross->Calc();
  return tblCross->fCalculated();
}

String MapCross::sExpression() const
{
  return String("MapCross(%S,%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), map2->sNameQuoted(true, fnObj.sPath()), 
                sCrossTbl);
}

bool MapCross::fDomainChangeable() const
{
  return false;
}

bool MapCross::fValueRangeChangeable() const
{
  return false;
}




