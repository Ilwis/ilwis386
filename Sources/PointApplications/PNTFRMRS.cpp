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
/* PointMapFromRas
   Copyright Ilwis System Development ITC
   nov 1995, by Martin Schouwenburg
	Last change:  JEL   6 May 97    5:54 pm
*/


#include "PointApplications\PNTFRMRS.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"


IlwisObjectPtr * createPointMapFromRas(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapFromRas::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapFromRas(fn, (PointMapPtr &)ptr);
}

const char* PointMapFromRas::sSyntax() {
  return "PointMapFromRas(map)";
}

PointMapFromRas* PointMapFromRas::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  Map map(as[0], fn.sPath());
  return new PointMapFromRas(fn, p, map);
}

PointMapFromRas::PointMapFromRas(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  ReadElement("PointMapFromRas", "RasterMap", map);
  fNeedFreeze = true;
  Init();
  objdep.Add(map.ptr());
}

PointMapFromRas::~PointMapFromRas()
{
}

PointMapFromRas::PointMapFromRas(const FileName& fn, PointMapPtr& p, const Map& mp)
: PointMapVirtual(fn, p, mp->cs(), mp->cb(), mp->dvrs()), map(mp)
{
  if (map->gr()->fnObj.sFile == "none")
    throw ErrorObject(WhatError(TR("Map has no georeference"), errPointMapFromRas),
                                sTypeName());
  if (map->gr()->fGeoRefNone())
    throw ErrorGeoRefNone(map->gr()->fnObj, errPointMapFromRas);
  fNeedFreeze = true;
  Init();
  objdep.Add(map.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  if (map->fTblAttSelf())
    SetAttributeTable(map->tblAtt());
}

void PointMapFromRas::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapFromRas");
  WriteElement("PointMapFromRas", "RasterMap", map);
}

String PointMapFromRas::sExpression() const
{
  return String("PointMapFromRas(%S)", map->sNameQuoted(true, fnObj.sPath()));
}

bool PointMapFromRas::fDomainChangeable() const
{
  return false;
}

void PointMapFromRas::Init()
{
  htpFreeze = "ilwisapp\\raster_to_points_algorithm.htm";
  sFreezeTitle = "PointMapFromRas";
}

bool PointMapFromRas::fFreezing()
{
  trq.SetText(String(TR("Calculating number of points. '%S'").c_str(), sName(true, fnObj.sPath())));
  
  long i,j;
  long iMapCol=map->iCols(),iMapLines= map->iLines(), iCount=0, iRec=1;
  Coord c;
  GeoRef grInpMap(map->gr());

  if ( fUseReals() ) {
    RealBuf rBufLine(iMapCol);
    for ( i=0; i<iMapLines; ++i  )
    {
        map->GetLineVal(i, rBufLine);
        for(j=0; j<iMapCol; ++j)
        {
            if( rBufLine[j] != rUNDEF) ++iCount;
        }
        if (trq.fUpdate(i, iMapLines))  return false;
    }
  }
  else {
    LongBuf iBufLine(iMapCol);
    for ( i=0; i<iMapLines; ++i )
    {
        map->GetLineRaw(i, iBufLine);
        for(j=0; j<iMapCol; ++j)
        {
          if( iBufLine[j] != iUNDEF) ++iCount;
        }
        if (trq.fUpdate(i, iMapLines))  return false;
    }
  }
   _iPoints=iCount;
  trq.SetText(TR("Mapping Raster points to pointmap"));
  if ( fUseReals() )
  {
    RealBuf rBufLine(iMapCol);
     for ( i=0; i<iMapLines; ++i )
    {
        map->GetLineVal(i, rBufLine);
        for(j=0; j<iMapCol; ++j)
        {
            if(rBufLine[j] != rUNDEF )
            {
              c = grInpMap->cConv(RowCol(i, j));
			  ILWIS::Point *p = CPOINT(ptr.newFeature());
			  p->setCoord(c);
              p->PutVal(rBufLine[j]);
             }
            if (trq.fUpdate(i, iMapLines))  return false;
        }
    }
  }
  else {
     LongBuf iBufLine(iMapCol);
     for ( i=0; i<iMapLines; ++i )
    {
        map->GetLineRaw(i, iBufLine);
        for(j=0; j<iMapCol; ++j)
        {
            if(iBufLine[j] != iUNDEF )
            {
              c = grInpMap->cConv(RowCol(i, j));
			  ILWIS::Point *p = CPOINT(ptr.newFeature());
			  p->setCoord(c);
              p->PutVal(iBufLine[j]);
             }
        }
        if (trq.fUpdate(i, iMapLines))  return false;
    }
  }
  trq.fUpdate(iMapLines*iMapCol, iMapLines*iMapCol);
  return true;
}




