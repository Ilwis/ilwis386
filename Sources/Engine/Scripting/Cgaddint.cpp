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
/* cgaddint.c
  CodeGenerator::AddInst(const char*, const String&)
  mainly 'internal functions'
	Last change:  WK   24 Mar 98   12:36 pm
*/
#include "Engine\Scripting\CODEGEN.H"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Scripting\InstAggregateValue.h"
#include "Engine\Function\FUNCUSER.H"
#include "Headers\Hs\mapcalc.hs"


#define errCALC 8000

class DATEXPORT ErrorIncompatibleDoms: public ErrorObject
{
public:
  ErrorIncompatibleDoms(const String& sDom, const WhereError& where)
    : ErrorObject(WhatError(String(TR("Incompatible domain: %S").c_str(), sDom), errCALC), where)
    {}
};

void CodeGenerator::FuncNotAllowedInUserDefFunc(const String& sFunc) const
{
  int iLine=iCursorLine, iCol=iCursorCol;
  if (!stkParmLinePos.fEmpty())
    if (stkParmLinePos.top().iSize()>0) {
      iLine = stkParmLinePos.top()[0];
      iCol = stkParmColPos.top()[0];
    }
  Error(String(TR("Function '%S' not allowed with parm in user defined function").c_str(), sFunc), iLine, iCol);
}

void CodeGenerator::CoordinateNeeded(const String& sFunc, int iParm) const
{
  int iLine=iCursorLine, iCol=iCursorCol;
  if (!stkParmLinePos.fEmpty())
    if (stkParmLinePos.top().iSize()>0) {
      iLine = stkParmLinePos.top()[iParm];
      iCol = stkParmColPos.top()[iParm];
    }
  Error(String(TR("Coordinate needed as parameter for function '%S'").c_str(), sFunc), iLine, iCol);
}

void CodeGenerator::AddInstCoord(const String& sVal)
{
  CoordSystem csy = CoordSystem(sVal);
  Domain dom(FileName(), csy);
  inst->Add(new InstCoord(inst, dom));
  stkCalcVar.pop();  // y
  stkCalcVar.pop();  // x
  CalcVariable cv(dom, vtCOORD);
  stkCalcVar.push(cv);
}  

void CodeGenerator::AddInstMapValue(const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtNONE).fValid())
    FuncNotAllowedInUserDefFunc("mapvalue");
  BaseMap bmap(sVal);
  CoordSystem csIn;
  CalcVariable cv = stkCalcVar.top();  // coord
  DomainCoord* pdcrd = cv->dvs.dm()->pdcrd();
  if (0 == pdcrd) { // error
    CoordinateNeeded("mapvalue", 1);
    return;
  }
  inst->Add(new InstMapValue(inst, bmap, csIn));
  stkCalcVar.pop();  // coord
	MapPtr *rmp = dynamic_cast<MapPtr *> ( bmap.pointer() );
	if ( rmp== 0 )
		cv = CalcVariable(bmap->dvrs(), vtVALUE);
	else
		cv = CalcVariable(rmp->dvrs(), vtVALUE, rmp->gr(), rmp->rcSize());

  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstRasValue(const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc("rasvalue");
  Map mp(sVal);
  inst->Add(new InstRasValue(inst, mp));
  stkCalcVar.pop();  // col
  stkCalcVar.pop();  // row
  CalcVariable cv(mp->dvrs(), vtVALUE, mp->gr(), mp->rcSize());
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstReturnType(const String& sVal)
{
  Domain dom;
  if (!fCIStrEqual(sVal , "coord"))
    try {
      dom = Domain(sVal);
      fdsCur->dmDefault = dom;
    }
    catch (const  ErrorObject& err) {
      err.Show();
      DummyError();
    }
  else {
    dom = Domain(FileName(), CoordSystem());
    fdsCur->dmDefault = dom;
  }
  if (dom->pdv())
    fdsCur->vtRes = vtVALUE;
  else
    fdsCur->vtRes = vt(sVal);
}  

void CodeGenerator::AddInstMinMaxVal(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  BaseMap bmap(sVal);
  if (bmap->fRealValues()) {
    RangeReal rrMinMax = bmap->rrMinMax();
    double r;
    if (fCIStrEqual(sFunc , "mapmin"))
      r =  rrMinMax.rLo();
    else
      r = rrMinMax.rHi();
    AddInst1("constr", String("%g",r));
  }
  else if (bmap->fValues()) {
    RangeInt riMinMax = bmap->riMinMax();
    long i;
    if (fCIStrEqual(sFunc , "mapmin"))
      i =  riMinMax.iLo();
    else
      i = riMinMax.iHi();
    AddInst1("consti", String("%li", i));
  }
  else {
    String sErr(TR("Only value domain allowed: '%S'").c_str(), bmap->dm()->sName(true, bmap->fnObj.sPath()));
    Error(sErr, iCursorLine);
  }
  MapPtr* map = dynamic_cast<MapPtr*>(bmap.ptr());
  if (0 != map) {
    CalcVariable cv = stkCalcVar.top();
    cv->gr = map->gr();
    cv->rcSize = map->rcSize();
  }
}

void CodeGenerator::AddInstMapRowsCols(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  Map map(sVal);
  long i;
  if (fCIStrEqual(sFunc , "maprows"))
    i = map->iLines();
  else if (fCIStrEqual(sFunc,"mapcols"))
    i = map->iCols();
  AddInst1("consti", String("%li", i));
  CalcVariable cv = stkCalcVar.top();
  cv->gr = map->gr();
  cv->rcSize = map->rcSize();
}

void CodeGenerator::AddInstMapRowCol(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  Map map(sVal);
  CoordSystem csIn;
  CalcVariable cv = stkCalcVar.top();  // coord
  DomainCoord* pdcrd = cv->dvs.dm()->pdcrd();
  if (0 == pdcrd) { // error
    CoordinateNeeded(sFunc, 1);
    return;
  }
  csIn = pdcrd->cs();
  bool fCol = fCIStrEqual(sFunc , "mapcol");
  inst->Add(new InstMapLineCol(inst, map, csIn, fCol));
  stkCalcVar.pop();  // coord
  RowCol rcSize = map->rcSize();
  DomainValueRangeStruct dvrs(1, fCol ? rcSize.Col : rcSize.Row);
  cv = CalcVariable(dvrs, vtVALUE, map->gr(), rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstPixSize(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  FileName fn(sVal, ".mpr", false);
  GeoRef gr;
  if (fCIStrEqual(fn.sExt, ".mpr")) {
    Map map(fn);
    gr = map->gr();
  }
  else
    gr = GeoRef(fn);
  double r;
  if (gr.fValid()) {
    r = gr->rPixSize();
    if (fCIStrEqual(sFunc , "pixarea"))
      r *= r;
    AddInst1("constr", String("%.2f", r));
    CalcVariable cv = stkCalcVar.top();
    cv->gr = gr;
    cv->rcSize = gr->rcSize();
  }
  else
    AddInst("undef");
}

void CodeGenerator::AddInstTableRecsCols(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  FileName fn(sVal, ".tbt", false);
  Table tbl(fn);
  long i;
  if (fCIStrEqual(sFunc , "tblrecs"))
    i = tbl->iRecs();
  else
    i = tbl->iCols();
  AddInst1("consti", String("%li", i));
}

void CodeGenerator::AddInstMinMaxCrd(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  BaseMap bmap(sVal);
  CoordBounds cb = bmap->cb();
  double r;
  if (fCIStrEqual(sFunc , "minCrdX"))
    r =  cb.MinX();
  else if (fCIStrEqual(sFunc , "maxCrdX"))
    r =  cb.MaxX();
  else if (fCIStrEqual(sFunc , "minCrdY"))
    r =  cb.MinY();
  else if (fCIStrEqual(sFunc , "maxCrdY"))
    r =  cb.MaxY();
  AddInst1("constr", String("%.2f", r));
  MapPtr* map = dynamic_cast<MapPtr*>(bmap.ptr());
  if (0 != map) {
    CalcVariable cv = stkCalcVar.top();
    cv->gr = map->gr();
    cv->rcSize = map->rcSize();
  }
}

void CodeGenerator::AddInstMapCrd(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  Map map(sVal);
  bool fRowCol = fCIStrEqual(sFunc , "mapcrdrc");
  if (!fRowCol) {
    AddInst("load", "%l");
    AddInst("load", "%c");
  }
  stkCalcVar.pop();  // col
  stkCalcVar.pop();  // row
  inst->Add(new InstMapCrd(inst, map));
  Domain dom(FileName(), map->cs());
  CalcVariable cv(dom, vtCOORD, map->gr(), map->rcSize());
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstMapColor(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  Map map(sVal);
  bool fRowCol = fCIStrEqual(sFunc , "mapcolorrc");
  if (!fRowCol) {
    AddInst("load", "%l");
    AddInst("load", "%c");
  }
  stkCalcVar.pop();  // col
  stkCalcVar.pop();  // row
  InstMapColor* instmpcol = new InstMapColor(inst, map);
  inst->Add(instmpcol);
  CalcVariable cv(instmpcol->dvsColor, vtCOLOR, map->gr(), map->rcSize());
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstRprColor(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtNONE).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  Representation rpr(sVal);
  CalcVariable pcv = stkCalcVar.pop();  // value
  InstRprColor* instrprcol = new InstRprColor(inst, rpr);
  inst->Add(instrprcol);
  CalcVariable cv(instrprcol->dvsColor, vtCOLOR, pcv->gr, pcv->rcSize);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstPntCrd(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtNONE).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  PointMap pmp(sVal);
  stkCalcVar.pop();  // index
  inst->Add(new InstPntCrd(inst, pmp));
  Domain dom(FileName(), pmp->cs());
  CalcVariable cv(dom, vtCOORD);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstPntXY(const String& sFunc,
                            const String& sVal)
{
  AddInstPntCrd("pntcrd", sVal);
  if (fCIStrEqual(sFunc , "pntx"))
    AddInst("call", "crdx");
  else
    AddInst("call", "crdy");
}

void CodeGenerator::AddInstPntVal(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtNONE).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  PointMap pmp(sVal);
  stkCalcVar.pop();  // index
  inst->Add(new InstPntVal(inst, pmp));
  CalcVariable cv(pmp->dvrs(), vtVALUE);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInstPntNr(const String& sFunc,
                            const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtNONE).fValid())
    FuncNotAllowedInUserDefFunc(sFunc);
  PointMap pmp(sVal);
  stkCalcVar.pop();  // index
  inst->Add(new InstPntNr(inst, pmp));
  CalcVariable cv(DomainValueRangeStruct(1, pmp->iFeatures()), vtVALUE);
  stkCalcVar.push(cv);
}

void CodeGenerator::AddInst(const char* sInst,
                            const String& sVal)
{
  String sInstruct = sInst;
  if (fCIStrEqual(sInstruct.sLeft(4)  , "load"))
    AddInstLoad(sVal, fCIStrEqual(sInstruct,"loadnb"));
  else if (fCIStrEqual(sInstruct , "nbnum"))
    AddInstNbNum(sVal);
  else if (fCIStrEqual(sInstruct , "nbflt"))
    AddInstNbFlt(sVal);
  else if (fCIStrEqual(sInstruct  , "store")) {
    if (stkCalcVar.fEmpty())
      return;
//    CalcVariable cv = *stkCalcVar.pop();
    CalcVariable cv = stkCalcVar.top();
    stkCalcVar.pop();
    inst->Add(new InstStore(inst, cvCreate(sVal, cv)));
//  inst->env.cvRes = cv;
  }
  else if (fCIStrEqual(sInstruct , "clfy"))
    AddInstClfy(sVal);
  else if (fCIStrEqual(sInstruct , "coord"))
    AddInstCoord(sVal);
  else if (fCIStrEqual(sInstruct , "mapvalue"))
    AddInstMapValue(sVal);
  else if (fCIStrEqual(sInstruct , "aggregatevalue"))
	  AddInstAggregateValue(sVal);
  else if (fCIStrEqual(sInstruct , "rasvalue"))
    AddInstRasValue(sVal);
  else if ((fCIStrEqual(sInstruct , "mapmin")) || (fCIStrEqual(sInstruct , "mapmax")))
    AddInstMinMaxVal(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "maprow")) || (fCIStrEqual(sInstruct , "mapcol")))
    AddInstMapRowCol(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "mapcolor")) | (fCIStrEqual(sInstruct , "mapcolorrc")))
    AddInstMapColor(sInstruct, sVal);
  else if (fCIStrEqual(sInstruct , "rprcolor"))
    AddInstRprColor(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "minCrdX")) || (fCIStrEqual(sInstruct , "minCrdY")) ||
           (fCIStrEqual(sInstruct , "maxCrdX")) || (fCIStrEqual(sInstruct , "maxCrdY")))
    AddInstMinMaxCrd(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "maprows")) || (fCIStrEqual(sInstruct , "mapcols")))
    AddInstMapRowsCols(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "pixsize")) || (fCIStrEqual(sInstruct , "pixarea")))
    AddInstPixSize(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "mapcrd")) || (fCIStrEqual(sInstruct , "mapcrdrc")))
    AddInstMapCrd(sInstruct, sVal);
  else if (fCIStrEqual(sInstruct , "pntcrd"))
    AddInstPntCrd(sInstruct, sVal);
  else if (fCIStrEqual(sInstruct , "pntval"))
    AddInstPntVal(sInstruct, sVal);
  else if (fCIStrEqual(sInstruct , "pntnr"))
    AddInstPntNr(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "pntx")) || (fCIStrEqual(sInstruct , "pnty")))
    AddInstPntXY(sInstruct, sVal);
  else if ((fCIStrEqual(sInstruct , "tblrecs")) || (fCIStrEqual(sInstruct , "tblcols")))
    AddInstTableRecsCols(sInstruct, sVal);
	else if (fCIStrEqual(sInstruct, "tblvalue"))
		AddInstTblValue(sVal);
  else if (fCIStrEqual(sInstruct , "funcdef")) {
    fdsCur = new UserDefFuncDesc(sVal);
    lstParms.push(Array<CalcVariable>());
  }
  else if (fCIStrEqual(sInstruct , "returntype")) 
    AddInstReturnType(sVal);
  else if (fCIStrEqual(sInstruct , "procdef")) {
    fdsCur = new UserDefFuncDesc(sVal);
  }
  else if (fCIStrEqual(sInstruct , "callcheck")) {
    if (!fCheckFuncName(sVal))
      Error(String(TR("Function '%s' not found").c_str(), sVal.c_str()), iCursorLine, iCursorCol);
  }
  else
    AddInst1(sInstruct, sVal);
}

bool CodeGenerator::fCheckFuncName(const String& sFunc)
{
  for (int i=0; i < lstInternFuncs.iSize(); ++i)
    if (fCIStrEqual(lstInternFuncs[i]->sFunc, sFunc))
      return true;
  FileName fnFunc(sFunc, ".fun", false);
  return File::fExist(fnFunc);
}

void CodeGenerator::AddInstAggregateValue(const String& sVal)
{
	if ((0 != fdsCur) && cvFindParm(sVal, vtMAP).fValid())
		FuncNotAllowedInUserDefFunc("aggregatevalue");

	FileName fn(sVal);
	String sAttribColumn;
	Map mpInput;
	Domain dmUsed;
	if ( fn.sExt == ".mpr" || fn.sExt == "")
	{
		mpInput = Map(sVal);
		dmUsed = mpInput->dm();
	}		
	else
	{
		mpInput = Map(fn.sRelative(false));
		sAttribColumn = fn.sExt.sTail(".");
		if ( mpInput->fTblAtt() )
		{
			Table tbl = mpInput->tblAtt();
			Column col = tbl->col(sAttribColumn);
			dmUsed = col->dm();
			if ( !col.fValid())
				Error(String(TR("Column %S is not an attrib column").c_str(), sAttribColumn), iCursorLine, iCursorCol);
		}
		else
			Error(String(TR("Column %S is not an attrib column").c_str(), sAttribColumn), iCursorLine, iCursorCol);
	}		
	
	if (!mpInput.fValid())
		Error(String(TR("Input map %S must be a valid raster map").c_str(), sVal), iCursorLine, iCursorCol);
	if ( stkCalcVar.iSize() == 0)
		Error(TR("Illegal instruction"), iCursorLine, iCursorCol);
	CalcVariable oper = stkCalcVar.pop();
	String sOper = oper->sValue();
	if ( !InstAggregateValue::fValidOperation(sOper)) // three parms or not?
	{
		if ( sOper.rVal() == rUNDEF) // operand was not a value
		{
			FileName fn;
			Map mpAddit;
			if (oper->map().fValid())
			{
				fn = oper->map()->fnObj;
				mpAddit = Map(fn);
			}
	
			if (stkCalcVar.fEmpty())
				Error(String(TR("Operation '%S' is not known ").c_str(), sOper), iCursorLine, iCursorCol);
			oper = stkCalcVar.pop();	// next parm must be the operation
			sOper = oper->sValue();

			if ( !InstAggregateValue::fValidOperation(sOper))
				Error(String(TR("Operation '%S' is not known ").c_str(), sOper), iCursorLine, iCursorCol);
			if ( !mpAddit.fValid())
				Error(String(TR("Additional map %S is not valid").c_str(), fn.sRelative()), iCursorLine, iCursorCol);			
			if ( mpInput->gr() != mpAddit->gr() )
				Error(String(TR("Input map and additional map have different georefs").c_str()), iCursorLine, iCursorCol);				
			if ( !InstAggregateValue::fMatchDomain(sOper, mpAddit->dm()))						
				Error(String(TR("Additional map %S's domain is not suitable").c_str(), fn.sRelative()), iCursorLine, iCursorCol);				
			if (!InstAggregateValue::fMatchDomain(sOper, dmUsed))
				Error(String(TR("Input map %S's domain is not suitable for operation %S").c_str(), sVal, sOper), iCursorLine, iCursorCol);	
			
			inst->Add(new InstAggregateValue(inst, mpInput, sAttribColumn, sOper, mpAddit));			
		}
		else // operand was a value, we have an operation like connectivityindex
		{
			double rPower = sOper.rVal();
			if ( rPower <= 1.0 )
				Error(TR("Power must be larger than 1"), iCursorLine, iCursorCol);
			oper = stkCalcVar.pop();	
			String sOper = oper->sValue(); // next oper must be 
			if (!InstAggregateValue::fMatchDomain(sOper, dmUsed))
				Error(String(TR("Input map %S's domain is not suitable for operation %S").c_str(), sVal, sOper), iCursorLine, iCursorCol);			
			inst->Add(new InstAggregateValue(inst, mpInput, sAttribColumn, sOper, rPower));				
		}
	}		
	else
	{
		if (!InstAggregateValue::fMatchDomain(sOper, dmUsed))
				Error(String(TR("Input map %S's domain is not suitable for operation %S").c_str(), sVal, sOper), iCursorLine, iCursorCol);		
		inst->Add(new InstAggregateValue(inst, mpInput, sAttribColumn, sOper, Map()));
	}
 	DomainValueRangeStruct dvs (-1.0e300, 1.0e300, 0.00001);
	CalcVariable cv(dvs, vtVALUE, mpInput->gr(), mpInput->rcSize());

	stkCalcVar.push(cv);
}

void CodeGenerator::AddInstTblValue(const String& sVal)
{
  if ((0 != fdsCur) && cvFindParm(sVal, vtTABLE).fValid())
    FuncNotAllowedInUserDefFunc("tblvalue");
  Table tbl(sVal);
  inst->Add(new InstTblValue(inst, tbl));
	Domain dm = tbl->dm();
	CalcVariable varCol = stkCalcVar.pop();
	CalcVariable varRow = stkCalcVar.pop();
	Column col;
	if ( varRow->vt == vtSTRING )
	{
		col = tbl->col(varRow->sValue())	;
	}
	else if ( varRow->vt == vtVALUE)
	{
		Column col = tbl->col(varRow->iValue())	;
	}
	
	if (col.fValid())
		dm = col->dm();	
 // stkCalcVar.pop();  // col
//stkCalcVar.pop();  // row
  CalcVariable cv(dm, vtVALUE);
  stkCalcVar.push(cv);
}
