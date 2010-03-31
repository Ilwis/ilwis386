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
/* ColumnCalculate
   Copyright Ilwis System Development ITC
   may 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:00 am
*/

#pragma warning( disable : 4786 )

#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Applications\Table\COLCALC.H"
#include "Engine\Scripting\Calc.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Applications\Table\COLAGGR.H"
#include "Applications\Table\AGGFUNCS.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

/*
static DList<CalcVariable*> dlcvDummy;
static DList<InstList*> dlilDummy;
static DList<short> dliDummy;
static DList<StackObject> dlsoDummy;
*/
class ErrorEmptyExpr : public ErrorObject
{
public:
  ErrorEmptyExpr() : ErrorObject(WhatError(STBLErrEmptyExpression, errColumnCalculate+2)) {}
};

static void EmptyExprError()
{
  throw ErrorEmptyExpr();
}

static void UnexpectedTokenError(const String& sVal)
{
  ErrorObject(WhatError(String("%S '%S'", STBLOthUnexpected, sVal), errColumnCalculate+3)).Show();
}

class ErrorCalcExpression : public ErrorObject
{
public:
  ErrorCalcExpression(const WhereError& where, const String& sExpr)
    : ErrorObject(WhatError(String("%S '%S'", STBLOthInvalidExpr, sExpr), errColumnCalculate+1), where) {}
};

static void CalcExpressionError(const FileName& fn, const String& sCol,  const String& sExpr)
{
  DummyError();
//throw ErrorCalcExpression(WhereErrorColumn(fn, sCol), sExpr);
}
/*
class ParserObject {
public:
  ParserObject(const String& sExpr, CodeGenerator* cg);
  ~ParserObject();
  CodeGenerator* cg;
  ProgramParser* parser;
  TextInput* txtInp;
  TokenizerBase* tb;  
};

ParserObject::ParserObject(const String& sExpr, CodeGenerator* codgen)
{
  txtInp = new TextInput(sExpr);
  tb = new TokenizerBase(txtInp);
  cg = codgen;
  cg->sExpr = sExpr;
  parser = new ProgramParser(tb, cg);
}

ParserObject::~ParserObject()
{
  delete parser;
  delete cg;
  delete tb;
  delete txtInp;
}
*/
Instructions* ColumnCalculate::instCreate(const Table& tbl, const String& sExpr, 
                                          const DomainValueRangeStruct& dvs, 
                                          ObjectDependency& objdep, const Table& tblSearch)
{
  if (sExpr.length() == 0)
    EmptyExprError();
  Calculator calc(tbl, sExpr);
  return calc.instColCalc(objdep);
/*  Instructions* inst = new Instructions();
  CodeGenerator* cg = new CodeGenerator(tbl, dvs.dm(), tblSearch, inst);
  // is deleted in ~ParserObject
  ParserObject po(sExpr, cg);
  Token tokLast;
  try {
    po.parser->GetNextToken();
    po.parser->Expression();
    po.cg->AddInst("stop");
    tokLast = po.parser->tokCur();
  }  
  catch (const ErrorObject& err) {
    err.Show();
    delete inst;
    return 0;
  } 
  if (tokLast.sVal().length() > 0) {
    UnexpectedTokenError(tokLast.sVal());
    delete inst;
    return 0;
  }  
  po.cg->DetermineDomain();
  po.cg->Get(objdep);
  return inst;
*/
}

IlwisObjectPtr * createColumnCalculate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		if ( parms.size() == 3) {
			String *sFunc = (String *)parms[2];
			if (fCIStrEqual(sFunc->sLeft(3), "agg")  || fCIStrEqual(sFunc->sLeft(15), "ColumnAggregate") ||
				fCIStrEqual(sFunc->sLeft(4), "join") || fCIStrEqual(sFunc->sLeft(10), "ColumnJoin")) {
					AggregateFunction* agf = AggregateFunction::create(*sFunc);
					if (0 == agf) {
						Array<String> as;
						int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
						String sExp;
						if (iParms == 2)
							sExp = String("%S.%S", as[0], as[1]);
						else if (iParms == 3)
							sExp = String("%S.%S[%S]", as[0], as[1], as[2]);
						else
							return (IlwisObjectPtr *)ColumnCalculate::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
						return (IlwisObjectPtr *)ColumnCalculate::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExp, *dvs);
					}
					if (ColumnAggregate::fValidFunc(*sFunc))
						return (IlwisObjectPtr *)ColumnAggregate::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
				}
			}
			return (IlwisObjectPtr *)ColumnCalculate::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
		}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnCalculate(Table(fn), *sCol, (ColumnPtr &)ptr);
		}
}

ColumnCalculate::ColumnCalculate(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr)
{
  fNeedFreeze = false;
  instruc = 0;
  ReadElement(sSection().scVal(), "Expression", _sExpression);
// err: invalid calc expression (syntax or data)        ::ColumnCalculate(fn)
//  if (instruc->dm()->pdv() == 0)
//    throw ErrorCalcExpression(this, instruc->dm());
//  Freeze();
}

bool ColumnCalculate::fInit()
{
  Table tbl(ptr.fnTbl);
  Table tblSearch;
  if (fnTblSearch.fValid())
    tblSearch = Table(fnTblSearch);
  if (0 != instruc)
    delete instruc;
  instruc = instCreate(tbl, _sExpression, dvrs(), objdep, tblSearch);
  if (0 == instruc)
    return false;
  if (dm()->pdsrt()) {
    instruc->ChangeConstStringsToRaw(dm());
    for (unsigned int i=0; i< instruc->admmrg.iSize(); ++i)
      instruc->admmrg[i].ApplyMergeDomain(dm());
  }
  return true;
}

ColumnCalculate::ColumnCalculate(const Table& tbl, const String& sColName, ColumnPtr& ptr, Instructions* inst, const String& sExpr,
                                 const ObjectDependency& objdepen, const Table& tblSearch)
: ColumnVirtual(tbl, sColName, ptr, inst->dm(), tblSearch)
{
  ptr.SetValueRange(inst->vr());
//  ColumnVirtual::Replace(sExpr);
  _sExpression = sExpr;
  objdep = objdepen;
  instruc = inst;
  fNeedFreeze = false;
//  Freeze();
  ptr.fChanged = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

ColumnCalculate* ColumnCalculate::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
                                         const String& sExpression, const DomainValueRangeStruct& dvs)
{
  String sExpr = sExpression;
  Table tblSearch;
  String sTblSrch = ColumnVirtual::sTblSearch(sExpr);
  if (sTblSrch.length() != 0) {
    try {
      tblSearch = Table(sTblSrch);
    }
    catch (const ErrorObject& err) {
      err.Show();
    }
  }  
  ObjectDependency objdep;
  Instructions *instruc = instCreate(tbl, sExpr, dvs, objdep, tblSearch);
  if (instruc == 0)
    CalcExpressionError(tbl->fnObj, sColName, sExpr);
  return new ColumnCalculate(tbl, sColName, ptr, instruc, sExpr, objdep, tblSearch);
}

void ColumnCalculate::Store()
{
  ColumnVirtual::Store();
  WriteEntry("Calc", "True");
  WriteEntry("Expression", _sExpression);
}

String ColumnCalculate::sType() const
{
  return "Column Calculate";
}

String ColumnCalculate::sExpression() const
{
  return _sExpression;
}


void ColumnCalculate::InitFreeze()
{
  fInit();
}

void ColumnCalculate::Replace(const String& sExpr)
{
  ObjectDependency od;
  Table tbl(ptr.fnTbl);
  Table tblSearch;
  if (fnTblSearch.fValid())
    tblSearch = Table(fnTblSearch);
  Instructions* inst = instCreate(tbl, sExpr, DomainValueRangeStruct()/*dvrs()*/, od, tblSearch);
  if (0 == inst)
    CalcExpressionError(tbl->fnObj, sName(), sExpr);
  _sExpression = sExpr;
  Column col;
  col.SetPointer(&ptr);
  if (od.fUses(col))
    throw ErrorObject(WhatError(STBLErrCyclicDefinition, errColumnCalculate+1), 
                      WhereErrorColumn(tbl->fnObj, sName()));
  ColumnVirtual::Replace(sExpr);
  if (0 != instruc)
    delete instruc;
  instruc = inst;
  if (dm()->pdsrt())
    instruc->ChangeConstStringsToRaw(dm());
  SetDomainValueRangeStruct(DomainValueRangeStruct(instruc->dm(), instruc->vr()));
  objdep = od;
//fFrozen = false;
}
/*
void ColumnCalculate::SetDomain(const Domain& dm)
{
  ColumnVirtual::SetDomain(dm);
  if (dm->pdsrt())
    if (0 != instruc)
      instruc->ChangeConstStringsToRaw(dm);
}
*/
long ColumnCalculate::iComputeRaw(long iKey) const
{
  if (dvrs().fRawIsValue())
    return longConv(rComputeVal(iKey));
  else if (dvrs().fValues())
    return dvrs().vr()->iRaw(rComputeVal(iKey));
  else if (instruc != 0)
    return dvrs().iRaw(instruc->sColumnCalcVal(iKey));
  else
    return iUNDEF;
}

double ColumnCalculate::rComputeVal(long iKey) const
{
  if (0 == instruc)
    return rUNDEF;
  return instruc->rColumnCalcVal(iKey);
}

Coord ColumnCalculate::cComputeVal(long iKey) const
{
  if (0 == instruc)
    return crdUNDEF;
  return instruc->cColumnCalcVal(iKey);
}

String ColumnCalculate::sComputeVal(long iKey, short iWid, short iDec) const
{
  if (0 == instruc)
    return sUNDEF;
  return instruc->sColumnCalcVal(iKey, iWid, iDec);
}
/*
void ColumnCalculate::ComputeBuf(ByteBuf& buf, long iKey, long iNum) const
{
  DomainValue* dv = dm()->pdv();
  if (dv != 0) {
    RealBuf b(iNum);
    ComputeBuf(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = byteConv(dv->iRaw(b[i]));
  }
  else {
    StringBuf b(iNum);
    ComputeBuf(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = byteConv(dm()->iRaw(b[i]));
  }
}

void ColumnCalculate::ComputeBuf(IntBuf& buf, long iKey, long iNum) const
{
  DomainValue* dv = dm()->pdv();
  if (dv != 0) {
    RealBuf b(iNum);
    ComputeBuf(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = shortConv(dv->iRaw(b[i]));
  }
  else {
    StringBuf b(iNum);
    ComputeBuf(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = shortConv(dm()->iRaw(b[i]));
  }
}
*/
void ColumnCalculate::ComputeBufRaw(LongBuf& buf, long iKey, long iNum) const
{
  if (dvrs().fRawIsValue()) {
    RealBuf b(iNum);
    ComputeBufVal(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = longConv(b[i]);
  }
  else if (dvrs().fValues()) {
    RealBuf b(iNum);
    ComputeBufVal(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = dvrs().vr()->iRaw(b[i]);
  }
  else {
    StringBuf b(iNum);
    ComputeBufVal(b, iKey, iNum);
    for (long i = 0; i < iNum; i++)
      buf[i] = dvrs().dm()->iRaw(b[i]);
  }
}

void ColumnCalculate::ComputeBufVal(RealBuf& buf, long iKey, long iNum) const
{
  if (0 == instruc) {
    for (int i=0; i < buf.iSize(); ++i)
      buf[i] = rUNDEF;
    return;
  }
  instruc->ColumnCalcVal(buf, iKey, iNum);
}

void ColumnCalculate::ComputeBufVal(StringBuf& buf, long iKey, long iNum, short iWid, short iDec) const
{
  if (0 == instruc) {
    for (int i=0; i < buf.iSize(); ++i)
      buf[i] = sUNDEF;
    return;
  }
  instruc->ColumnCalcVal(buf, iKey, iNum, iWid, iDec);
}

void ColumnCalculate::ComputeBufVal(CoordBuf& buf, long iKey, long iNum) const
{
  if (0 == instruc) {
    for (int i=0; i < buf.iSize(); ++i)
      buf[i] = crdUNDEF;
    return;
  }
  instruc->ColumnCalcVal(buf, iKey, iNum);
}

ColumnCalculate::~ColumnCalculate()
{
  if (instruc != 0)
    delete instruc;
}

void ColumnCalculate::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvs)
{
  if (dvrs().dm()->pdsrt())
    if (0 != instruc) {
      instruc->ChangeConstStringsToRaw(dvrs().dm());
      for (unsigned int i=0; i< instruc->admmrg.iSize(); ++i)
        instruc->admmrg[i].ApplyMergeDomain(dvrs().dm());
    }
}

bool ColumnCalculate::fDomainChangeable() const
{
  return true;
}

bool ColumnCalculate::fValueRangeChangeable() const
{
  return true;
}

bool ColumnCalculate::fExpressionChangeable() const
{
  return true;
}

DomainValueRangeStruct ColumnCalculate::dvrsDefault(const String& sExpression) const
{
  Table tbl(ptr.fnTbl);
  ObjectDependency objdep;
  Instructions* instruc = instCreate(tbl, sExpression, DomainValueRangeStruct(), objdep, tbl);
  if (0 == instruc)
    DomainValueRangeStruct();
  DomainValueRangeStruct dvrs(instruc->dm(), instruc->vr());
  delete instruc;
  return dvrs;
}

bool ColumnCalculate::fMergeDomainForCalc(Domain& dm, const String& sExpr)
{
  DomainSort* pdsrt = dm->pdsrt();
  if (0 == pdsrt)
    return true;
  Table tbl(ptr.fnTbl);
  ObjectDependency objdep;
  Instructions* instruc = instCreate(tbl, sExpr, DomainValueRangeStruct(), objdep, tbl);
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
  UnFreeze();
  return true;
}




