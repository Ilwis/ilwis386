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
/* $Log: /ILWIS 3.0/Calculator/CGADD_SS.cpp $
 * 
 * 10    10-05-04 17:15 Retsios
 * [bug=6521] error message for wrong parameters at function should not
 * depend on the case in which the user typed the function.
 * 
 * 9     5/29/00 10:50a Wind
 * adaption to allow ?tbl.ext.col[index] on commandline
 * 
 * 8     16-02-00 11:16 Wind
 * bug 1325
 * 
 * 7     12-01-00 11:38 Wind
 * cosmetic changes
 * 
 * 6     11-01-00 12:59 Wind
 * some polishing
 * 
 * 5     4-01-00 16:23 Wind
 * improved error message for ifundef and ifnotundef if parameters are
 * incompatible
 * 
 * 4     4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 3     12-11-99 9:21 Wind
 * bug 145 (two dim tab;le was not added to dependencies)
 * 
 * 2     3/12/99 3:04p Martin
 * Added support for case insensitive
// Revision 1.7  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.6  1997/09/22 15:09:24  Wim
// ifundef should use dmmerger for both params in AddInstCall()
//
// Revision 1.5  1997-09-16 20:45:36+02  Wim
// ValueRange of result of function depends on domain,
// no more vr.SetStep(1) which is totally redundant and wrong
//
// Revision 1.4  1997-09-11 17:33:43+02  Wim
// Table2Dim is now checking on correct domains
//
// Revision 1.3  1997/09/09 13:46:30  Wim
// Initialization sequence changed in AddInstIndexLoad()
//
// Revision 1.2  1997-08-15 19:50:36+02  Wim
// Added error message when using square brackets with too much parameters
//
/* cgadd_s.c
  CodeGenerator::AddInst(const char*, const String&, const String&)
  CodeGenerator::AddInst(const char*, const String&, const String&, const String& s)
	Last change:  WK   22 Sep 97    4:59 pm
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
#include "Headers\Hs\mapcalc.hs"

static VarType VarTypes[] = { vtVALUE,  vtMAP, vtTABLE, vtCOLUMN,
                              vtCOORD, vtTABLE2, vtNONE,
                              vtVALUE, vtVALUE, vtVALUE, vtVALUE,
                              vtSTRING, vtVALUE, vtCOLOR};

VarType vt(Array<char*>& sList, const String& sVal)
{
  for (int i = 0; i < sList.iSize(); i++)
    if (!stricmp(sList[i], sVal.scVal()))
      return VarTypes[i];
  return vtNONE;
}

VarType CodeGenerator::vt(const String& sVal)
{
  return ::vt(sVarTypes, sVal);
}

#define errCALC 8000

class DATEXPORT ErrorIncompatibleDoms: public ErrorObject
{
public:
  ErrorIncompatibleDoms(const String& sDom, const WhereError& where)
    : ErrorObject(WhatError(String(SCLCErrIncompatibleDomains_S.scVal(), sDom), errCALC), where)
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
	int i=0;
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

void CodeGenerator::AddInstIndexLoad(const String& sVal1, const String& sVal2)
{
  int iDim = sVal2.shVal();
  if (iDim < 1 || iDim > 2)
    Error(SCLCErrParmsExpected, iCursorLine, iCursorCol-1);
  if (iDim == 1) {
    CalcVariable cv = cvFind(sVal1, vtCOLUMN);
    CalcVariable cvIndex = stkCalcVar.top();
    stkCalcVar.pop();
    bool fErr = cv->col()->dmKey() != cvIndex->dvs.dm();
    if (fErr) {
      if (tbl.fValid() && !tbl->dm()->pdsrt()) { // only value allowed
        if (0 != cvIndex->dvs.dm()->pdv()) {
          fErr = false;
          cvIndex->SetDomainValueRangeStruct(cv->col()->dmKey());
          CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cvIndex.ptr);
          if (cvs)
            if (cvs->ics)
              cvs->ics->cv->SetDomainValueRangeStruct(cvIndex->dvs);
        }
      }
      else
        fErr = false;
    }
    if (!tbl.fValid()) {
      if (fErr)
        Error(ErrorIncompatibleDoms(cvIndex->dvs.dm()->sName(true, tbl->fnObj.sPath()), WhereError()).sShowError(), iCursorLine);
      stkCalcVar.push(cv);
      GeoRef gr = cvIndex->gr;
      cv->gr = gr;
      if (gr.fValid()) {
        inst->env.iLines = gr->rcSize().Row;
        inst->env.iCols = gr->rcSize().Col;
      }
      inst->Add(new InstIndexLoad(inst, cv, Table()));
    }
    else {
      if (fErr) {
        Column colIndex = cvIndex->col();
        if (colIndex.fValid()) {
          Error(ErrorIncompatibleDoms(cvIndex->dvs.dm()->sName(true, colIndex->fnTbl.sPath()),
                WhereErrorColumn(colIndex->fnTbl, colIndex->sTableAndColumnName(colIndex->fnTbl.sPath()))).sShowError(), iCursorLine);
        }
        else
          Error(ErrorIncompatibleDoms(cvIndex->dvs.dm()->sName(true, tbl->fnObj.sPath()), WhereError()).sShowError(), iCursorLine);
        fErr = true;
        stkCalcVar.push(cvIndex);
      }
      else {
        if (tbl->dm()->pdsrt() && cvIndex->dvs.dm()->pds()) {
          CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cvIndex.ptr);
          if (0 != cvs)
            cvs->ics->cv->SetDomainValueRangeStruct(tbl->dm());
        }
        stkCalcVar.push(cv);
      }
      inst->Add(new InstIndexLoad(inst, cv, tbl));
    }
  }
  else { // 2 dim table
    CalcVariable cv;
    Table2Dim tb2(sVal1);
    CalcVarTable2* cv2 = new CalcVarTable2(tb2);
    cv.SetPointer(cv2);
      lstCalcVars.top() &= cv;
    CalcVariable cvIndex2 = stkCalcVar.top();
    stkCalcVar.pop();
    CalcVariable cvIndex1 = stkCalcVar.top();
    stkCalcVar.pop();

    if (cvIndex1->dvs.dm() != tb2->dm1())
      Error(ErrorIncompatibleDoms(cvIndex1->dvs.dm()->sName(true, tb2->fnObj.sPath()),
        WhereError()).sShowError(), iCursorLine);
    if (cvIndex2->dvs.dm() != tb2->dm2())
      Error(ErrorIncompatibleDoms(cvIndex2->dvs.dm()->sName(true, tb2->fnObj.sPath()),
        WhereError()).sShowError(), iCursorLine);

    stkCalcVar.push(cv);
    stkCalcVar.top()->gr = cvIndex1->gr;
    inst->Add(new InstIndex2Load(inst, cv2, tbl));
    GeoRef gr = cvIndex1->gr;
    if (gr.fValid()) {
      inst->env.iLines = gr->rcSize().Row;
      inst->env.iCols = gr->rcSize().Col;
    }  
//    delete cvIndex1;
//    delete cvIndex2;
  }
}

void CodeGenerator::AddInstIndexStore(const String& sVal1, const String& sVal2)
{
  CalcVariable cv = cvFind(sVal1, vtCOLUMN);
  stkCalcVar.pop();
  CalcVariable cvIndex = stkCalcVar.top();
  stkCalcVar.pop();
  if (!cvIndex->dvs.fValues()) {
    Domain dmKey = cv->col()->dmKey();
    DomainSort* dmsrtKey = dmKey->pdsrt();
    if (0 != dmsrtKey) { // if index is string or same domain
      if ((0 == cvIndex->dvs.dm()->pds()) && (cvIndex->dvs.dm() != dmKey)) {
        Column colIndex = cv->col();
        Error(ErrorIncompatibleDoms(colIndex->dm()->sName(true, colIndex->fnTbl.sPath()),
            WhereErrorColumn(colIndex->fnTbl, colIndex->sTableAndColumnName(colIndex->fnTbl.sPath()))).sShowError(), iCursorLine, iCursorCol);
      }
    }
    if (0 != cvIndex->dvs.dm()->pds()) {
      cvIndex->SetDomainValueRangeStruct(cv->col()->dmKey());
      CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cvIndex.ptr);
      if (0 != cvs)
        cvs->ics->cv->SetDomainValueRangeStruct(tbl->dm());
    }
  }
  stkCalcVar.push(cv);
  inst->Add(new InstIndexStore(inst, cv, tbl));
}  

void CodeGenerator::AddInstTransform(const String& sVal1, const String& sVal2)
{
  CoordSystem csyOld, csyNew(sVal1);
  if (sVal2.length() != 0)
    csyOld = CoordSystem(sVal2);
  else {
    CalcVariable cv = stkCalcVar.top();
    DomainCoord* dmcrd = cv->dvs.dm()->pdcrd();
    if ((cv->vt != vtCOORD) && (0 == dmcrd)) {
      CoordinateNeeded("transform", 0);
//    Error(String("Invalid domain %S", cv->dvs.dm()->sName()),0);
      return;
    }  
    if (0 != dmcrd) {
      csyOld = dmcrd->cs();
      if (!csyNew->fConvertFrom(csyOld))
        Error(ErrorIncompatibleCoordSystems(csyNew->sName(true), csyOld->sName(true), WhereError(), 9999), iCursorLine, iCursorCol); // needs proper code
    }
  }
  inst->Add(new InstTransformCoord(inst, csyOld, csyNew));
  stkCalcVar.pop();  // coord
  Domain dom(FileName(), csyNew);
  CalcVariable cv(dom, vtCOORD);
  stkCalcVar.push(cv);
}  

void CodeGenerator::AddInstCall(const String& sVal1, const String& sVal2)
{
  int iParms = sVal2.shVal();

  // check existence of function on disk
  FileName fnFunc(sVal1, ".fun", true);
  if (File::fExist(fnFunc)) {
    if ((fdsCur == 0) || (fdsCur->sName != sVal1)) // otherwise loop in calls
      try {
        inst->Add(new InstEnterExternFunc(inst, iParms));
        FuncUser fnc(fnFunc);
        fnc->fdsParse(inst);
        if (fnc->iParms() == iParms) {
          ArrayCalcVariable acv;
          for (int i=0; i<iParms; i++) {
            acv.Insert(0, 1);
            if (stkCalcVar.fEmpty())
              return; // error
            acv[0] = stkCalcVar.top();
            stkCalcVar.pop();
          }
          GeoRef gr;
          for (int i=0; i<iParms; i++)
            if (acv[i]->gr.fValid()) {
              gr = acv[i]->gr;
              break;
            }   
          if (gr.fValid()) {
            inst->env.iLines = gr->rcSize().Row;
            inst->env.iCols = gr->rcSize().Col;
          }
          Domain dom = fnc->dmDefault();
          if (!dom.fValid())
            dom = Domain("value");
          VarType vt = vtVALUE;
          if (!dom->pdv())
            vt = vtSTRING;
          DomainValueRangeStruct dvrs(dom);
          CalcVariable cv(dvrs, vt, gr, RowCol());
          stkCalcVar.push(cv);
          inst->Add(new InstLeaveExternFunc(inst, iParms));
          return;
        }
      }
      catch (const ErrorObject& ) {
        DummyError();
        int iLine=iCursorLine, iCol=iCursorCol;
        if (!stkParmLinePos.fEmpty())
          if (stkParmLinePos.top().iSize()>0) {
            iLine = stkParmLinePos.top()[0];
            iCol = stkParmColPos.top()[0];
          }
      }
  }
  int i;
  // no external function found:
  ArrayCalcVariable acv;
  for (i=0; i<iParms; i++) {
    acv.Insert(0, 1);
    if (stkCalcVar.fEmpty())
      return; // error
    acv[0] = stkCalcVar.top();
    stkCalcVar.pop();
  }
  InternFuncDesc* fd = InternFuncDesc::fdFindFunc(lstInternFuncs, sVal1, acv);
  if (fd != 0) {
    if (!fCheckGeoRefs(acv)) {
      Error(String(SCLCErrIncompGeoRefsInFunc_S.scVal(), sVal1), iCursorLine);
      CalcVariable cv(Domain("value"), vtVALUE);
      stkCalcVar.push(cv);
      return;
    }
    GeoRef gr;
    for (i=0; i<iParms; i++)
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
    try {
      (*fd->fpdm)(dvs, sot, fd->fmath, acv);
    }
    catch (...) {
      dvs = DomainValueRangeStruct(Domain("value"));
      sot = sotRealVal;
    }
    if (iParms == 2) {
      if (fCIStrEqual(sVal1 , "ifundef")) {
        DomainMerger dmmrg(acv[0], acv[1]);
        if (!dmmrg.fIgnore())
          inst->admmrg &= dmmrg;
      }
      else if (fCIStrEqual(sVal1 , "ifnotundef")) {
        DomainMerger dmmrg(acv[1], CalcVariable());
        if (!dmmrg.fIgnore())
          inst->admmrg &= dmmrg;
      }
    }
    else if (iParms == 3) {
      if ((fCIStrEqual(sVal1 , "iff")) || (fCIStrEqual(sVal1 , "ifundef")) || (fCIStrEqual(sVal1 , "ifnotundef"))) {
        DomainMerger dmmrg(acv[1], acv[2]);
        if (!dmmrg.fIgnore())
          inst->admmrg &= dmmrg;
      }
    }
    RowCol rc(0L,0L);
    if (acv.iSize() > 0)
      rc = acv[0]->rcSize;
    CalcVariable cv(dvs, fd->vtRes, gr, rc);
    stkCalcVar.push(cv);
    int kk = inst->lstInst.iSize();
    for (int i=iParms-1; i >=0; i--) {
      if (fd->vtl[i] == vtCOLUMN) {
          for (int k= kk-1; k >= 0; k--) {
            InstLoad* instload = dynamic_cast<InstLoad*>((inst->lstInst)[k]);
            if (instload)
              if ((instload->cv->vt == vtCOLUMN) &&
                  (instload->cv->col() == acv[i]->col())) {
                instload->cv.SetPointer(new CalcVarColumn(acv[i]->col()));
                instload->cv->sot = sotColumn;
                kk = k; // start for next parm 
                break; 
             //   break;    7-feb-97: there could be another parm
            }
//          }
        }
      }
      else if (fd->vtl[i] == vtMAP)
        acv[i]->sot = sotMap;
    }
    inst->Add(new InstInternFunc(inst, fd, dvs, sot));
  }
  else {
    // check if func name exists
    int i=0;
    for (;i < lstInternFuncs.iSize(); i++) {
      if (fCIStrEqual(sVal1, lstInternFuncs[i]->sFunc)) {
        String sErr;
        int iWrongParm = 0;
        if (lstInternFuncs[i]->iParms() != iParms) {
          sErr = String(SCLCErrFuncRequiresParms_Si.scVal(), sVal1, lstInternFuncs[i]->iParms());
          iWrongParm = max(0,min(lstInternFuncs[i]->iParms(), iParms)-1);
        }
        else
          lstInternFuncs[i]->fCheckParms(acv, iWrongParm);
        int iLine=iCursorLine, iCol=iCursorCol;
        if (iWrongParm < stkParmLinePos.top().iSize()) {
          iLine = stkParmLinePos.top()[iWrongParm];
          iCol = stkParmColPos.top()[iWrongParm];
        }
        if (sErr.length() == 0) {
          if (fCIStrEqual(sVal1 , "ifundef") || fCIStrEqual(sVal1 , "ifnotundef")) 
            sErr = String(SCLCErrIncompatibleDomains_S.scVal(), acv[iWrongParm]->dvs.dm()->sName(true));
          else
            sErr = String(SCLCErrInvalidParameter_iS.scVal(), iWrongParm+1, sVal1);
        }
        Error(sErr, iLine, iCol);
        break;
      }
    }
    if (i == lstInternFuncs.iSize()) // not found
      Error(String(SCLCErrFuncParmNotFound_Si.scVal(), sVal1, acv.iSize()), iCursorLine, iCursorCol);
    CalcVariable cv(Domain("value"), vtVALUE);
    stkCalcVar.push(cv);
  }
}  

void CodeGenerator::AddInst(const char* sInst,
                            const String& sVal1, const String& sVal2)
{
  String sInstruct = sInst;
  if (fCIStrEqual(sInstruct , "call")) 
    AddInstCall(sVal1, sVal2);
  else if (fCIStrEqual(sInstruct , "transform")) 
    AddInstTransform(sVal1, sVal2);
  else if (fCIStrEqual(sInstruct , "indexload")) 
    AddInstIndexLoad(sVal1, sVal2);
  else if (fCIStrEqual(sInstruct , "indexstore"))
    AddInstIndexStore(sVal1, sVal2);
  else {
    Error(String(SCLCErrInvalidInstruction_S.scVal(), sInstruct), iCursorLine);
  }
}

void CodeGenerator::AddInst(const char* sInst,
                            const String& sVal1, const String& sVal2,
                            const String& sVal3)
{
  CalcVariable cv;
  String sInstruct = sInst;
  if (fCIStrEqual(sInstruct , "var")) {  // sVal1 : name; sVal2 : type; sVal3 : domain
    String sDomain = sVal3;
    if (fCIStrEqual(sVal2 , "string")) {
      cv.SetPointer(new CalcVarString(sVal1));
        lstCalcVars.top() &= cv;
    } 
    else if ((fCIStrEqual(sVal2 , "real")) ||(fCIStrEqual(sVal2 , "value"))) {
      cv.SetPointer(new CalcVarReal(sVal1));
        lstCalcVars.top() &= cv;
    } 
    else if (fCIStrEqual(sVal2 , "color")) {
      cv.SetPointer(new CalcVarColor(sVal1, Domain("color")));
        lstCalcVars.top() &= cv;
    } 
    else if (fCIStrEqual(sVal2 , "coord")) {
      String sCoordSys = sDomain;
      if (sCoordSys.length() == 0)
        sCoordSys = "unknown";
      cv.SetPointer(new CalcVarCoord(sVal1, Domain(FileName(),CoordSystem(sCoordSys))));
        lstCalcVars.top() &= cv;
    }  
    else {
      cv.SetPointer(new CalcVarInt(sVal1));
        lstCalcVars.top() &= cv;
    }  
  }
  else if (fCIStrEqual(sInstruct , "parm")) { // sVal1 : name; sVal2 : type; sVal3 : domain
    VarType vtp = vt(sVal2);
    String sDomain = sVal3;
//    if ((vtp == vtVALUE) && (sVal2 != sVAR))
    if (sDomain.length() == 0)
      sDomain = sVal2;
    if (sDomain.length() == 0)
      sDomain = "value";
    Domain dom;
    if ((!fCIStrEqual(sVal2 , "coord")) && (!fCIStrEqual(sVal2 , "color")) && (!fCIStrEqual(sVal2 , "map")))
      dom = Domain(sDomain);
    cv.SetPointer(new CalcVarParm(dom, vtp, sVal1));
    fdsCur->iPrms++;
    lstParms.top() &= cv;
  }
  else {
    Error(String(SCLCErrInvalidInstruction_S.scVal(), sInstruct), iCursorLine);
  }
}




