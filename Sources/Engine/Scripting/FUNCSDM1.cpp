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
/* funcsdm1.c
	Last change:  WK   10 Sep 97    7:15 pm
*/

#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\DataObjects\Buf.h"
static double min2(double r1, double r2)
{
  return r1 < r2 ? r1 : r2;
}

static double max2(double r1, double r2)
{
  return r1 > r2 ? r1 : r2;
}

void FuncDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (acv[0]->dvs.fRealValues())
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  RangeReal rr = acv[0]->dvs.rrMinMax();
  double rStep;
  if (rr.rLo() >= rr.rHi())
    rStep = 0.0001;
  else 
    rStep = min2(0.0001, acv[0]->dvs.rStep());
  double rLo;
  try {
    rLo = fm(rr.rLo());
  }
  catch (...) {
    rLo = rUNDEF;
  }
  if (rLo == rUNDEF) {
    rLo = fm(fabs(rr.rLo()));
    if ((rLo == rUNDEF) && (rr.rLo() == 0))
      rLo = fm(1.0e-10);
  }
  double rHi;
  try {
    rHi = fm(rr.rHi());
  }
  catch (...) {
    rHi = rUNDEF;
  }
  if (rHi == rUNDEF) {
    rHi = fm(fabs(rr.rLo()));
    if ((rHi == rUNDEF) && (rr.rLo() == 0))
      rHi = fm(1.0e-10);
  }
  if (rLo == rUNDEF || rHi == rUNDEF) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0.0001;
  }
  if (rLo > rHi) {
    double r = rLo; rLo = rHi; rHi = r;
  }
  if (rr.fContains(0)) {
    double r = fm(0);
    if (r == rUNDEF || rHi == rUNDEF) {
      rLo = -1e100;
      rHi = 1e100;
      rStep = 0.0001;
    }
    if (r < rLo)
      rLo = r;
    else if (r > rHi)
      rHi = r;  
  }
//  rLo -= rStep;  // gerommel in de marge
//  rHi += rStep;
/*
  if (rLo == rHi) { // to ensure pocket calculator proper use
    rLo /= 2;
    rHi *= 2;
  }  */
  sot = sotRealVal;
  ValueRange vrr(rLo, rHi, rStep);
  dvs = DomainValueRangeStruct(Domain(sDomName), vrr);
}

void ExpDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (acv[0]->dvs.fRealValues())
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  RangeReal rr = acv[0]->dvs.rrMinMax();
  double rStep;
  if (rr.rLo() >= rr.rHi())
    rStep = 0.0001;
  else 
    rStep = min2(0.0001, acv[0]->dvs.rStep());
  if (rr.rLo() > 300)
    rr.rLo() = 300;
  if (rr.rHi() > 300)
    rr.rHi() = 300;
  double rLo;
  try {
    rLo = fm(rr.rLo());
  }
  catch (...) {
    rLo = rUNDEF;
  }
  if (rLo == rUNDEF) {
    rLo = fm(fabs(rr.rLo()));
    if ((rLo == rUNDEF) && (rr.rLo() == 0))
      rLo = fm(1.0e-10);
  }
  double rHi;
  try {
    rHi = fm(rr.rHi());
  }
  catch (...) {
    rHi = rUNDEF;
  }
  if (rHi == rUNDEF) {
    rHi = fm(fabs(rr.rLo()));
    if ((rHi == rUNDEF) && (rr.rLo() == 0))
      rHi = fm(1.0e-10);
  }
  if (rLo == rUNDEF || rHi == rUNDEF) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0.0001;
  }
  if (rLo > rHi) {
    double r = rLo; rLo = rHi; rHi = r;
  }
  if (rr.fContains(0)) {
    double r = fm(0);
    if (r == rUNDEF || rHi == rUNDEF) {
      rLo = -1e100;
      rHi = 1e100;
      rStep = 0.0001;
    }
    if (r < rLo)
      rLo = r;
    else if (r > rHi)
      rHi = r;  
  }
//  rLo -= rStep;  // gerommel in de marge
//  rHi += rStep;
/*
  if (rLo == rHi) { // to ensure pocket calculator proper use
    rLo /= 2;
    rHi *= 2;
  }  */
  sot = sotRealVal;
  ValueRange vrr(rLo, rHi, rStep);
  dvs = DomainValueRangeStruct(Domain(sDomName), vrr);
}

void FuncNonNegDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (acv[0]->dvs.fRealValues())
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  RangeReal rr = acv[0]->dvs.rrMinMax();
  double rStep;
  if (rr.rLo() >= rr.rHi())
    rStep = 0.0001;
  else 
    rStep = min2(0.0001, acv[0]->dvs.rStep());
  if (rr.rLo() < 0)
    rr.rLo() = 0;
  if (rr.rHi() < 0)
    rr.rHi() = 0;
  double rLo = fm(rr.rLo());
  double rHi = fm(rr.rHi());
  if (rLo == rUNDEF || rHi == rUNDEF) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0.0001;
  }
  if (rLo > rHi) {
    double r = rLo; rLo = rHi; rHi = r;
  }
  if (rr.fContains(0)) {
    double r = fm(0);
    if (r == rUNDEF || rHi == rUNDEF) {
      rLo = -1e100;
      rHi = 1e100;
      rStep = 0.0001;
    }
    if (r < rLo)
      rLo = r;
    else if (r > rHi)
      rHi = r;  
  }
//  rLo -= rStep;  // gerommel in de marge
//  rHi += rStep;
/*
  if (rLo == rHi) { // to ensure pocket calculator proper use
    rLo /= 2;
    rHi *= 2;
  }  */
  sot = sotRealVal;
  ValueRange vrr(rLo, rHi, rStep);
  dvs = DomainValueRangeStruct(Domain(sDomName), vrr);
}

void FuncPosDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (acv[0]->dvs.fRealValues())
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  RangeReal rr = acv[0]->dvs.rrMinMax();
  double rStep;
  if (rr.rLo() >= rr.rHi())
    rStep = 0.0001;
  else 
    rStep = min2(0.0001, acv[0]->dvs.rStep());
  if (rr.rLo() <= 0)
    rr.rLo() = 1e-10;
  if (rr.rHi() <= 0)
    rr.rHi() = 1e-10;
  double rLo = fm(rr.rLo());
  double rHi = fm(rr.rHi());
  if (rLo == rUNDEF || rHi == rUNDEF) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0.0001;
  }
  if (rLo > rHi) {
    double r = rLo; rLo = rHi; rHi = r;
  }
  if (rr.fContains(0)) {
    double r = fm(0);
    if (r == rUNDEF || rHi == rUNDEF) {
      rLo = -1e100;
      rHi = 1e100;
      rStep = 0.0001;
    }
    if (r < rLo)
      rLo = r;
    else if (r > rHi)
      rHi = r;  
  }
//  rLo -= rStep;  // gerommel in de marge
//  rHi += rStep;
/*
  if (rLo == rHi) { // to ensure pocket calculator proper use
    rLo /= 2;
    rHi *= 2;
  }  */
  sot = sotRealVal;
  ValueRange vrr(rLo, rHi, rStep);
  dvs = DomainValueRangeStruct(Domain(sDomName), vrr);
}

void HypoDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (acv[0]->dvs.dm()->sName() == acv[1]->dvs.dm()->sName())
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  if (sDomName == "image")
    sDomName = "value";
  RangeReal rr0 = acv[0]->dvs.rrMinMax();
  RangeReal rr1 = acv[1]->dvs.rrMinMax();
  double rStep;
  rStep = min2(acv[0]->dvs.rStep(), acv[1]->dvs.rStep());
  double rLo, rHi, r1, r2;
  if (rr0.fContains(0) && rr1.fContains(0)) 
    rLo = 0;
  else {
    double r1 = min2(fabs(rr0.rLo()), fabs(rr0.rHi()));
    double r2 = min2(fabs(rr1.rLo()), fabs(rr1.rHi()));
    rLo = sqrt(r1*r1+r2*r2);
  }
  r1 = max2(fabs(rr0.rLo()), fabs(rr0.rHi()));
  r2 = max2(fabs(rr1.rLo()), fabs(rr1.rHi()));
  rHi = sqrt(r1*r1+r2*r2);
	double dummy;
	if ( rStep == 1 && modf(rHi, &dummy) !=0.0)
		rStep = 0.0001;
  if (rLo == rUNDEF || rHi == rUNDEF) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0.0001;
  }
  if (rLo > rHi) {
    double r = rLo; rLo = rHi; rHi = r;
  }
  sot = sotRealVal;
  ValueRange vrr(rLo, rHi, rStep);
  dvs = DomainValueRangeStruct(Domain(sDomName), vrr);
}

void FuncDomainsInt(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv)
{
  RangeReal rr= acv[0]->dvs.rrMinMax();
  long iLo = longConv(fm(rr.rLo()));
  long iHi = longConv(fm(rr.rHi()));
  if (iLo == iUNDEF)
    iLo = -LONG_MAX;
  if (iHi == iUNDEF)
    iHi = LONG_MAX;
  String sDomName("Value");
  dvs = DomainValueRangeStruct(Domain(sDomName), ValueRange(iLo, iHi));
  sot = sotLongVal;
}

void StringDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>&)
{
  dvs = DomainValueRangeStruct(Domain("string"));
  sot = sotStringVal;
}

void IffDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>& acv)
{
  if (acv[1]->dvs.dm()->pdnone() ||acv[2]->dvs.dm()->pdnone()) {
    if (acv[1]->dvs.dm()->pdnone())
      dvs = acv[2]->dvs;
    else {
      dvs = acv[1]->dvs;
    }
    if (dvs.fRealValues())
      sot = sotRealVal;
    else if (dvs.fValues())
      sot = sotLongVal;
    else if (dvs.fRawAvailable())
      sot = sotLongRaw;
    else
      sot = sotStringVal;
    return;
  }
  if (acv[1]->dvs.fValues()) {
    ValueRange vr;
    String sDomName;
    if (acv[2]->dvs.dm()->sName() == acv[1]->dvs.dm()->sName())
      sDomName = acv[1]->dvs.dm()->sName();
    else {
      sDomName = "value";
      // if one is image and the other is a constant between 0..255 return image
      // if one is bool and the other is a constant 0 or 1 return the bool one
      CalcVarConstInt* cvci1 = dynamic_cast<CalcVarConstInt*>(acv[1].ptr);
      CalcVarConstInt* cvci2 = dynamic_cast<CalcVarConstInt*>(acv[2].ptr);
      if (acv[1]->dvs.dm()->pdi() && 0 != cvci2)
        if (cvci2->iValue() >= 0 && cvci2->iValue() <= 255)
          sDomName = "image";
      if (acv[2]->dvs.dm()->pdi() && 0 != cvci1)
        if (cvci1->iValue() >= 0 && cvci1->iValue() <= 255)
          sDomName = "image";
      if (acv[1]->dvs.dm()->pdbool() && 0 != cvci2)
        if (cvci2->iValue() == 0 && cvci2->iValue() == 1)
          sDomName = acv[1]->dvs.dm()->sName();
      if (acv[2]->dvs.dm()->pdbool() && 0 != cvci1)
        if (cvci1->iValue() == 0 && cvci1->iValue() == 1)
          sDomName = acv[2]->dvs.dm()->sName();
    }
    if (acv[1]->dvs.fRealValues() || acv[2]->dvs.fRealValues()) {
      RangeReal rr1 = acv[1]->dvs.rrMinMax();
      RangeReal rr2 = acv[2]->dvs.rrMinMax();
      double rStep = 1;
      rStep = min2(rStep, acv[1]->dvs.rStep());
      rStep = min2(rStep, acv[2]->dvs.rStep());
      vr = ValueRange(min2(rr2.rLo(),rr1.rLo()),
                      max2(rr2.rHi(),rr1.rHi()),rStep);
    }
    else {
      RangeInt ri1 = acv[1]->dvs.riMinMax();
      RangeInt ri2 = acv[2]->dvs.riMinMax();
      vr = ValueRange(min2(ri2.iLo(),ri1.iLo()),
                              max2(ri2.iHi(),ri1.iHi()));
    }
    dvs = DomainValueRangeStruct(Domain(sDomName), vr);
    if (dvs.fRealValues())
      sot = sotRealVal;
    else
      sot = sotLongVal;
  }
  else { // string, class or id
    if (acv[2]->dvs.dm()->pds()) {
      CalcVariable cv = acv[2];
      cv->SetDomainValueRangeStruct(acv[1]->dvs);
//      CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cv.ptr);
//      if (cvs)
//        if (cvs->ics)
//          cvs->ics->cv->SetDomainValueRangeStruct(acv[1]->dvs);
    }
    else if (acv[1]->dvs.dm()->pds()) {
      CalcVariable cv = acv[1];
      cv->SetDomainValueRangeStruct(acv[2]->dvs);
//      CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cv.ptr);
//      if (cvs)
//        if (cvs->ics)
//          cvs->ics->cv->SetDomainValueRangeStruct(acv[2]->dvs);
    }
    dvs = acv[2]->dvs;
    if ((acv[1]->dvs.dm()->pds()) && (acv[1]->dvs.dm()->pds()))
      sot = sotStringVal;
    else
      sot = sotLongRaw;
  }
}
void IfUndefDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath fm, const Array<CalcVariable>& acv)
{
  Array<CalcVariable> acvIff;
  CalcVariable cv;
  cv.SetPointer(new CalcVarUndef());
  acvIff &= cv;
  acvIff &= acv[0];//->cvCopy();
  acvIff &= acv[1];//->cvCopy();
  IffDomain(dvs, sot, fm, acvIff);
}

void IfNotUndefDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath fm, const Array<CalcVariable>& acv)
{
  Array<CalcVariable> acvIff;
  CalcVariable cv;
  cv.SetPointer(new CalcVarUndef());
  acvIff &= cv;
  acvIff &= acv[1];//->cvCopy(); // add the same parameter twice: in ifnotundef with 2 params the first parameter is only used to check on undef;
  acvIff &= acv[1];//->cvCopy(); // unlike with ifundef, the values only come from the 2nd parameter
  IffDomain(dvs, sot, fm, acvIff);
}

static RangeReal undef (const RangeReal&) { return RangeReal(); }

void MinDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>& acv)
{
  Array<RangeReal> rr(acv.iSize());
  RealArray rStep(acv.iSize());
  for (int i=0; i < acv.iSize(); i++) {
    rStep[i] = acv[i]->dvs.rStep();
    rr[i] = acv[i]->dvs.rrMinMax();
  }
  double rLo = 1e100;
  double rHi = 1e100;
  double rStp = 1;
  for (int i=0; i < acv.iSize(); i++) {
    rLo = min2(rLo, rr[i].rLo());
    rHi = min2(rHi, rr[i].rHi());
    rStp = min2(rStp, rStep[i]);
  }
  Domain dm("value");
  if (acv[0]->dvs.dm()->sName() == acv[1]->dvs.dm()->sName())
    dm = acv[0]->dvs.dm();
  if ((rStp == 1) && (rLo >= -LONG_MAX) && (rHi <= LONG_MAX)) {
    dvs = DomainValueRangeStruct(dm, ValueRange(longConv(rLo), longConv(rHi)));
    sot = sotLongVal;
  }
  else {
    dvs = DomainValueRangeStruct(dm, ValueRange(rLo, rHi, rStp));
    sot = sotRealVal;
  }
}

void MaxDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                FuncMath, const Array<CalcVariable>& acv)
{
  Array<RangeReal> rr(acv.iSize());
  RealArray rStep(acv.iSize());
  for (int i=0; i < acv.iSize(); i++) {
    rStep[i] = acv[i]->dvs.rStep();
    rr[i] = acv[i]->dvs.rrMinMax();
  }
  double rLo = -1e100;
  double rHi = -1e100;
  double rStp = 1;
  for (int i=0; i < acv.iSize(); i++) {
    rLo = max2(rLo, rr[i].rLo());
    rHi = max2(rHi, rr[i].rHi());
    rStp = min2(rStp, rStep[i]);
  }
  Domain dm("value");
  if (acv[0]->dvs.dm()->sName() == acv[1]->dvs.dm()->sName())
    dm = acv[0]->dvs.dm();
  if ((rStp == 1) && (rLo >= -LONG_MAX) && (rHi <= LONG_MAX)) {
    dvs = DomainValueRangeStruct(dm, ValueRange(longConv(rLo), longConv(rHi)));
    sot = sotLongVal;
  }
  else {
    dvs = DomainValueRangeStruct(dm, ValueRange(rLo, rHi, rStp));
    sot = sotRealVal;
  }
}

void DegDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                FuncMath, const Array<CalcVariable>& acv)
{
  dvs = DomainValueRangeStruct(Domain("value"), ValueRange(0,360, 0.01));
  sot = sotRealVal;
}




