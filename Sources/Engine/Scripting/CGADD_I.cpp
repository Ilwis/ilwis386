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
/* cgadd_i.c
  CodeGenerator::AddInst(const char*, long i)
	Last change:  JEL  11 Jul 97    8:42 am
*/

#include "Engine\Scripting\CODEGEN.H"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Function\FUNCUSER.H"
#include "Headers\Hs\mapcalc.hs"

#define errCALC 8000

void CodeGenerator::AddInstNbSum()
{
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, "nbsum");
  stkCalcVar.pop();
  double rStep = cv->dvs.rStep();
  RangeReal rr = cv->dvs.rrMinMax();
  rr.rLo() = min(0, rr.rLo()*9);
  rr.rHi() *= 9;
  DomainValueRangeStruct dvrs(rr.rLo(), rr.rHi(), rStep);
  inst->Add(new InstNbSum(inst, dvrs));
  cv = CalcVariable(dvrs, vtVALUE, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstNbAvg()
{
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, "nbavg");
  stkCalcVar.pop();
  double rStep = cv->dvs.rStep()/100;
  RangeReal rr = cv->dvs.rrMinMax();
  DomainValueRangeStruct dvrs(rr.rLo(), rr.rHi(), rStep);
  inst->Add(new InstNbAvg(inst, dvrs));
  cv = CalcVariable(dvrs, vtVALUE, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstNbStd()
{
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, "nbstd");
  stkCalcVar.pop();
  DomainValueRangeStruct dvrs(Domain("value")); // ?? could be more intelligent
  inst->Add(new InstNbStd(inst, dvrs));
  cv = CalcVariable(dvrs, vtVALUE, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstNbMinMax(const String& sInstruct, bool fPos, bool fCond)
{
  inst->Add(new InstNbMinMax(inst, fCIStrEqual(sInstruct.sLeft(5), "nbmin"), fPos, fCond));
  if (fCond) {
    CalcVariable cv = stkCalcVar.top();
    if (!cv->dvs.fValues())
      InvalidParm(1, sInstruct);
    stkCalcVar.pop();
  }
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, sInstruct);
  if (fPos){
    stkCalcVar.pop();
    cv = CalcVariable(DomainValueRangeStruct(1,9), vtVALUE, cv->gr, cv->rcSize);
    stkCalcVar.push(cv);
  }
}

void CodeGenerator::AddInstNbPrd(bool fPos, bool fCond)
{
  CalcVariable cv = stkCalcVar.top();
  inst->Add(new InstNbPrd(inst, fPos, fCond, cv->dvs.fUseReals()));
  if (fCond) {
    if (!cv->dvs.fValues())  {
      String sFunc = "nbprd";
      if (fPos)
        sFunc &= 'p';
      InvalidParm(1, sFunc);
    }
    stkCalcVar.pop();
  }
  if (fPos){
    stkCalcVar.pop();
    cv = CalcVariable(DomainValueRangeStruct(1,9), vtVALUE, cv->gr, cv->rcSize);
    stkCalcVar.push(cv);
  }
}

void CodeGenerator::AddInstNbCondPos()
{
  inst->Add(new InstNbCondPos(inst));
  CalcVariable cv = stkCalcVar.top();
  if (!cv->dvs.fValues())
    InvalidParm(0, "nbcndp");
  stkCalcVar.pop();
  cv = CalcVariable(DomainValueRangeStruct(1,9), vtVALUE, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstNbCnt()
{
  inst->Add(new InstNbCnt(inst));
  CalcVariable cv = stkCalcVar.top();
  stkCalcVar.pop();
  cv = CalcVariable(DomainValueRangeStruct(0,9), vtVALUE, cv->gr, cv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInst(const char* sInst, long iVal)
{
  String sInstruct = sInst;
  if (fCIStrEqual(sInstruct  , "statementstart")) {
  }
  else if (fCIStrEqual(sInstruct  , "statementend")) {
  }
  else if (fCIStrEqual(sInstruct  , "parmstart")) {
    stkParmColPos.push(Array<int>());
    stkParmLinePos.push(Array<int>());
  }
  else if (fCIStrEqual(sInstruct  , "parmnext")) {
    stkParmColPos.top() &= iCursorCol;
    stkParmLinePos.top() &= iCursorLine;
  }
  else if (fCIStrEqual(sInstruct  , "parmend")) {
    stkParmColPos.pop();
    stkParmLinePos.pop();
  }
  else if (fCIStrEqual(sInstruct  , "exprstart")) {
  }
  else if (fCIStrEqual(sInstruct  , "exprend")) {
  }
  else if (fCIStrEqual(sInstruct  , "assignexprstart")) {
  }
  else if (fCIStrEqual(sInstruct  , "assignexprend")) {
  }
  else if (fCIStrEqual(sInstruct  , "boolexprstart")) {
//    stkInstList.push(new InstList(SimpleCalcEnv()));
  }
  else if (fCIStrEqual(sInstruct  , "boolexprend")) {
//    InstList* lst = stkInstList.pop();
//    lst->iNr = lstInstList.iSize();
//    lstInstList &= lst;
  }
  else if (fCIStrEqual(sInstruct  , "intexprstart")) {
//    stkInstList.push(new InstList(SimpleCalcEnv()));
  }
  else if (fCIStrEqual(sInstruct  , "intexprend")) {
//    InstList* lst = stkInstList.pop();
//    lst->iNr = lstInstList.iSize();
//    lstInstList &= lst;
  }
  else if ((fCIStrEqual(sInstruct.sLeft(5) , "nbmin")) || (fCIStrEqual(sInstruct.sLeft(5) , "nbmax")))
    AddInstNbMinMax(sInstruct, 0 != (iVal & 1), 0 != (iVal & 2)); // fPos==1, fCond == 2
  else if (fCIStrEqual(sInstruct.sLeft(5) , "nbsum"))
    AddInstNbSum();
  else if (fCIStrEqual(sInstruct.sLeft(5) , "nbcnt"))
    AddInstNbCnt();
  else if (fCIStrEqual(sInstruct.sLeft(5) , "nbavg"))
    AddInstNbAvg();
  else if (fCIStrEqual(sInstruct.sLeft(5) , "nbstd"))
    AddInstNbStd();
  else if (fCIStrEqual(sInstruct.sLeft(6) , "nbcndp"))
    AddInstNbCondPos();
  else if (fCIStrEqual(sInstruct.sLeft(5) , "nbprd"))
    AddInstNbPrd(0 != (iVal & 1), 0 != (iVal & 2)); // fPos==1, fCond == 2
  else if (fCIStrEqual(sInstruct  , "stop")) {
    inst->Add(new InstStop(inst));
  }  
  else if (fCIStrEqual(sInstruct  , "undef")) {
    CalcVariable cv;
    cv.SetPointer(new CalcVarUndef());
    stkCalcVar.push(cv);
    inst->Add(new InstLoad(inst, cv));
  }
  else if (fCIStrEqual(sInstruct  , "color")) {
    inst->Add(new InstColor(inst));
    GeoRef gr;
    for (int i=0; i<3; i++) {
      CalcVariable cv = stkCalcVar.top();
      if (cv->gr.fValid())
        gr = cv->gr;
      stkCalcVar.pop();
    }
    RowCol rc(0L,0L);
    if (gr.fValid())
      rc = gr->rcSize();
    CalcVariable cv(Domain("color"), vtCOLOR, gr, rc);
    stkCalcVar.push(cv);
  }
  else if (fCIStrEqual(sInstruct  , "colorhsi")) {
    inst->Add(new InstColorHSI(inst));
    GeoRef gr;
    for (int i=0; i<3; i++) {
      CalcVariable cv = stkCalcVar.top();
      if (cv->gr.fValid())
        gr = cv->gr;
      stkCalcVar.pop();
    }
    RowCol rc(0L,0L);
    if (gr.fValid())
      rc = gr->rcSize();
    CalcVariable cv(Domain("color"), vtCOLOR, gr, rc);
    stkCalcVar.push(cv);
  }
  else if (fCIStrEqual(sInstruct , "inmask")) {
    GeoRef gr;
    for (int i=0; i<2; i++) {
      CalcVariable cv = stkCalcVar.top();
      if (cv->gr.fValid())
        gr = cv->gr;
      stkCalcVar.pop();
    }
    RowCol rc(0L,0L);
    if (gr.fValid())
      rc = gr->rcSize();
    inst->Add(new InstInMask(inst));
    CalcVariable cv(Domain("bool"), vtVALUE, gr, rc);
    stkCalcVar.push(cv);
  }
  else if (fCIStrEqual(sInstruct , "nbsta"))
    inst->Add(new InstNbStart(inst, iVal));
  else if (fCIStrEqual(sInstruct , "nbend"))
    inst->Add(new InstNbEnd(inst));
  else if (fCIStrEqual(sInstruct , "endfuncdef")) {
    inst->Add(new InstFuncEnd(inst, fdsCur));
    fds = fdsCur;
    fdsCur = 0;
    lstParms.pop();
  }
  else if (fCIStrEqual(sInstruct , "endprocdef")) {
    inst->Add(new InstFuncEnd(inst, fdsCur));
    fdsCur = 0;
    lstParms.pop();
  }
  else if (fCIStrEqual(sInstruct  , "label")) {
    if (iVal >= inst->lstLabels.iSize())
      inst->lstLabels.Resize(iVal+1);
    inst->lstLabels[iVal] = inst->lstInst.iSize();
  }
  else if (fCIStrEqual(sInstruct  , "goto")) {
   inst->Add(new InstGoto(inst, iVal));
  }
  else if (fCIStrEqual(sInstruct  , "gotofalse")) {
   inst->Add(new InstGotoFalse(inst, iVal));
  }
  else
    Error(String(TR("Invalid instruction: '%S'").c_str(), sInstruct), iCursorLine, iCursorCol);
}
