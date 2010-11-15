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
/* $Log: /ILWIS 3.0/RasterApplication/MAPSTRCH.cpp $
 * 
 * 7     9-12-99 9:17 Wind
 * conversion of word to unsigned short
 * 
 * 6     30-11-99 12:22 Wind
 * solved problem with TableHistogramInfo (caused by use of threads)
 * 
 * 5     9/29/99 9:56a Wind
 * added case insensitive string comparison
 * 
 * 4     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
 * 
 * 3     15-03-99 16:06 Koolhoven
 * 
 * 2     3/11/99 12:17p Martin
 * Added support for Case insesitive 
// Revision 1.4  1998/09/16 17:24:31  Wim
// 22beta2
//
// Revision 1.3  1997/09/18 16:19:58  janh
// In InitCalc made a check on (0!=pdi) this means iIntervals ==256 yes or no
// if yes new Map gets image domain, values from o to 255
// if no, values are from 1 to iIntervals
//
// Revision 1.2  1997/08/14 13:02:49  martin
// Counting of entries for hash tablesis now from 0..255 nstead of 1..256
//
/* MapStretch
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JHE  18 Sep 97    5:19 pm
*/
#define MAPSTRETCH_C
#include "Applications\Raster\MAPSTRCH.H"
#include "Applications\Raster\MAPCALC.H"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapStretch(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapStretch::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapStretch(fn, (MapPtr &)ptr);
}

#define rHalf 0.4999999999

const char* MapStretch::sSyntax() {
  return "MapStretchLinear(map,rangefrom,domain,range)\nMapStretchHistEq(map,rangefrom,intervals)";
}
/*
class DATEXPORT ErrorSteps: public ErrorObject
{
public:
  ErrorSteps(const String& sSteps, const WhereError& where)
  : ErrorObject(WhatError(String("Invalid step: %S", sSteps), errMapStretch+1), where) {}
};*/

MapStretch* MapStretch::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  bool fLinear;
  if (fCIStrEqual(sFunc, "MapStretchLinear") || fCIStrEqual(sFunc, "MapStretch"))
    fLinear = true;
  else if (fCIStrEqual(sFunc, "MapStretchHistEq"))
    fLinear = false;
  else
    AppNameError(fn, sFunc);
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms <3 ) || (iParms > 4))
    ExpressionError(sExpr, sSyntax());
  if (!fLinear && (iParms != 3))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  double rPerc = as[1].rVal();
  RangeReal rrFrom(as[1]);
  long iIntervals = iUNDEF;
  DomainValueRangeStruct dvrs;
  if (fLinear) {
    Domain dm = Domain(as[2]);
    if (0 == dm->pdv())
      ValueDomainError(dm->sName(true, fn.sPath()), fn, errMapStretch);
    dvrs.SetDomain(dm);
    if ((iParms == 4) && (!dvrs.fRawIsValue())) {
      ValueRange vr = ValueRange(as[3]);
      dvrs = DomainValueRangeStruct(dm, vr);
    }  
  }
  else {
    iIntervals = as[2].iVal();
    if (iIntervals <= 0)
      ExpressionError(sExpr, sSyntax());
  }  
  return new MapStretch(fn, p, mp, rPerc, rrFrom, dvrs, iIntervals);
}

MapStretch::MapStretch(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p), rFact(-1)
{
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), fnObj, errMapStretch);
  String s;
  ReadElement("MapFromMap", "Type", s);
  fLinear = fCIStrEqual(s , "MapStretchLinear");
  iIntervals = iUNDEF;
  if (!fLinear)
    ReadElement("MapStretch", "Intervals", iIntervals);
  ReadElement("MapStretch", "From", rrFrom);
  rPerc = rUNDEF;
  if (rrFrom.rWidth() <= 0)
    ReadElement("MapStretch", "Perc", rPerc);
  //InitCalc();
  Init();
}

MapStretch::MapStretch(const FileName& fn, MapPtr& p,
           const Map& mp, double rPrc, const RangeReal& rrFrm, const DomainValueRangeStruct& dvr,
           long iIntervls)
: MapFromMap(fn, p, mp), rPerc(rPrc), rrFrom(rrFrm), rFact(-1), iIntervals(iIntervls)
{
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapStretch);
//if ((rPerc > 0) && ((mp->st() > stBYTE) || !mp->dvrs().fRawIsValue())) // can't calculate histogram
//  throw ErrorObject(WhatError(String("No percentage allowed for domain '%S'", dm()->sName(true, fnObj.sPath())), errMapStretch+2), fnObj);
//if (!fLinear && ((mp->st() > stBYTE) || !mp->dvrs().fRawIsValue())) // can't calculate histogram
//  throw ErrorObject(WhatError(String("No hist.eq. allowed for domain '%S'", dm()->sName(true, fnObj.sPath())), errMapStretch+2), fnObj);
  if (rPerc != rUNDEF)
    if ((rPerc < 0) || (rPerc > 50))
      PercError(rPerc, fnObj, errMapStretch+1, 50);
  fLinear = iIntervals <= 0;
  if (!fLinear) {
    if ((iIntervals == 256) && mp->dvrs().fRawIsValue())
      SetDomainValueRangeStruct(Domain("image"));
    else
      SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value"), ValueRange(1, iIntervals)));
  }  
  else  
    SetDomainValueRangeStruct(dvr);
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapStretch::Store()
{
  MapFromMap::Store();
  if (fLinear)
    WriteElement("MapFromMap", "Type", "MapStretchLinear");
  else {
    WriteElement("MapFromMap", "Type", "MapStretchHistEq");
    WriteElement("MapStretch", "Intervals", iIntervals);
  }  
  if (rPerc != rUNDEF)
    WriteElement("MapStretch", "Perc", rPerc);
  else
    WriteElement("MapStretch", "From", rrFrom);
//  WriteElement("MapStretch", "To", rrTo);
}

MapStretch::~MapStretch()
{
}

void MapStretch::Init()
{
  fNeedFreeze = false;
  sFreezeTitle = "MapStretch";
  htpFreeze = htpMapStretchT;
  fInitCalc = false;
  rrTo = dvrs().rrMinMax();
}

static int iHash(const StretchConvRR& scrr)
{
  unsigned short *pw = static_cast<unsigned short*>((void*)&scrr.rVal);
  unsigned short w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;  
}

static int iHash(const StretchConvLL& scll)
{
  unsigned short *pw = static_cast<unsigned short*>((void*)&scll.iVal);
  unsigned short w = *pw;
  ++pw;
  w ^= *pw;
  return w % 16000;  
}

void MapStretch::InitCalc()
{
//  TableHistogramInfo histinf;
//  TableHistogram* his = 0;
  if ((rrFrom.rWidth() <= 0) || !fLinear) {
    try {
      histinf = TableHistogramInfo(mp);
    }
    catch (const ErrorObject&) {
      rrFrom = mp->rrMinMax();
    }
    if (histinf.fValid() && (rPerc != rUNDEF))
      rrFrom = histinf.rrMinMax(rPerc);
    trq.SetTitle(sFreezeTitle);
  }
  else if (rPerc != rUNDEF)
    rrFrom = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);
  if (fLinear) {
    if (rrFrom.rWidth() <= 0)
      rFact = 0;
    else
      rFact = (rrTo.rHi()-rrTo.rLo()) / (rrFrom.rHi() - rrFrom.rLo());
    rOff = rrTo.rLo();
  }
  else {
    if (!histinf.fValid())
      return;
    htrr.Resize(16000);
    htll.Resize(16000);
    RangeInt ri = dvrs().riMinMax();
    long iSteps = ri.iHi() - ri.iLo() + 1;
    Column colCumPix = histinf.colNPixCum();
    long iRecMin = colCumPix->iOffset();
    long iRecMax = colCumPix->iOffset()+colCumPix->iRecs()-1;
    long iTotal = colCumPix->iValue(iRecMax);

    Column colValue = histinf.colValue();
    double rVal, rCum, rCumOld=0;
    long iRec=iRecMin;
    double rStep = ((double)iTotal)/iSteps;
    double rBound;
    DomainImage* pdi = dm()->pdi();
    long i;
    for (long j=1; j <= iSteps; ++j) {
      i = (0 != pdi)? j-1 : j;
      rBound = j * rStep;
      rCum = colCumPix->iValue(iRec);
      while (rCum < rBound) {
        if (colValue.fValid())
          rVal = colValue->rValue(iRec);
        else
          rVal = mp->dvrs().rValue(iRec);
        htrr.add(StretchConvRR(rVal, i));
        htll.add(StretchConvLL(longConv(rVal), i));
        iRec++;
        if (iRec > iRecMax)
          break;
        rCumOld = rCum;
        rCum = colCumPix->iValue(iRec);
      }
      if (fabs(rCum - rBound) < fabs(rBound - rCumOld)) {
        if (colValue.fValid())
          rVal = colValue->rValue(iRec);
        else
          rVal = mp->dvrs().rValue(iRec);
        htrr.add(StretchConvRR(rVal, i));
        htll.add(StretchConvLL(longConv(rVal), i));
        iRec++;
        if (iRec > iRecMax)
          break;
        rCumOld = rCum;
        rCum = colCumPix->iValue(iRec);
      }
    }
  }
  fInitCalc = true;
  fUseRealValues = fRealValues() || mp->fRealValues();
}

long MapStretch::iComputePixelRaw(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
  if (dvrs().fRawIsValue())
    return longConv(buf[0]);
  return dvrs().iRaw(buf[0]);
}

double MapStretch::rComputePixelVal(RowCol rc) const
{
  RealBuf buf(1);
  ComputeLineVal(rc.Row, buf, rc.Col, 1);
  return buf[0];
}

void MapStretch::ComputeLineRaw(long Line, ByteBuf& buf, long iFrom, long iNum) const
{
  if (fLinear) {
    RealBuf rb(iNum);
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (dvrs().fRawIsValue())
      for (long i = 0; i < iNum; ++i)
        buf[i] = byteConv(rb[i]);
    else
    {
      for (long i = 0; i < iNum; ++i)
      {
        byte b=byteConv(dvrs().iRaw(rb[i]));
        buf[i] = b;
      }
    }
  }
  else {
    if (!fInitCalc)
      const_cast<MapStretch *>(this)->InitCalc();
    if (fUseRealValues) {
      RealBuf rb(iNum);
      mp->GetLineVal(Line, rb, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvRR scrr(rb[i]);
        StretchConvRR& scrHash = htrr.get(scrr);
        byte b=byteConv(dvrs().iRaw(scrHash.rStretch));
        buf[i] = b;
      }
    }
    else {
      LongBuf ib(iNum);
      mp->GetLineVal(Line, ib, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvLL scll(ib[i]);
        StretchConvLL& sclHash = htll.get(scll);
        byte b=byteConv(dvrs().iRaw(sclHash.iStretch));
        buf[i] = b;
      }
    }
  }  
}

void MapStretch::ComputeLineRaw(long Line, IntBuf& buf, long iFrom, long iNum) const
{
  if (fLinear) {
    RealBuf rb(iNum);
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (dvrs().fRawIsValue())
      for (long i = 0; i < iNum; ++i)
        buf[i] = shortConv(rb[i]);
    else
      for (long i = 0; i < iNum; ++i)
        buf[i] = shortConv(dvrs().iRaw(rb[i]));
  }
  else {
    if (!fInitCalc)
      const_cast<MapStretch *>(this)->InitCalc();
    if (fUseRealValues) {
      RealBuf rb(iNum);
      mp->GetLineVal(Line, rb, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvRR scrr(rb[i]);
        StretchConvRR& scrHash = htrr.get(scrr);
        buf[i] = shortConv(dvrs().iRaw(scrHash.rStretch));
      }
    }
    else {
      LongBuf ib(iNum);
      mp->GetLineVal(Line, ib, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvLL scll(ib[i]);
        StretchConvLL& sclHash = htll.get(scll);
        buf[i] = shortConv(dvrs().iRaw(sclHash.iStretch));
      }
    }    
  }
}

void MapStretch::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (fLinear) {
    RealBuf rb(iNum);
    ComputeLineVal(Line, rb, iFrom, iNum);
    if (dvrs().fRawIsValue())
      for (long i = 0; i < iNum; ++i)
        buf[i] = longConv(rb[i]);
    else
      for (long i = 0; i < iNum; ++i)
        buf[i] = dvrs().vr()->iRaw(rb[i]);
  }
  else {
    if (!fInitCalc)
      const_cast<MapStretch *>(this)->InitCalc();
    if (fUseRealValues) {
      RealBuf rb(iNum);
      mp->GetLineVal(Line, rb, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvRR scrr(rb[i]);
        StretchConvRR& scrHash = htrr.get(scrr);
        buf[i] = dvrs().iRaw(scrHash.rStretch);
      }
    }
    else {
      LongBuf ib(iNum);
      mp->GetLineVal(Line, ib, iFrom, iNum);
      for (long i = 0; i < iNum; ++i) {
        StretchConvLL scll(ib[i]);
        StretchConvLL& sclHash = htll.get(scll);
        buf[i] = dvrs().iRaw(sclHash.iStretch);
      }
    }    
  }  
}

void MapStretch::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (fLinear) {
    if (rrFrom.rWidth() <= 0 || rFact < 0)
      const_cast<MapStretch *>(this)->InitCalc();
    RealBuf rb(iNum);
    mp->GetLineVal(Line, rb, iFrom, iNum);
    for (long i = 0; i < iNum; ++i) {
      if (rb[i] == rUNDEF)
        buf[i] = iUNDEF;
      else if (rb[i] <= rrFrom.rLo())
        buf[i] = longConv(rrTo.rLo());
      else if (rb[i] >= rrFrom.rHi())
        buf[i] = longConv(rrTo.rHi());
      else
        buf[i] = longConv(rFact * (rb[i] - rrFrom.rLo()) + rOff);
    }
  }
  else {
    if (!fInitCalc)
      const_cast<MapStretch *>(this)->InitCalc();
    RealBuf rb(iNum);
    mp->GetLineVal(Line, rb, iFrom, iNum);
    for (long i = 0; i < iNum; ++i) {
      StretchConvRR scrr(rb[i]);
      StretchConvRR& scrHash = htrr.get(scrr);
      buf[i] = longConv(scrHash.rStretch);
    }
  }
}

void MapStretch::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  if (fLinear) {
    if (rrFrom.rWidth() <= 0 || rFact < 0)
      const_cast<MapStretch *>(this)->InitCalc();
    mp->GetLineVal(Line, buf, iFrom, iNum);
    for (long i = 0; i < iNum; ++i) {
      if (buf[i] == rUNDEF)
        buf[i] = rUNDEF;
      else if (buf[i] <= rrFrom.rLo())
        buf[i] = rrTo.rLo();
      else if (buf[i] >= rrFrom.rHi())
        buf[i] = rrTo.rHi();
      else
        buf[i] = rFact * (buf[i] - rrFrom.rLo()) + rOff;
    }
  }  
  else {
    if (!fInitCalc)
      const_cast<MapStretch *>(this)->InitCalc();
    RealBuf rb(iNum);
    mp->GetLineVal(Line, rb, iFrom, iNum);
    for (long i = 0; i < iNum; ++i) {
      StretchConvRR scrr(rb[i]);
      StretchConvRR& scrHash = htrr.get(scrr);
      buf[i] = scrHash.rStretch;
    }  
/*    
    LongBuf ib(iNum);
    mp->GetLineRaw(Line, ib, iFrom, iNum);
    for (long i = 0; i < iNum; ++i)
      buf[i] = dvrs().iValue(iBnd[ib[i]]);
*/      
  }
}

String MapStretch::sExpression() const
{
  String s;
  if (fLinear)
    s = "MapStretchLinear";
  else
    s = "MapStretchHistEq";
  if (rPerc != rUNDEF) {
    if (fLinear) {
      if (dvrs().fRawIsValue())
        return String("%S(%S,%g,%S)", s, mp->sNameQuoted(true, fnObj.sPath()), rPerc, dm()->sNameQuoted(true, fnObj.sPath()));
      else
        return String("%S(%S,%g,%S,%S)", s, mp->sNameQuoted(true, fnObj.sPath()), rPerc, dm()->sNameQuoted(true, fnObj.sPath()),rrTo.s());
    }
    else  
      return String("%S(%S,%g,%li)", s, mp->sNameQuoted(true, fnObj.sPath()), rPerc, iIntervals);
  }  
  else {
    if (fLinear) {
      if (!vr().fValid())
        return String("%S(%S,%S,%S)", s, mp->sNameQuoted(true, fnObj.sPath()), rrFrom.s(), dm()->sNameQuoted(true, fnObj.sPath()));
      else
        return String("%S(%S,%S,%S,%S)", s, mp->sNameQuoted(true, fnObj.sPath()), rrFrom.s(), dm()->sNameQuoted(true, fnObj.sPath()),dvrs().vr()->sRange()/*rrTo.s()*/);
    }
    else  
      return String("%S(%S,%S,%li)", s, mp->sNameQuoted(true, fnObj.sPath()), rrFrom.s(), iIntervals);
  }  
}

bool MapStretch::fDomainChangeable() const
{
  return fLinear;
}

bool MapStretch::fValueRangeChangeable() const
{
  return fLinear;
}

void MapStretch::SetValueRange(const ValueRange& vr)
{
  MapVirtual::SetValueRange(vr);
  Init();
  InitCalc();
}
