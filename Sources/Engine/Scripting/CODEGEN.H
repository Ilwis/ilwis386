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
#ifndef ILW_CODEGENH
#define ILW_CODEGENH
#include "Engine\Base\DataObjects\Stack.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Scripting\Instrucs.h"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Scripting\CALCERR.H"
#include "Engine\Base\objdepen.h"
#include <stack>


//!
enum CalcType { ctSIMPLE=0, ctMAP, ctCOL };

//!
typedef Array<int>  CursorArray;

//!
class CodeGeneratorBase
{
  friend class Calculator;
  friend class Parser;
public:
  CodeGeneratorBase();
  virtual _export ~CodeGeneratorBase();
  virtual void AddInst(const char* sInstruct, const String& sVal)=0;
  virtual void AddInst(const char* sInstruct,
               const String& sVal1, const String& sVal2)=0;
  virtual void AddInst(const char* sInstruct, const String& sVal1,
               const String& sVal2, const String& sVal3)=0;
  virtual void AddInst(const char* sInstruct, long iVal = 0)=0;
  virtual void Error(const String& s, int iLine, int iCol=shUNDEF) const=0;
  virtual void Error(const ErrorObject& err, int iLine, int iCol=shUNDEF) const=0;
  virtual int iNewLabel()=0;
  String sExpr;
protected:
  int iCursorLine, iCursorCol;
};

//!
class CodeGenerator : public CodeGeneratorBase
{
  friend class Calculator;
  friend class Instructions;
public:
  _export CodeGenerator(CalcType ct, Instructions*);
  CodeGenerator(const Table& table, Instructions*);
  _export ~CodeGenerator();
  // functions used by parser
  virtual void AddInst(const char* sInstruct, const String& sVal);
  virtual void AddInst(const char* sInstruct,
               const String& sVal1, const String& sVal2);
  virtual void AddInst(const char* sInstruct, const String& sVal1,
               const String& sVal2, const String& sVal3);
  virtual void AddInst(const char* sInstruct, long iVal = 0);
  virtual void Error(const String& s, int iLine, int iCol=shUNDEF) const;
  virtual void Error(const ErrorObject& err, int iLine, int iCol=shUNDEF) const;
  int iNewLabel();
  // functions used by others
  static double rCalc(const String& sExpr);
  static String sCalc(const String& sExpr);
  static String sCalc(const String& sExpr, const Table&);
  void DetermineDomain();
  void DetermineGeoRef();
  void Get(ObjectDependency&);
protected:
private:
  void SetInst(Instructions* inst);
  void Init(Instructions* inst=0);
  bool fCheckFuncName(const String& sFunc);
  CalcVariable cvCreate(const String& sName, const CalcVariable& cv);
  CalcVariable cvFind(const String& sName, VarType vt);
  CalcVariable cvFindParm(const String& sName, VarType vt);
  VarType vt(const String& sVal);
  void FuncNotAllowedInUserDefFunc(const String& sFunc) const;
  void CoordinateNeeded(const String& sFunc, int iParm) const;
  void InvalidParm(int iParm, const String& sFunc); // parm starts at 0
  // actual instruction generating functions
  void AddOperator(const String& sVal);
  void AddInst1(const String& sInstruct, const String& sVal);
  void AddInstLoad(const String& sVal, bool fNeighb);
  void AddInstNbNum(const String& sVal);
  void AddInstNbSum();
  void AddInstNbAvg();
  void AddInstNbStd();
  void AddInstNbCnt();
  void AddInstNbCondPos();
  void AddInstNbMinMax(const String& sVal, bool fPos, bool fCond);
  void AddInstNbPrd(bool fPos, bool fCond);
  void AddInstNbFlt(const String& sFlt);
  void AddInstClfy(const String& sVal);
  void AddInstCoord(const String& sVal);
  void AddInstMapValue(const String& sVal);
  void AddInstRasValue(const String& sVal);
  void AddInstMinMaxVal(const String& sFunc, const String& sMap);
  void AddInstMinMaxCrd(const String& sFunc, const String& sMap);
  void AddInstMapRowsCols(const String& sFunc, const String& sMap);
  void AddInstTableRecsCols(const String& sFunc, const String& sMap);
  void AddInstPixSize(const String& sFunc, const String& sMap);
  void AddInstMapCrd(const String& sFunc, const String& sMap);
  void AddInstPntCrd(const String& sFunc, const String& sMap);
  void AddInstPntVal(const String& sFunc, const String& sMap);
  void AddInstPntNr(const String& sFunc, const String& sMap);
  void AddInstPntXY(const String& sFunc, const String& sMap);
  void AddInstMapRowCol(const String& sFunc, const String& sMap);
  void AddInstMapColor(const String& sFunc, const String& sMap);
  void AddInstRprColor(const String& sFunc, const String& sRpr);
  void AddInstReturnType(const String& sVal);
  void AddInstIndexLoad(const String& sVal1, const String& sVal2);
  void AddInstIndexStore(const String& sVal1, const String& sVal2);
  void AddInstTransform(const String& sVal1, const String& sVal2);
  void AddInstCall(const String& sVal1, const String& sVal2);
	void AddInstTblValue(const String& sVal);
  void AddInstAggregateValue(const String& sVal);

	
  Table tbl;
  Stack<CalcVariable> stkCalcVar;
  stack<Array<CalcVariable> > lstCalcVars;
  stack<Array<CalcVariable> > lstParms;
  Array<InternFuncDesc*> lstInternFuncs;
  Array<char*>     sVarTypes;
  Instructions*    inst;
  UserDefFuncDesc*  fds, *fdsCur;
  FileName fnObj;
  CalcType ct;
  Stack<Array<int> > stkParmLinePos, stkParmColPos;
};

#endif
