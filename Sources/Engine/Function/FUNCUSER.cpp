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
/* FuncUserPtr
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\Function\FUNCUSER.H"
#include "Engine\Scripting\Calc.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

FuncUser::FuncUser()
: Function()
{
}

FuncUser::FuncUser(const FuncUser& fnc)
: Function(fnc.pointer())
{
}

FuncUser::FuncUser(const FileName& fn)
: Function()
{  
  FileName fnFil(fn, ".fun");
  MutexFileName mut(fnFil);
  FunctionPtr* p = static_cast<FunctionPtr*>(Function::pGet(fnFil));
  if (p) // if already open return it
    SetPointer(p);
  else
    SetPointer(new FuncUserPtr(fnFil));
}

FuncUser::FuncUser(const String& sFuncName)
: Function(FileName(sFuncName, ".fun", true))
{
  if (!pointer())
    SetPointer(new FuncUserPtr(FileName(sFuncName, ".fun", true)));
}

FuncUser::FuncUser(IlwisObjectPtr* ptr)
: Function(ptr)
{
}

FuncUser::FuncUser(const FileName& fn, const String& sFuncDef, bool fOnlyRetExpr)
: Function(new FuncUserPtr(fn, sFuncDef, fOnlyRetExpr))
{
}

FuncUserPtr::~FuncUserPtr()
{
	if ( fErase )
	{
		String s;
		ReadElement("FuncUser", "FuncDefFile", s);
		if ( s != "" && File::fExist(s))
			DeleteFile(s.c_str());
	}
}

String FuncUserPtr::sType() const
{
  return "Function";
}

FuncUserPtr::FuncUserPtr(const FileName& fn)
: FunctionPtr(fn)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
     if (!File::fExist(filnam))
      NotFoundError(fn);
    FileName fn = fnObj;
    fn.Dir(getEngine()->getContext()->sStdDir());
		SetFileName(fn);
  }
  ReadElement("FuncUser", "FuncDefFile", fnFuncDef);
  File filDef(fnFuncDef, facRO);
  filDef.Read(sDef);
  _iParms = iReadElement("FuncUser", "Parms");
//  _fds = 0;
}

FuncUserPtr::FuncUserPtr(const FileName& fn, const String& sFuncDef, bool fOnlyRetExpr)
: FunctionPtr(fn, Domain("value"))
{
  if (fOnlyRetExpr)
    sDef = sGenerateDef(sFuncDef);
  else  
    sDef = sFuncDef;
//  _fds = 0;
  _iParms = shUNDEF;
  fnFuncDef = FileName(fn, ".fnd");
}

void FuncUserPtr::Store()
{
	FunctionPtr::Store();
	WriteElement("Function", "Type", "FuncUser");
	WriteElement("FuncUser", "FuncDefFile", fnFuncDef);
	File filDef(fnFuncDef, facCRT);
	filDef.Write(sDef);
    if (_iParms != shUNDEF)
		WriteElement("FuncUser", "Parms", _iParms);
    else
		WriteElement("FuncUser", "Parms", (const char*)0);
}

void FuncUserPtr::SetFuncDef(const String& sFuncDef)
{
  String sDefSav = sDef;
  sDef = sFuncDef;
  if (sDef[sDef.length()] != '\n')
    sDef &= "\r\n";
  // check
  Calculator calc(sFuncDef, fnObj);
  try {
    UserDefFuncDesc* fds = calc.fdsParse(0);
    if (0 != fds) {
      _iParms = fds->iParms();
      SetDefaultDomain(fds->dmDefault);
      if (0 == fds->dmDefault->pdv())
        SetDefaultValueRange(ValueRange());
      delete fds;
      fChanged = true;
    }
  }
  catch (const ErrorObject& err) {
    sDef = sDefSav;
    throw err;
  }    
}

bool FuncUserPtr::fFuncDefOk(const String& sFuncDef)
{
  Calculator calc(sFuncDef, fnObj);
  try {
    UserDefFuncDesc* fds = calc.fdsParse(0);
    if (0 != fds) {
      delete fds;
      return true;
    }
  }
  catch (const ErrorObject&) {
  }
  return false;
}

UserDefFuncDesc* FuncUserPtr::fdsParse(Instructions* inst)
{
  String sFuncDef;
  if (fCIStrEqual(fnFuncDef.sExt, ".fnc")) {
    unsigned int iParms = 0, i;
    // find nr of parms
    for (i=0; i < sDef.length(); i++)
      if (sDef[i] == '@')
        iParms = max(iParms, sDef[i+1]-'0');
    assert(iParms > 0);
    sFuncDef &= String("function %S(", fnObj.sFile);
    for (i = 1; i <= iParms; ++i) {
      sFuncDef &= String("parm%i", i);
      if (i < iParms)
        sFuncDef &= ',';
      else
        sFuncDef &= ")\r\nbegin\r\n  return ";
    }
    char cPrev=0, cPrevPrev=0;
    for (i=0; i < sDef.length(); i++) {
      if (sDef[i] == '@')
        sFuncDef &= "parm";
      else {
        if (sDef[i] == '(') // check on if-function and replace by iff
          if ((cPrevPrev == 'i') && (cPrev == 'f'))
            sFuncDef &= 'f';
        sFuncDef &=  sDef[i];
      }
      cPrevPrev = cPrev;
      cPrev = sDef[i];
    }
    sFuncDef &= ";\r\nend;\r\n";
  }
  else
    sFuncDef = sDef;
  Calculator calc(sFuncDef, fnObj);
  UserDefFuncDesc* fds = calc.fdsParse(inst);
  _iParms = fds->iParms();
  SetDefaultDomain(fds->dmDefault);
  if (0 == fds->dmDefault->pdv())
    SetDefaultValueRange(ValueRange());
  return fds;
}

void FuncUser::import(const FileName& fnFnc14)
{
  FuncUserPtr* p = new FuncUserPtr(FileName(fnFnc14, ".fun"), "");
  p->fnFuncDef = fnFnc14;
  { File filDef(fnFnc14, facRO);
    filDef.Read(p->sDef);
  }  
  p->Store();
  delete p;
}

String FuncUserPtr::sGenerateDef(const String& sRetExpr)
{
  Array<String> asVars;
  // may be too simple implementation
  Instructions inst;
  CodeGenerator cg(ctSIMPLE, &inst);
  cg.sExpr = sRetExpr;
	TextInput ip(sRetExpr);
  TokenizerBase tb(&ip);
  ExpressionParser parser(&tb, &cg);
  Token tok, tokNext;
  String sVar;
  bool fVarFound = false;
  tok = tb.tokGet();
  while (!tb.fEnd()) {
    tokNext = tb.tokGet();
    if ((tok.tt() == ttIDENTIFIER) && (tokNext.sVal() != "(")) {
      sVar = tok.sVal();
      if (0 == sVar.strchrQuoted('.')) {
		  int i=0;
        for (; i < asVars.iSize(); ++i )
          if (asVars[i] == sVar)
            break;
        if (i == asVars.iSize())
          asVars &= sVar;
      }
    }
    tok = tokNext;
  }
  if (tok.tt() == ttIDENTIFIER) {
    sVar = tok.sVal();
	int i=0;
    for (; i < asVars.iSize(); ++i )
      if (asVars[i] == sVar)
        break;
    if (i == asVars.iSize())
      asVars &= sVar;
  }
  String sFuncDef = String("Function %S(", fnObj.sFile);
  for (int i = 0; i < asVars.iSize(); ++i) {
    sFuncDef &= "Value ";
    sFuncDef &= asVars[i];
    if (i < asVars.iSize()-1)
      sFuncDef &= ',';
  }
  sFuncDef &= ") : Value\r\nBegin\r\n  Return ";
  sFuncDef &= sRetExpr;
  if (sRetExpr.sRight(1) != ";")
    sFuncDef &= ";";
  sFuncDef &= "\r\nEnd;\r\n";
  return sFuncDef;
}

void FuncUserPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  ObjectInfo::Add(afnDat, fnFuncDef, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "FuncUser";
    (*asEntry) &= "FuncDefFile";
  }
}

void FuncUserPtr::Rename(const FileName& fnNew)
{
  IlwisObjectPtr::Rename(fnNew);
  // rename script data file
  FileName fn = fnFuncDef;
  fn.sFile = fnNew.sFile;
  rename(fnFuncDef.sFullName().c_str(), fn.sFullName().c_str());
  fnFuncDef.sFile = fnNew.sFile;
}

void FuncUserPtr::GetObjectStructure(ObjectStructure& os)
{
	FunctionPtr::GetObjectStructure(os);
	os.AddFile(fnObj, "FuncUser", "FuncDefFile");
}

void FuncUserPtr::DoNotUpdate()
{
	FunctionPtr::DoNotUpdate();
	
}




