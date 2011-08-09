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
// $Log: /ILWIS 3.0/Column/Colaggr.cpp $
 * 
 * 10    2/19/02 3:56p Martin
 * force loading of relevant column else the data is undefined
 * 
 * 9     16-01-01 8:44a Martin
 * table on which join columns are dependent is now memeber of
 * ColumnAggregate. It prevents going out of scope of the table after
 * which the table can not be loadedn any more
 * 
 * 8     3/21/00 2:46p Wind
 * incorrect error message
 * 
 * 7     15-02-00 11:20 Wind
 * adapt for domain bool
 * 
 * 6     14-02-00 17:18 Wind
 * quoted name for tables if necessary
 * 
 * 5     29-10-99 12:57 Wind
 * case sensitive stuff
 * 
 * 4     9/29/99 10:10a Wind
 * added case insensitive string comparison
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
// Revision 1.6  1997/09/16 15:48:09  Wim
// Do not throw an error if using 5 parameters with ColumnJoinFunc()
// the fifth is always the keycol
//
// Revision 1.5  1997-09-15 21:34:45+02  Wim
// Check for 0==agf
//
// Revision 1.4  1997-08-25 12:32:35+02  Wim
// Added some not found errors
//
// Revision 1.3  1997-08-11 10:58:02+02  Wim
// Also domain None of table works for grouping
//
// Revision 1.2  1997-08-11 10:28:02+02  Wim
// Also aggregate for tables with Domain None
//
/* ColumnAggregate
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  WK    1 Jul 98   10:01 am
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblstore.h"
#include "Applications\Table\COLAGGR.H"
#include "Applications\Table\AGGFUNCS.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

class DATEXPORT ErrorNoWeightAllowed: public ErrorObject
{
public:
  ErrorNoWeightAllowed(const String& sAggFunc, const WhereError& where)
  : ErrorObject(WhatError(String(TR("No weight allowed for '%S'").c_str(), sAggFunc), errColumnAggregate+2),
                where) {}
};

static void NoWeightAllowedError(const String& sAggFunc, const WhereError& where) 
{
  ErrorNoWeightAllowed(sAggFunc, where).Show();
}

class DATEXPORT ErrorIncompatibleTables: public ErrorObject
{
public:
  ErrorIncompatibleTables(const String& sTblDom1, const String& sTblDom2, const WhereError& where)
  : ErrorObject(WhatError(String(TR("Incompatible domain of tables '%S' and '%S'").c_str(), sTblDom1, sTblDom2), errColumnAggregate+1),
                where) {}
};

static void IncompatibleTablesError(const String& sTblDom1, const String& sTblDom2, const WhereError& where) 
{
  ErrorIncompatibleTables(sTblDom1, sTblDom2, where).Show();
}

class DATEXPORT ErrorIncompatibleJoinColumns: public ErrorObject
{
public:
  ErrorIncompatibleJoinColumns(const String& sColDom1, const String& sColDom2, const WhereError& where)
  : ErrorObject(WhatError(String(TR("Incompatible domains '%S' and '%S'").c_str(), sColDom1, sColDom2), errColumnAggregate+3),
                where) {}
};

static void IncompatibleJoinColumnsError(const String& sColDom1, const String& sColDom2, const WhereError& where) 
{
  ErrorIncompatibleJoinColumns(sColDom1, sColDom2, where).Show();
}

class DATEXPORT ErrorStringColPrd: public ErrorObject
{
public:
  ErrorStringColPrd(const WhereError& where, const String& sColAgg)
  : ErrorObject(WhatError(String(TR("No string domain allowed for predominant function for column '%S'").c_str(), sColAgg), errColumnAggregate+4),
                where) {}
};

const char* ColumnAggregate::sSyntax() 
{ return "AggFunc(colagg,colgroup,colweight)\nJoinFunc(tbl,colagg,colgroup,colweight,coljoin)\n"
"Join2ndKey(tbl,colagg,colgroup,coljoin)"; }

bool ColumnAggregate::fValidFunc(const String& sAggFunc)
{
  if (fCIStrEqual(sAggFunc, "join") || fCIStrEqual(sAggFunc, "ColumnJoin"))
    return true;
  try {
    AggregateFunction* agf;
    agf = AggregateFunction::create(sAggFunc);
    if (0 == agf)
      return false;
    delete agf;
    return true;
  }
  catch (const ErrorObject&) {
    return false;
  }
}

IlwisObjectPtr * createColumnAggregate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnAggregate::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnAggregate(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

ColumnAggregate::ColumnAggregate(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr),
  tblOther(Table())
{
  String s;
  ReadElement(sSection().c_str(), "AggregateFunction", s);
  agf = AggregateFunction::create(s);
  ReadElement(sSection().c_str(), "ColumnAggregate", s);
  colAgg = Column(s, tbl);
  s = String();
  ReadElement(sSection().c_str(), "ColumnGroupBy", s);
  if (s.length())
    colGroup = Column(s, tbl);
  s = String();
  ReadElement(sSection().c_str(), "ColumnWeight", s);
  if (s.length())
    colWeight = Column(s, tbl);
  s = String();
  ReadElement(sSection().c_str(), "ColumnLocalJoin", s);
  if (s.length())
    colLocalJoin = Column(s, tbl);
  objdep.Add(colAgg.ptr());
  sJoinTable = colAgg->fnTbl.sRelativeQuoted(true, ptr.fnTbl.sPath());
  if (fCIStrEqual(sJoinTable, fnObj.sRelativeQuoted(true, tbl->fnObj.sPath())))
    sJoinTable = String();
  if (colGroup.fValid())
    objdep.Add(colGroup.ptr());
  if (colWeight.fValid())
    objdep.Add(colWeight.ptr());
  if (colLocalJoin.fValid()) {
    objdep.Add(colLocalJoin.ptr());
  }  
}

ColumnAggregate::ColumnAggregate(const Table& tbl, const String& sColName, ColumnPtr& ptr,
                                const DomainValueRangeStruct& dvs, AggregateFunction* af,
                                const Column& colAggr, const Column& colGrp, const Column& colWgth, const Column& colJn,
																const Table& _tblOther)
: ColumnVirtual(tbl, sColName, ptr, dvs, Table()), colAgg(colAggr), colGroup(colGrp), 
  colWeight(colWgth), colLocalJoin(colJn), agf(af),
	tblOther(_tblOther)
{
  ptr.SetDomainValueRangeStruct(agf->dvrsDflt(colAgg));
  objdep.Add(colAgg.ptr());
  if (colGroup.fValid())
    objdep.Add(colGroup.ptr());
  if (colWeight.fValid())
   objdep.Add(colWeight.ptr());
  sJoinTable = colAgg->fnTbl.sRelativeQuoted(true, ptr.fnTbl.sPath());
  if (fCIStrEqual(sJoinTable, fnObj.sRelativeQuoted(true, tbl->fnObj.sPath())))
    sJoinTable = String();
  if (colLocalJoin.fValid()) 
    objdep.Add(colLocalJoin.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  ColumnVirtual::Replace(sExpression());
}

ColumnVirtual* ColumnAggregate::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
                                         const String& sExpression, const DomainValueRangeStruct& dvs)
{
/* Syntax;
Aggregation:
    AggFunc(colval, colgroup, colweight)
    AggFunc(colval, , colweight)        : no grouping
    AggFunc(colval,0, colweight)       : no grouping
    AggFunc(colval, colgroup)           : no weight
    AggFunc(colval)                    : no grouping, no weight
Join:
    JoinFunc(tbl, colval, colgroup, colweight, coljoin)    
    Join2ndKey(tbl, colval, colgroup, coljoin)    
*/  
  Array<String> as;
  AggregateFunction* agf;
  Column colAgg;
  Column colGroup;
  Column colWeight;
  Column colLocalJoin;
  Table tblSearch;
  String sExpr = sExpression;
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  agf = AggregateFunction::create(sFunc);
  if (0 == agf)
    return 0;
  String sAgfName = agf->sName(true);
  bool fJoin = fCIStrEqual(sFunc.sLeft(4), "Join") || fCIStrEqual(sFunc.sLeft(10), "ColumnJoin");
  bool fJoin2ndKey = fJoin && fCIStrEqual(sAgfName, "join2ndkey");
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  try {  
    if ((iParms < 1) || (iParms > 5))
      ExpressionError(sExpr, sSyntax());
    if (!fJoin) {
      if (iParms > 3)
        ExpressionError(sExpr, sSyntax());
    }
    else if (fJoin2ndKey) {
      if (iParms != 3 && iParms != 4)
        ExpressionError(sExpr, sSyntax());
    }    
    
    if (fJoin) {
      tblSearch = Table(as[0], tbl->fnObj.sPath());
      try {
        colAgg = Column(as[1], tblSearch);
      }
      catch (const ErrorObject&) {
      }
      if (!colAgg.fValid()) {
        ErrorNotFound(WhereErrorColumn(tblSearch->fnObj, as[1])).Show();
        goto error;
      }  
      if (fJoin2ndKey || fCIStrEqual(sFunc.sLeft(7), "joinprd")) {
        if (colAgg->dm()->pds()) {
          ErrorStringColPrd(WhereErrorColumn(tblSearch->fnObj, as[1]), as[1]).Show();
          goto error;
        }
      }
      if ((iParms > 2) && (as[2].length() != 0) && (as[2] != "0")) {
        try {
          colGroup = Column(as[2], tblSearch);
        }  
        catch (const ErrorObject&) {
        }
        if (!colGroup.fValid()) {
          ErrorNotFound(WhereErrorColumn(tblSearch->fnObj, as[2])).Show();
          goto error;
        }
      }  
      if (!fJoin2ndKey && (iParms > 3) && (as[3].length() != 0) && (as[3] != "1")) {
        try {
          colWeight = Column(as[3], tblSearch);
        }  
        catch (const ErrorObject&) {
        }
        if (!colWeight.fValid()) {
          ErrorNotFound(WhereErrorColumn(tblSearch->fnObj, as[3])).Show();
          goto error;
        }
      }  
      if ((fJoin2ndKey && (iParms == 4)) || (iParms == 5)) {
        try {
          colLocalJoin = Column(as[as.iSize()-1], tbl);
        }
        catch (const ErrorObject&) {
        }
        if (colLocalJoin.fValid() && (colGroup->dm() != colLocalJoin->dm())) {
          String sDom1 = colGroup->dm()->sName(true, tbl->fnObj.sPath());
          String sDom2 = colLocalJoin->dm()->sName(true, tbl->fnObj.sPath());
          IncompatibleJoinColumnsError(sDom1, sDom2, tbl->fnObj);
          goto error;
        }                               
      }  
      if (colGroup.fValid())
        if (!colLocalJoin.fValid() && (colGroup->dm() != tbl->dm())) {
          String sDom1 = colGroup->dm()->sName(true, tbl->fnObj.sPath());
          String sDom2 = tbl->dm()->sName(true, tbl->fnObj.sPath());
          IncompatibleJoinColumnsError(sDom1, sDom2, tbl->fnObj);
          goto error;
        }                               
    }
    else {
      try {
        colAgg = Column(as[0], tbl);
      }
      catch (const ErrorObject&) {
      }
      if (!colAgg.fValid()) {
        ErrorNotFound(WhereErrorColumn(tbl->fnObj, as[0])).Show();
        goto error;
      }  
      if (fCIStrEqual(sFunc.sLeft(6), "aggprd")) {
        if (colAgg->dm()->pds()) {
          ErrorStringColPrd(WhereErrorColumn(tblSearch->fnObj, as[0]), as[0]).Show();
          goto error;
        }
      }
      if ((iParms > 1) && (as[1].length() != 0) && (as[1] != "0")) {
        try {
          colGroup = Column(as[1], tbl);
        }  
        catch (const ErrorObject&) {
        }
        if (!colGroup.fValid()) {
          ErrorNotFound(WhereErrorColumn(tbl->fnObj, as[1])).Show();
          goto error;
        }
      }  
      if ((iParms > 2) && (as[2].length() != 0) && (as[2] != "1")) {
        try {
          colWeight = Column(as[2], tbl);
        }  
        catch (const ErrorObject&) {
        }
        if (!colWeight.fValid()) {
          ErrorNotFound(WhereErrorColumn(tbl->fnObj, as[2])).Show();
          goto error;
        }
      }  
    }  
    if (colWeight.fValid())
      if (colAgg->dmKey() != colWeight->dmKey()) {
        String sDom1 = colAgg->dmKey()->sName(true, tbl->fnObj.sPath());
        String sDom2 = colWeight->dmKey()->sName(true, tbl->fnObj.sPath());
        IncompatibleTablesError(sDom1, sDom2, tbl->fnObj);
        goto error;
      }                                
    if (colWeight.fValid() && !agf->fWeightPossible()) {
      String sAggFunc = agf->sName();
      NoWeightAllowedError(sAggFunc, tbl->fnObj);
      goto error;
    }  
    if (!colAgg->fValues() && agf->fValuesNeeded()) {
      String sDom = colAgg->dm()->sName(true, tbl->fnObj.sPath());
      String sTypeName = colAgg->sTypeName();
      ErrorValueDomain(sDom, sTypeName, errColumnAggregate+3).Show();
      goto error;
    }  
    if (colWeight.fValid())
      if (!colWeight->fValues()) {
        String sDom = colWeight->dm()->sName(true, tbl->fnObj.sPath());
        String sTypeName = colWeight->sTypeName();
        ErrorValueDomain(sDom, sTypeName, errColumnAggregate+3).Show();
        goto error;
      }  
    return new ColumnAggregate(tbl, sColName, ptr, dvs, agf, colAgg, colGroup, colWeight, colLocalJoin, tblSearch);
  }
  catch (const ErrorObject& err) {
    err.Show();
  }
error:
  delete agf;
  DummyError();
  return 0;
}

String ColumnAggregate::sExpression() const
{
  String sFirstPart; // function name + table name (if join)
  bool fJoin = sJoinTable.length() != 0;
  sFirstPart = agf->sName(fJoin);
  sFirstPart &= '(';
  if (fJoin && (sJoinTable.length() != 0)) 
    sFirstPart &= String("%S,", sJoinTable);
  String sColAgg, sColGroup;
  if (colAgg->fnTbl.sFullName() != ptr.fnTbl.sFullName())
    sColAgg = colAgg->sTableAndColumnName(ptr.fnTbl.sPath());
  else
    sColAgg = colAgg->sNameQuoted();
  String sColWeight;
  if (colWeight.fValid())
    if (colWeight->fnTbl.sFullName() != ptr.fnTbl.sFullName())
      sColWeight = colWeight->sTableAndColumnName(ptr.fnTbl.sPath());
    else
      sColWeight = colWeight->sNameQuoted();
  if (!colGroup.fValid()) {
    if (!colWeight.fValid()) {
      String sExpr("%S%S)", sFirstPart, sColAgg);
      return sExpr;
    }    
    else {
      String sExpr("%S%S,,%S)", sFirstPart, sColAgg, sColWeight);
      return sExpr;
    }    
  }  
  if (colGroup->fnTbl.sFullName() != ptr.fnTbl.sFullName())
    sColGroup = colGroup->sTableAndColumnName(ptr.fnTbl.sPath());
  else
    sColGroup = colGroup->sNameQuoted();
  String sExpr;
  if (!colWeight.fValid()) {
    if (colLocalJoin.fValid())
      if (fCIStrEqual(agf->sName(true),"join2ndkey"))
        sExpr = String("%S%S,%S,%S)", sFirstPart, sColAgg, sColGroup, colLocalJoin->sNameQuoted());
      else
        sExpr = String("%S%S,%S,1,%S)", sFirstPart, sColAgg, sColGroup, colLocalJoin->sNameQuoted());
    else
      sExpr = String("%S%S,%S)", sFirstPart, sColAgg, sColGroup);
  }  
  else {
    if (colLocalJoin.fValid())
      sExpr = String("%S%S,%S,%S,%S)", sFirstPart, sColAgg, sColGroup, sColWeight, colLocalJoin->sNameQuoted());
    else
      sExpr = String("%S%S,%S,%S)", sFirstPart, sColAgg, sColGroup, sColWeight);
  }  
  return sExpr;
}

void ColumnAggregate::Store()
{
  ColumnVirtual::Store();
  WriteEntry("AggregateFunction", agf->sName());
  WriteEntry("ColumnAggregate", colAgg->sTableAndColumnName(fnObj.sPath()));
  if (colGroup.fValid())
    WriteEntry("ColumnGroupBy", colGroup->sTableAndColumnName(fnObj.sPath()));
  else
    WriteEntry("ColumnGroupBy", (char*)0);
  if (colWeight.fValid())
    WriteEntry("ColumnWeight", colWeight->sTableAndColumnName(fnObj.sPath()));
  else
    WriteEntry("ColumnWeight", (char*)0);
  if (colLocalJoin.fValid())
    WriteEntry("ColumnLocalJoin", colLocalJoin->sTableAndColumnName(fnObj.sPath()));
  else
    WriteEntry("ColumnLocalJoin", (char*)0);
}

void ColumnAggregate::Replace(const String& sExpr)
{
  ColumnVirtual::Replace(sExpr);
//fFrozen = false;
}

bool ColumnAggregate::fFreezing()
{
  bool fUseWeight = colWeight.fValid() && agf->fWeightPossible();
  if (fUseWeight)
    agf->SetUseWeight();

  bool fValuesColAgg = colAgg->dvrs().fValues();
  bool fStringColAgg = 0 != colAgg->dm()->pds();
  DomainSort *pdsrtKeyColAgg = colAgg->dmKey()->pdsrt();
  bool fDomainNone = 0 != colAgg->dmKey()->pdnone();
  bool fDomainImage = colAgg->dmKey()->pdi() != 0;
	colAgg->Loaded(false);
  if (!colGroup.fValid()) {
    agf->reset(true, false);
//    agf->reset(true, 0 != colAgg->dm()->pds());
    if (colWeight.fValid())
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF)))
          if (fValuesColAgg)
            agf->AddRaw(0L, colAgg->rValue(j), colWeight->rValue(j));
          else
            agf->AddRaw(0L, colAgg->iRaw(j), colWeight->rValue(j));
      }
    else
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF)))
          if (fValuesColAgg)
            agf->AddRaw(0L, colAgg->rValue(j));
          else if (fStringColAgg)
            agf->AddRaw(0L, colAgg->sValue(j));
          else
            agf->AddRaw(0L, colAgg->iRaw(j));
      }
    if (dvrs().fValues()) {
      double r = agf->rValResult(0L);
      for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
        pcs->PutVal(j, r);
    }
    else {
      long iRaw= agf->iRawResult(0L);
      for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
        pcs->PutRaw(j, iRaw);
    }
    return true;
  }

  bool fUseRaw = colGroup->dvrs().fRawAvailable();
  if (fUseRaw && colLocalJoin.fValid())
    fUseRaw = colLocalJoin->dvrs() == colGroup->dvrs();
  bool fUseString = 0 != colGroup->dvrs().dm()->pds();
  agf->reset(fUseRaw, fUseString);
  if (fUseWeight) { 
    if (fUseRaw)
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          long iRaw = colGroup->iRaw(j);
          if (fValuesColAgg)
            agf->AddRaw(iRaw, colAgg->rValue(j), colWeight->rValue(j));
           else
            agf->AddRaw(iRaw, colAgg->iRaw(j), colWeight->rValue(j));
        }
      }    
    else if (fUseString)
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          String sVal = colGroup->sValue(j,0);
          if (fValuesColAgg)
            agf->AddVal(sVal, colAgg->rValue(j), colWeight->rValue(j));
           else
            agf->AddVal(sVal, colAgg->iRaw(j), colWeight->rValue(j));
        }
      }
    else    
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          double rVal = colGroup->rValue(j);
          if (fValuesColAgg)
            agf->AddVal(rVal, colAgg->rValue(j), colWeight->rValue(j));
           else
            agf->AddVal(rVal, colAgg->iRaw(j), colWeight->rValue(j));
        }
      }    
  }      
  else {
    if (fUseRaw)
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          long iRaw = colGroup->iRaw(j);
          if (fValuesColAgg)
            agf->AddRaw(iRaw, colAgg->rValue(j));
          else
            agf->AddRaw(iRaw, colAgg->iRaw(j));
        }
      }    
    else if (fUseString)  
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          String sVal = colGroup->sValue(j,0);
          if (fValuesColAgg)
            agf->AddVal(sVal, colAgg->rValue(j));
          else
            agf->AddVal(sVal, colAgg->iRaw(j));
        }
      }
    else  
      for (long j=colAgg->iOffset(); j < colAgg->iOffset()+colAgg->iRecs(); ++j) {
        if (fDomainNone || fDomainImage ||
            ((0 != pdsrtKeyColAgg) && (pdsrtKeyColAgg->iOrd(j) != iUNDEF))) {
          double rVal = colGroup->rValue(j);
          if (fValuesColAgg)
            agf->AddVal(rVal, colAgg->rValue(j));
          else
            agf->AddVal(rVal, colAgg->iRaw(j));
        }
      }
  }   
  Column colIndex;
  if (colGroup->fnTbl == ptr.fnTbl)
    colIndex = colGroup;
  if (colLocalJoin.fValid())
    colIndex = colLocalJoin;
  DomainSort* pdsrt = ptr.dmKey()->pdsrt();
  if (dvrs().fValues()) {
    if (fUseRaw) {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, agf->rValResult(colIndex->iRaw(j)));
      else // if (0 != pdsrt)
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, agf->rValResult(j/*pdsrt->iKey(j)*/));
/*      else
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, rUNDEF);*/
    }  
    else if (fUseString) {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, agf->rValResult(colIndex->sValue(j,0)));
      else //if (0 != pdsrt)
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, agf->rValResult(pdsrt->sValueByRaw(j/*pdsrt->iKey(j)*/)));
/*      else
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, rUNDEF);*/
    }    
    else {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, agf->rValResult(colIndex->rValue(j)));
      else
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutVal(j, rUNDEF);
    }  
  }
  else {
    if (fUseRaw) {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, agf->iRawResult(colIndex->iRaw(j)));
      else //if (0 != pdsrt)
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, agf->iRawResult(j/*pdsrt->iKey(j)*/));
/*      else
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, iUNDEF);*/
    }  
    else if (fUseString) {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, agf->iRawResult(colIndex->sValue(j,0)));
      else //if (0 != pdsrt) 
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, agf->iRawResult(pdsrt->sValueByRaw(j/*pdsrt->iKey(j)*/)));
/*      else  
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, iUNDEF);*/
    }     
    else  {
      if (colIndex.fValid())
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, agf->iRawResult(colIndex->rValue(j)));
      else
        for (long j=iOffset(); j < iOffset()+iRecs(); ++j)
          pcs->PutRaw(j, iUNDEF);
    }    
  }
  return true;
}

ColumnAggregate::~ColumnAggregate()
{
  if (0 != agf)
    delete agf;
}

bool ColumnAggregate::fDomainChangeable() const
{
  String sAggName = agf->sName();
  return fCIStrEqual(sAggName, "aggsum") || fCIStrEqual(sAggName, "aggavg") || fCIStrEqual(sAggName, "aggstd");
}

bool ColumnAggregate::fValueRangeChangeable() const
{
  return fDomainChangeable();
}




