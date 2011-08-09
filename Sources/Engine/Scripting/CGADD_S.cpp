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
/* cgadd_s.c
  CodeGenerator::AddInst(const char*, const String&)
	Last change:  WK   16 Sep 97    5:57 pm
*/

#include "Engine\Scripting\CODEGEN.H"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Function\FUNCUSER.H"
#include "Engine\Function\FLTLIN.H"
#include "Headers\Hs\mapcalc.hs"

#define errCALC 8000

class DATEXPORT ErrorIncompatibleDoms: public ErrorObject
{
public:
  ErrorIncompatibleDoms(const String& sDom, const WhereError& where)
    : ErrorObject(WhatError(String(TR("Incompatible domain: %S").c_str(), sDom), errCALC), where)
    {}
};

static bool fCheckGeoRefs(const Array<CalcVariable>& acv)
{
  // check for existence of GeoRrefNone
  bool fExistGeoRefNone = false;
  
  for (int i=0; i < acv.iSize(); i++) 
    if (acv[i]->gr.fValid())
      if (acv[i]->gr->fGeoRefNone()) {
        fExistGeoRefNone = true;
        break;
      }  
  if (fExistGeoRefNone) {
    // find size of a map
    RowCol rcSize = rcUNDEF;
	int i=0;
    for (; i < acv.iSize(); i++) 
      if (acv[i]->map().fValid()) {
        rcSize = acv[i]->map()->rcSize();
        break;
      }  
    if (rcSize != rcUNDEF) { // check rest on this size
      for (; i < acv.iSize(); i++) 
        if (acv[i]->map().fValid())
          if (rcSize != acv[i]->map()->rcSize())
            return false;
    }
  }
  else { // check georefs        
    GeoRef gr;
    // find georef of a map
	unsigned int i=0;
    for (; i < acv.iSize(); i++) 
      if (acv[i]->gr.fValid()) {
        gr = acv[i]->gr;
        break;
      }  
    if (gr.fValid()) { // check rest on this georef
      for (; i < acv.iSize(); i++) 
        if (acv[i]->gr.fValid())
          if (gr != acv[i]->gr)
            return false;
    }
  } 
  return true;
}

void CodeGenerator::AddOperator(const String& sVal)
{
  ArrayCalcVariable acv;
  if (stkCalcVar.fEmpty())
    return;
  acv &= stkCalcVar.top();
  stkCalcVar.pop();
  InternFuncDesc* fd = InternFuncDesc::fdFindFunc(lstInternFuncs, sVal, acv);
  if (fd != 0) {
    GeoRef gr = acv[0]->gr;
    DomainValueRangeStruct dvs;
    StackObjectType sot;
    (*fd->fpdm)(dvs, sot, fd->fmath, acv);
    if (gr.fValid()) {
      inst->env.iLines = gr->rcSize().Row;
      inst->env.iCols = gr->rcSize().Col;
    }
    RowCol rc = acv[0]->rcSize;
    CalcVariable cv(dvs, fd->vtRes, gr, rc);
    stkCalcVar.push(cv);
    inst->Add(new InstInternFunc(inst, fd, dvs, sot));
  }
  else {
    if (!stkCalcVar.fEmpty()) {
      acv.Insert(0, 1);
      acv[0] = stkCalcVar.top();
      stkCalcVar.pop();
    }
    InternFuncDesc* fd = InternFuncDesc::fdFindFunc(lstInternFuncs, sVal, acv);
    if (fd != 0) {
      if (!fCheckGeoRefs(acv)) {
        Error(String(TR("Incompatible georefs for function %S").c_str(), sVal), iCursorLine, iCursorCol);
        CalcVariable cv(Domain("value"), vtVALUE);
        stkCalcVar.push(cv);
        return;
      }
      GeoRef gr;
      for (unsigned int i=0; i<acv.iSize(); i++)
        if (acv[i]->gr.fValid()) {
          gr = acv[i]->gr;
          break;
        }
      if (gr.fValid()) {
        inst->env.iLines = gr->rcSize().Row;
        inst->env.iCols = gr->rcSize().Col;
      }
      DomainValueRangeStruct dvs;
      StackObjectType sot;
      (*fd->fpdm)(dvs, sot, fd->fmath, acv);
      RowCol rc = acv[0]->rcSize;
      CalcVariable cv(dvs, fd->vtRes, gr, rc);
      stkCalcVar.push(cv);
      inst->Add(new InstInternFunc(inst, fd, dvs, sot));
    }
    else {
        // check if func name exists
      for (unsigned int i=0; i < lstInternFuncs.iSize(); i++) {
        if (sVal == lstInternFuncs[i]->sFunc) {
          int iWrongParm = shUNDEF;
          lstInternFuncs[i]->fCheckParms(acv, iWrongParm);
          int iLine=iCursorLine, iCol=iCursorCol;
          if (!stkParmLinePos.fEmpty())
            if (iWrongParm < (int)stkParmLinePos.top().iSize()) {
              iLine = stkParmLinePos.top()[iWrongParm];
              iCol = stkParmColPos.top()[iWrongParm];
            }
          Error(String(TR("Invalid operand %i for operator %S").c_str(), iWrongParm+1, sVal), iLine, iCol);
        }
      }
      CalcVariable cv(Domain("value"), vtVALUE);
      stkCalcVar.push(cv);
    }
  }
}

void CodeGenerator::AddInst1(const String& sInstruct,
                             const String& sVal)
{
  if (fCIStrEqual(sInstruct , "operator")) {
    AddOperator(sVal);
    return;
  }
  if (fCIStrEqual(sInstruct  , "consti")) {
    long iVal = sVal.iVal();
    CalcVariable cv;
    cv.SetPointer(new CalcVarConstInt(iVal));
    stkCalcVar.push(cv);
    inst->Add(new InstConstInt(inst, iVal));
  }
  else if (fCIStrEqual(sInstruct  , "constr")) {
   double rVal = sVal.rVal();
   CalcVariable cv;
   cv.SetPointer(new CalcVarConstReal(rVal, sVal));
   stkCalcVar.push(cv);
   inst->Add(new InstConstReal(inst, rVal, sVal));
  }
  else if (fCIStrEqual(sInstruct  , "consts")) {
    InstConstString* ics = new InstConstString(inst, sVal);
    CalcVariable cv ;
    cv.SetPointer(new CalcVarConstString(sVal, ics));
    stkCalcVar.push(cv);
    inst->Add(ics);
  }
  else {
    Error(String(TR("Invalid instruction: '%S'").c_str(), sInstruct), iCursorLine, iCursorCol);
  }
}

void CodeGenerator::AddInstLoad(const String& sVal, bool fNb)
{
  CalcVariable cv;
  if ( sVal == "%l" || sVal == "%L") {
    cv.SetPointer(new CalcVarInt("%l"));
    cv->SetDomainValueRangeStruct(DomainValueRangeStruct(1, inst->env.iLines));
    stkCalcVar.push(cv);
    inst->Add(new InstCurrLine(inst));
    return;
  }  
  if ( sVal == "%c" || sVal == "%C") {
    cv.SetPointer(new CalcVarInt("%c"));
    cv->SetDomainValueRangeStruct(DomainValueRangeStruct(1, inst->env.iCols));
    stkCalcVar.push(cv);
    inst->Add(new InstCurrColumn(inst));
    return;
  }  
  if (tbl.fValid() && ( sVal == "%r" || sVal == "%R")) {
    cv.SetPointer(new CalcVarInt("%r"));
    cv->SetDomainValueRangeStruct(DomainValueRangeStruct(0, inst->env.iRecs));
    stkCalcVar.push(cv);
    inst->Add(new InstCurrRecord(inst, tbl->dm()));
    return;
  }
  if (tbl.fValid() && ( sVal == "%k" || sVal == "%K")) {
     if (0 != tbl->dm()->pdnone()) { // key == record nr
      AddInst("load", "%r");
      return;
    }
    if (0 != tbl->dm()->pdv()) 
      cv.SetPointer(new CalcVarInt("%k"));
    else
      cv.SetPointer(new CalcVarString("%k"));
    cv->SetDomainValueRangeStruct(tbl->dm());
    stkCalcVar.push(cv);
    inst->Add(new InstCurrKey(inst, tbl->dm()));
    return;
  }
  if (fCIStrEqual(sVal,"%x")) {
    cv.SetPointer(new CalcVarReal("%x"));
    cv->SetDomainValueRangeStruct(Domain("value"));
    stkCalcVar.push(cv);
    inst->Add(new InstCurrCoordX(inst));
    return;
  }
  if (fCIStrEqual(sVal,"%y")) {
    cv.SetPointer(new CalcVarReal("%y"));
    cv->SetDomainValueRangeStruct(Domain("value"));
    stkCalcVar.push(cv);
    inst->Add(new InstCurrCoordY(inst));
    return;
  }
  if (fCIStrEqual(sVal,"%p") || fCIStrEqual(sVal,"nbpos")) {
    cv.SetPointer(new CalcVarInt("%p"));
    cv->SetDomainValueRangeStruct(DomainValueRangeStruct(1, 9));
    stkCalcVar.push(cv);
    inst->Add(new InstNbPos(inst));
    return;
  }
  if (fCIStrEqual(sVal,"%d") || fCIStrEqual(sVal,"nbdis")) {
    cv.SetPointer(new CalcVarReal("%d"));
    Domain dm("distance");
    cv->SetDomainValueRangeStruct(DomainValueRangeStruct(dm));
    stkCalcVar.push(cv);
    inst->Add(new InstNbDis(inst));
    return;
  }
  String sRight2 = sVal.sRight(2);
  VarType vt = vtMAP;
  if (tbl.fValid())
    vt = vtCOLUMN;
  if (!lstParms.empty())  {
    for (unsigned int j=0; j < lstParms.top().iSize(); j++)
      if (lstParms.top()[j]->sName() == sVal) {
        cv = CalcVariable(lstParms.top()[j]);
        stkCalcVar.push(cv);
        inst->Add(new InstLoadParm(inst, j));
        return;
      } 
  }
  cv = cvFind(sVal, vt);
  if (cv->vt == vtCOLUMN) {
    Column colIndex = cv->col();
    if (tbl.fValid() && colIndex->fnTbl.sFullName() != tbl->fnObj.sFullName()) // check domain types
      if (colIndex->dmKey() != tbl->dm()) {
        Error(ErrorIncompatibleDoms(colIndex->dmKey()->sName(true, colIndex->fnTbl.sPath()),
               WhereErrorColumn(colIndex->fnTbl, colIndex->sTableAndColumnName(colIndex->fnTbl.sPath()))).sShowError(), iCursorLine, iCursorCol);
      }
  }
  InstLoad* instl = new InstLoad(inst, cv, fNb);
  cv->SetInstruc(instl);
  inst->Add(instl);
  stkCalcVar.push(cv);
  if (cv->gr.fValid()) {
    inst->env.iLines = cv->gr->rcSize().Row;
    inst->env.iCols = cv->gr->rcSize().Col;
  }
}

void CodeGenerator::AddInstNbNum(const String& sVal)
{
  Map mp(sVal);
  inst->Add(new InstNbNum(inst, mp));
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, "#[..]");
  stkCalcVar.pop();
  cv.SetPointer(new CalcVarMap(mp));
  stkCalcVar.push(cv);
}


void CodeGenerator::AddInstNbFlt(const String& sVal)
{
  Filter flt(sVal);
  FilterLinear* fltLin = dynamic_cast<FilterLinear*>(flt.ptr());
  if (0 == fltLin)
    Error(String(TR("Not a linear filter: '%S'").c_str(), sVal), iCursorLine, iCursorCol);
  if ((flt->iRows() != 3) || (flt->iCols() != 3))
    Error(String("%S '%S' : %i x %i", TR("Invalid size for filter"), sVal, flt->iRows(), flt->iCols()), iCursorLine, iCursorCol);
  InstNbFlt* instnbf = new InstNbFlt(inst, fltLin);
  inst->Add(instnbf);
  CalcVariable cv(instnbf->dvsNbFlt, vtVALUE);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstClfy(const String& sVal)
{
  Domain dom(sVal);
  DomainGroup* pdgrp = dom->pdgrp();
  if (0 == pdgrp) {
    Error(String(TR("Only group domain allowed: '%S'").c_str(), dom->sName(true)), iCursorLine, iCursorCol);
    return;
  }
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    throw ErrorValueDomain(cv->dvs.dm()->sName(true), FileName(), 1);// needs a proper number
  cv = stkCalcVar.top();
  stkCalcVar.pop();
  inst->Add(new InstClfy(inst, dom, cv->dvs));
//  inst->aGroupDom &= pdgrp;
  cv = CalcVariable(dom, cv->vt, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}
