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
/* $Log: /ILWIS 3.0/Table/tblchdom.cpp $
 * 
 * 5     22-11-99 12:44 Wind
 * bug 1195 (ignore duplicate undefs for error message)
 * 
 * 4     9/08/99 10:28a Wind
 * adpated to use of quoted file names in sExpression()
 * 
 * 3     15-03-99 16:07 Koolhoven
 * 
 * 2     3/11/99 12:17p Martin
 * Added support for Case insesitive 
// Revision 1.1  1998/09/16 17:25:32  Wim
// 22beta2
//
/* TableChangeDomain
   Copyright Ilwis System Development ITC
   original by Martin Schouwenburg, may 1998
   extended by Wim Koolhoven, may 1998
	Last change:  WK    5 Aug 98    3:02 pm
*/

#include "Applications\Table\tblchdom.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\tbl.hs"

IlwisObjectPtr * createTableChangeDomain(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableChangeDomain::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableChangeDomain(fn, (TablePtr &)ptr);
}

static void ThrowErrorExpression(const String& sExpr, const String& sSyntax)
{
  throw ErrorExpression(sExpr, sSyntax);
}

static void ThrowIncompatibleDomainError(const String& sMessage, const FileName& fn)
{
  throw ErrorObject(WhatError(sMessage, 1), fn);
}


const char* TableChangeDomain::sSyntax()
{
  return "TableChangeDomain(table, domain)\n"
	 "TableChangeDomain(table, none)\n"
	 "TableChangeDomain(table, column)\n"
	 "TableChangeDomain(table, column, no|avg|sum|min|max|last";
}

TableChangeDomain* TableChangeDomain::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2 || iParms > 3)
    ThrowErrorExpression(sExpr, sSyntax());
  Table tab(as[0]);
  Column col = tab->col(as[1]);
  if (col.fValid())
  {
    enumAggregate eAgg = eNO;
    if (iParms == 3) {
      String sAgg = as[2];
      if (fCIStrEqual("no" , sAgg))
        eAgg = eNO;
      else if (fCIStrEqual("avg" , sAgg))
        eAgg = eAVG;
      else if (fCIStrEqual("sum" , sAgg))
        eAgg = eSUM;
      else if (fCIStrEqual("min" , sAgg))
        eAgg = eMIN;
      else if (fCIStrEqual("max" , sAgg))
        eAgg = eMAX;
      else if (fCIStrEqual("last" , sAgg))
        eAgg = eLAST;
      else
       ThrowErrorExpression(sExpr, sSyntax());
    }
    CheckColumn(col, eAgg != eNO);
    return new TableChangeDomain(fn, p, tab, col, eAgg);
  }
  else {
    Domain dom(as[1]);
    if (iParms==3)
      ThrowErrorExpression(sExpr, sSyntax());
    if (!dom->pdsrt() && !dom->pdnone())
      ThrowIncompatibleDomainError(TR("Domain must be Class/Id or None"), fn);
    return new TableChangeDomain(fn, p, tab, dom);
  }
}

TableChangeDomain::TableChangeDomain(const FileName &fName, TablePtr& p, const Table& tab, const Column& col, enumAggregate eAgg)
: TableVirtual(fName, p, true),
  colSource(col),
  tblSource(tab),
  domNew(col->dm()),
  eAggregate(eAgg)
{
  Init();
}

TableChangeDomain::TableChangeDomain(const FileName &fName, TablePtr& p, const Table& tab, const Domain& dom)
: TableVirtual(fName, p, true),
  domNew(dom),
  tblSource(tab),
  eAggregate(eNO)
{
   Init();
}

TableChangeDomain::TableChangeDomain(const FileName& fName, TablePtr &tab) :
  TableVirtual(fName, tab)
{
  ReadElement("TableChangeDomain", "OriginalTable", tblSource);

  String sAgg = "No";
  ReadElement("TableChangeDomain", "Aggregate", sAgg);
  eAggregate = eNO;
  if (fCIStrEqual("no" , sAgg))
    eAggregate = eNO;
  else if (fCIStrEqual("avg" , sAgg))
    eAggregate = eAVG;
  else if (fCIStrEqual("sum" , sAgg))
    eAggregate = eSUM;
  else if (fCIStrEqual("min" , sAgg))
    eAggregate = eMIN;
  else if (fCIStrEqual("max" , sAgg))
    eAggregate = eMAX;
  else if (fCIStrEqual("last" , sAgg))
    eAggregate = eLAST;

  String sColumn;
  int iRet = ReadElement("TableChangeDomain", "colSource", sColumn);
  if (iRet==0)
    ReadElement("TableChangeDomain", "domNew", domNew);
  else {
    colSource = tblSource->col(sColumn);
    domNew = colSource->dm();
  }    

  fNeedFreeze = true;
  sFreezeTitle = "TableChangeDomain";
  objdep.Add(tblSource);
  objdep.Add(domNew);
}

void TableChangeDomain::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableChangeDomain");
  WriteElement("TableChangeDomain", "OriginalTable", tblSource);
  String sAgg;
  switch (eAggregate) {
    case eNO:   sAgg = "No";   break;
    case eAVG:  sAgg = "Avg";  break;
    case eSUM:  sAgg = "Sum";  break;
    case eMIN:  sAgg = "Min";  break;
    case eMAX:  sAgg = "Max";  break;
    case eLAST: sAgg = "Last"; break;
  }
  WriteElement("TableChangeDomain", "Aggregate", sAgg);
  if (colSource.fValid())
    WriteElement("TableChangeDomain", "colSource", colSource->sName());
  else  
    WriteElement("TableChangeDomain", "domNew", domNew);
}  

void TableChangeDomain::UnFreeze()
{
  TableVirtual::UnFreeze();
}

void TableChangeDomain::Init()
{
  ptr.SetDomain(domNew);
  objdep.Add(tblSource);
  if (domNew.fValid() && !domNew->pdnone())
    objdep.Add(domNew);
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  sFreezeTitle = "TableChangeDomain";
}

long TableChangeDomain::iInd(DomainSort* pdsrt, long iK)
{
    if (!pdsrt) return iK;
    long iN=pdsrt->iKey(iK);
    return iN;
}

bool TableChangeDomain::fFreezing()
{   
  trq.SetText(TR("Copying data"));
  long iOffsetNew = iOffset();
  long iOffsetOld = tblSource->iOffset();
  int c;
  long i;

  DomainSort *domTable=tblSource->dm()->pdsrt();
  for (c=0; c < tblSource->iCols(); ++c)
  {
     Column col = tblSource->col(c);
     DomainValueRangeStruct dvrs = col->dvrs();
     if (dvrs.fValues()) {
       if (eAVG == eAggregate) {
         ValueRange vr = dvrs.vr();
         double rStep = vr->rStep();
         rStep /= 100;
//         rStep = 0;
         vr.SetStep(rStep);
         dvrs.SetValueRange(vr);
       }
       else if (eSUM == eAggregate) {
         ValueRange vr = dvrs.vr();
         RangeReal rr = vr->rrMinMax();
         double rStep = vr->rStep();
         if (rr.rLo() < 0)
           rr.rLo() *= tblSource->iRecs();
         if (rr.rHi() > 0)
           rr.rHi() *= tblSource->iRecs();
         dvrs.SetValueRange(ValueRange(rr,rStep));
       }
     }
     Column colNew = ptr.colNew(col->sName(), dvrs);
     colNew->Fill();
  }
  if (domNew->pdnone())
  {
    ptr.iRecNew(tblSource->iRecs());
    for (c=0; c < tblSource->iCols(); ++c)
    {
      if (trq.fUpdate(c+1, tblSource->iCols()))
        return false;
      Column colOld = tblSource->col(c);
      Column colNew = ptr.col(c);
      if (colOld->fValues()) {
        for (i=0; i < tblSource->iRecs(); ++i) {
          double rVal = colOld->rValue(iInd(domTable, iOffsetOld+i));
          colNew->PutVal(iOffsetNew+i, rVal);
        }
      }
      else {
        for (i=0; i < tblSource->iRecs(); ++i) {
          String sVal = colOld->sValue(iInd(domTable, iOffsetOld+i),0);
          colNew->PutVal(iOffsetNew+i, sVal);
        }
      }
    }
  }
  else if (colSource.fValid())
  {
    CheckColumn(colSource, eAggregate != eNO);
    if (eNO != eAggregate) 
    {
      String sCount = "Count";
      colCount = pts->col(sCount);
      if (!colCount.fValid()) {
        colCount = pts->colNew(sCount, Domain("count"));
        colCount->sDescription = "Number of aggregated records";
      }
      colCount->SetOwnedByTable(true);
      colCount->SetReadOnly(true);
      for (i = 1; i <= iRecs(); ++i)
        colCount->PutVal(i, 0L);
    }
    long iNr = tblSource->iRecs();
    for (long i=0; i < iNr; ++i) 
    {
      if (trq.fUpdate(1+i, iNr))
        return false;
      long iRec = colSource->iRaw(iOffsetOld+i);
      if (iUNDEF == iRec)
        continue;
      long iCount=iUNDEF;
      if (colCount.fValid())
      {
        iCount = colCount->iValue(iRec);
        ++iCount;
        colCount->PutVal(iRec, iCount);
      }         
      for (c=0; c < tblSource->iCols(); ++c)
      {
        Column colOld = tblSource->col(c);
        Column colNew = ptr.col(c);
        if (colOld->fValues()) {
          double rVal = colOld->sValue(iOffsetOld+i).rVal();
          if (rVal==rUNDEF) continue;
          if (iCount > 1 && eNO != eAggregate && eLAST != eAggregate) {
            double rCurr = colNew->rValue(iRec);
            if (rCurr != rUNDEF) {
              switch (eAggregate) {
                case eMIN:
                  if (rCurr < rVal)
                    rVal = rCurr;
                  break;
                case eMAX:
                  if (rCurr > rVal)
                    rVal = rCurr;
                  break;
                case eSUM:
                  rVal += rCurr;
                  break;
                case eAVG:
                  rCurr *= iCount - 1;
                  rVal += rCurr;
                  rVal /= iCount;
                  break;
              }
            }
          }
          colNew->PutVal(iRec, rVal);
        }
        else {
          String sVal = colOld->sValue(iOffsetOld+i,0);
          colNew->PutVal(iRec, sVal);
        }
      }
    }
  }
  else
  {
    long iC=0;
    long iNr = tblSource->iRecs();
    Domain dmOld = tblSource->dm();
    bool fDomainNone = dmOld->pdnone() !=0;
    for (long i=1; i <= iNr; ++i) {
      if (trq.fUpdate(i, iNr))
        return false;
      String sRec = dmOld->sValueByRaw(i);
      long iRec = fDomainNone ? i  : domNew->iRaw(sRec);
      if (iUNDEF == iRec || iC >= ptr.iRecs())
        continue;
      ++iC;  
      for (c=0; c < tblSource->iCols(); ++c)
      {
        Column colOld = tblSource->col(c);
        Column colNew = ptr.col(c);
        if (colOld->fValues()) {
          double rVal = colOld->rValue(iOffsetOld+i-1);
          colNew->PutVal(iRec, rVal);
        }
        else {
          String sVal = colOld->sValue(iOffsetOld+i-1,0);
          colNew->PutVal(iRec, sVal);
        }
      }
    }
  }
  return true;
}

void TableChangeDomain::CheckColumn(const Column& col, bool fAgg)
{
  if (!col->dm()->pdsrt())
    ThrowIncompatibleDomainError(TR("Column Domain must be Class or ID"), col->fnObj);
  if (fAgg)
    return;
  for (long i=0; i < col->iRecs(); ++i)
  {
    long iRaw = col->iRaw(i);
    for (long j = 0; j < i; ++j)
      if ((iRaw == col->iRaw(j)) && (iRaw != iUNDEF))
        ThrowIncompatibleDomainError(TR("Column contains duplicates"),
                        col->fnObj);
  }
}

String TableChangeDomain::sExpression() const
{
  String sExpr;
  if (tblSource.fValid()) {
    String sTblName = tblSource->sNameQuoted(false, fnObj.sPath());
    if (colSource.fValid()) {
      String sAgg;
      switch (eAggregate) {
        case eNO:   sAgg = "no";   break;
        case eAVG:  sAgg = "avg";  break;
        case eSUM:  sAgg = "sum";  break;
        case eMIN:  sAgg = "min";  break;
        case eMAX:  sAgg = "max";  break;
        case eLAST: sAgg = "last"; break;
      }
      sExpr = String("TableChangeDomain(%S,%S,%S)",
                     sTblName, colSource->sNameQuoted(), sAgg);
    }
    else if (domNew.fValid()) {
      String sDomName = domNew->sNameQuoted(false, fnObj.sPath());
      sExpr = String("TableChangeDomain(%S,%S)",
                     sTblName, sDomName);
    }
  }
  return sExpr;
}






