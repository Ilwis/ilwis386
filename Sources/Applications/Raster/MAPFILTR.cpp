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
// $Log: /ILWIS 3.0/RasterApplication/MAPFILTR.cpp $
 * 
 * 8     29-02-00 17:32 Wind
 * binary filter bug
 * 
 * 7     28-02-00 16:33 Wind
 * bug in 'usera' and binary filters
 * 
 * 6     22-12-99 10:28 Wind
 * changed fUseRaw use
 * 
 * 5     9/24/99 10:39a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 4     9-09-99 9:46a Martin
 * Added 2.22 stuff
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:54a Wind
 * changed sName() to sNameQuoted() in sExpression() tot suport quoted
 * file names
*/
// Revision 1.5  1998/09/16 17:24:31  Wim
// 22beta2
//
// Revision 1.4  1997/09/25 17:20:18  Wim
// Changed fUseRaw to true in init() for fBinaryFilter and fAdd1ToRaw to true if not bit
//
// Revision 1.3  1997-09-23 10:39:10+02  Wim
// Check on invalid default domain in constructor of MapFilter
//
/* MapFilter
   Copyright Ilwis System Development ITC
   july 1995, by Jelle Wind
	Last change:  WK   25 Sep 97    7:17 pm
*/
#include "Applications\Raster\MAPFILTR.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Function\FLTBIN.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapFilter(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapFilter::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapFilter(fn, (MapPtr &)ptr);
}

const char * MapFilter::sSyntax()
 { return "MapFilter(map,filter)"; }

class ErrorValueDomainFilter : public ErrorObject
{
public:
  ErrorValueDomainFilter(const WhereError& where, const Domain& dm, const Filter& flt)
   : ErrorObject(WhatError(String(SMAPErrDomainValueRequired_SS.scVal(), flt->sName(), dm->sName()),
                           errMapFilter), where) {}
};

void ValueDomainFilterError(const String& s, const Domain& dm, const Filter& flt)
{
  throw ErrorValueDomainFilter(s,dm,flt);
}

void InvalidFilterError(const String& sExpr)
{
  String s(SMAPErrInvalidFilter_S.scVal(), sExpr);
  throw ErrorObject(s, errMapFilter+1);
}

static void InvalidFilterMapCombination(const Map& mp, const Filter& flt)
{
  String s(SMAPErrFilterNotPossible_SS.scVal(), flt->sName(), mp->sName());
  throw ErrorObject(s, errMapFilter+2);
}

Domain MapFilter::dmDefault(const Map& mp, const Filter& flt)
{
  return flt->dmDefault(mp);
}

ValueRange MapFilter::vrDefault(const Map& mp, const Filter& flt, const Domain& dom)
{
  return flt->vrDefault(mp, dom);
}

MapFilter* MapFilter::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  Filter flt(as[1], fn.sPath());
  if (!flt.fValid())
    InvalidFilterError(sExpr);
  Domain dm;
  if (iParms == 3)
    dm = Domain(as[2]);
  return new MapFilter(fn, p, mp, flt, dm);
}

MapFilter::MapFilter(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ptr.ReadElement("MapFilter", "Filter", flt);
  objdep.Add(flt.ptr());
  Init();
}

MapFilter::MapFilter(const FileName& fn, MapPtr& p,
           const Map& mp, const Filter& filter, const Domain& dmn)
: MapFromMap(fn, p, mp), flt(filter)
{
  if (!flt->fRawAllowed() && !mp->dvrs().fValues())
    ValueDomainFilterError(sTypeName(), mp->dm(), flt);
  if (dmn.fValid())
    SetDomainValueRangeStruct(dmn);
  else {//{  // check if filter has default domain
    Domain dmDflt = dmDefault(mp, flt);
    if (!dmDflt.fValid())
      InvalidFilterMapCombination(mp,flt);
    ValueRange vr = vrDefault(mp, flt, dmDflt);
    DomainValueRangeStruct dvrs(dmDflt,vr);
    SetDomainValueRangeStruct(dvrs);
  }  
  Init();
  objdep.Add(flt.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapFilter::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapFilter");
  WriteElement("MapFilter", "Filter", flt);
}

MapFilter::~MapFilter()
{
  if (bufListRaw)
    delete [] bufListRaw;
  if (bufListVal)
    delete [] bufListVal;
}

bool MapFilter::fDomainChangeable() const
{
  return flt->fDomainChangeable();
}

bool MapFilter::fValueRangeChangeable() const
{
  return flt->fValueRangeChangeable();
}

long MapFilter::iComputePixelRaw(RowCol rc) const
{
  LongBuf buf(1);
  ComputeLineRaw(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

double MapFilter::rComputePixelVal(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

void MapFilter::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (!fRawAvailable()) {
    for (i=0; i < buf.iSize(); i++)
      buf[i] = 0;
    return;
  }
  if (fUseRaw) {
    const_cast<MapFilter *>(this)->ReadBufLines(Line, iFrom, iNum);
    flt->ExecuteRaw(bufListRaw, buf);
    if (fAdd1ToRaw)
      for (i=0; i < buf.iSize(); i++)
        if (buf[i] != 0)
          buf[i]++;
  }
  else {
    RealBuf rb(buf.iSize());
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (fRawIsValue())
      for (i=0; i < buf.iSize(); i++)
        buf[i] = byteConv(longConv(rb[i]));
    else
      for (i=0; i < buf.iSize(); i++)
        buf[i] = byteConv(dvrs().iRaw(rb[i]));
  }
}

void MapFilter::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (!fRawAvailable()) {
    for (i=0; i < buf.iSize(); i++)
      buf[i] = shUNDEF;
    return;
  }
  if (fUseRaw){
    const_cast<MapFilter *>(this)->ReadBufLines(Line, iFrom, iNum);
    flt->ExecuteRaw(bufListRaw, buf);
    if (fAdd1ToRaw)
      for (i=0; i < buf.iSize(); i++)
        if (buf[i] != shUNDEF)
          buf[i]++;
  }
  else {
    RealBuf rb(buf.iSize());
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (fRawIsValue())
      for (i=0; i < buf.iSize(); i++)
        buf[i] = shortConv(longConv(rb[i]));
    else  
      for (i=0; i < buf.iSize(); i++)
        buf[i] = shortConv(dvrs().iRaw(rb[i]));
  }
}

void MapFilter::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (!fRawAvailable()) {
    for (i=0; i < buf.iSize(); i++)
      buf[i] = iUNDEF;
    return;
  }
  if (fUseRaw) {
    const_cast<MapFilter *>(this)->ReadBufLines(Line, iFrom, iNum);
    flt->ExecuteRaw(bufListRaw, buf);
    if (fAdd1ToRaw)
      for (i=0; i < buf.iSize(); i++)
        if (buf[i] != iUNDEF)
          buf[i]++;
  }
  else {
    RealBuf rb(buf.iSize());
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (fRawIsValue())
      for (i=0; i < buf.iSize(); i++)
        buf[i] = longConv(rb[i]);
    else  
      for (i=0; i < buf.iSize(); i++)
        buf[i] = dvrs().iRaw(rb[i]);
  }
}


void MapFilter::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (!fValues()) {
    for (i = 0; i < buf.iSize(); i++)
      buf[i] = iUNDEF;
    return;
  }
  if (fUseRaw) {
    ComputeLineRaw(Line, buf, iFrom, iNum);
    for (i = 0; i < buf.iSize(); i++)
      buf[i] = dvrs().iValue(buf[i]);
  }
  else {
    const_cast<MapFilter *>(this)->ReadBufLines(Line, iFrom, iNum);
    flt->ExecuteVal(bufListVal, buf);
  }
}

void MapFilter::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  int i;
  if (!dvrs().fValues()) {
    for (i = 0; i < buf.iSize(); i++)
      buf[i] = rUNDEF;
    return;
  }
  if (fUseRaw) {
    LongBuf lb(buf.iSize());
    ComputeLineRaw(Line, lb, iFrom, iNum);
    for (i = 0; i < buf.iSize(); i++)
      buf[i] = dvrs().rValue(lb[i]);
  }
  else {
    const_cast<MapFilter *>(this)->ReadBufLines(Line, iFrom, iNum);
    flt->ExecuteVal(bufListVal, buf);
  }
}

void MapFilter::Init()
{
  fNeedFreeze = false;
  sFreezeTitle = "MapFilter";
  htpFreeze = htpMapFilterT;
  iFltRows = flt->iRows();
  iFltCols = flt->iCols();
  iLastLine = iUNDEF;
  iLastFrom = iUNDEF;
  iLastNum = iUNDEF;
  fUseRaw = flt->fRawAllowed() && fRawAvailable();
  fAdd1ToRaw = false;

  bool fBinaryFilter = 0 != dynamic_cast<FilterBinary*>(flt.ptr());
  if (fBinaryFilter && mp->dm()->pdv())
    fUseRaw = false;
  if (fBinaryFilter && (0 == dm()->pdbit()))
    fAdd1ToRaw = true;
//  if (fBinaryFilter && (0 != mp->dm()->pdbool()))
//    fAdd1ToRaw = true;
  if (fUseRaw && !fBinaryFilter) {
//  if (fUseRaw) {
    if (dm() != mp->dm())
      fUseRaw = false;
    else if (!vr().fValid() && !mp->vr().fValid())
      fUseRaw = true;
    else if (vr().fValid() && mp->vr().fValid())
      fUseRaw = vr() == mp->vr();
    else
      fUseRaw = false;
  }  
  if (fUseRaw){
    bufListRaw = new LongBufExt[iFltRows];
    for (int i=0; i < iFltRows; i++)
      bufListRaw[i].Size(mp->iCols(), iFltCols/2, iFltCols/2);
    bufListVal = 0;
  }
  else {
    bufListVal = new RealBufExt[iFltRows];
    for (int i=0; i < iFltRows; i++)
      bufListVal[i].Size(mp->iCols(), iFltCols/2, iFltCols/2);
    bufListRaw = 0;
  }
}

void MapFilter::ReadLineRaw(long iLine, LongBufExt& buf, long iFrom, long iNum)
{
  int i;
  long iNewFrom = max(0L, iFrom - iFltCols/2);
  long iTo = iFrom + iNum - 1;
  long iNewTo = min(mp->iCols()-1, iTo + iFltCols/2);
  if (iLine < 0)
    iLine = 0;
  else if (iLine >= mp->iLines())
    iLine = mp->iLines()-1;
  mp->GetLineRaw(iLine, buf, iNewFrom, iNewTo - iNewFrom + 1);
  for (i=-iFltCols/2 + iNewFrom; i < 0; i++)
    buf[i] = buf[0];
  for (i = mp->iCols() + iFltCols/2 - 1; i > iNewTo; i--)
    buf[i] = buf[iNum-1];
}

void MapFilter::ReadLineVal(long iLine, RealBufExt& buf, long iFrom, long iNum)
{
  int i;
  long iNewFrom = max(0L, iFrom - iFltCols/2);
  long iTo = iFrom + iNum - 1;
  long iNewTo = min(mp->iCols()-1, iTo + iFltCols/2);
  if (iLine < 0)
    iLine = 0;
  else if (iLine >= mp->iLines())
    iLine = mp->iLines()-1;
  mp->GetLineVal(iLine, buf, iNewFrom, iNewTo - iNewFrom + 1);
  for (i=-iFltCols/2 + iNewFrom; i < 0; i++)
    buf[i] = buf[0];
  for (i = mp->iCols() + iFltCols/2 - 1; i > iNewTo; i--)
    buf[i] = buf[iNum-1];
  iLastLine = iLine;
  iLastFrom = iFrom;
  iLastNum = iNum;
}

void MapFilter::ReadBufLines(long iLine, long iFrom, long iNum)
{
  if ((iLastLine == iLine - 1) && (iLastFrom == iFrom) && (iLastNum == iNum)) {
    if (fUseRaw) {
      for (int i=0; i < iFltRows-1; i++)
        Swap(bufListRaw[i], bufListRaw[i+1]);
      ReadLineRaw(iLine+iFltRows/2, bufListRaw[iFltRows-1], iFrom, iNum);
    }
    else {
      for (int i=0; i < iFltRows-1; i++)
        Swap(bufListVal[i], bufListVal[i+1]);
      ReadLineVal(iLine+iFltRows/2, bufListVal[iFltRows-1], iFrom, iNum);
    }
  }
  else if ((iLastLine != iLine) || (iLastFrom != iFrom) || (iLastNum == iNum)) {
    int i;
    if (fUseRaw)
      for (i=0; i < iFltRows; i++)
        ReadLineRaw(iLine-iFltRows/2+i, bufListRaw[i], iFrom, iNum);
    else
      for (i=0; i < iFltRows; i++)
        ReadLineVal(iLine-iFltRows/2+i, bufListVal[i], iFrom, iNum);
  }
}

String MapFilter::sExpression() const
{
  return String("MapFilter(%S,%S)", 
                mp->sNameQuoted(true, fnObj.sPath()), 
                flt->sNameQuoted(true, fnObj.sPath()));
}




