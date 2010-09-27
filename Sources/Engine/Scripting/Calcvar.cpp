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
/* $Log: /ILWIS 3.0/Calculator/Calcvar.cpp $
 * 
 * 7     14-11-00 9:04a Martin
 * added destructor, mainly for debugging purposes
 * 
 * 6     10-01-00 17:33 Wind
 * removed two static summy variables and changed interface of
 * CalcVariablePtr::col() and map()
 * 
 * 5     2-12-99 12:05 Wind
 * CalcVariable now maintains a list of all InstLoad 's that it is used in
 * (was only one). This information is used in the DomainMerger
 * 
 * 4     26-11-99 11:31 Wind
 * removed superfluous constructor of CalcVariablePtr
 * 
 * 3     10/18/99 9:35a Wind
 * bugs with params in user defined functions
 * 
 * 2     3/12/99 3:04p Martin
 * Added support for case insensitive
// Revision 1.3  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.2  1997/08/26 14:22:18  Wim
// Changed constructor of CalcValReal to use dvrs(-1e20,1e20,0.0001) instead of Domain("value")
//
/* calcvar.c  'Variables' for calculator
   Copyright Ilwis System Development ITC
   nov 1995, by Jelle Wind
	Last change:  WK   10 Aug 98   12:03 pm
*/
#define CALCVAR_C
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Domain\dmcoord.h"

#include "Engine\Base\System\LOGGER.H"

CalcVariable::CalcVariable()
{
//  Logger log("VarConst");
//  log.LogLine(String("%p",this));
  ptr = 0;
}

CalcVariable::~CalcVariable()
{
//  Logger log("VarDestr");
//  log.LogLine(String("%p %p",this, ptr));
  SetPointer(0);
}

void CalcVariable::SetPointer(CalcVariablePtr* p)
{ 
//  Logger log("SetPtr");
//  log.LogLine(String("%p %p %p",this, ptr, p));
  if (0 != ptr) {
    ptr->iRef--;
    if (0 == ptr->iRef) {
      delete ptr;
      ptr = 0;
    }
  }    
  ptr = p;
  if (0 != ptr)
    ptr->iRef++;
}   

CalcVariable::CalcVariable(const DomainValueRangeStruct& dvs, VarType vart, const GeoRef& gr, RowCol rc)
{
  ptr = 0;
  SetPointer(CalcVariablePtr::create(dvs, vart, gr, rc));
}


CalcVariable::CalcVariable(const DomainValueRangeStruct& dvs, VarType vart)
{
  ptr = 0;
  SetPointer(CalcVariablePtr::create(dvs, vart, GeoRef(), RowCol()));
}


long    CalcVariablePtr::iValue() const { return iUNDEF; }
double  CalcVariablePtr::rValue() const { return rUNDEF; }
String  CalcVariablePtr::sValue() const { return sUNDEF; }
Coord   CalcVariablePtr::cValue() const { return crdUNDEF; }
Color   CalcVariablePtr::clrValue() const { return Color(); }
String  CalcVariablePtr::sName() const  { return ""; }
Map  CalcVariablePtr::map() const    { return Map(); }
Column CalcVariablePtr::col() const   { return Column(); }

long   CalcVarConstInt::iValue() const { return _iVal; }
double CalcVarConstInt::rValue() const { return doubleConv(iValue()); }
String CalcVarConstInt::sValue() const { return String("%li", iValue()); }

long   CalcVarConstReal::iValue() const { return longConv(rValue()); }
double CalcVarConstReal::rValue() const { return _rVal; }
String CalcVarConstReal::sValue() const { return String("%g", rValue()); }

long   CalcVarConstString::iValue() const { return sValue().iVal(); }
double CalcVarConstString::rValue() const { return sValue().rVal(); }
String CalcVarConstString::sValue() const { return _sVal; }

Coord CalcVarConstCoord::cValue() const { return _cVal; }

Color CalcVarConstColor::clrValue() const { return _clrVal; }

String CalcVarSimple::sName() const { return _sName; }
void CalcVarSimple::PutVal(long) {}
void CalcVarSimple::PutVal(double) {}
void CalcVarSimple::PutVal(const String&) {}
void CalcVarSimple::PutVal(const Coord& cVal) {}
void CalcVarSimple::PutVal(const Color& cVal) {}

long CalcVarInt::iValue() const   { return _iVal; }
double CalcVarInt::rValue() const { return doubleConv(iValue()); }
String CalcVarInt::sValue() const { return String("%li", rValue()); }
void CalcVarInt::PutVal(long iVal)   { _iVal = iVal; }
void CalcVarInt::PutVal(int iVal)   { _iVal = iVal; }
void CalcVarInt::PutVal(double rVal) { _iVal = longConv(rVal); }
void CalcVarInt::PutVal(const String& sVal) { _iVal = sVal.iVal(); }

long CalcVarUndef::iValue() const   { return iUNDEF; }
double CalcVarUndef::rValue() const { return rUNDEF; }
String CalcVarUndef::sValue() const { return sUNDEF; }
void CalcVarUndef::PutVal(long)   { }
void CalcVarUndef::PutVal(double) { }
void CalcVarUndef::PutVal(const String&) { }

long CalcVarReal::iValue() const   { return longConv(rValue()); }
double CalcVarReal::rValue() const { return _rVal; }
String CalcVarReal::sValue() const { return String("%g", rValue()); }
void CalcVarReal::PutVal(long iVal)   { _rVal = doubleConv(iVal); }
void CalcVarReal::PutVal(double rVal) { _rVal = rVal; }
void CalcVarReal::PutVal(const String& sVal) { _rVal = sVal.rVal(); }

long CalcVarString::iValue() const   { return sValue().iVal(); }
double CalcVarString::rValue() const { return sValue().rVal(); }
String CalcVarString::sValue() const { return _sVal; }
void CalcVarString::PutVal(long iVal)   { _sVal = String("%li", iVal); }
void CalcVarString::PutVal(double rVal) { _sVal = String("%g", rVal); }
void CalcVarString::PutVal(const String& sVal) { _sVal = sVal; }

Coord CalcVarCoord::cValue() const { return _cVal; }
void CalcVarCoord::PutVal(const Coord& cVal) { _cVal = cVal; }

Color CalcVarColor::clrValue() const { return _clrVal; }
void CalcVarColor::PutVal(const Color& clrVal) { _clrVal = clrVal; }

Map CalcVarMap::map() const { return _map; }

const Table2Dim& CalcVarTable2::tb2() const 
{ return _tb2; }

String CalcVarParm::sName() const { return _sName; }
void CalcVarParm::PutVal(long) {}
void CalcVarParm::PutVal(double) {}
void CalcVarParm::PutVal(const String&) {}

ArrayCalcVariable::ArrayCalcVariable()
: Array<CalcVariable>()
{
}

ArrayCalcVariable::~ArrayCalcVariable()
{
}

CalcVariablePtr* CalcVariablePtr::create(const DomainValueRangeStruct& dvrs,
                           VarType vt, const GeoRef& grf, RowCol rcSiz)
{
  CalcVariablePtr* cv = 0;
  if (vt == vtVALUE && !dvrs.fValues())
    if (0 != dvrs.dm()->pdcrd())
      vt = vtCOORD;
    else if (0 != dvrs.dm()->pdp())
      vt = vtCOLOR;
    else if (0 != dvrs.dm()->pdcol())
      vt = vtCOLOR;
    else
      vt = vtSTRING;
  switch (vt) {
    case vtVALUE:
      { if (dvrs.fRealValues())
          cv = new CalcVarReal("");
        else  
          cv = new CalcVarInt("");
      }
      break;
    case vtSTRING:
      {
        cv = new CalcVarString("");
        cv->SetDomainValueRangeStruct(dvrs);
      }
      break;
    case vtCOLOR:
      {
        cv = new CalcVarColor("", dvrs.dm());
      }
      break;
    case vtMAP:
      {
        cv = new CalcVarMap("");
      }
      break;
    case vtCOLUMN:
      {
        cv = new CalcVarColumn("");
      }
      break;
    case vtCOORD:
      {
        Domain dm(FileName(),CoordSystem("unknown"));
        cv = new CalcVarCoord("", dm);
      }
      break;
    case vtTABLE2:
      {
        cv = new CalcVarTable2("");
      }
      break;
    default :
      break;
  }
  if (cv) {
    cv->gr = grf;
    cv->SetDomainValueRangeStruct(dvrs);
    cv->rcSize = rcSiz;
  }
  return cv;
}

CalcVariablePtr::CalcVariablePtr(const DomainValueRangeStruct& dvrs,
                           VarType vart)
: iRef(0)
{ 
  Init(dvrs, vart, GeoRef());
}

void CalcVariablePtr::Init(const DomainValueRangeStruct& dvrs,
                           VarType vart, const GeoRef& grf)
{ 
//  Logger log("VarPtrConst");
//  log.LogLine(String("%p",this));

  SetDomainValueRangeStruct(dvrs);
  vt = vart;
  gr = grf;
  switch (vt) {
    case vtVALUE:
      if (dvs.fRealValues())
        sot = sotRealVal;
      else  
        sot = sotLongVal;
      break;
    case vtSTRING:
      sot = sotStringVal;
      break;
    case vtCOORD:
      sot = sotCoordVal;
      break;
    case vtCOLOR:
      sot = sotLongRaw;
      break;
    case vtMAP:
//      sot = sotMap;
      if (dvs.fValues()) {
        if (dvs.fRealValues())
          sot = sotRealVal;
        else  
          sot = sotLongVal;
      }
      else  
        sot = sotLongRaw;
      break;
    case vtTABLE2:
    case vtCOLUMN:
//      sot = sotColumn;
      if (dvs.fValues()) {
        if (dvs.fRealValues())
          sot = sotRealVal;
        else  
          sot = sotLongVal;
      }
      else if (dvs.dm()->pds())
        sot = sotStringVal;
      else if (dvs.dm()->pdcrd())
        sot = sotCoordVal;
      else
        sot = sotLongRaw;
      break;
    default :
      sot = sotRealVal;
      break;
  }
}

CalcVariablePtr::~CalcVariablePtr()
{ 
//Logger log("VarPtrDestr");
//log.LogLine(String("%p",this));
}

bool CalcVariablePtr::fEqual(const CalcVariablePtr& cv) const
{
  if (vt != cv.vt)
    return false;
  return sName() == cv.sName();
}

CalcVarMap* CalcVariablePtr::pcvMap()
{
  return dynamic_cast<CalcVarMap*>(this);
}

CalcVarTable2* CalcVariablePtr::pcvTb2()
{
  return dynamic_cast<CalcVarTable2*>(this);
}

CalcVarColumn* CalcVariablePtr::pcvCol()
{
  return dynamic_cast<CalcVarColumn*>(this);
}

CalcVarSimple* CalcVariablePtr::pcvSimple()
{
  return dynamic_cast<CalcVarSimple*>(this);
}

CalcVarParm* CalcVariablePtr::pcvParm()
{
  return dynamic_cast<CalcVarParm*>(this);
}

CalcVarConstString* CalcVariablePtr::pcvConstString()
{
  return dynamic_cast<CalcVarConstString*>(this);
}

/*InstLoad* CalcVariablePtr::ild() const
{ 
  return  dynamic_cast<InstLoad*>(instbase);
}*/

CalcVarConstInt::CalcVarConstInt(long iVal)
: CalcVariablePtr(DomainValueRangeStruct(iVal, iVal), vtVALUE)
{ 
  _iVal = iVal; 
}

CalcVarConstReal::CalcVarConstReal(double rVal, const String& sVal)
: CalcVariablePtr(DomainValueRangeStruct(rVal, rVal, 0.0001), vtVALUE)
{ 
  // sVal is needed to determine step size
  // mathematical method causes problems due to rounding
  const char *p = strchr(sVal.scVal(), '.');
  if (0 != p) { // check numbers after decimal point, ignore trailing zeroes
    char* pEnd = const_cast<String &>(sVal).sVal()+sVal.length()-1;
    // skip trailing zeroes
    while (pEnd != p) {
      if (*pEnd != '0')
        break;
      pEnd--;
    }  
    int iDec = pEnd - p;
    dvs.vr().SetStep(pow(10.0, -iDec));
  }
  _rVal = rVal; 
}

CalcVarConstString::CalcVarConstString(const String& sVal, InstConstString* icstr)
: CalcVariablePtr(Domain("string"), vtSTRING)
{ 
  _sVal = sVal; ics = icstr; 
}

CalcVarConstCoord::CalcVarConstCoord(const Coord& cVal, const Domain& dm)
: CalcVariablePtr(dm, vtCOORD)
{ 
  _cVal = cVal; 
}

CalcVarSimple::CalcVarSimple(const DomainValueRangeStruct& dvs, VarType vart, const String& sName)
: CalcVariablePtr(dvs, vart)
{ 
  _sName = sName; 
}

CalcVarInt::CalcVarInt(const String& sName)
: CalcVarSimple(DomainValueRangeStruct(-LONG_MAX+1,LONG_MAX), vtVALUE, sName)
{ 
  _iVal = iUNDEF; 
}

CalcVarUndef::CalcVarUndef()
: CalcVarSimple(Domain("none"), vtVALUE, "undef")
{ 
  sot = sotLongRaw;
}

CalcVarReal::CalcVarReal(const String& sName)
: CalcVarSimple(DomainValueRangeStruct(-1e20,1e20,0.0001), vtVALUE, sName)
{
  _rVal = rUNDEF; 
}

CalcVarString::CalcVarString(const String& sName)
: CalcVarSimple(Domain("string"), vtSTRING, sName)
{ 
  _sVal = sUNDEF; 
}

CalcVarCoord::CalcVarCoord(const String& sName, const Domain& dm)
: CalcVarSimple(dm, vtCOORD, sName)
{ 
  _cVal = crdUNDEF; 
}
    
CalcVarColor::CalcVarColor(const String& sName, const Domain& dm)
: CalcVarSimple(dm, vtCOLOR, sName)
{ 
  _clrVal = iUNDEF; 
}
    
CalcVarIlwisObject::CalcVarIlwisObject(const String& sName, VarType vart)
  : CalcVariablePtr(Domain(),vart)
{ ptr = 0;
  _sName = sName;
}

CalcVarIlwisObject::CalcVarIlwisObject(const DomainValueRangeStruct& dvs,
                                       VarType vart, const IlwisObject& obj)
  : CalcVariablePtr(dvs, vart)
{ ptr = obj.pointer();
  _sName = ptr->sName();
}

String CalcVarIlwisObject::sName() const
{ if (ptr)
    return ptr->sName();
  else
    return _sName;
}

CalcVarMap::CalcVarMap(const String& sName)
: CalcVarIlwisObject(sName, vtMAP)
{
}

CalcVarMap::CalcVarMap(const Map& map)
    : CalcVarIlwisObject(map->dvrs(), vtMAP, map)
{ 
  SetMap(map);
}

void CalcVarMap::SetMinMax()
{
  if (dvs.fRealValues()) {
    RangeReal rr = map()->rrMinMax(false);
    if (rr.rLo() <= rr.rHi())
      dvs.SetValueRange(ValueRange(rr.rLo(), rr.rHi(), dvs.rStep()));
  }
  else {
    RangeInt ri = map()->riMinMax(false);
    if (ri.iLo() <= ri.iHi()) 
      dvs.SetValueRange(ValueRange(ri.iLo(), ri.iHi()));
  }  
}

void CalcVarMap::SetMap(const Map& mp)
{ 
  _map = mp;
  gr = map()->gr();
  if (gr->fnObj.sFile == "none")
    gr = GeoRef(map()->rcSize());
  SetDomainValueRangeStruct(map()->dvrs());
  SetMinMax();
}

CalcVarColumn::CalcVarColumn(const String& sName)
: CalcVarIlwisObject(sName, vtCOLUMN)
{
}

CalcVarColumn::CalcVarColumn(const Column& col)
  : CalcVarIlwisObject(col->dvrs(), vtCOLUMN, col)
{
  SetColumn(col); 
}

Column CalcVarColumn::col() const   
{ 
  return _col; 
}

void CalcVarColumn::SetMinMax()
{
  if (dvs.fRealValues()) {
    RangeReal rr = col()->rrMinMax();
    if (rr.rLo() <= rr.rHi())
      dvs.SetValueRange(ValueRange(rr.rLo(), rr.rHi(), dvs.rStep()));
  }
  else {
    RangeInt ri = col()->riMinMax();
    if (ri.iLo() <= ri.iHi()) 
      dvs.SetValueRange(ValueRange(ri.iLo(), ri.iHi()));
  }  
}

void CalcVarColumn::SetColumn(const Column& cl)
{
   _col = cl;
  SetDomainValueRangeStruct(col()->dvrs());
  SetMinMax();
}

CalcVarTable2::CalcVarTable2(const String& sName)
: CalcVarIlwisObject(sName, vtTABLE2)
{ 
}

CalcVarTable2::CalcVarTable2(const Table2Dim& tb2)
  : CalcVarIlwisObject(tb2->dvrsVal(), vtTABLE2, tb2)
{
  SetTable2(tb2);
}

CalcVarTable2::~CalcVarTable2()
{
}

void CalcVarTable2::SetTable2(const Table2Dim& tb2)
{
   _tb2 = tb2;
  SetDomainValueRangeStruct(tb2->dvrsVal());
  SetMinMax();
}

void CalcVarTable2::SetMinMax()
{
  if (dvs.fRealValues()) {
    RangeReal rr = _tb2->colValue()->rrMinMax();
    if (rr.rLo() <= rr.rHi())
      dvs.SetValueRange(ValueRange(rr.rLo(), rr.rHi(), dvs.rStep()));
  }
  else {
    RangeInt ri = _tb2->colValue()->riMinMax();
    if (ri.iLo() <= ri.iHi()) 
      dvs.SetValueRange(ValueRange(ri.iLo(), ri.iHi()));
  }  
}

CalcVarParm::CalcVarParm(const DomainValueRangeStruct& dvs, VarType vart, const String& sName)
: CalcVariablePtr(dvs, vart)
{ 
  _sName = sName; 
}




