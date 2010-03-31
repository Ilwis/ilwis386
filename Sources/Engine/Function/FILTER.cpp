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
/* FilterPtr
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\Function\FILTER.H"
#include "Engine\Function\FLTMAJOR.H"
#include "Engine\Function\FLTRANK.H"
#include "Engine\Function\FLTLIN.H"
#include "Engine\Function\FLTBIN.H"
#include "Engine\Function\FLTPATT.H"
#include "Engine\Function\FLTRADAR.H"
#include "Engine\Function\FLTSTDEV.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Headers\Hs\DAT.hs"

class DATEXPORT ErrorInvalidFilterSize : public ErrorObject 
{
public:  
  ErrorInvalidFilterSize::ErrorInvalidFilterSize(int iRows, int iCols, const WhereError& where)
  : ErrorObject(WhatError(String("%S %i x %i", SDATErrInvalidFilterSize, iRows, iCols), errFilter), where) 
  {}
};

void InvalidFilterSizeError(int iRows, int iCols, const FileName& fn)
{
  throw ErrorInvalidFilterSize(iRows, iCols, fn);
}

Filter::Filter()
: Function()
{
}

Filter::Filter(const Filter& flt)
: Function(flt.pointer())
{
}

Filter::Filter(const FileName& fn)
: Function()
{
  FileName fnFil(fn, ".fil");
  MutexFileName mut(fnFil);
  FilterPtr* p = static_cast<FilterPtr*>(Function::pGet(fnFil));
  if (p) // if already open return it
    SetPointer(p);
  else
    SetPointer(FilterPtr::create(fnFil));
}

Filter::Filter(IlwisObjectPtr* ptr)
: Function(ptr)
{
}

Filter::Filter(const FileName& fn, const String& sExpression)
: Function(FilterPtr::create(fn, sExpression))
{}

Filter::Filter(const String& sExpression)
: Function(FilterPtr::create(FileName(), sExpression))
{}

Filter::Filter(const String& sExpression, const String& sPath)
: Function()
{
  try {
    FileName fn(sExpression, ".fil", true);
    fn.Dir(sPath);
    SetPointer(FilterPtr::create(fn));
  }
  catch (const ErrorObject&) {
  }
  if (0 != ptr())
    return;
  try {
    SetPointer(FilterPtr::create(FileName(sExpression)));
  }
  catch (const ErrorObject&) {
  }
  if (0 == ptr())
    SetPointer(FilterPtr::create(FileName::fnPathOnly(sPath), sExpression));
}

Filter::Filter(const char * sExpression)
: Function(FilterPtr::create(FileName(), String(sExpression)))
{}

FilterPtr::~FilterPtr()
{
}

String FilterPtr::sType() const
{
  return "Filter";
}

FilterPtr* FilterPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam)) 
      NotFoundError(fn);
  }
  MutexFileName mut(fn);
  FilterPtr* p = static_cast<FilterPtr*>(Function::pGet(fn));
  if (p) // if already open return it
    return p;
  String sType;
  ObjectInfo::ReadElement("Filter", "Type", filnam, sType);
  if ("FilterRankOrder" == sType)
    return FilterRankOrder::create(filnam);
  if ("FilterMajority" == sType)
    return new FilterMajority(filnam);
  if ("FilterLinear" == sType)
    return FilterLinear::create(filnam);
  if ("FilterRadar" == sType)
    return FilterRadar::create(filnam);
  if ("FilterStandardDev" == sType)
    return new FilterStandardDev(filnam);
  if ("FilterBinary" == sType)
    return new FilterBinary(filnam);
  if ("FilterPattern" == sType)
    return new FilterPattern(filnam);
  InvalidTypeError(filnam, "Filter", sType);
  return 0;
}

FilterPtr* FilterPtr::create(const FileName& fn, const String& sExpression)
{
  String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
  if (sFunc == sUNDEF && fn.sFile.length() == 0)
    return FilterPtr::create(FileName(sExpression,".fil",true));
  if (fCIStrEqual(sFunc, "RankOrder"))
    return FilterRankOrder::create(fn, sExpression);
  if (fCIStrEqual(sFunc,"Median"))
    return FilterMedian::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "Majority"))
    return FilterMajority::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "UndefMajority"))
    return FilterMajority::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "Average"))
    return FilterAverage::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "Pattern"))
    return FilterPattern::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterLinear"))
    return FilterLinear::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterLee"))
    return FilterLee::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterLeeEnhanced"))
    return FilterLeeEnhanced::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterKuan"))
    return FilterKuan::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterFrost"))
    return FilterFrost::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterFrostEnhanced"))
    return FilterFrostEnhanced::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterGammaMap"))
    return FilterGammaMap::create(fn, sExpression);
  if (fCIStrEqual(sFunc, "FilterStandardDev"))
    return FilterStandardDev::create(fn, sExpression);
  NotFoundError(String("Filter: %S", sExpression));
  return 0;
}

FilterPtr::FilterPtr(const FileName& fn)
: FunctionPtr(fn)
{
  iFltRows = iReadElement("Filter", "FilterRows");
  iFltCols = iReadElement("Filter", "FilterColumns");
}

FilterPtr::FilterPtr(const FileName& fn, short iRows, short iCols,
                     const Domain& dmDefault)
: FunctionPtr(fn, dmDefault)
{
  if ((iRows < 0) || (iCols < 0) || ((iRows % 2) == 0) || ((iCols % 2) == 0))
    InvalidFilterSizeError(iRows, iCols, fnObj);
  iFltRows = iRows;
  iFltCols = iCols;
}

void FilterPtr::Store()
{
  FunctionPtr::Store();
  WriteElement("Function", "Type", "Filter");
  WriteElement("Filter", "FilterRows", (long)iFltRows);
  WriteElement("Filter", "FilterColumns", (long)iFltCols);
}

Domain FilterPtr::dmDefault(const Map& mp) const
{
  Domain dom = FunctionPtr::dmDefault();
  if (!dom.fValid()) 
    return mp->dm();
  if (dom->sName() == "bool") {
    if (mp->dm()->pdbit() || mp->dm()->pdi()) // no undefs in map
      return Domain("bit");
    else
      return Domain("bool");
  }  
  return dom;
}

ValueRange FilterPtr::vrDefault(const Map& mp, const Domain& dm) const
{
  ValueRange vr = FunctionPtr::vrDefault();
  if (!vr.fValid()) {
    if (mp->vr().fValid()) {
      vr = mp->vr();
      RangeReal rr = mp->rrMinMax();
      if (rr.fValid())
        vr = ValueRange(rr.rLo(), rr.rHi(), vr->rStep());
    }
    else if (dm.fValid())
      vr = ValueRange(dm);
    else
      vr = ValueRange(mp->dm());
/*    
    ValueRangeInt* vri = vr->vri();  
    ValueRangeReal* vrr = vr->vrr();  
    if (vri) {
      RangeInt ri = map->riMinMax();
      if (ri.iHi() > ri.iLo()) {
        vri->iHi() = ri.iHi();
        vri->iLo() = ri.iLo();
        vri->init();
      }  
    }
    else if (vrr) {
      RangeReal rr = map->rrMinMax();
      if (rr.rHi() > rr.rLo()) {
        vrr->rHi() = rr.rHi();
        vrr->rLo() = rr.rLo();
        vrr->init();
      }  
    }
    // should filter influence vr ?
    */
  } 
  return vr;
}

bool FilterPtr::fRawAllowed() const
{
  return true;
}

void FilterPtr::ExecuteRaw(const LongBufExt* bufListInp, ByteBuf& bufRes)
{
  LongBuf lb(bufRes.iSize());
  ExecuteRaw(bufListInp, lb);
  for (int i = 0; i < bufRes.iSize(); i++)
    bufRes[i] = byteConv(lb[i]);
}

void FilterPtr::ExecuteRaw(const LongBufExt* bufListInp, IntBuf& bufRes)
{
  LongBuf lb(bufRes.iSize());
  ExecuteRaw(bufListInp, lb);
  for (int i = 0; i < bufRes.iSize(); i++)
    bufRes[i] = shortConv(lb[i]);
}

void FilterPtr::ExecuteVal(const RealBufExt* bufListInp, LongBuf& bufRes)
{
  RealBuf rb(bufRes.iSize());
  ExecuteVal(bufListInp, rb);
  for (int i = 0; i < bufRes.iSize(); i++)
    bufRes[i] = longConv(rb[i]);
}

void Filter::ErrorThreshold(double rThres, const String& sObjName)
{
  throw ErrorObject(WhatError(String("%S %g", SDATErrPositiveThreshNeeded, rThres), errFilter+2), sObjName);
}




