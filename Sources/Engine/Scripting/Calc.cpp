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
/*$Log: /ILWIS 3.0/Calculator/Calc.cpp $
 * 
 * 12    25-07-01 17:30 Koolhoven
 * in instExprX() and instExprXY() do not take the address of an inline
 * constructed (and destructed) object TokenizerBase, but create it as a
 * named local var
 * 
 * 11    6/26/00 11:57a Wind
 * added instExprX
 * 
 * 10    14-02-00 13:00 Wind
 * cleaning
 * 
 * 9     11-01-00 12:59 Wind
 * some polishing
 * 
 * 8     5-01-00 11:09 Wind
 * rename class ExternFuncDesc to UserDefFuncDesc
 * 
 * 7     4-01-00 10:27 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 6     13-10-99 10:57a Martin
 * prevent pure virtual function call
 * 
 * 5     8/24/99 10:13a Wind
 * debugged sSimpleCalc(const String&)
 * 
 * 4     1-07-99 15:53 Koolhoven
 * debugged sSimpleCalc()
 * 
 * 3     3/12/99 3:04p Martin
 * Added support for case insensitive
 * 
 * 2     3/11/99 12:15p Martin
 * Added support for Case insesitive 
// Revision 1.5  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.4  1997/09/19 14:23:13  Wim
// In fdsParse() delete inst when an error is thrown
//
// Revision 1.3  1997-09-03 20:28:45+02  Wim
// Set iRecs of env in instColCalc()
//
// Revision 1.2  1997-08-14 20:05:11+02  Wim
// Added inst = 0 after deleting inst in column calculae
//
/* calc.c  Calculator class
Jelle Wind, ILWIS System Development
june 1997
	Last change:  WK   14 Apr 98    4:22 pm
*/
#include "Engine\Scripting\Calc.h"

class ParserObject { //used for convenience (hides some of details 
// in working together between Tokenizer, Parser and CodeGenerator
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

Calculator::Calculator(const Table& tbl, const String& sExpr)
{
  CodeGenerator* cg = new CodeGenerator(tbl, 0);
  // is deleted in ~ParserObject
  po = new ParserObject(sExpr, cg);
  iRecs = tbl->iRecs();
}

Calculator::Calculator(const String& sExpr, const FileName& fnMap) 
{
  CodeGenerator* cg = new CodeGenerator(ctMAP, 0);
  // is deleted in ~ParserObject
  cg->fnObj = fnMap;
  po = new ParserObject(sExpr, cg);
}

Calculator::Calculator(const String& sExpr)
{
  CodeGenerator* cg = new CodeGenerator(ctSIMPLE, 0);
  // is deleted in ~ParserObject
  po = new ParserObject(sExpr, cg);
}

Calculator::~Calculator()
{
  delete po;
}

Instructions* Calculator::instMapCalc(ObjectDependency& objdep, bool& fNoGeoRef) 
{
  fNoGeoRef = false;
  Instructions* inst = new Instructions();
  po->cg->SetInst(inst);
  Token tokLast;
  try {
    po->parser->GetNextToken();
    po->parser->Expression();
    tokLast = po->parser->tokCur();
    if (tokLast == ";") {
      po->parser->GetNextToken();
      tokLast = po->parser->tokCur();
    }
    if (tokLast.sVal().length() > 0) {
      delete inst;
      inst = 0;
      po->parser->ErrUnexp(tokLast);
      return 0;
    }
  }
  catch (const ErrorObject& err) {
    err.Show();
    delete inst;
    inst = 0;
    return 0;
  }
  po->cg->DetermineDomain();
  po->cg->DetermineGeoRef();
  po->cg->Get(objdep);
  po->cg->AddInst("stop");

  fNoGeoRef = inst->gr()->fGeoRefNone();
  if (fNoGeoRef)
    fNoGeoRef = (inst->env.iLines == 0) && (inst->env.iCols == 0);
  return inst;
}

Instructions* Calculator::instColCalc(ObjectDependency& objdep)
{
  Instructions* inst = new Instructions();
  inst->env.iRecs = iRecs;
  po->cg->SetInst(inst);
  Token tokLast;
  try {
    po->parser->GetNextToken();
    po->parser->Expression();
    po->cg->AddInst("stop");
    tokLast = po->parser->tokCur();
    if (tokLast == ";") {
      po->parser->GetNextToken();
      tokLast = po->parser->tokCur();
    }
    if (tokLast.sVal().length() > 0) {
      delete inst;
      inst = 0;
      po->parser->ErrUnexp(tokLast);
      return 0;
    }
  }
  catch (const ErrorObject& err) {
    err.Show();
    delete inst;
    inst = 0;
    return 0;
  }
  po->cg->DetermineDomain();
  po->cg->Get(objdep);
  return inst;
}

void Calculator::ExecColumnIndexStore() 
{
  Instructions inst;
  po->cg->SetInst(&inst);
  try {
    Token tokLast;
    po->parser->GetNextToken();
    po->parser->Expression();
    po->cg->AddInst("stop");
    tokLast = po->parser->tokCur();
    if (tokLast.sVal().length() > 0)
      po->parser->ErrUnexp(tokLast);
    LongBuf buf(1024);
	long i=0;
    for (; i < iRecs/1024; ++i)
      inst.ColumnCalcRaw(buf, i*1024, 1024);
    inst.ColumnCalcRaw(buf, i*1024, iRecs-i*1024);
  }
  catch (const ErrorObject& err) {
    err.Show();
  }
}

UserDefFuncDesc* Calculator::fdsParse(Instructions* inst)
{
  CodeGenerator* cg = po->cg;
  bool fInstCreated = false;
  if (0 == inst) {
    inst = new Instructions();
    fInstCreated = true;
  }
  cg->SetInst(inst);
  po->parser->GetNextToken();
  try {
    po->parser->FuncDef();
  }
  catch (const ErrorObject&) {
    if (fInstCreated && (0 != inst)) {
      delete inst;
      inst = 0;
    }
    DummyError();
  }
  return po->cg->fds;
}

Instructions* Calculator::instExprX(const String& sExpr)
{
  Instructions* inst = new Instructions();
  CodeGenerator cg(ctSIMPLE, inst);
  CalcVariable cv;
  cv.SetPointer(new CalcVarReal("x"));
  cg.stkCalcVar.push(cv);
  cg.AddInst("var", "x", "real", "");
  cg.AddInst("store", "x");
  cg.sExpr = sExpr;
	TextInput inp(sExpr);
  TokenizerBase tokbase(&inp);
  ProgramParser parser(&tokbase, &cg);
  parser.GetNextToken();
  parser.Program();
  cg.DetermineDomain();
  return inst;
}

Instructions* Calculator::instExprXY(const String& sExpr)
{
  Instructions* inst = new Instructions();
  CodeGenerator cg(ctSIMPLE, inst);
  CalcVariable cv;
  cv.SetPointer(new CalcVarReal("x"));
  cg.stkCalcVar.push(cv);
  cg.AddInst("var", "x", "real", "");
  cg.AddInst("store", "x");
  cv.SetPointer(new CalcVarReal("y"));
  cg.stkCalcVar.push(cv);
  cg.AddInst("var", "y", "real", "");
  cg.AddInst("store", "y");
  cg.sExpr = sExpr;
	TextInput inp(sExpr);
  TokenizerBase tokbase(&inp);
  ProgramParser parser(&tokbase, &cg);
  parser.GetNextToken();
  parser.Program();
  cg.DetermineDomain();
  return inst;
}

#include "Engine\Function\FUNCUSER.H"
Instructions* Calculator::instCoordFunc(const String& sFuncName)
{
  Instructions* instFunc = new Instructions();
  CodeGenerator cg(ctSIMPLE, instFunc);
  CalcVariable cv;
  cv.SetPointer(new CalcVarReal("x"));
  cg.stkCalcVar.push(cv);
  cv.SetPointer(new CalcVarReal("y"));
  cg.stkCalcVar.push(cv);
  cg.AddInst("call", sFuncName, "2");
  cg.AddInst("stop");
  return instFunc;
}

String Calculator::sSimpleCalc(const String& sExpr)
{
  String s;
  if (sExpr[0] == '@') {
    FileName fn(String("%s", (sExpr.c_str())+1));
    if (File::fExist(fn)) {
      File fil(fn);
      fil.Read(s);
    }
    else
      NotFoundError(fn);
  }
  else
    s = sExpr;
  Instructions inst;
  CodeGenerator cg(ctSIMPLE, &inst);
  cg.sExpr = s;
  TextInput inp(s);
  TokenizerBase tokbase(&inp);
  ProgramParser parser(&tokbase, &cg);
  parser.GetNextToken();
  try {
    parser.Program();
    Token tokLast = parser.tokCur();
    if (tokLast.sVal().length() > 0)
      parser.ErrUnexp(tokLast);
  }
  catch (const ErrorObject& err) {
    err.Show();
    return sUNDEF;
  }  
  cg.DetermineDomain();
  String sRet = cg.inst->sCalcVal();
  return sRet;

}

String Calculator::sSimpleCalc(const String& sExpr, const Table& tbl)
{
  Instructions inst;
  CodeGenerator cg(tbl, &inst);
  String s;
  if (sExpr[0] == '@') {
    FileName fn(String("%s", (sExpr.c_str())+1));
    if (File::fExist(fn)) {
      File fil(fn);
      fil.Read(s);
    }
    else
      NotFoundError(fn);
  }
  else
    s = sExpr;
	TextInput ti(s);
	TokenizerBase tokbase(&ti);
  ProgramParser parser(&tokbase, &cg);
  parser.GetNextToken();
  try {
    parser.Program();
    if (!parser.fEnd())
      ExpressionError(sExpr, String());
  }
  catch (const ErrorObject& err) {
    err.Show();
    return sUNDEF;
  }  
  cg.DetermineDomain();
  String sRet = cg.inst->sCalcVal();
  return sRet;
}




