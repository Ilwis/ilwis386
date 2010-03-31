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
// $Log: /ILWIS 3.0/RasterApplication/Mappntrs.cpp $
 * 
 * 9     8/01/01 4:27p Martin
 * repalced Error typedef with int to prevent nameclashes
 * 
 * 8     5-01-00 13:17 Hendrikse
 * corrected  case ptrsSUM:
 * removed assignements to err
 * 
 * 7     16-11-99 9:30a Martin
 * mistook line for comment (was nested in comments), missed it in 2.23.
 * Now properly ported
 * 
 * 6     9/29/99 9:56a Wind
 * added case insensitive string comparison
 * 
 * 5     9/10/99 11:54a Wind
 * changed call to AppNameError in create(..)
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
*/
// Revision 1.4  1998/09/16 17:24:42  Wim
// 22beta2
//
// Revision 1.3  1997/08/28 15:39:53  Dick
// changed so that for ptrsNORM domain value is used and at 2 places changed
// dvrs().fUseReals() to dvrs().fValues.
//

/* MapRasterizePoint
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
	Last change:  WK    5 Jun 98   11:17 am
*/

#include "Applications\Raster\Mappntrs.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapRasterizePoint(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapRasterizePoint::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapRasterizePoint(fn, (MapPtr &)ptr);
}

const char* MapRasterizePoint::sSyntax() {
  return "MapRasterizePoint(pntmap,georef,pointsize)";
}

const char* MapRasterizePoint::sSyntaxCount() {
  return "MapRasterizePointCount(pntmap,georef,pointsize)";
}

const char* MapRasterizePoint::sSyntaxSum() {
  return "MapRasterizePointSum(pntmap,georef,pointsize)";
}

MapRasterizePoint* MapRasterizePoint::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  ptrsType  type;
  if (fCIStrEqual(sFunc,"MapRasterizePoint"))
    type = ptrsNORM;
  else if (fCIStrEqual(sFunc,"MapRasterizePointCount"))
    type = ptrsCOUNT; 
  else if (fCIStrEqual(sFunc,"MapRasterizePointSum"))
    type = ptrsSUM; 
  else
    AppNameError(sExpr, fn.sFullPath());
  Array<String> as(3);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    if (type == ptrsNORM)
      ExpressionError(sExpr, sSyntax());
    else if (type == ptrsCOUNT)
      ExpressionError(sExpr, sSyntaxCount());
    else if (type == ptrsSUM)
      ExpressionError(sExpr, sSyntaxSum());
  PointMap mp(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
  int err = errMapRasterizePoint;
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, err);
  short iPointSize = as[2].shVal();
  return new MapRasterizePoint(fn, p, mp, gr, iPointSize, type);
}

MapRasterizePoint::MapRasterizePoint(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
  fNeedFreeze = true;
  String sType;
  ReadElement("MapFromPointMap", "Type", sType);
  if (fCIStrEqual(sType, "MapRasterizePoint"))
    type = ptrsNORM;
  else if (fCIStrEqual(sType,"MapRasterizePointCount"))
    type = ptrsCOUNT; 
  else if (fCIStrEqual(sType, "MapRasterizePointSum"))
    type = ptrsSUM; 
  ReadElement("MapRasterizePoint", "PointSize", iPointSize);
  Init();
  objdep.Add(gr().ptr());
}

MapRasterizePoint::MapRasterizePoint(const FileName& fn, MapPtr& p, const PointMap& pmp, 
                         const GeoRef& gr, int iPointSz, ptrsType tpe)
: MapFromPointMap(fn, p, pmp, gr), iPointSize(iPointSz), type(tpe)
{
  fNeedFreeze = true;
  int err = errMapRasterizePoint;
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, err);
  if (iPointSize <= 0)
    throw ErrorObject(WhatError(String(SMAPErrPointSizeNotPositiv.scVal(), iPointSize), err+1), sTypeName());
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), err+2);

  switch (type)
  {
    case ptrsSUM:
      if (!pmp->fValues())
        throw ErrorValueDomain(pmp->dm()->sName(true, fnObj.sPath()), fnObj, err+3);
      SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value")));
      break;
    case ptrsCOUNT:
      SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("count"), ValueRange(0L, pmp->iFeatures())));
      break;
  }
  Init();
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapRasterizePoint::Store()
{
  MapFromPointMap::Store();
  switch (type) {
    case ptrsNORM:
      WriteElement("MapFromPointMap", "Type", "MapRasterizePoint");
      break;
    case ptrsCOUNT:
      WriteElement("MapFromPointMap", "Type", "MapRasterizePointCount");
      break;
    case ptrsSUM:
      WriteElement("MapFromPointMap", "Type", "MapRasterizePointSum");
      break;
	}   
  WriteElement("MapRasterizePoint", "PointSize", (long)iPointSize);
}

MapRasterizePoint::~MapRasterizePoint()
{
}

String MapRasterizePoint::sExpression() const
{
  switch (type) {
    case ptrsNORM:
      return String("MapRasterizePoint(%S,%S,%i)", 
             pmp->sNameQuoted(true, fnObj.sPath()), 
             gr()->sNameQuoted(true, fnObj.sPath()), iPointSize);
    case ptrsCOUNT:
      return String("MapRasterizePointCount(%S,%S,%i)", 
             pmp->sNameQuoted(true, fnObj.sPath()), 
             gr()->sNameQuoted(true, fnObj.sPath()), iPointSize);
    case ptrsSUM:
      return String("MapRasterizePointSum(%S,%S,%i)", 
             pmp->sNameQuoted(true, fnObj.sPath()), 
             gr()->sNameQuoted(true, fnObj.sPath()), iPointSize);
  }   
  return sUNDEF;
}

bool MapRasterizePoint::fFreezing()
{

  if (!fInitFill())
    return false;

  long iPoints = pmp->iFeatures();
  Coord crd;
  RowCol rc;
  long iFirstLine, iFirstCol, iLastLine, iLastCol;
  
  bool fTransformCoords = cs() != pmp->cs();
  
  trq.SetText(SMAPTextRasterize);
  for (long p = 1; p <= iPoints; p++) {
    if (trq.fUpdate(p, iPoints))
      return false;
    crd = pmp->cValue(p);
    if (fTransformCoords)
      crd = cs()->cConv(pmp->cs(), crd);
    rc = gr()->rcConv(crd);
    if (rc.fUndef())
      continue;
    iFirstLine = rc.Row - iHalfSize;
    iFirstCol = rc.Col - iHalfSize;
    iLastLine = iFirstLine + iPointSize;
    iLastCol = iFirstCol + iPointSize;
    switch (type) {
      case ptrsNORM:
        { 
          if (dvrs().fValues()) {                      //rev 1.2 changed from fUseReals to fValues
            double rVal = pmp->rValue(p);
            for (long l = iFirstLine; l < iLastLine; l++)
              for (long c = iFirstCol; c < iLastCol; c++)
                pms->PutVal(RowCol(l,c), rVal);
          }
          else {   
            long iRaw = pmp->iRaw(p);
            for (long l = iFirstLine; l < iLastLine; l++)
              for (long c = iFirstCol; c < iLastCol; c++)
                pms->PutRaw(RowCol(l,c), iRaw);
          }   
        }
        break;
      case ptrsCOUNT:
        { 
          for (long l = iFirstLine; l < iLastLine; l++)
            for (long c = iFirstCol; c < iLastCol; c++) {
              RowCol rc(l, c);
              long iCount = pms->iValue(rc);              
              pms->PutVal(rc, iCount+1);
            } 
        }
        break;
      case ptrsSUM:
        { double rVal = pmp->rValue(p);
          for (long l = iFirstLine; l < iLastLine; l++)
            for (long c = iFirstCol; c < iLastCol; c++) {
              RowCol rc(l, c);
              double rOldVal = pms->rValue(rc);              
              //rOldVal += rVal;              
              //pms->PutVal(rc, rOldVal);
              pms->PutVal(rc, rOldVal + rVal);
            } 
        }
        break;
    }    
  }
  if (type == ptrsNORM) {
    Table tblAtt = pmp->tblAtt();
    if (tblAtt.fValid())
      SetAttributeTable(tblAtt);
  }  
  return true;
}

void MapRasterizePoint::Init()
{
  sFreezeTitle = "MapRasterizePoint";
  htpFreeze = htpMapRasterizePointT;
  iHalfSize = iPointSize / 2;  // was "(iPointSize+1) / 2" which causes a shift of (-1,-1)
}

bool MapRasterizePoint::fInitFill()
{
  trq.SetText(SMAPTextInitializeMap);
  if (dvrs().fValues() && (type == ptrsNORM)) { //rev 1.2 changed from fUseReals to fValues
    RealBuf rBufLine(iCols());
    for (int c = 0; c < iCols(); c++)
      rBufLine[c] = rUNDEF;
    for (long l = 0; l < iLines(); l++) {
      if ((l % 25) == 0)
        if (trq.fUpdate(l, iLines()))
          return false;
      pms->PutLineVal(l, rBufLine);
    }
  }
  else {  
    LongBuf iBufLine(iCols());
    if ((type == ptrsCOUNT) || (type == ptrsSUM)) {
      for (int c = 0; c < iCols(); c++)
        iBufLine[c] = 0;
      for (long l = 0; l < iLines(); l++) {
        if ((l % 25) == 0)
          if (trq.fUpdate(l, iLines()))
            return false;
        pms->PutLineVal(l, iBufLine);
      }
    }  
    else {
      for (int c = 0; c < iCols(); c++)
        iBufLine[c] = iUNDEF;
      for (long l = 0; l < iLines(); l++) {
        if ((l % 25) == 0)
          if (trq.fUpdate(l, iLines()))
             return false;
        pms->PutLineRaw(l, iBufLine);
      }
    }
  }  
  trq.fUpdate(iLines(), iLines());
  return true;
}
/*
void MapRasterizePoint::Replace(const String& sExpr)
{
  MapVirtual::Replace(sExpr);
  MapRasterizePoint* p = 0;
  p = MapRasterizePoint::create(FileName(), sExpr);
  type = p->type;
  pmp = p->pmp;
  SetDomainValueRangeStruct(p->dvrs());
  SetGeoRef(p->gr());
  iPointSize = p->iPointSize;
  delete p;
  Init();
}*/




