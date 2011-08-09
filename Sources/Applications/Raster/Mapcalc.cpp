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
// $Log: /ILWIS 3.0/RasterApplication/Mapcalc.cpp $
 * 
 * 5     28-09-00 2:57p Martin
 * tranquilzier is added to instructions
 * 
 * 4     14-02-00 16:06 Wind
 * set georef before calculation  (bug 499)
 * 
 * 3     9/08/99 1:11p Wind
 * comment
 * 
 * 2     9/08/99 11:51a Wind
 * comment problem
*/
// Revision 1.7  1998/09/16 17:24:28  Wim
// 22beta2
//
// Revision 1.6  1997/09/16 16:33:06  Wim
// If storetype is larger than stREAL dvrsDefault() will return invalid domain
//
// Revision 1.5  1997-08-11 15:18:48+02  Wim
// InitFreeze() now changes instruct in the proper way,
// SetDomainValueRangeStruct() is removed because it is never called.
//
// Revision 1.4  1997-07-31 14:01:49+02  Wim
// In dvrsDefault() if instruc is 0 return
//
// Revision 1.3  1997-07-24 17:51:33+02  Wim
// After cyclic def error is shown, DummyError() is called
//
// Revision 1.2  1997/07/24 15:01:58  Wim
// In case of GeoRefNone set size of map
//
/* MapCalculate
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    4 Sep 98    6:07 pm
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Applications\Raster\MAPCALC.H"
#include "Engine\Scripting\Calc.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\mapcalc.hs"

IlwisObjectPtr * createMapCalculate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCalculate::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCalculate(fn, (MapPtr &)ptr);
}

/*
static DList<CalcVariable*> dlcvDummy;
static DList<InstList*> dlilDummy;
static DList<short> dliDummy;
static DList<StackObject> dlsoDummy;
*/

const char* MapCalculate::sSyntax() {
  return TR("(see manual)").c_str();
}

class ErrorEmptyExpr : public ErrorObject
{
public:
  ErrorEmptyExpr(const WhereError& where)
    : ErrorObject(WhatError(TR("Empty Expression"), errMapCalculate), where) {}
};

static void EmptyExprError()
{
  throw ErrorEmptyExpr(String());
}

static void EmptyExprError(const FileName& fn)
{
  throw ErrorEmptyExpr(fn);
}

static void UnexpectedTokenError(const String& sVal)
{
  ErrorObject(WhatError(String(TR("Unexpected '%S'").c_str(), sVal), errColumnCalculate+3)).Show();
}

class ErrorCalcExpression : public ErrorObject
{
public:
  ErrorCalcExpression(const WhereError& where, const String& sExpr)
    : ErrorObject(WhatError(String(TR("Invalid calc expression: '%S'").c_str(), sExpr), errMapCalculate+1), where) {}
};

static void CalcExpressionError(const String& s, const String& sExpr)
{
  throw ErrorCalcExpression(s, sExpr);
}

static void CalcExpressionError(const FileName& fn, const String& sExpr)
{
  DummyError();
//  throw ErrorCalcExpression(fn, sExpr);
}

static void NoGeoRefError()
{
  throw ErrorObject(WhatError(TR("No georeference"), errMapCalculate+2));
} 

static void CyclicDefError(const FileName& fn)
{
  throw ErrorObject(WhatError(TR("Cyclic definition"), errMapCalculate+1), fn);
}

Instructions* MapCalculate::instCreate(const String& sExpr, const FileName& fn, ObjectDependency& objdep, bool& fNoGeoRef)
{
  if (sExpr.length() == 0)
    EmptyExprError();
  Calculator calc(sExpr, fn);
//  bool fNoGeoRef;
  Instructions* inst = calc.instMapCalc(objdep, fNoGeoRef);
/*  if (fNoGeoRef) {
    delete inst;
    NoGeoRefError();
    return 0;
  }  */
  return inst;
}

MapCalculate* MapCalculate::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  ObjectDependency objdep;
  bool fNoGeoRef;
  Instructions *instruc = instCreate(sExpr, fn, objdep, fNoGeoRef);
  if (instruc == 0)
    return 0;
  try {
    if (objdep.fUses(fn))
      CyclicDefError(fn);
  }
  catch (ErrorObject& err) {
    err.Show();
    DummyError();
    return 0;
  }
  return new MapCalculate(fn, p, instruc, sExpr, objdep, fNoGeoRef);
}

MapCalculate::MapCalculate(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  fNeedFreeze = false;
  ReadElement("MapVirtual", "Expression", _sExpression);
  if (sExpression().length() == 0)
    EmptyExprError(fn);
  instruc = 0; // next functions may throw
  instruc = instCreate(sExpression(), fn, objdep, fNoGeoRef);
  if (instruc == 0)
    CalcExpressionError(sTypeName(), sExpression());
//  if (instruc->dm()->pdv() == 0)
//    throw ErrorValueDomain(this, instruc->dm());
  if (dm()->pdsrt()) {
    instruc->ChangeConstStringsToRaw(dm());
    for (unsigned int i=0; i< instruc->admmrg.iSize(); ++i)
      instruc->admmrg[i].ApplyMergeDomain(dm());
  }
  Init();
	instruc->SetTranquilizer( &trq);	
//  instruc->SetGeoRef(p.gr());
//  if (p.gr()->fGeoRefNone())
//    _rcSize = instruc->rcSize();
}

MapCalculate::MapCalculate(const FileName& fn, MapPtr& p, Instructions* inst, const String& sExpr, 
                           const ObjectDependency& objdepen, bool fNoGrf)
: MapVirtual(fn, p, inst->gr(), inst->rcSize(), inst->dm()), fNoGeoRef(fNoGrf)
{
  fNeedFreeze = false;
  SetValueRange(inst->vr());
  _sExpression = sExpr;
  instruc = inst;
	instruc->SetTranquilizer( &trq);	
  objdep = objdepen;
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapCalculate::Init()
{
  htpFreeze = "ilwisapp\\map_calculation_algorithm.htm";
  sFreezeTitle = "MapCalculate";
  SetUse();  
}

String MapCalculate::sType() const
{
  return "Map Calculate";
}

void MapCalculate::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapCalculate");
}

void MapCalculate::Replace(const String& sExpr)
{
  ObjectDependency od;
  Instructions* inst = instCreate(sExpr, fnObj, od, fNoGeoRef);
  if (0 == inst)
    CalcExpressionError(sTypeName(), sExpr);
  if (od.fUses(&ptr))
    throw ErrorObject(WhatError(TR("Cyclic definition"), errMapCalculate+1), fnObj);
//  MapVirtual::Replace(sExpr);
  if (0 != instruc)
    delete instruc;
  instruc = inst;
//  if (dm()->pdsrt())
//    instruc->ChangeConstStringsToRaw(dm());
  SetDomainValueRangeStruct(DomainValueRangeStruct(instruc->dm(), instruc->vr()));
  _sExpression = sExpr;
  objdep = od;
//  instruc->SetGeoRef(ptr.gr());
//fFrozen = false;
//  SetUse();  
}

bool MapCalculate::fGeoRefChangeable() const
{
  return fNoGeoRef;
}

bool MapCalculate::fDomainChangeable() const
{
  return true;
}

bool MapCalculate::fValueRangeChangeable() const
{
  return true;
}

bool MapCalculate::fExpressionChangeable() const
{
  return true;
}

/*
long MapCalculate::iComputePixelRaw(RowCol rc) const
{
  if (vr)
    return vr->iRaw(rComputePixelVal(rc));
  else if (dv())
    return longConv(rComputePixelVal(rc));
  else
    return dm()->iRaw(instruc->sMapCalc(rc));
}
*/
double MapCalculate::rComputePixelVal(RowCol rc) const
{
  assert(instruc);
  return instruc->rMapCalcVal(rc);
}

long MapCalculate::iComputePixelVal(RowCol rc) const
{
  assert(instruc);
  if (instruc->env.iLines == 0) {
    instruc->env.iLines = ptr.rcSize().Row;
    instruc->env.iCols = ptr.rcSize().Col;
    instruc->SetGeoRef(ptr.gr());
  }
  return instruc->iMapCalcVal(rc);
}

long MapCalculate::iComputePixelRaw(RowCol rc) const
{
  assert(instruc);
  if (instruc->env.iLines == 0) {
    instruc->env.iLines = ptr.rcSize().Row;
    instruc->env.iCols = ptr.rcSize().Col;
    instruc->SetGeoRef(ptr.gr());
  }
  return instruc->iMapCalcRaw(rc);
}
/*
void MapCalculate::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  if (vr != 0) {
    RealBuf b(iNum);
    ComputeLineVal(Line, b, iFrom, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = vr->iRaw(b[i]);
  }
  else if (dv() != 0) {
    RealBuf b(iNum);
    ComputeLineVal(Line, b, iFrom, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = longConv(b[i]);
  }
  else
    for (long i = 0; i < iNum; i++)
       buf[i] = iUNDEF;
}*/

void MapCalculate::ComputeLineVal(long Line, RealBuf& buf, long iFrom, long iNum) const
{
  assert(instruc);
  if (instruc->env.iLines == 0) {
    instruc->env.iLines = ptr.rcSize().Row;
    instruc->env.iCols = ptr.rcSize().Col;
    instruc->SetGeoRef(ptr.gr());
  }
  instruc->MapCalcVal(Line, buf, iFrom, iNum);
}

void MapCalculate::ComputeLineVal(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  assert(instruc);
  if (instruc->env.iLines == 0) {
    instruc->env.iLines = ptr.rcSize().Row;
    instruc->env.iCols = ptr.rcSize().Col;
    instruc->SetGeoRef(ptr.gr());
  }
  instruc->MapCalcVal(Line, buf, iFrom, iNum);
}

void MapCalculate::ComputeLineRaw(long Line, LongBuf& buf, long iFrom, long iNum) const
{
  assert(instruc);
  if (instruc->env.iLines == 0) {
    instruc->env.iLines = ptr.rcSize().Row;
    instruc->env.iCols = ptr.rcSize().Col;
    instruc->SetGeoRef(ptr.gr());
  }
  if (fUseRaw)
    instruc->MapCalcRaw(Line, buf, iFrom, iNum);
  else {
    if (fUseReal) {
      RealBuf rBuf(iNum);
      instruc->MapCalcVal(Line, rBuf, iFrom, iNum);
      if (vr().fValid())
        for (long i = 0; i < iNum; ++i)
          buf[i] = vr()->iRaw(rBuf[i]);
      else
        for (long i = 0; i < iNum; ++i)
          buf[i] = longConv(rBuf[i]);
    }
    else {
      instruc->MapCalcVal(Line, buf, iFrom, iNum);
      if (vr().fValid())
        for (long i = 0; i < iNum; ++i)
          buf[i] = vr()->iRaw(buf[i]);
    }
  }
}
/*
void MapCalculate::ComputeLineVal(long Line, StringBuf& buf, long iFrom, long iNum) const
{
  assert(instruc);
  instruc->MapCalc(Line, buf, iFrom, iNum);
}
*/

MapCalculate::~MapCalculate()
{
  if (instruc != 0)
    delete instruc;
}

String MapCalculate::sExpression() const
{
  return _sExpression;
}

/*
void MapCalculate::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvs)
{
  MapVirtual::SetDomainValueRangeStruct(dvs);
  if (dvs.dm()->pdsrt())
    if (0 != instruc) {
      instruc->ChangeConstStringsToRaw(dvs.dm());
      for (int i=0; i< instruc->admmrg.iSize(); ++i)
        instruc->admmrg[i].ApplyMergeDomain(dvs.dm());
    }
  SetUse();  
}
*/

void MapCalculate::InitFreeze()
{
  if (dm()->pdsrt())
    if (0 != instruc) 
		{
      instruc->ChangeConstStringsToRaw(dm());
      for (unsigned int i=0; i< instruc->admmrg.iSize(); ++i)
        instruc->admmrg[i].ApplyMergeDomain(dm());
    }
  if (instruc->gr().fValid())
    ptr.SetGeoRef(instruc->gr());
  else
    ptr.SetSize(instruc->rcSize());
  SetUse();  
}

void MapCalculate::SetUse()
{
  fUseRaw = false;
  if (0 != instruc) {
    if (vr().fValid() && instruc->vr().fValid())
//      fUseRaw = vr() == instruc->vr();
      fUseRaw = false;
    else if (instruc->dm()->pds())
      fUseRaw = 0 != dm()->pdsrt();
    else if (0 != dm()->pdsrt())
      fUseRaw = true;
    else
      fUseRaw = dm() == instruc->dm();
  }  
  fUseReal = false;
  if ((0 != instruc) && instruc->vr().fValid())
    fUseReal = instruc->vr()->fRealValues();
}

DomainValueRangeStruct MapCalculate::dvrsDefault(const String& sExpression) const
{
  ObjectDependency objdep;
  bool fNoGeoRef;
  Instructions* instruc = instCreate(sExpression, fnObj, objdep, fNoGeoRef);
  if (0 == instruc)
    return DomainValueRangeStruct();
  DomainValueRangeStruct dvrs(instruc->dm(), instruc->vr());
  if (dvrs.st() > stREAL)
    dvrs = Domain();
  delete instruc;
  return dvrs;
}

bool MapCalculate::fMergeDomainForCalc(Domain& dm, const String& sExpr)
{
  DomainSort* pdsrt = dm->pdsrt();
  if (0 == pdsrt)
    return true;
  ObjectDependency objdep;
  bool fNoGeoRef;
  Instructions* instruc = instCreate(sExpr, fnObj, objdep, fNoGeoRef);
  if (0 == instruc)
    return true;
  for (unsigned int i=0; i< instruc->admmrg.iSize(); ++i)
    if (!instruc->admmrg[i].fAskMergeDomain(pdsrt)) {
      delete instruc;
      return false;
    }
  SetDomainValueRangeStruct(dm);
//for (i=0; i< instruc->admmrg.iSize(); ++i)
//  instruc->admmrg[i].ApplyMergeDomain();
  delete instruc;
  return true;
}




