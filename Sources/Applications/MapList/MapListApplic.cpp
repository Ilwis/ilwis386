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
// MapListApplic.cpp: implementation of the MapListApplic class.
//
//////////////////////////////////////////////////////////////////////

#include "Applications\MapList\MapListApplic.h"
#include "Headers\Hs\maplist.hs"

namespace {
  const char* sSyntax() 
  {
    return "MapListApplic(maplist,mapapplicationstring)";
  }
}

MapListApplic::MapListApplic(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
  ReadElement("MapListApplic", "MapList", ml);
  ReadElement("MapListApplic", "ApplicationExpression", sApplicExpr);
  Init();
}

IlwisObjectPtr * createMapListApplic(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)new MapListApplic(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListApplic(fn, (MapListPtr &)ptr);
}

MapListApplic::MapListApplic(const FileName& fn, MapListPtr& ptr, const String& sExpr)
: MapListVirtual(fn, ptr, true)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());  
  ml = MapList(FileName(as[0]));
  if (!ml.fValid())
    throw ErrorObject(WhatError("MapList expected",0), WhereError(as[0]));
  sApplicExpr = as[1];  
  Init();
}

MapListApplic::~MapListApplic()
{
}

void MapListApplic::Store()
{
  MapListVirtual::Store();
  WriteElement("MapListVirtual", "Type", "MapListApplic");
  WriteElement("MapListApplic", "MapList", ml);
  WriteElement("MapListApplic", "ApplicationExpression", sApplicExpr);
}

void MapListApplic::Init()
{
  sFreezeTitle = "MapListApplic";
}

String MapListApplic::sExpression() const
{
  return String("MapListApplic(%S,%S)", ml->sName(), sApplicExpr);
}

bool MapListApplic::fFreezing()
{ 
  try {
    SetSize(0);
    trq.SetText(TR("Constructing"));
    int iMaps = ml->iSize();
    SetSize(iMaps);

    for (int i = 0; i < iMaps; ++i)
    {
      if (trq.fUpdate(i, iMaps)) 
      {
        SetSize(0);
        return false;
      }
      String sExpr = sApplicExpr;
      String sMap = ml->map(i+ml->iLower())->sName();

      for (;;)
      {
        int iPos = sExpr.find("##");
        if (iPos == String::npos)
          break;
        String sLeft = sExpr.sLeft(iPos);
        String sRight = &sExpr[iPos+2];
        sExpr = String("%S%S%S", sLeft, sMap, sRight);
      }

      String sMapName("%S_%0*i", fnObj.sFile, (int)floor(1 + log10((double)iMaps)), i+1);
      FileName fnMap(sMapName);
      map(i) = Map(fnMap,sExpr);
      if (dvrsMaps.dm().fValid())
        map(i)->SetDomainValueRangeStruct(dvrsMaps);
    }
    if (trq.fUpdate(iMaps, iMaps)) 
    {
      SetSize(0);
      return false;
    }
	if (iMaps > 0)
		ptr.SetGeoRef(map(0)->gr());
    trq.SetText(TR("Calculating Map"));
    for (int i = 0; i < iMaps; ++i) 
    {
      if (trq.fUpdate(i, iMaps)) 
        return false;
      map(i)->Calc();
      if (!map(i)->fCalculated())
        return false;
    }
  }
  catch (ErrorObject& err)
  {
    err.Show();
    SetSize(0);
    return false;
  }
  return true;
}
