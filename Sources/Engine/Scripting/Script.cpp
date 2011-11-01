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

/* ScriptPtr
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  J    21 Oct 99   10:03 am
*/

#define SCRIPT_C
#include "Headers\constant.h"
#include "Headers\base.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\File\COPIER.H"
#include "Engine\Base\File\objinfo.h"
#include "Engine\Base\Tokbase.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\DataExchange\MAPIMP.H"
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\DataExchange\POLIMP.H"
#include "Engine\DataExchange\PNTIMP.H"
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Scripting\SCRIPTOK.H"
#include "Engine\DataExchange\CONV.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\commandhandler.h"
#include "Headers\messages.h"
#include "Headers\Hs\DAT.hs"

static void UpdateCatalog()
{
	AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG); 
}

void AllObjectExtensions(Array<String>& asExt)
{
  asExt.Resize(0);
  asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
  asExt &= ".grf"; asExt &= ".dom"; asExt &= ".csy"; asExt &= ".rpr";
  asExt &= ".his"; asExt &= ".hss"; asExt &= ".hsa"; asExt &= ".hsp";
  asExt &= ".tbt"; asExt &= ".mpv"; asExt &= ".mpl"; asExt &= ".isl";
  asExt &= ".sms"; asExt &= ".fun"; asExt &= ".fil"; asExt &= ".mat";
}
  
IlwisObjectPtrList Script::lstScript;

Script::Script()
: IlwisObject(lstScript)
{
}

Script::Script(const Script& scrpt)
: IlwisObject(lstScript, scrpt.pointer())
{
}

Script::Script(const FileName& filnam)
: IlwisObject(lstScript)
{
  FileName fn(filnam, ".isl");
  ScriptPtr* p = static_cast<ScriptPtr*>(lstScript.pGet(fn));
  if (p) { // if already open return it
    SetPointer(p);
    return;
  }
  MutexFileName mut(fn);
  p = static_cast<ScriptPtr*>(lstScript.pGet(fn));
  if (p) // if already open return it
    SetPointer(p);
  else
    SetPointer(new ScriptPtr(fn));
}

Script::Script(const String& sScript)
: IlwisObject(lstScript, new ScriptPtr(sScript))
{
}

Script::Script(IlwisObjectPtr* ptr)
: IlwisObject(lstScript, ptr)
{
}

Script::Script(const FileName& fn, const String& sScript)
: IlwisObject(lstScript, new ScriptPtr(fn, sScript))
{
}

ScriptPtr* Script::pGet(const FileName& fn)
{
  return static_cast<ScriptPtr*>(lstScript.pGet(fn));
}


ScriptPtr::~ScriptPtr()
{
  for (unsigned int i=0; i < atbl.iSize(); ++i)
    atbl[i] = Table();
  if (fErase)
    _unlink(fnScript.sFullName().c_str());
}

String ScriptPtr::sType() const
{
  return "Script";
}

ScriptPtr::ScriptPtr(const FileName& fn)
: IlwisObjectPtr(fn),
  iParms(0)
{
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    //filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(filnam);
    //fnObj = filnam;
    FileName fn = fnObj;
    fn.Dir(getEngine()->getContext()->sStdDir());
		SetFileName(fn);
  }
  ReadElement("Script", "ScriptFile", fnScript);
  ReadElement("Script", "NrParams", iParms);
	if (iParms < 0)
		iParms = 0;
  File filScript(fnScript, facRO);
  filScript.Read(_sScript);
  if (0 != fnObj.sFile.length())
	  getEngine()->getLogger()->SetPrefix(String("script %S", sNameQuoted(true)));
}

ScriptPtr::ScriptPtr(const String& sScrpt)
: IlwisObjectPtr(),
  iParms(0)
{
  if (sScrpt[sScrpt.length()-1]!=';')
    _sScript = sScrpt;
  else
    _sScript = sScrpt.sLeft(sScrpt.length()-1);
}

ScriptPtr::ScriptPtr(const FileName& fn, const String& sScrpt)
: IlwisObjectPtr(FileName(fn, ".isl"), true, ".isl"),
  iParms(0)
{
  fnScript = fnObj;
  fnScript.sExt = ".isf";
  File fil(fnScript, facCRT);
  fil.Write(sScrpt);
  _sScript = sScrpt;
  if (0 != fnObj.sFile.length())
	  getEngine()->getLogger()->SetPrefix(String("script %S", sNameQuoted(true)));
}

void ScriptPtr::Store()
{
  IlwisObjectPtr::Store();
  WriteElement("Ilwis", "Type", "Script");
  WriteElement("Script", "ScriptFile", fnScript);
	WriteElement("Script", "NrParams", iParms);
  File fil(fnScript, facCRT);
  fil.Write(_sScript);
}

void ScriptPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  ObjectInfo::Add(afnDat, fnScript, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "Script";
    (*asEntry) &= "ScriptFile";
  }
}

void ScriptPtr::SetScript(const String& sScript)
{
  _sScript = sScript;
  fChanged = true;
}

static void Freeze(const FileName& fnObj)
{
  IlwisObject obj = IlwisObject::obj(fnObj);
  if (obj.fValid())
    obj->Calc();
}

static void UnFreeze(const FileName& fnObj)
{
  IlwisObject obj = IlwisObject::obj(fnObj);
  if (obj.fValid())
    obj->DeleteCalc();
}

bool ScriptPtr::fScanDomainValueRange(Token& tok, TokenizerBase& tkn, Domain& dm, ValueRange& vr)
{
// syntax:  {dom=domname;vr=valrange}
  tok = tkn.tokGet();
  while ((tok != "}") && (tok.sVal().length() != 0)) {
    if (tok == ";") {
      tok = tkn.tokGet();
      continue;
    }
    if (fCIStrEqual(tok.sVal(), "dom")) {
      tok = tkn.tokGet();
      if (tok != "=")
        return false;
      tok = tkn.tokGet();
      try {
       dm = Domain(tok.sVal());
      } 
      catch (const ErrorObject& err) {
        err.Show();
        return false;
      }
      tok = tkn.tokGet();
    }
    else if (fCIStrEqual(tok.sVal(), "vr")) {
      tok = tkn.tokGet();
      if (tok != "=")
        return false;
      tok = tkn.tokGet();
      String sRange;
      if (tok != ":") {
        sRange = tok.sVal();
        if (tok == "-") {
          tok = tkn.tokGet();
          sRange &= tok.sVal();
        }
        tok = tkn.tokGet();
      }
      if (tok == ":") {
        sRange &= ':';
        tok = tkn.tokGet();
        if (tok != ":") {
          sRange &= tok.sVal();
          if (tok == "-") {
            tok = tkn.tokGet();
            sRange &= tok.sVal();
          }
          tok = tkn.tokGet();
        }
        if (tok == ":") {
          sRange &= ':';
          tok = tkn.tokGet();
          sRange &= tok.sVal();
          tok = tkn.tokGet();
        }
        try {
          vr = ValueRange(sRange);
        }  
        catch (const ErrorObject& err) {
          err.Show();
          return false;
        }
      }
    }
    else 
      tok = tkn.tokGet();
  }
  if (tok != "}") {
    ErrorObject(TR("\'}\' expected"), 0).Show();  // '}'
    return false;
  }
  tok = tkn.tokGet(); 
  return true;
}
  
void ScriptPtr::Exec(const String& sParmList)
{
  Array<String> asParms;
  ParmList pl(sParmList);
  for (int i=0; i < pl.iFixed(); i++) {
    String s = pl.sGet(i);
/*    if ((s[0] == '"') && (s[s.length()-1] == '"')) {
      s[0] = '\'';
      s[s.length()-1] = '\'';
    }*/
    asParms &= s;
  }
	if (pl.fExist("dir")) {
		String sCurDir = pl.sGet("dir");
		getEngine()->SetCurDir(sCurDir.sUnQuote());
	}
  Exec(&asParms);
}
void ScriptPtr::UnknownCommandError(const String& sComm, const FileName& fnObj) {
  throw ErrorObject(WhatError(String(TR("Unknown command: '%S'").c_str(), sComm), errScript));
}  

void ScriptPtr::DomainValueRangeExpected(const FileName& fnObj)
{
  throw ErrorObject(WhatError(TR("Domain/valuerange expected"), errScript+4));
} 

void ScriptPtr::InvalidLineError(const String& sLine, const FileName& fnObj) {
  throw ErrorObject(WhatError(String(TR("Invalid line: ' %S '").c_str(), sLine), errScript+2), fnObj);
} 

void ScriptPtr::SortDomainError(const String& sDomain, const FileName& fnObj)
{
  throw ErrorSortDomain(sDomain, fnObj, errScript+1);
}

void ScriptPtr::NotDomainGroupError(const String& sDomain, const FileName& fnObj)
{
  throw ErrorObject(
    WhatError(String(TR("Only group domain allowed: '%S'").c_str(), sDomain),
		errScript+1), fnObj);
}

void ScriptPtr::InvalidDirError(const String& sDir, const FileName& fnObj)
{
  throw ErrorObject(WhatError(String(TR("Invalid directory: \'%S\'").c_str(), sDir), errScript+1), fnObj);
}

void ScriptPtr::Pause(ParmList& pm)
{
  long iVal = pm.sGet(0).iVal() * 1000;
  if (iVal < 0)
    return;
  long iTime = GetTickCount();
	AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG); // scripts do not send update to the catalog, unless a pause
  iTime += iVal;
  while ((DWORD)iTime > GetTickCount())
    if (trq.fAborted())
      return;
}

void ScriptPtr::ExecAction(Token& tok, TokenizerBase& tkn)
{
	String sCurrLine;
	if (tok.sVal() == "*") { 
		tok = tkn.tokGet(); // skip it
		sCurrLine = tkn.sCurrentLine();
	}
	else
		sCurrLine = tkn.sCurrentLine();
	
	ParmList pm(sCurrLine);
	if (fCIStrEqual(tok.sVal(),"rem") || (tok == "//"))
		return;
	if (tok.sVal() == "!")
		getEngine()->Execute(String("!%S",sCurrLine));
	else if (fCIStrEqual(tok.sVal() , "cd")) 
	{
		String sDir = sCurrLine.sTrimSpaces();
		SetCurrentDirectory(getEngine()->sGetCurDir().c_str());
		Directory dir(sDir);
		if (sDir == "\\")
			SetCurrentDirectory("\\");
		else if (sDir == "..")
			SetCurrentDirectory("..");
		else {
			word iFileAtt;
			iFileAtt=GetFileAttributes(dir.sFullPath().c_str());
			bool fExist = 0xffff != iFileAtt;
			if (!fExist || !(iFileAtt & FILE_ATTRIBUTE_DIRECTORY))
				InvalidDirError(sDir, fnObj);
			SetCurrentDirectory(dir.sFullPath().c_str());
		}
		char sBuf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, sBuf);
		getEngine()->SetCurDir( String(sBuf));
	}
	else if (fCIStrEqual(tok.sVal() , "md"))	
	{
		String sDir = pm.sGet(0);	
		if (sDir.find("-quiet") != -1)
			sDir.erase( sDir.find("-quiet"), 6);
		
		Directory dir(sDir);				
		if (_mkdir(dir.sFullPath().c_str()) == -1)
			throw DosError(errno);		
	}		
	else if (fCIStrEqual(tok.sVal() , "run")) 
	{
		FileName fnScript(pm.sGet(0), ".isl");
		Script scp(fnScript);
		Array<String> asParms;
		for (int i=1; i<pm.iFixed(); ++i)
			asParms &= pm.sGet(i);
		scp->Exec(&asParms);
	}
	else if (fCIStrEqual(tok.sVal() , "breakdep"))
		CommandHandler::BreakDepObjects(sCurrLine, &trq);
	else if (fCIStrEqual(tok.sVal() , "update"))
		CommandHandler::UpdateObjects(sCurrLine, &trq);
	else if (fCIStrEqual(tok.sVal() , "calc"))
		CommandHandler::CalcObjects(sCurrLine, &trq);
	else if (fCIStrEqual(tok.sVal() , "del"))
		CommandHandler::DeleteObjects(AfxGetMainWnd(), sCurrLine, &trq);
	else if (fCIStrEqual(tok.sVal() , "import"))
		Import(sCurrLine);
	else if (fCIStrEqual(tok.sVal() , "export"))
		Export(sCurrLine);
	else if (fCIStrEqual(tok.sVal() , "message"))
	{
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG);
		MessageBox(0, sCurrLine.sTrimSpaces().c_str(), sName().c_str(), MB_OK|MB_TOPMOST);
	}		
	else if (fCIStrEqual(tok.sVal() , "pause"))
		Pause(pm);
	else if (fCIStrEqual(tok.sVal() , "opentbl")) {
		atbl &= Table(pm.sGet(0));
	}
	else if (fCIStrEqual(tok.sVal() , "closetbl")) {
		Table tbl(pm.sGet(0));
		for (unsigned int i=0; i < atbl.iSize(); ++i)
			if (atbl[i] == tbl) {
				atbl[i] = Table();
				atbl.Remove(i,1);
				break;
			}
	}
	else if (fCIStrEqual(tok.sVal() , "copy")) {
		String sCmd("%S -quiet", sCurrLine);
		CommandHandler::CopyObjects(sCmd, &trq);
	}
	else if (fCIStrEqual(tok.sVal(), "copyfile")) {
		CommandHandler::CopyFiles(sCurrLine, &trq);
	}
	else if (fCIStrEqual(tok.sVal() , "open")) 
	{
		ParmList pm(sCurrLine);
		FileName fn(IlwisObjectPtr::fnCheckPath(pm.sGet(0)));
		IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(fn);
		String sC = pm.sCmd();	
		String sCmd("*%S %S", tok.sVal(), sC); // * means: don't show put in history of command line and don't ask questions		
		const char *s = sCmd.c_str();	
		if (iotType == IlwisObject::iotOBJECTCOLLECTION || iotType == IlwisObject::iotMAPLIST)
			getEngine()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)s);
		else
			getEngine()->Execute(sCmd);
	}
	else 
	{
		String sC = sCurrLine;	;	
		String sCmd("*%S %S", tok.sVal(), sC); // * means: don't show put in history of command line and don't ask questions		
		getEngine()->Execute(sCmd);
	}
}


int ScriptPtr::iParams()
{
	return iParms;
}

String ScriptPtr::sParam(int i)
{
	String s("Param%d", i);
	String sPar;
	ReadElement("Params", s.c_str(), sPar);
	return sPar;
}

String ScriptPtr::sDefaultValue(int i)
{
	String sVal;
	ReadElement("Params", String("DefaultValue%d", i).c_str(), sVal);
	return sVal;
}


ScriptPtr::ParamType ScriptPtr::ptParam(int i)
{
	String s("Type%d", i);
	String sPar;
	ReadElement("Params", s.c_str(), sPar);
	sPar = sPar.toLower();

	if ("string" == sPar)
		return ptSTRING;
	else if ("value" == sPar)
		return ptVALUE;
	else if ("domain" == sPar)
		return ptDOMAIN;
	else if ("rpr" == sPar)
		return ptRPR;
	else if ("georef" == sPar)
		return ptGEOREF;
	else if ("coordsys" == sPar)
		return ptCOORDSYS;
	else if ("rasmap" == sPar)
		return ptRASMAP;
	else if ("segmap" == sPar)
		return ptSEGMAP;
	else if ("polmap" == sPar)
		return ptPOLMAP;
	else if ("pntmap" == sPar)
		return ptPNTMAP;
	else if ("table" == sPar)
		return ptTABLE;
	else if ("mapview" == sPar)
		return ptMAPVIEW;
	else if ("maplist" == sPar)
		return ptMAPLIST;
	else if ("tbl2d" == sPar)
		return ptTBL2D;
	else if ("anntxt" == sPar)
		return ptANNTXT;
	else if ("sms" == sPar)
		return ptSMS;
	else if ("matrix" == sPar)
		return ptMATRIX;
	else if ("filter" == sPar)
		return ptFILTER;
	else if ("function" == sPar)
		return ptFUNCTION;
	else if ("script" == sPar)
		return ptSCRIPT;
	else if ("filename" == sPar)
		return ptFILENAME;
	else if ("column" == sPar)
		return ptCOLUMN;
	return ptANY;
}

bool ScriptPtr::fParamIncludeExtension(int i)
{
	String s("Ext%d", i);
  bool fExt;
  if (0 == ReadElement("Params", s.c_str(), fExt))
    fExt = true; // default is the old behaviour of always including the extension
  return fExt;
}

String ScriptPtr::sParamType(ParamType pt)
{
	String s;
	switch (pt) {
		case ptSTRING:
			s = "string";
			break;
		case ptVALUE:
	    s = "value";
			break;
		case ptDOMAIN:
	    s = "domain";
			break;
		case ptRPR:
	    s = "rpr";
			break;
		case ptGEOREF:
	    s = "georef";
			break;
		case ptCOORDSYS:
	    s = "coordsys";
			break;
		case ptRASMAP:
	    s = "rasmap";
			break;
		case ptSEGMAP:
	    s = "segmap";
			break;
		case ptPOLMAP:
	    s = "polmap";
			break;
		case ptPNTMAP:
	    s = "pntmap";
			break;
		case ptTABLE:
	    s = "table";
			break;
		case ptMAPVIEW:
			s = "mapview";
			break;
		case ptMAPLIST:
			s = "maplist";
			break;
		case ptTBL2D:
			s = "tbl2d";
			break;
		case ptANNTXT:
			s = "anntxt";
			break;
		case ptSMS:
			s = "sms";
			break;
		case ptMATRIX:
			s = "matrix";
			break;
		case ptFILTER:
			s = "filter";
			break;
		case ptFUNCTION:
		  s = "function";
			break;
		case ptSCRIPT:
		  s = "script";
      break;
		case ptFILENAME:
		  s = "filename";
      break;
		case ptCOLUMN:
		  s = "column";
			break;
	}
	return s;
}

void ScriptPtr::SetParams(int iNr)
{
  iParms = iNr;
}

void ScriptPtr::SetDefaultParam(int i, const String& sDefault)
{
	if ( sDefault != "" ) WriteElement("Params", String("DefaultValue%d", i).c_str(), sDefault);
}

void ScriptPtr::SetParam(int i, ParamType pt, const String& sQuestion, bool fIncExt, const String& sDefault)
{
	String sParam("Param%d", i);
	WriteElement("Params", sParam.c_str(), sQuestion);
	iParms = max(iParms, i+1);
	String sType("Type%d", i);
	String s;
	switch (pt) {
		case ptSTRING:
			s = "string";
			break;
		case ptVALUE:
	    s = "value";
			break;
		case ptDOMAIN:
	    s = "domain";
			break;
		case ptRPR:
	    s = "rpr";
			break;
		case ptGEOREF:
	    s = "georef";
			break;
		case ptCOORDSYS:
	    s = "coordsys";
			break;
		case ptRASMAP:
	    s = "rasmap";
			break;
		case ptSEGMAP:
	    s = "segmap";
			break;
		case ptPOLMAP:
	    s = "polmap";
			break;
		case ptPNTMAP:
	    s = "pntmap";
			break;
		case ptTABLE:
	    s = "table";
			break;
		case ptMAPVIEW:
			s = "mapview";
			break;
		case ptMAPLIST:
			s = "maplist";
			break;
		case ptTBL2D:
			s = "tbl2d";
			break;
		case ptANNTXT:
			s = "anntxt";
			break;
		case ptSMS:
			s = "sms";
			break;
		case ptMATRIX:
			s = "matrix";
			break;
		case ptFILTER:
			s = "filter";
			break;
		case ptFUNCTION:
		  s = "function";
			break;
		case ptSCRIPT:
		  s = "script";
      break;
		case ptFILENAME:
		  s = "filename";
      break;
		case ptCOLUMN:
		  s = "column";
			break;
	}
	WriteElement("Params", sType.c_str(), s);
	SetDefaultParam(i, sDefault);
	String sExt("Ext%d", i);
	WriteElement("Params", sExt.c_str(), fIncExt);
}

void ScriptPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	FileName fn(fnObj, "isf");
	os.AddFile(fnObj, "Script", "ScriptFile");
	FileName fnHelp(fnObj, ".htm");
	os.AddFile(fnHelp); // if it does not exist the os will reject it anyway
	os.AddFile("Ilwis.css");
}
