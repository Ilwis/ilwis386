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

#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )

#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\DomainTime.h"
#include "Engine\Base\DataObjects\valrange.h"

static const double rDELTA=1.0e-9;

static RangeInt riConv(const RangeReal& rr) 
{ 
  RangeInt ri(longConv(rr.rLo()), longConv(rr.rHi())); 
  if (ri.iLo() == iUNDEF)
    ri.iLo() = -LONG_MAX+1;
  if (ri.iHi() == iUNDEF)
    ri.iHi() = LONG_MAX;
  return ri;
}  

ValueRange::ValueRange() 
{
  ptr = 0;
}
/*
ValueRange::ValueRange(ValueRangePtr* p) 
{
  ptr = 0;
  SetPointer(p);
}*/

ValueRange::~ValueRange()
{
  SetPointer(0);
}

void ValueRange::SetPointer(ValueRangePtr* p)
{ 
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

ValueRange::ValueRange(const Domain& dom)
{
  ptr = 0;
  if (dom->pdvr())
    SetPointer(new ValueRangeReal(dom));
  else if (dom->pdvi())
    SetPointer(new ValueRangeInt(dom));
  else if (dom->pdbool())
    SetPointer(new ValueRangeInt(0,1));
  else if (dom->pdbit())
    SetPointer(new ValueRangeInt(0,1));
  else if (dom->pdi())
    SetPointer(new ValueRangeInt(0,255));
}

ValueRange::ValueRange(const ValueRange& vr)
{
  ptr = 0;
  SetPointer(vr.ptr);
}
 
ValueRange::ValueRange(const RangeInt& ri)
{
  ptr = 0;
  SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi()));
}

ValueRange::ValueRange(double rMin, double rMax, double rStep)
{
  ptr = 0;
  if (rStep < 0)
    rStep = 0;
  if (rStep == 1){
    double r = rMin - rMax;
    if ((rMin > -LONG_MAX) && (rMax < LONG_MAX) && (r < LONG_MAX)) {
      RangeInt ri =  riConv(RangeReal(rMin, rMax));
      SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi(), iUNDEF));
      return;
    }
  }  
  SetPointer(new ValueRangeReal(rMin, rMax, rStep));
}

ValueRange::ValueRange(const RangeReal& rr, double rStep)
{
  ptr = 0;
  if (rStep < 0)
    rStep = 0;
  if (rStep == 1){
    double r = rr.rLo() - rr.rHi();
    if ((rr.rLo() > -LONG_MAX) && (rr.rHi() < LONG_MAX) && (r < LONG_MAX)) {
      RangeInt ri =  riConv(rr);
      SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi(), iUNDEF));
      return;
    }  
  }  
  SetPointer(new ValueRangeReal(rr.rLo(), rr.rHi(), rStep));
}

ValueRange::ValueRange(const ILWIS::TimeInterval& tiv)
{
  ptr = 0;
  double rStep = tiv.getStep();
  if (rStep < 0)
    rStep = 0;
  SetPointer(new ValueRangeReal(tiv));
}
 
ValueRange::ValueRange(const ValueRangeReal& rr)
{
  ptr = 0;
  if (rr.rStep() == 1){
    double r = rr.rLo() - rr.rHi();
    if ((rr.rLo() > -LONG_MAX) && (rr.rHi() < LONG_MAX) && (r < LONG_MAX)) {
      RangeInt ri =  riConv(rr);
      SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi(), iUNDEF));
      return;
    }  
  }  
  SetPointer(new ValueRangeReal(rr.rLo(), rr.rHi(), rr.rStep()));
}
 
ValueRange::ValueRange(const String& sRng)
{
  ptr = 0;
  String sRange = sRng;
  char *p1 = strchr(sRange.sVal(), ':');
  if (0 == p1)
    return;
  char *p3 = strstr(sRange.sVal(), ",offset=");
  if (0 == p3)
    p3 = strstr(sRange.sVal(), ":offset=");
  long iRaw0 = iUNDEF;
  double rRaw0 = rUNDEF;
  if (0 != p3) {
    iRaw0 = String(p3+8).iVal();
    rRaw0 = String(p3+8).rVal();
    *p3 = 0;
  }
  char *p2 = strrchr(sRange.sVal(), ':');
  double rStep = 1;
  bool fStep = false;
  if (p1 != p2) { // step
    fStep = true;
    rStep = String(p2+1).rVal();
    *p2 = 0;
  }
  RangeInt ri(String(sRange.sVal()));
  RangeReal rr(String(sRange.sVal()));
  if ((rr.rHi() == ri.iHi()) && (rr.rLo() == ri.iLo()) && (rStep == 1)) {
    if (fStep) {
      double r = rr.rHi() - rr.rLo();
      if ((rr.rHi() > -LONG_MAX) && (rr.rLo() < LONG_MAX) && (r < LONG_MAX)) {
        SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi(), iRaw0));
        return;
      }  
    }
    else  {
      SetPointer(new ValueRangeInt(ri.iLo(), ri.iHi(), iRaw0));
      return;
    }  
  }  
  SetPointer(new ValueRangeReal(rr.rLo(), rr.rHi(), rStep, rRaw0));
}

void ValueRange::SetStep(double rStp) 
{
  if (0 == ptr)
    return;
  (*this) = ValueRange(ptr->rrMinMax().rLo(), ptr->rrMinMax().rHi(), rStp);
/*
  if (0 != ptr->vri() && (rStp != 1)) {
    SetPointer(new ValueRangeReal(rrMinMax().rLo(), rrMinMax().rHi(), rStp);
  }
  else if (0 != ptr->vrr())
    vrr()->SetStep(rStp);*/
}


ValueRangePtr::ValueRangePtr() 
{
  iRef = 0;
}

RangeReal ValueRangePtr::rrMinMax() const
  { return RangeReal(); }

RangeInt ValueRangePtr::riMinMax() const
  { return RangeInt(); }

double ValueRangePtr::rStep() const
{
  return 1;
}

int ValueRangePtr::iDec() const
{ 
  return 0;
}

void ValueRangePtr::AdjustRangeToStep()
{
}

bool ValueRangePtr::fRealValues() const
{
  return false;
}

bool ValueRangePtr::fContains(const ValueRange& vr, bool includeStep) {
	if ( !vr.fValid())
		return false;
	RangeReal rr = vr->rrMinMax();
	if ( rrMinMax().fContains(rr.rLo()) && rrMinMax().fContains(rr.rHi())) {
		if ( !includeStep)
			return true;
		return vr->rStep() == rStep();
	}
	return false;
}

bool ValueRangePtr::fEqual(const ValueRangePtr&) const
{
  return false;
}

String ValueRangePtr::sValue(double rValue, short w, short dec) const
{
  return sUNDEF;
}

static double rDefaultRaw0(const RangeReal& rr, double rStep, StoreType st)
{
  if (st == stREAL)
    return 0;
  double rMinDivStep = rr.rLo();
  double rMaxDivStep = rr.rHi();
  if (rStep > 1e-30) {
    rMinDivStep /= rStep;
    rMaxDivStep /= rStep;
  }
  double r0 = 0;
  double r = rounding(rMinDivStep / 2 + rMaxDivStep / 2 - 0.0001);
  r0 = r;
  switch (st) {
    case stREAL:
      r0 = 0;
      break;
    case stLONG:
      if (rMinDivStep < -LONG_MAX || rMaxDivStep > LONG_MAX)
        r0 = rMinDivStep / 2 + rMaxDivStep / 2 - 0.0001;
      else
        r0 = 0;
      break;
    case stINT:
      if (rMinDivStep < -SHRT_MAX || rMaxDivStep > SHRT_MAX)
        r0 = rMinDivStep / 2 + rMaxDivStep / 2 - 0.0001;
      else
        r0 = 0;
      break;
    case stBYTE:
      if (rMinDivStep < 0 || rMaxDivStep > 255)
        r0 = rMinDivStep-1;
      else
        r0 = -1;
      break;
    case stNIBBLE:
      if (rMinDivStep < 0 || rMaxDivStep > 15)
        r0 = rMinDivStep-1;
      else
        r0 = -1;
      break;
    case stDUET:
      if (rMinDivStep < 0 || rMaxDivStep > 3)
        r0 = rMinDivStep-1;
      else
        r0 = -1;
      break;
    case stBIT:
      r0 = rMinDivStep-1;
  }
  return r0;
}

static long iDefaultRaw0(const RangeInt& ri, StoreType st)
{
  long i0;
  switch (st) {
    case stLONG:
      if (ri.iLo() < -LONG_MAX || ri.iHi() > LONG_MAX)
        i0 = ri.iLo() / 2 + ri.iHi() / 2;
      else
        i0 = 0;
      break;
    case stINT:
      if (ri.iLo() < -SHRT_MAX || ri.iHi() > SHRT_MAX)
        i0 = ri.iLo() / 2 + ri.iHi() / 2;
      else
        i0 = 0;
      break;
    case stBYTE:
      if (ri.iLo() < 1 || ri.iHi() > 255)
        i0 = ri.iLo() - 1;
      else
        i0 = -1;
      break;
    case stNIBBLE:
      if (ri.iLo() < 1 || ri.iHi() > 15)
        i0 = ri.iLo() - 1;
      else
        i0 = -1;
      break;
    case stDUET:
      if (ri.iLo() < 1 || ri.iHi() > 3)
        i0 = ri.iLo() - 1;
      else
        i0 = -1;
      break;
    case stBIT:
      i0 = ri.iLo() - 1;
  }
  return i0;
}

bool ValueRange::fEqual(const ValueRange& vr) const
{
  if (fValid() && vr.fValid()) {
    if ((void*)ptr == (void*)vr.ptr)
      return true;
    return ptr->fEqual(*vr.ptr);
  }  
  return false;
/*if (fValid())
    throw ErrorObject(String("Comparison of %S with uninitialized Object", ptr->sTypeName()) , 1000);
  if (obj.fValid())
    throw ErrorObject(String("Comparison of %S with uninitialized Object", obj->sTypeName()) , 1000);
  throw ErrorObject("Comparison with uninitialized Object", 1000);*/
}

double ValueRangePtr::getOffset() const{
	return rUNDEF;
}

ValueRangeInt* ValueRangePtr::vri() const
{
  return dynamic_cast<ValueRangeInt*>(const_cast<ValueRangePtr*>(this));
}

ValueRangeReal* ValueRangePtr::vrr() const
{
  return dynamic_cast<ValueRangeReal*>(const_cast<ValueRangePtr*>(this));
}

ValueRangeInt::ValueRangeInt() :
  ValueRangePtr(), RangeInt(0,255)
{
  _i0 = 0;
  iRawUndef = shUNDEF;
  st = stBYTE;
}

ValueRangeInt::ValueRangeInt(const ValueRangeInt& vr)
  : ValueRangePtr(), RangeInt(vr.iLo(), vr.iHi())
{
  init();
}

ValueRangeInt::ValueRangeInt(long min, long max, long iRaw0)
  : ValueRangePtr(), RangeInt(min,max)
{
  init(iRaw0);
}

ValueRangeInt::ValueRangeInt(const RangeInt& ri)
  : ValueRangePtr(), RangeInt(ri)
{
  init();
}

ValueRangeInt::ValueRangeInt(const Domain& dm) 
   : ValueRangePtr(), RangeInt()
{
  DomainValue* dv = dm->pdv();
  if (0 != dv) {
    RangeInt ri = dv->riMinMax();
    iHi() = ri.iHi();
    iLo() = ri.iLo();
  }
  else {
    iHi() = iLo() = 0;
  }
  init();
}

bool ValueRangeInt::fEqual(const ValueRangePtr& vr) const
{
  const ValueRangeInt* vri = dynamic_cast<const ValueRangeInt*>(&vr);
  if (vri == 0)
    return false;
  return (vri->iLo() == iLo()) && (vri->iHi() == iHi()) && (vri->iRaw0() == iRaw0());
}

double ValueRangeInt::rStep() const
{
  return 1;
}

long ValueRangeInt::iDefaultRaw0() const
{
  return ::iDefaultRaw0(riMinMax(),  stUsed());
}

void ValueRangeInt::init(long iRaw0)
{
//  if (iHi() < iLo())  to enable invalid ranges 30/7/97
//    iHi() = iLo();
  int w1;
  if (iLo() == 0)
    w1 = 1;
  else
    w1 = (int)floor(log10((double)abs(iLo()))) + 1 + (iLo() > 0 ? 0 : 1);
  if (iHi() > 0) {
    int w2 = (int)floor(log10((double)iHi())) + 1;
    _iWidth = max(w1, w2);
  }
  else
    _iWidth = w1;
  st = DomainPtr::st(iHi()-iLo()+2);
  if (st < stBYTE) // this should be left as-is; required for BOOL data to work properly, otherwise they don't get their iRaw offset set to -1
    st = stBYTE;
  if (iRaw0 == iUNDEF)
    _i0 = iDefaultRaw0();
  else {
    _i0 = iRaw0;
    // adjust store type if necessary
    long iNrUnder0 = iRaw0 - iLo() + 1;
    long iNrAbove0 = iHi() - iRaw0;
    switch (st) {
      case stBYTE:
         if (iNrUnder0 > 0)
           st = stINT;
         break;
      case stINT:
         if ((iNrUnder0 > SHRT_MAX) || (iNrAbove0 > SHRT_MAX))
           st = stLONG;
         break;
      default :
        break;
    }
  }
  if (st > stINT)
    iRawUndef = iUNDEF;
  else if (st == stINT)
    iRawUndef = shUNDEF;
  else
    iRawUndef = 0;
}

long ValueRangeInt::iValue(long iRaw) const
{
  if (iUNDEF == iRaw || iRawUndef == iRaw)
    return iUNDEF;
  long iVal = _i0 + iRaw;
  if ((iVal < iLo()) || (iVal > iHi()))
    return iUNDEF;
  return iVal;
}

double ValueRangeInt::rValue(long iRaw) const
{
  return doubleConv(iValue(iRaw));
}

String ValueRangeInt::sValue(double rValue, short w, short dec) const
{
  if (w < 0)
    w = _iWidth;
  if (rValue == rUNDEF)
    return String("%*s", w, "?");
  String s;
  long iValue = longConv(rValue);
  if (w == 0)
    s = String("%li", iValue);
  else
    s = String("%*li", w, iValue);
  if ((w > 0) && (s.length() > (unsigned)w)) // too large
    s = String('*', w);
  return s;
}

long ValueRangeInt::iRaw(long iValue) const
{
  if (iValue == iUNDEF) // || !fContains(iValue))
    return iUNDEF;
  if (iValue < iLo())
    return iUNDEF;
//    iValue = iLo();
  else if (iValue > iHi())
    return iUNDEF;
//    iValue = iHi();
  iValue -= _i0;
  return iValue;
}

long ValueRangeInt::iRaw(double rValue) const
{
  return iRaw(longConv(rValue));
}

String ValueRangeInt::sValueByRaw(const Domain& dm, long iRaw,
                                  short w, short) const
{
  DomainValue* dv = dm->pdv();
  if (0 == dv)
    return sUNDEF;
  long iVal = iValue(iRaw);
  if (w < 0)
    w = _iWidth;
  return dv->sValue(iVal,w);
}
/*
ValueRange* ValueRangeInt::vrCopy() const
{
  return new ValueRangeInt(iLo(), iHi(), iRaw0());
}*/

RangeReal ValueRangeInt::rrMinMax() const
  { return RangeReal(iLo(), iHi()); }

RangeInt ValueRangeInt::riMinMax() const
  { return RangeInt(iLo(), iHi()); }

String ValueRangeInt::sRange(bool fInternal) const
{
  String s("%li:%li", iLo(), iHi());
  if (fInternal/* && (iRaw0() != iDefaultRaw0())*/)
    s &= String(":offset=%li", iRaw0());
  return s;
}

//////////////////////

ValueRangeReal::ValueRangeReal() :
  ValueRangePtr()
{
}

ValueRangeReal::ValueRangeReal(const ValueRangeReal& vr)
  : ValueRangePtr(), RangeReal(vr.rLo(), vr.rHi()), _rStep(vr.rStep())
{
  init();
}

ValueRangeReal::ValueRangeReal(double min, double max, double step, double rRaw0)
  : ValueRangePtr(), RangeReal(min,max), _rStep(step)
{
  init(rRaw0);
}

ValueRangeReal::ValueRangeReal(const ILWIS::TimeInterval& tiv)
  : ValueRangePtr(), RangeReal(tiv.rLo(), tiv.rHi()), _rStep(tiv.getStep())
{
	st = stREAL;
	_iDec = 10;
	_r0 = 0;
}

ValueRangeReal::ValueRangeReal(const RangeReal& rr, double step)
  : ValueRangePtr(), RangeReal(rr), _rStep(step)
{
  init();
}

ValueRangeReal::ValueRangeReal(const Domain& dm) 
  : ValueRangePtr(), RangeReal()
{
  DomainValue* dv = dm->pdv();
  if (0 != dv) {
    RangeReal rr = dv->rrMinMax();
    rHi() = rr.rHi();
    rLo() = rr.rLo();
    if (dm->pdvr())
      _rStep = dm->pdvr()->rStep();
    else
      _rStep = 1;
  }
  else {
    rHi() = rLo() = 0; _rStep = 1;
  }
  init();
}

bool ValueRangeReal::fEqual(double r1, double r2)
{
	double rDelta1 = abs(r1 * 0.00001);
	double r1Min = r1 - rDelta1;
	double r1Max = r1 + rDelta1;
	double rDelta2 = abs(r2 * 0.00001);
	double r2Min = r2 - rDelta2;
	double r2Max = r2 + rDelta2;
	bool fInr2 = r1 >= r2Min && r1 <= r2Max;
	bool fInr1 = r2 >= r1Min && r2 <= r1Max;
	return fInr1 || fInr2;
}

bool ValueRangeReal::fEqual(const ValueRangePtr& vr) const
{
  ValueRangeReal* vrrl = vr.vrr();
  if (vrrl == 0)
    return false;
	bool f1 = fEqual(vrrl->rLo(), rLo());
	bool f2 = fEqual(vrrl->rHi() , rHi());
	bool f3 = fEqual(vrrl->rStep() , rStep());
	bool f4 = fEqual(vrrl->rRaw0() , rRaw0());
	return f1 && f2 && f3 && f4;
}

double ValueRangeReal::rDefaultRaw0() const
{
  try {
    return ::rDefaultRaw0(rrMinMax(), rStep(), stUsed());
  }
  catch (const ErrorObject&) {
    return 0;
  }
}

void ValueRangeReal::AdjustRangeToStep()
{
  if (_rStep > 1e-30) {
    if (rHi() > 0) {
      double rMod = fmod(rHi(), _rStep);
      if ((rMod > 1e-8) && (fabs(rMod-_rStep) > 1e-8)) {
        rHi() -= rMod;
        rHi() += _rStep;
      }
    }
    else if (rHi() < 0) {
      double rMod = fmod(rHi(), _rStep);
      if ((rMod < 0) && (fabs(rMod+_rStep) > 1e-8)) {
        rHi() -= rMod;
      }
    }
    if (rLo() > 0) {
      double rMod = fmod(rLo(), _rStep);
      if ((rMod > 1e-8) && (fabs(rMod-_rStep) > 1e-8)) {
        rLo() -= rMod;
      }
    }
    else if (rLo() < 0) {
      double rMod = fmod(rLo(), _rStep);
      if ((rMod < 0) && (fabs(rMod+_rStep) > 1e-8)) {
        rLo() -= rMod;
        rLo() -= _rStep;
      }
    }
  }
  init();
}

void ValueRangeReal::init(double rRaw0)
{
  try {
    _iDec = 0;
    if (_rStep < 0)
      _rStep = 0;
    double r = _rStep;
    if (r <= 1e-20)
      _iDec = 3;
    else while (r - floor(r) > 1e-20) {
      r *= 10;
      _iDec++;
      if (_iDec > 10)
        break;
    }
//    if (rHi() < rLo())     to enable invalid ranges
//      rHi() = rLo() + 1;

    short iBeforeDec = 1;
    double rMax = max(fabs(rLo()), fabs(rHi()));
    if (rMax != 0)
      iBeforeDec = floor(max(0,log10(rMax))) + 1;
    if (rLo() < 0)
      iBeforeDec++;
    _iWidth = iBeforeDec + _iDec;
    if (_iDec > 0)
      _iWidth++;
    if (_iWidth > 12)
      _iWidth = 12;
/*
    double rMinDivStep = rLo();
    double rMaxDivStep = rHi();
    if (_rStep > 1e-30) {
      rMinDivStep /= _rStep;
      rMaxDivStep /= _rStep;
    }
*/
    if (_rStep < 1e-06)
		{
      st = stREAL;
			_rStep = 0;
		}			
    else {
      r = rHi() - rLo();
      if (r <= ULONG_MAX) {
        r /= _rStep;
        r += 1;
      }
      r += 1; // Jelle - 23-dec-1996 : add one for undefined
//      if (r > ULONG_MAX)
      if (r > LONG_MAX)
        st = stREAL;
      else {
        st = DomainPtr::st(floor(r+0.5));
        if (st < stBYTE)
          st = stBYTE;
      }
    }
    if (rRaw0 == rUNDEF)
      _r0 = rDefaultRaw0();
    else {
      _r0 = rRaw0;
      if (st != stREAL) {
        // adjust store type if necessary
        long iNrUnder0 = round(_r0 - (rLo() / _rStep) + 1);
        long iNrAbove0 = round((rHi() / _rStep) - _r0);
        switch (st) {
          case stBYTE:
             if (iNrUnder0 > 0)
               st = stINT;
             // fall through
          case stINT:
             if ((iNrUnder0 >= SHRT_MAX) || (iNrAbove0 > SHRT_MAX))
               st = stLONG;
             // fall through
          case stLONG:
             if ((iNrUnder0 >= LONG_MAX) || (iNrAbove0 > LONG_MAX))
               st = stREAL;
             break;
          default :
            break;
        }
      }
    }
    if (st > stINT)
      iRawUndef = iUNDEF;
    else if (st == stINT)
      iRawUndef = shUNDEF;
    else
      iRawUndef = 0;
  }
  catch (const ErrorFloatingPoint&) {
    st = stREAL;
    _r0 = 0;
    _rStep = 0.0001;
    rHi() = DBL_MAX;
    rLo() = -DBL_MAX;
    iRawUndef = iUNDEF;
  }
}

double ValueRangeReal::rStep() const 
{ 
  return _rStep; 
}

long ValueRangeReal::iValue(long iRaw) const
{
  return longConv(rValue(iRaw));
}

double ValueRangeReal::rValue(long iRaw) const
{
  if (stREAL == st)
    return rUNDEF;
  if (iRaw == iUNDEF || iRaw == iRawUndef)
    return rUNDEF;
  double rVal = iRaw + _r0;
  rVal *= _rStep;
  if (rLo() == rHi())
    return rVal;
  double rEpsilon = _rStep == 0.0 ? 1e-6 : _rStep / 3.0; // avoid any rounding problems with an epsilon directly based on the
	// the stepsize
  if ((rVal - rLo() < -rEpsilon) || (rVal - rHi() > rEpsilon))
    return rUNDEF;
  return rVal;
}

long ValueRangeReal::iRaw(long iValue) const
{
  return iRaw((double)iValue);
}

long ValueRangeReal::iRaw(double rValue) const
{
  if (stREAL == st)
    return iUNDEF;
  if (rValue == rUNDEF) // || !fContains(rValue))
    return iUNDEF;
  double rEpsilon = _rStep == 0.0 ? 1e-6 : _rStep / 3.0;	
  if (rValue - rLo() < -rEpsilon) // take a little rounding tolerance
    return iUNDEF;
//    rValue = rLo();
  else if (rValue - rHi() > rEpsilon) // take a little rounding tolerance
    return iUNDEF;
//    rValue = rHi();
  rValue /= _rStep;
  double rVal = floor(rValue+0.5);
  rVal -= _r0;
  long iVal;
  iVal = longConv(rVal);
 // iVal -= _i0;
  return iVal;
//  return round(rValue) - _i0;
}

String ValueRangeReal::sValueByRaw(const Domain& dm, long iRaw,
                           short w, short dec) const
{
  DomainValue* dv = dm->pdv();
  if (0 == dv)
    return sUNDEF;
  if (w < 0)
    w = _iWidth;
  if (dec < 0)
    dec = _iDec;
  double rVal = rValue(iRaw);
  return dv->sValue(rVal,w,dec);
}
/*
ValueRange* ValueRangeReal::vrCopy() const
{
  return new ValueRangeReal(rLo(), rHi(), rStep(), iRaw0());
}*/

String ValueRangeReal::sValue(double rValue, short w, short dec) const
{
  if (w < 0)
    w = _iWidth;
  if (rValue == rUNDEF)
    return String("%*s", w, "?");
  String s;
  long iValue = longConv(rValue);
	if (dec < 0) 
	{
    dec = _iDec;
    // 9/6/99 Wim:
    // in FieldReal values smaller than 0.001
    // got rounded to 0.
    if (rStep() == 0 && abs(rValue) < 0.1 && abs(rValue) > 0.0001)
      dec = max(dec,6);
  }  
  if (dec < 0)
    if (w == 0)
      s = String("%g", rValue);
    else
      s = String("%*g", w, rValue);
  else if (dec > 0 || iValue == iUNDEF) 
	{
    if (w == 0) 
		{
      if (dec == 0)
        s = String("%.*f", dec, rValue);
			else if (rStep() == 0 && abs(rValue) <= 0.0001 && abs(rValue) > 1e-20) 
        s = String("%.*e", dec, rValue);
      else
        s = String("%#.*f", dec, rValue);
      if (s.length() > 18 && s.length() > dec + 6)
        s = String("%.*e", dec, rValue);
    }
    else 
		{
      if (dec == 0)
        s = String("%*.*f", w, dec, rValue);
      else if (rStep() == 0 && abs(rValue) <= 0.0001 && abs(rValue) > 1e-20) 
			{
        s = String("%*.*e", w, dec, rValue);
        short dif = s.length() - w;
        //if (dif <= dec)
          s = String("%*.*e", w, dec-dif, rValue);
        //else  // too large
        //  s = String('*', w);
      }
      else 
			{
        s = String("%#*.*f", w, dec, rValue);
				short dif = s.length() - w;
				if (dif > 0) 
				{
					if (dif <= dec)
						s = String("%#*.*f", w, dec-dif, rValue);
					else 
					{
						s = String("%#*.*g", w, dec, rValue);
						dif = s.length() - w;
						if (dif <= dec)
							s = String("%#*.*g", w, dec-dif, rValue);
						else  // too large
							s = String('*', w);
					}
				}
      }
    }
  }
  else {
    s = String("%*li", w, iValue);
    if ((w > 0) && (s.length() > w)) {
      dec = 6;
      s = String("%*.*e", w, dec, rValue);
      short dif = s.length() - w;
      if (dif > 0) {
        if (dif <= dec)
          s = String("%#*.*e", w, dec-dif, rValue);
        else  // too large
          s = String('*', w);
      }
    }
  }
  return s;
}

RangeReal ValueRangeReal::rrMinMax() const
{
	RangeReal rr = RangeReal(rLo(), rHi());
	return rr;
}

RangeInt ValueRangeReal::riMinMax() const
  { 
    return riConv(RangeReal(rLo(), rHi())); 
  }

String ValueRangeReal::sRange(bool fInternal) const
{
  String s;
  if (abs(rLo()) > 1e20 || abs(rHi()) > 1e20)
    s = String("%g:%g:%f", rLo(), rHi(), rStep());
  else if (iDec() >= 0)
    s = String("%.*f:%.*f:%.*f", iDec(), rLo(), iDec(), rHi(), iDec(), rStep());
  else
    s = String("%f:%f:%f", rLo(), rHi(), rStep());
   if (fInternal)
    if (abs(rLo()) > 1e20 || abs(rHi()) > 1e20)
      s &= String(":offset=%g", rRaw0());
    else
      s &= String(":offset=%.0f", rRaw0());
  return s;
}


bool ValueRangeReal::fRealValues() const
{
	return true;
}

int ValueRangeReal::iDec() const
{ 
	return _iDec; 
}  

////  DomainValueRangeStruct

DomainValueRangeStruct::DomainValueRangeStruct()

{
}

DomainValueRangeStruct::DomainValueRangeStruct(const DomainValueRangeStruct& dvs)
{ 
	SetDomain(dvs.dm()); 
	SetValueRange(dvs.vr()); 
}

DomainValueRangeStruct::DomainValueRangeStruct(const Domain& dm)
{
	SetDomain(dm, true); 
}

DomainValueRangeStruct::DomainValueRangeStruct(const Domain& dm, const ValueRange& vr)
{
	SetDomain(dm, true); // set also default range
	if (_vr.fValid() && vr.fValid())
		SetValueRange(vr); 
}

DomainValueRangeStruct::DomainValueRangeStruct(const ValueRange& vr)
{
	SetDomain(Domain("value"));
	SetValueRange(vr);
}

DomainValueRangeStruct::DomainValueRangeStruct(long iMin, long iMax)
{ 
	SetDomain(Domain("value")); 
	SetValueRange(ValueRange(iMin, iMax)); 
}

DomainValueRangeStruct::DomainValueRangeStruct(double rMin, double rMax, double rStep)
{ 
	SetDomain(Domain("value")); 
	SetValueRange(ValueRange(rMin, rMax, rStep)); 
}

DomainValueRangeStruct::~DomainValueRangeStruct()
{
}

void DomainValueRangeStruct::operator=(const DomainValueRangeStruct& dvs)
{ 
	SetDomain(dvs.dm()); 
	SetValueRange(dvs.vr()); 
}

void DomainValueRangeStruct::SetDomain(const Domain& dm, bool fDefaultValueRange)
{
	_dm = dm;
	if (0 == dm->pdv())
		_vr = ValueRange();
	_vr = ValueRange(dm);
	SetPrivateMembers();
}

void DomainValueRangeStruct::SetValueRange(const ValueRange& vr)
{
	DomainValue* dv = dm()->pdv();
	if (0 == dv)
		return;
	bool fKeepPrecision = dv->fFixedPrecision();
	bool fKeepRange = dv->fFixedRange();
	if (vr.fValid() && !vr->rrMinMax().fValid())
		fKeepRange = true;
	if (!_vr.fValid())
		fKeepPrecision = fKeepRange = false;
	
	if (fKeepPrecision)
	{
		if (fKeepRange)
			return;
		else
		{
			if (!vr.fValid())
				return;
			RangeReal rr = vr->rrMinMax();
			_vr = ValueRange(rr, _vr->rStep());
		}
	}
	else
	{
		if (fKeepRange)
		{
			if (!vr.fValid())
				return;
			RangeReal rr = _vr->rrMinMax();
			_vr = ValueRange(rr, vr->rStep());
		}
		else
		{
			_vr = vr;
		}
	}
	SetPrivateMembers();
}

String DomainValueRangeStruct::sValueByRaw(long iRaw, short iWidth, short iDec) const
{
	if (0 == dm()->pdbool() && vr().fValid())
		return vr()->sValueByRaw(dm(), iRaw, iWidth, iDec);
	return dm()->sValueByRaw(iRaw, iWidth, iDec);
}

String DomainValueRangeStruct::sValue(long iValue, short iWidth) const
{
	if (!fValues())
		return sUNDEF;
	if (vr().fValid())
		return sValueByRaw(iRaw(iValue), iWidth);
	return dm()->pdv()->sValue(iValue, iWidth);
}

String DomainValueRangeStruct::sValue(double rValue, short iWidth, short iDec) const
{
	if (!fValues())
		return sUNDEF;
	if (vr().fValid() && dm()->pdtime() == 0)
	{
		long raw = iRaw(rValue);
		if (raw != iUNDEF)
			return sValueByRaw(raw, iWidth, iDec);
		if (vr()->vrr() )
			return vr()->vrr()->sValue(rValue, iWidth, iDec);
	}
	return dm()->pdv()->sValue(rValue, iWidth, iDec);
}

String DomainValueRangeStruct::sValue(const Coord& cValue, short iWid) const
{
	DomainCoord* pdcrd = dm()->pdcrd();
	if (0 == pdcrd)
		return sUNDEF;
	return pdcrd->sValue(cValue, iWid);
}

long DomainValueRangeStruct::iRaw(const String& sValue) const
{
	if (fValues())
	{
		if (fRealValues())
			return vr()->iRaw(sValue.rVal());
		else if (fRawIsValue())
			return sValue.iVal();
		else if (vr().fValid())
			if (!fStringRep())
				return vr()->iRaw(sValue.iVal());
	}
	return dm()->iRaw(sValue);
}

long DomainValueRangeStruct::iValue(long iRaw) const
{
	if (!fValues())
		return iUNDEF;
	if (fRawIsValue())
		return iRaw;
	return vr().fValid() ? vr()->iValue(iRaw) : dm()->pdv()->iValueByRaw(iRaw);
}

double DomainValueRangeStruct::rValue(long iRaw) const
{
	if (!fValues())
		return rUNDEF;
	if (fRawIsValue())
		return doubleConv(iRaw);
	if (vr().fValid())
		return vr()->rValue(iRaw);
	if (0 == dm()->pdvr())
		return doubleConv(dm()->pdv()->iValueByRaw(iRaw));
	return dm()->pdvr()->rValueByRaw(iRaw);
}

double DomainValueRangeStruct::rValue(const String& sValue) const
{
	if ( dm()->pdtime())
		return dm()->pdtime()->tValue(sValue);
	if (!fValues())
		return rUNDEF;
	return sValue.rVal();
}

long DomainValueRangeStruct::iRaw(long iValue) const
{
	if (!fValues())
		return iUNDEF;
	if (fRawIsValue())
		return iValue;
	if (vr().fValid())
		return vr()->iRaw(iValue);
	return dm()->pdv()->iRaw(iValue);
}

long DomainValueRangeStruct::iRaw(double rValue) const
{
	if (!fValues())
		return iUNDEF;
	if (fRawIsValue())
		return longConv(rValue);
	if (vr().fValid())
		return vr()->iRaw(rValue);
	if (0 == dm()->pdvr())
		return dm()->pdv()->iRaw(longConv(rValue));
	return dm()->pdvr()->iRaw(rValue);
}

bool DomainValueRangeStruct::fValid(const String& sValue) const
{
	if (0 == dm()->pdbool() && vr().fValid()) {
		double r = dm()->pdtime() ? ILWIS::Time(sValue) : sValue.rVal();
		RangeReal rr = vr()->rrMinMax();
		return ((r >= rr.rLo()) && (r <= rr.rHi()));
	}
	else
		return dm()->fValid(sValue);
}

RangeInt DomainValueRangeStruct::riMinMax() const
{
	if (vr().fValid())
		return vr()->riMinMax();
	if (fValues())
		return dm()->pdv()->riMinMax();
	return RangeInt();
}

RangeReal DomainValueRangeStruct::rrMinMax() const
{
	if (vr().fValid())
		return vr()->rrMinMax();
	if (fValues())
		return dm()->pdv()->rrMinMax();
	return RangeReal();
}

StoreType DomainValueRangeStruct::st() const
{
	if (!dm().fValid())
		return stBIT;  // the first in the enum

	if ( dm()->pdtime())
		return stREAL;

	return !vr().fValid() || 0 != dm()->pdi() ? dm()->stNeeded() : vr()->stUsed();
}

bool DomainValueRangeStruct::fUseReals() const
{
	if (!dm().fValid())
		return false;

	return st() == stREAL;
}

bool DomainValueRangeStruct::fValues() const
{
	if (!dm().fValid())
		return false;

	return 0 != dm()->pdv();
}

bool DomainValueRangeStruct::fRawAvailable() const
{
	if (!dm().fValid())
		return false;

	return (st() != stREAL) && (st() != stSTRING) && (st() != stCRD) && (st() != stCRD3D) && (st() != stBINARY) && (st() != stCOORDBUF) ;
}

bool DomainValueRangeStruct::fRawIsValue() const
{
	if (!dm().fValid())
		return false;

	return fValues() && !vr().fValid() && (0 == dm()->pdbool());
}

bool DomainValueRangeStruct::fRealValues() const
{
	if (!dm().fValid())
		return false;

	bool fRealValues = fUseReals();
	if (!fRealValues && fValues() && vr().fValid())
		fRealValues = vr()->rStep() != 1.0;
	return fRealValues;
}

bool DomainValueRangeStruct::fStringRep() const
{
	if (!dm().fValid())
		return false;

	return 0 != dm()->pdbool();
}

double DomainValueRangeStruct::rStep() const
{
	if (!fValues())
		return rUNDEF;
	if (vr().fValid())
		return vr()->rStep();
	else
		return dm()->pdv()->rStep();
}

long DomainValueRangeStruct::iWidth() const
{
	if (!dm().fValid())
		return 0;
	return dm()->iWidth();
}

long DomainValueRangeStruct::iDec() const
{
	if (vr().fValid())
		if (0 != vr()->vrr())
			return vr()->vrr()->iDec();
		DomainCoord* pdcrd = dm()->pdcrd();
		if (0 != pdcrd)
			return pdcrd->cs()->iDec();
		return 0;
}

void DomainValueRangeStruct::SetPrivateMembers()
{
// Changed into a no-op; the status functions now calculate their values
// based on the currently stored Domain/ValueRange
// instead of relying on members that are only set once.
}

bool DomainValueRangeStruct::operator == (const DomainValueRangeStruct& dvs) const
{
  if (!dm().fValid() || !dvs.dm().fValid())
    return false;
  if (dm() != dvs.dm())
    return false;
  if (!vr().fValid() && !dvs.vr().fValid())
    return true;
  return vr() == dvs.vr();
}

long DomainValueRangeStruct::iRawUndef() const
{
  if (_vr.fValid())
    return _vr->iUndef();
  if ((st()==stBYTE) && ((0 != dm()->pdbool()) || (0 != dm()->pdsrt())))
    return 0;
  return iUNDEF;
}

bool DomainValueRangeStruct::fCoords() const
{
  return 0 != dm()->pdcrd();
}
