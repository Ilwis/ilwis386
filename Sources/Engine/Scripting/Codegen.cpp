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
/* $Log: /ILWIS 3.0/Calculator/Codegen.cpp $
 * 
 * 14    9/24/03 3:36p Martin
 * Merged from AggregateValue
 * 
 * 15    7/23/03 8:37a Martin
 * Changed the behavior of the cvFind function for simplecalc expressions.
 * For unknown reasons it did not accepts maps as paramters (except the
 * first). This was needed for AggregateValue. 
 * 
 * 13    2/21/02 11:45a Martin
 * default table of IndexLoad instruction can be overruled by a different
 * table to enable the use of data from different tables
 * 
 * 12    13-11-00 11:32a Martin
 * extra check for validity of the cv. Unknown vars get caught now.
 * 
 * 11    25-09-00 10:23 Koolhoven
 * in cvFind() only reset iLine and iCol when stack is really not empty
 * 
 * 10    5/29/00 10:51a Wind
 * adaption to allow ?tbl.ext.col[index] on commandline
 * 
 * 9     3/29/00 3:03p Wind
 * added true, false and "true", "false" in all calculators
 * 
 * 8     14-02-00 16:13 Wind
 * added functions to retrieve dependent objects from instructions (bug
 * 404)
 * 
 * 7     12-01-00 11:38 Wind
 * cosmetic changes
 * 
 * 6     11-01-00 12:59 Wind
 * some polishing
 * 
 * 5     10-01-00 17:34 Wind
 * removed static dummy arrays
 * 
 * 4     4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 3     10/18/99 9:35a Wind
 * bugs with params in user defined functions
 * 
 * 2     3/12/99 3:04p Martin
 * Added support for case insensitive
// Revision 1.3  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.2  1997/08/18 08:29:31  Wim
// Protected DetermineGeoRef() against empty calc stack
//
/* codegen.c
	Last change:  WK   10 Aug 98   12:04 pm
*/
#define CODEGEN_C
#include "Engine\Scripting\CODEGEN.H"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\Parser.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Base\File\File.h"
#include "Engine\Domain\dmgroup.h"
#include "Headers\Hs\mapcalc.hs"

//static Array<Column> acDummy;
//static Array<Map> amDummy;

/*extern "C" {
  int _matherr(struct exception *e)
  {
    e->retval = rUNDEF;
    return 1;
  }
}*/

//static String sDum;

CodeGeneratorBase::CodeGeneratorBase()
//: sExpr(sDum)
{
}

CodeGeneratorBase::~CodeGeneratorBase()
{
}

CodeGenerator::CodeGenerator(CalcType ctp, Instructions* inst)
{
  ct = ctp;
  Init(inst);
}

CodeGenerator::CodeGenerator(const Table& table, Instructions* inst)
: tbl(table)
  {
    Init(inst);
    ct = ctCOL;
  }

CodeGenerator::~CodeGenerator()
{ 
    int i;
    while (!stkCalcVar.fEmpty())
      stkCalcVar.pop();

    while (!stkParmLinePos.fEmpty())
      stkParmLinePos.pop();
    while (!stkParmColPos.fEmpty())
      stkParmColPos.pop();

    for (i=0; i < lstInternFuncs.iSize(); i++)
      if (0 != lstInternFuncs[i])
        delete lstInternFuncs[i];
}

void CodeGenerator::Init(Instructions* instruc)
{
  iCursorLine = iCursorCol = 0;
  seedrand(clock());
  fds = fdsCur = 0;
  sVarTypes &= sVAR;
  sVarTypes &= sMAP;
  sVarTypes &= sTABLE;
  sVarTypes &= sCOLUMN;
  sVarTypes &= sCOORD;
  sVarTypes &= sTABLE2;
  sVarTypes &= sCLFY;
  sVarTypes &= sBYTE;
  sVarTypes &= sINT;
  sVarTypes &= sREAL;
  sVarTypes &= sBOOL;
  sVarTypes &= sSTRING;
  sVarTypes &= sVALUE;
  sVarTypes &= sCOLOR;
  lstCalcVars.push(Array<CalcVariable>());
  CalcVariable cv;
  cv.SetPointer(new CalcVarReal("pi"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(M_PI);
  cv.SetPointer(new CalcVarUndef());
  lstCalcVars.top() &= cv;
  cv.SetPointer(new CalcVarReal("pidiv2"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(M_PI_2);
  cv.SetPointer(new CalcVarReal("pidiv4"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(M_PI_4);
  cv.SetPointer(new CalcVarReal("pi2"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(2*M_PI);
  cv.SetPointer(new CalcVarInt("true"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(1L);
  cv.SetPointer(new CalcVarInt("false"));
  lstCalcVars.top() &= cv;
  cv->pcvSimple()->PutVal(0L);
  InternFuncDesc::CreateInternFuncs(lstInternFuncs);
  SetInst(instruc);
}

void CodeGenerator::SetInst(Instructions* instruc)
{
  inst = instruc;
}

void CodeGenerator::DetermineDomain()
{
  if (!stkCalcVar.fEmpty()) {
    inst->_dm = stkCalcVar.top()->dvs.dm();
    ValueRange vr = stkCalcVar.top()->dvs.vr();
    if (vr.fValid())
      inst->_vr = vr;
  }
  else {
    inst->_dm = Domain("value");
    inst->_vr = ValueRange();
  }
  if (inst->_vr.fValid())
    if (inst->_vr->vrr())
     inst->_vr->vrr()->AdjustRangeToStep();
  if (0 != inst->_dm->pdv())
    return;
}

void CodeGenerator::DetermineGeoRef()
{
  bool fOk = !stkCalcVar.fEmpty();
  if (fOk) {
    inst->_gr = (stkCalcVar.top())->gr;
    fOk = inst->_gr.fValid();
  }
  if (fOk) {
    inst->env.iLines = inst->_gr->rcSize().Row;
    inst->env.iCols = inst->_gr->rcSize().Col;
  }  
  else {
    inst->_gr = GeoRef(String("none"));
    inst->env.iLines = 0;
    inst->env.iCols = 0;
  }
}

void CodeGenerator::Error(const String& s, int iLine, int iCol) const
{
  String sTitle;
  IlwisError err;
  if (iCol == shUNDEF)
    iCol = iCursorCol;
  switch (ct) {
    case ctSIMPLE:
      sTitle = "Calculate";
      err = errCalculate;
      break;
    case ctMAP:
      sTitle = "MapCalculate";
      err = errMapCalculate;
      break;
    case ctCOL:
      sTitle = "ColumnCalculate";
      err = errColumnCalculate;
      break;
    default :
      break;
  }
  CalcError(sExpr, s, sTitle, err, iLine, iCol+1).Show();
  DummyError();
}

void CodeGenerator::Error(const ErrorObject& err, int iLine, int iCol) const
{
  String sTitle;
  switch (ct) {
    case ctSIMPLE:
      sTitle = "Calculate";
      break;
    case ctMAP:
      sTitle = "MapCalculate";
      break;
    case ctCOL:
      sTitle = "ColumnCalculate";
      break;
    default :
      break;
  }
  CalcError(sExpr, err.sWhat(), sTitle, err.err(), iLine, iCol+1).Show();
  DummyError();
}

void CodeGenerator::Get(ObjectDependency& objdep)
{
  inst->Get(objdep);
  /*
  for (int i = 0; i < lstCalcVars.top().iSize(); i++) {
    CalcVariable cv = lstCalcVars.top()[i];
    if (cv->pcvMap())
      objdep.Add(cv->pcvMap()->map().ptr());
    else if (cv->pcvCol())
      objdep.Add(cv->pcvCol()->col().ptr());
    else if (cv->pcvTb2())
      objdep.Add(cv->pcvTb2()->tb2().ptr());
  }
  for (i = 0; i < inst->aGroupDom.iSize(); i++)
    objdep.Add(inst->aGroupDom[i]);*/
}

CalcVariable CodeGenerator::cvFind(const String& sName, VarType vt)
{
  CalcVariable cv;
  int iLine=iCursorLine, iCol=iCursorCol;
  int i;
  cv = cvFindParm(sName, vt);
  if (cv.fValid())
    return cv;
  stack<Array<CalcVariable> > stkTemp;
  while (!lstCalcVars.empty()) {
    stkTemp.push(lstCalcVars.top());
    lstCalcVars.pop();
    
    for (i=0; i < stkTemp.top().iSize(); i++)
      if (fCIStrEqual(stkTemp.top()[i]->sName() , sName)) {
        cv = stkTemp.top()[i];
        break;
      }
    for (i=0; i < stkTemp.top().iSize(); i++)
      if ((stkTemp.top()[i]->vt == vt) && (fCIStrEqual(stkTemp.top()[i]->sName() , sName))){
        cv = stkTemp.top()[i];
        break;
      }
    if (vt != vtVALUE)
      for (i=0; i < stkTemp.top().iSize(); i++)
        if ((stkTemp.top()[i]->vt == vtVALUE) && (fCIStrEqual(stkTemp.top()[i]->sName() , sName))) {
          cv = stkTemp.top()[i];
          break;
        }
    else if (vt != vtSTRING)
      for (i=0; i < stkTemp.top().iSize(); i++)
        if ((stkTemp.top()[i]->vt == vtSTRING) && (fCIStrEqual(stkTemp.top()[i]->sName() , sName))){
          cv = stkTemp.top()[i];
          break;
        }
    else if (vt != vtCOORD)
      for (i=0; i < stkTemp.top().iSize(); i++)
        if ((stkTemp.top()[i]->vt == vtCOORD) && (fCIStrEqual(stkTemp.top()[i]->sName() , sName))){
          cv = stkTemp.top()[i];
          break;
        }
    else if (vt != vtCOLOR)
      for (i=0; i < stkTemp.top().iSize(); i++)
        if ((stkTemp.top()[i]->vt == vtCOLOR) && (fCIStrEqual(stkTemp.top()[i]->sName() , sName))){
          cv = stkTemp.top()[i];
          break;
        }
    if (cv.fValid()) 
      break;
  }
  while (!stkTemp.empty()) { // restore original stack
    lstCalcVars.push(stkTemp.top());
    stkTemp.pop();
  }
  if (cv.fValid()) 
    return cv;

  if (!stkParmLinePos.fEmpty()) {
    int i = stkParmLinePos.top().iSize()-1;
		if (i >= 0) {
			iLine = stkParmLinePos.top()[i];
			iCol = stkParmColPos.top()[i];
		}
  }
  switch (vt) {
    case vtMAP :
      { 
        //if (ct != ctSIMPLE) { // this limitations was for unknown purposes, 
		//it is overruled because of the form of the expression for aggregatevalue (third parm is a map)
			Map map;
			try 
			{
				 map = Map(sName, fnObj.sPath());
			}
			catch (const ErrorObject& ) 
			{
				Error(String(SCLCErrMapNotFound_S.scVal(), sName), iLine, iCol);
			}
			cv.SetPointer(new CalcVarMap(map));
			lstCalcVars.top() &= cv;
			return cv;
	//			}
      }
    case vtCOLUMN :
      { Column col;
        try {
//            if (tblSearch.fValid())
//              col = Column(tblSearch, sName);
          if (!col.fValid())
            col = Column(tbl, sName);
          if (!col.fValid())
            Error(String(SCLCErrColumnNotFound_S.scVal(), sName), iLine, iCol);
        }
        catch (const ErrorObject& ) {
          Error(String(SCLCErrColumnNotFound_S.scVal(), sName), iLine, iCol);
        }
				if ( col->fnTbl != tbl->fnObj ) // previous assumption incorrect
					tbl = Table(col->fnTbl);
        cv.SetPointer(new CalcVarColumn(col));
        lstCalcVars.top() &= cv;
        return cv;
      }
    default :
      if (ct != ctSIMPLE && cv.fValid()) 
        return cv;
  }

  Error(String(SCLCErrVariableNotFound_S.scVal(), sName), iLine, iCol);
  return cv;
}

CalcVariable CodeGenerator::cvFindParm(const String& sName, VarType vt)
{
  int i;
  if (!lstParms.empty()) 
    for (i=0; i < lstParms.top().iSize(); i++)
      if (lstParms.top()[i]->sName() == sName)
        return lstParms.top()[i];
  return CalcVariable();
}

CalcVariable CodeGenerator::cvCreate(const String& sName, const CalcVariable& cv)
{
  switch (cv->vt) {
    case vtMAP :
      {
        // check if it is a stored map that is already in use
        CalcVariable cvExist = cvFind(sName, cv->vt);
        if (cvExist.fValid()) {
          Map map = cvExist->map();
          if (map.fValid())
//            if (map->pms())
            if (!map->fDependent())
              return cvExist;
        }
        Map map(sName, cv->gr, cv->rcSize, cv->dvs);
        CalcVariable cv;
        cv.SetPointer(new CalcVarMap(map));
        return cv;
      }
    case vtCOLUMN :
      { Column col = tbl->colNew(sName, cv->dvs);
        if (!col.fValid())
          throw ErrorNotCreated(WhereErrorColumn(tbl->fnObj, sName));
        CalcVariable cv;
        cv.SetPointer(new CalcVarColumn(col));
        return cv;
      }
    case vtVALUE :
    case vtSTRING:
    case vtCOORD:
    case vtCOLOR:
      {
        return cvFind(sName, cv->vt);
      }
    default :
      return CalcVariable();
  }
}


void CodeGenerator::InvalidParm(int iParm, const String& sFunc)
{
  int iLine=iCursorLine, iCol=iCursorCol;
  if (iParm < stkParmLinePos.top().iSize()) {
    iLine = stkParmLinePos.top()[iParm];
    iCol = stkParmColPos.top()[iParm];
   }
   Error(String(SCLCErrInvalidParameter_iS.scVal(), iParm+1, sFunc), iLine, iCol);
}


int CodeGenerator::iNewLabel()
{
  return inst->iNewLabel();
}
