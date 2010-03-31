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
// $Log: /ILWIS 3.0/Column/Collsf.cpp $
 * 
 * 4     29-08-01 18:01 Koolhoven
 * in fFreezing() SetAdditionalInfo() now calls sFormula() and adds itself
 * some extra text
 * 
 * 3     9/10/99 12:34p Wind
 * comments
 * 
 * 2     9/08/99 10:13a Wind
 * adpated for quoted column  names
*/
// Revision 1.7  1998/09/16 17:25:26  Wim
// 22beta2
//
// Revision 1.6  1997/08/28 10:33:10  janh
// added if (rX==rUNDEF) return rUNDEF in function rComputeVal(long)
//
// Revision 1.5  1997/08/18 12:14:50  janh
// Freezing provided with skip records with nonvalid x or y
//
/* ColumnLeastSquaresFit
   Copyright Ilwis System Development ITC
   january 1997, by Wim Koolhoven
	Last change:  WK    1 Jul 98   10:01 am
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblstore.h"
#include "Applications\Table\COLLSF.H"
#include "Engine\Base\Algorithm\Lsf.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\DAT.hs"

static const char * sFT[] = { 0, "polynomial", "trigonometric", "power", 
                                   "exponential", "logarithmic", 0 };

IlwisObjectPtr * createColumnLeastSquaresFit(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnLeastSquaresFit::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnLeastSquaresFit(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}
                                   
ColumnLeastSquaresFit::FitType ColumnLeastSquaresFit::ftFitType(const String& s)
{
  int i = 1;
  while (sFT[i]) {
    if (strnicmp(sFT[i], s.scVal(), s.length()) == 0)
      return (FitType)i;
    i++;
  }
  return ftNONE;
}

String ColumnLeastSquaresFit::sFitType(FitType ft)
{
  return sFT[(int)ft];
}

static void FitTypeError(const String& sFitType, const FileName& fn)
{
  throw ErrorObject(WhatError(String("%S '%S'", STBLInvalidFitType, sFitType),
                    errLeastSquaresFit + 5), fn);
}

const char* ColumnLeastSquaresFit::sSyntax() 
{ 
  return "ColumnLeastSquaresFit(colX,colY,fittype,terms)"; 
}

ColumnLeastSquaresFit::ColumnLeastSquaresFit(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr), lsf(0)
{
  String s;
  ReadElement(sSection().scVal(), "FitType", s);
  ft = ftFitType(s);
  ReadElement(sSection().scVal(), "Terms", iNrTerms);
  s = String();
  ReadElement(sSection().scVal(), "ColumnX", s);
  colX = Column(s, tbl);
  s = String();
  ReadElement(sSection().scVal(), "ColumnY", s);
  colY = Column(s, tbl);
  objdep.Add(colX);
  objdep.Add(colY);
}

ColumnLeastSquaresFit::ColumnLeastSquaresFit(const Table& tbl, const String& sColName, ColumnPtr& ptr, 
                  const DomainValueRangeStruct& dvs,
                  const Column& cX, const Column& cY, FitType type, int iTerms) 
: ColumnVirtual(tbl, sColName, ptr, dvs, Table()),
  colX(cX), colY(cY), 
  iNrTerms(iTerms), ft(type),
  lsf(0)
{
  //ptr.SetDomainValueRangeStruct(colY->dvrs());
  double rStep = colY->vr()->rStep();
  ValueRange vr(-1e300,1e300,rStep);
  DomainValueRangeStruct dvrs(colY->dm(), vr);
  ptr.SetDomainValueRangeStruct(dvrs);
  objdep.Add(colX);
  objdep.Add(colY);
  ptr.fChanged = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  ColumnVirtual::Replace(sExpression());
}

ColumnVirtual* ColumnLeastSquaresFit::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
                                       const String& sExpr, const DomainValueRangeStruct& dvs)
{
/*
  Syntax:
  "ColumnLeastSquaresFit(colX,colY,fittype,terms)" 
*/  
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 3) || (iParms > 4))
    ExpressionError(sExpr, sSyntax());
  Column colX(tbl, as[0]);
  Column colY(tbl, as[1]);
  String sFitType = as[2];
  FitType ft = ftFitType(sFitType);
  if (ft == ftNONE)
    ExpressionError(sExpr, sSyntax());  
  int iTerms = 2;
  if (ft == ftTRIGONOMETRIC)
    iTerms = 3;
  if (iParms == 4)
    iTerms = as[3].shVal();
  return new ColumnLeastSquaresFit(tbl, sColName, ptr, dvs, colX, colY, ft, iTerms);
}

String ColumnLeastSquaresFit::sExpression() const
{
  if (ft == ftPOLYNOMIAL || ft == ftTRIGONOMETRIC)
    return String("ColumnLeastSquaresFit(%S,%S,%S,%li)", 
                  colX->sNameQuoted(), colY->sNameQuoted(), sFitType(ft), iNrTerms);
  else                
    return String("ColumnLeastSquaresFit(%S,%S,%S)", 
                  colX->sNameQuoted(), colY->sNameQuoted(), sFitType(ft));
}

void ColumnLeastSquaresFit::Store()
{
  ColumnVirtual::Store();
  WriteEntry("ColumnX", colX->sTableAndColumnName(fnObj.sPath()));
  WriteEntry("ColumnY", colY->sTableAndColumnName(fnObj.sPath()));
  WriteEntry("FitType", sFitType(ft));
  WriteEntry("Terms", iNrTerms);
}

void ColumnLeastSquaresFit::Replace(const String& sExpr)
{
  assert(0 == "Not implemented!");
  ColumnVirtual::Replace(sExpr);
//fFrozen = false;
}

bool ColumnLeastSquaresFit::fFreezing()
{
  if (0 == lsf) {
    int ic = colX->iOffset();
    long iNrValidPnts = 0;
    for (int i = 0; i < iRecs(); ++i, ++ic) {
      if (colX->rValue(ic) == rUNDEF || colY->rValue(ic) == rUNDEF)   
        continue;
      iNrValidPnts++;     // count all valid X,Y points
    }
    CVector cvX(iNrValidPnts), cvY(iNrValidPnts);
    long j = 0;
    ic = colX->iOffset();
    for (int i = 0; i < iRecs(); ++i, ++ic) {
      if (colX->rValue(ic) == rUNDEF || colY->rValue(ic) == rUNDEF)   
        continue;         // skip non_valid points
      cvX(j) = colX->rValue(ic);
      cvY(j) = colY->rValue(ic);
      j++;
    }

    switch (ft) 
    {
      case ftPOLYNOMIAL:
        lsf = new LeastSquaresFitPolynomial(cvX,cvY,iNrTerms);
        break;
      case ftTRIGONOMETRIC:
        lsf = new LeastSquaresFitTrigonometric(cvX,cvY,iNrTerms);
        break;
      case ftPOWER:
        lsf = new LeastSquaresFitPower(cvX,cvY);
        break;
      case ftEXPONENTIAL:
        lsf = new LeastSquaresFitExponential(cvX,cvY);
        break;
      case ftLOGARITHMIC:  
        lsf = new LeastSquaresFitLogarithmic(cvX,cvY);
        break;
      default: 
        return false;  
    }  
    String str("%S:\r\n%S", SDATMsgFitFormula, lsf->sFormula()); 
    ptr.SetAdditionalInfo(str);
    ptr.SetAdditionalInfoFlag(true);
  }
  return ColumnVirtual::fFreezing();
}

ColumnLeastSquaresFit::~ColumnLeastSquaresFit()
{
  if (0 != lsf)
    delete lsf;
}

bool ColumnLeastSquaresFit::fDomainChangeable() const
{
  return false;
}

bool ColumnLeastSquaresFit::fValueRangeChangeable() const
{
  return true;
}

double ColumnLeastSquaresFit::rComputeVal(long iKey) const
{
  if (lsf) {
    double rX = colX->rValue(iKey);
    if (rX == rUNDEF)
      return rUNDEF;  // no Y-value computed if X-value undefined
    else
      return lsf->rCalc(rX);
  }
  else
    return rUNDEF;  
}





