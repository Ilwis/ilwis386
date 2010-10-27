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
// $Log: /ILWIS 3.0/Function/FLTLIN.cpp $
 * 
 * 4     5-01-00 18:11 Wind
 * removed limitation of 8000
 * 
 * 3     9/10/99 1:13p Wind
 * comments
 * 
 * 2     9/10/99 12:39p Wind
 * adapted for changes in FileName constructors
*/
// Revision 1.4  1998/09/16 17:30:33  Wim
// 22beta2
//
// Revision 1.3  1997/09/03 08:48:53  Wim
// Explicitly initialize iMat
//
// Revision 1.2  1997-08-20 08:52:51+02  Wim
// FilterLinear::sName() now returns formula when now correct filename is available
//
/* FilterLinear
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  WK    3 Sep 97   10:33 am
*/

#include "Engine\Function\FLTLIN.H"
#include "Engine\Scripting\Instrucs.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\ILWISDAT.ERR"
#include "Headers\Hs\DAT.hs"

class DATEXPORT ErrorLinearFilterSize : public ErrorObject 
{
public:  
  ErrorLinearFilterSize(int iRows, int iCols, const WhereError& where)
  : ErrorObject(WhatError(String("%S %i x %i", SDATErrFilterTooLarge, iRows, iCols), errFilter+1), where) 
  {}
};

void LinearFilterSizeError(int iRows, int iCols, const FileName& fn)
{
  throw ErrorLinearFilterSize(iRows, iCols, fn);
}

FilterLinear* FilterLinear::create(const FileName& fn)
{

  String sType;
  ObjectInfo::ReadElement("FilterLinear", "Type", fn, sType);
  if (sType == "FilterAverage")
    return new FilterAverage(fn);
  return new FilterLinear(fn);
//  throw ErrorInvalidType(fn, "FilterLinear", sType);
}

FilterLinear::FilterLinear(const FileName& fn)
: FilterPtr(fn), iMat(0)
{
  ReadElement("FilterLinear", "UseRealMatrix", fUseRealMat);
  if (fUseRealMat)
    ReadElement("FilterLinear", "Matrix", rMat);
  else
    ReadElement("FilterLinear", "Matrix", iMat);
  rGain = rReadElement("FilterLinear", "Gain");
  ReadElement("FilterLinear", "Expression", sExpression);
  if (sExpression.length() != 0)
    FillMatrix(sExpression);
  // downward compatib with 2.02 :
  bool f;
  if (0 != ReadElement("FilterLinear", "DomainChangeable", f))
    SetDomainChangeable(f);
  if (0 != ReadElement("FilterLinear", "ValueRangeChangeable", f))
    SetValueRangeChangeable(f);
}

FilterLinear::FilterLinear(const FileName& fn, const IntMatrix& matrix,
                           double rGn, const Domain& dmDefault)
: FilterPtr(fn, matrix.iRows(), matrix.iCols(), dmDefault), iMat(matrix), 
  fUseRealMat(false), rGain(rGn)
{
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
}

FilterLinear::FilterLinear(const FileName& fn, const RealMatrix& matrix,
                           double rGn, const Domain& dmDefault)
: FilterPtr(fn, matrix.iRows(), matrix.iCols(), dmDefault), rMat(matrix), 
  fUseRealMat(true), rGain(rGn), iMat(0)
{
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
}

FilterLinear::FilterLinear(const FileName& fn, int iRows, int iCols, bool fUseReal)
: FilterPtr(fn, iRows, iCols, Domain()), 
  fUseRealMat(fUseReal), rGain(1), iMat(0)
{
//  if ((long)iRows * iCols > 8000)
//    LinearFilterSizeError(iRows, iCols, fnObj);
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
  if (fUseRealMat) {
    rMat = RealMatrix(iFltRows, iFltCols);
    for (int i=0; i < iFltRows; i++)
      for (int j=0; j < iFltCols; j++)
        rMat(i, j) = 0;
    rMat(iFltRows/2, iFltCols/2) = 1;
  }  
  else {
    iMat = IntMatrix(iFltRows, iFltCols);
    for (int i=0; i < iFltRows; i++)
      for (int j=0; j < iFltCols; j++)
        iMat(i, j) = 0;
    iMat(iFltRows/2, iFltCols/2) = 1;
  }  
}

FilterLinear::FilterLinear(const FileName& fn, int iRows, int iCols, const String& sExpr, const Domain& dmDefault)
: FilterPtr(fn, iRows, iCols, dmDefault), 
  fUseRealMat(true), sExpression(sExpr), rGain(1), iMat(0)
{
//  if ((long)iRows * iCols > 8000)
//    LinearFilterSizeError(iRows, iCols, fnObj);
  SetDomainChangeable(true);
  SetValueRangeChangeable(true);
  FillMatrix(sExpression);
}

FilterLinear::~FilterLinear()
{
}

void FilterLinear::Store()
{
  FilterPtr::Store();
  WriteElement("Filter", "Type", "FilterLinear");
  if (fUseRealMat) {
    WriteElement("FilterLinear", "UseRealMatrix", true);
    WriteElement("FilterLinear", "Matrix", rMat);
  }  
  else {
    WriteElement("FilterLinear", "UseRealMatrix", false);
    WriteElement("FilterLinear", "Matrix", iMat);
  }  
  WriteElement("FilterLinear", "Gain", rGain);
  if (sExpression.length() != 0)
    WriteElement("FilterLinear", "Expression", sExpression);
  else
    WriteElement("FilterLinear", "Expression", (const char*) 0);
}

Domain FilterLinear::dmDefault(const Map& mp) const
{
  if (!mp->fValues())
    return Domain();  // linear filter not defined for non value maps
  if (fDomainChangeable())
    return Domain("value");
  return mp->dm();
/*  
  Domain dom = FilterPtr::dmDefault(map);
  ValueRange* vr = FilterPtr::vrDefault(map, dom);
  if (0 != vr) { // check
  }
  return dom;*/
}

ValueRange FilterLinear::vrDefault(const Map& mp, const Domain& dm) const
{
  RangeReal rr = mp->rrMinMax();
  if (!rr.fValid()) {
    ValueRange vr = FilterPtr::vrDefault(mp, dm);
    if (vr.fValid())
      rr = vr->rrMinMax();
    return vr; 
  }  
  // sum all neg. and pos values in kernel and multiply by max and min in map resp.
  // to find extremes
  double rSumNegKernel = 0;
  double rSumPosKernel = 0;
  if (fUseRealMat) {
    for (int i=0; i < iFltRows; i++)
      for (int j = -(iFltCols>>1); j <= (iFltCols>>1) ; j++) {
        double r = rMat(i, j);
        if (r < 0)
          rSumNegKernel += r;
        else
          rSumPosKernel += r;
      }
  }
  else {
    for (int i = 0; i < iFltRows ; i++)
      for (int j = 0; j < iFltCols; j++) {
        double r = iMat(i, j);
        if (r < 0)
          rSumNegKernel += r;
        else
          rSumPosKernel += r;
      }
  }
  double r1 = rSumNegKernel*rr.rHi();
  double r2 = rSumPosKernel*rr.rLo();
  double rLo = r1+r2;
  rLo *= rGain;
  double r3 = rSumNegKernel*rr.rLo();
  double r4 = rSumPosKernel*rr.rHi();
  double rHi = r3+r4;
  rHi *= rGain;
  double rStep = 0.1;
  if (rLo == rHi) { // use default
    ValueRange vr = FunctionPtr::vrDefault();
    if (!vr.fValid())
      return ValueRange();
    rr = vr->rrMinMax();
    rStep = vr->rStep();
    rLo = rr.rLo();
    rHi = rr.rHi();
  }
  return ValueRange(rLo, rHi, rStep);
}

bool FilterLinear::fRawAllowed() const
{
  return false;
}

void FilterLinear::ExecuteRaw(const LongBufExt* /*bufListInp*/, LongBuf& bufRes)
{
  for (int i=0; i < bufRes.iSize() ; i++) {
    bufRes[i] = iUNDEF;
  }
}

void FilterLinear::ExecuteVal(const RealBufExt* bufLstInp, RealBuf& bufRes)
{
  double rVal;
  int i, j, c, cc;
  double r;
  for (c = 0; c < bufRes.iSize() ; c++) {
    rVal = 0;
    for (i = 0; i < iFltRows ; i++)
      for (j = 0, cc = c-(iFltCols>>1); cc <= c+(iFltCols>>1) ; cc++, j++) {
        r = bufLstInp[i][cc];
        if (r == rUNDEF) {
          rVal = rUNDEF; goto label1;
        }
        else if (fUseRealMat)
          rVal += r * rMat(i, j);
        else
          rVal += r * iMat(i, j);
      }
    if (!fUseRealMat)  
      rVal *= rGain;
label1:
    bufRes[c] = rVal;
  }
}

FilterLinear* FilterLinear::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(3);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(fn.sFullName(), sExpr);
  int iRows = as[0].shVal();
  int iCols = as[1].shVal();
  if ((as[2] == "int") || (as[2] == "real"))
    return new FilterLinear(fn, iRows, iCols, as[2]=="real");
  return new FilterLinear(fn, iRows, iCols, as[2]);
}

void FilterLinear::FillMatrix(const String& sExpression)
{
  rMat = RealMatrix(iFltRows, iFltCols);
  String sExpr = "int x, y; real r; ";
  sExpr &= sExpression; 
  Instructions* inst = Instructions::instSimple(sExpr);
  Array<CalcVariable> acv;
  inst->GetVars(acv);

  CalcVarInt* cvX=0, *cvY=0;
  CalcVarReal* cvR=0;
  for (int i = 0; i < acv.iSize(); i++) {
    if (acv[i]->sName() == "x")
      cvX = dynamic_cast<CalcVarInt*>(acv[i].ptr);
    else if (acv[i]->sName() == "y")
      cvY = dynamic_cast<CalcVarInt*>(acv[i].ptr);
    else if (acv[i]->sName() == "r")
      cvR = dynamic_cast<CalcVarReal*>(acv[i].ptr);
  }
  assert(cvX || cvY || cvR);
  for (int i=0; i < iFltRows; i++)
    for (int j=0; j < iFltCols; j++) {
      if (0 != cvX)
        cvX->PutVal((long)i-iFltRows/2);
      if (0 != cvY)
        cvY->PutVal((long)j-iFltCols/2);
	  if (0 != cvR) {
		  double ssq = sqr(((long)i-iFltRows/2)+(double)sqr((long)j-iFltCols/2));
        cvR->PutVal(sqrt(ssq));
	  }
      rMat(i,j) = inst->rCalcVal();
      if (rMat(i, j) == rUNDEF)
        rMat(i, j) = 0;
    }
  delete inst;
}

String FilterLinear::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  s = String("FilterLinear(%i,%i,%S)", iFltRows, iFltCols, sExpression);
  return s;
}

const char* FilterAverage::sSyntax()
{
  return "Average(rows,cols)";
}

FilterAverage* FilterAverage::create(const FileName& fn, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  short iFltRows = as[0].shVal();
  short iFltCols = as[1].shVal();
//  if ((long)iFltRows * iFltCols > 8000)
//    LinearFilterSizeError(iFltRows, iFltCols, fn);
  return new FilterAverage(fn, iFltRows, iFltCols);
}

FilterAverage::FilterAverage(const FileName& fn)
: FilterLinear(fn)
{
}

FilterAverage::FilterAverage(const FileName& fn, short iRows, short iCols)
: FilterLinear(fn, IntMatrix(iRows, iCols), 1.0/(iRows*iCols), Domain())
{
  for (short i=0; i < iFltRows; i++ )
    for (short j=0; j < iFltCols; j++ )
      iMat(i, j) = 1;
  SetDomainChangeable(false);
  SetValueRangeChangeable(false);
}

FilterAverage::~FilterAverage()
{
}

Domain FilterAverage::dmDefault(const Map& mp) const
{
  return FilterPtr::dmDefault(mp);
}

ValueRange FilterAverage::vrDefault(const Map& mp, const Domain& dm) const
{
  return FilterPtr::vrDefault(mp, dm);
}

void FilterAverage::Store()
{
  FilterLinear::Store();
  WriteElement("FilterLinear", "Type", "FilterAverage");
}

String FilterAverage::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("Average(%i,%i)", iFltRows, iFltCols);
}




