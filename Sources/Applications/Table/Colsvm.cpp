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
/* ColumnSemiVariogram
   Copyright Ilwis System Development ITC
   january 1999, by Jan Hendrikse
	Last change:  WK    8 Jul 99    9:59 am
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblstore.h"
#include "Applications\Table\COLSVM.H"
#include "Engine\Base\Algorithm\semivar.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"


const char* ColumnSemiVariogram::sSyntax() 
{
  return "ColumnSemiVariogram(colX,modeltype(nugget,sill|slope,range|power))";
}

IlwisObjectPtr * createColumnSemiVariogram(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnSemiVariogram::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnSemiVariogram(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

ColumnSemiVariogram::ColumnSemiVariogram(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr), svm()
{
  String s;
  ReadElement(sSection().c_str(), "ColumnX", s);
  colX = Column(s, tbl);
  String sSect("%S:Semivariogram", sSection());
  svm = SemiVariogram(fnObj, sSect.c_str());
  objdep.Add(colX);
}

ColumnSemiVariogram::ColumnSemiVariogram(const Table& tbl, const String& sColName, ColumnPtr& ptr, 
                  const DomainValueRangeStruct& dvs,
                  const Column& cX, const SemiVariogram& sv )
: ColumnVirtual(tbl, sColName, ptr, dvs, Table()),
  colX(cX), svm(sv)
{ 
  if (!colX->fValues() || colX->dvrs().fStringRep())
    ValueDomainError(colX->dm()->sName(true, fnObj.sPath()), sName(), errColumnSemiVariogram);
  ValueRange vr(-1e300,1e300,0);
  DomainValueRangeStruct dvrs(vr);
  ptr.SetDomainValueRangeStruct(dvrs);
  objdep.Add(colX);
 
  ptr.fChanged = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  ColumnVirtual::Replace(sExpression());
}

ColumnVirtual* ColumnSemiVariogram::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
                                       const String& sExpr, const DomainValueRangeStruct& dvs)
{
/*
  Syntax:
  "ColumnSemiVariogram(colX,modeltype(nugget,sill|slope,range|power))"
*/
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());
  Column clX(tbl, as[0]);
  SemiVariogram sv(as[1]);
  return new ColumnSemiVariogram(tbl, sColName, ptr, dvs, clX, sv);
}

String ColumnSemiVariogram::sExpression() const
{
    return String("ColumnSemiVariogram(%S,%S)",
                  colX->sName(), svm.sExpression());
}

void ColumnSemiVariogram::Store()
{
  ColumnVirtual::Store();
  String sSect("%S:Semivariogram", sSection());
  svm.Store(fnObj, sSect.c_str());
  WriteEntry("ColumnX", colX->sTableAndColumnName(fnObj.sPath()));
}

void ColumnSemiVariogram::Replace(const String& sExpr)
{
  assert(0 == "Not implemented!");
  ColumnVirtual::Replace(sExpr);
//fFrozen = false;
}

ColumnSemiVariogram::~ColumnSemiVariogram()
{
}

bool ColumnSemiVariogram::fDomainChangeable() const
{
  return false;
}

bool ColumnSemiVariogram::fValueRangeChangeable() const
{
  return true;
}

double ColumnSemiVariogram::rComputeVal(long iKey) const
{
  
  double rX = colX->rValue(iKey);
  if (rX == rUNDEF)
    return rUNDEF;  // no Y-value computed if X-value undefined
  else
    return svm.rCalc(rX);
  
}

