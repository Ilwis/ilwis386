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
/* $Log: /ILWIS 3.0/Script_Copier/Script.cpp $
 * 
 * 62    20-01-03 15:32 Willem
 * - Changed: for the "message" command the message is stripped of leading
 * and trailing spaces
 * 
 * 61    27-06-02 19:13 Koolhoven
 * ScriptPtr::ExecAction(): also "del" is executed in the Script thread
 * and not in a seperate thread
 * 
 * 60    6/03/02 2:29p Martin
 * Merge from ASTER branch
 * 
 * 61    5/28/02 11:58a Martin
 * foreign objects are no longer dependent on the document so no need to
 * start them in the main thread (MDI windows)
 * 
 * 59    11/02/01 2:18p Martin
 * parmlist could not be used in ExecAction. Parmlist can not handle
 * expressions with a minus (-) in it.
 * 
 * 58    21-09-01 19:02 Koolhoven
 * added fParamIncludeExtension to indicate whether the extension should
 * be added to the file name for the parameter
 * 
 * 57    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 56    9-05-01 12:31 Koolhoven
 * in ExecAction() only call SendMessage when needed with the open
 * command. Never with others
 * 
 * 55    5/03/01 2:50p Martin
 * the foreign extension were not included in the set of types that must
 * be opened through a sendmessage (MDI windows must open in the main
 * thread)
 * 
 * 54    26-04-01 17:42 Koolhoven
 * md now only does a makedir, not a changedir anymore, to prevent
 * confusion
 * 
 * 53    5-03-01 12:39 Koolhoven
 * in UpdateCatalog() use PostMessage() instead of PostThreadMessage()
 * 
 * 52    26-02-01 2:30p Martin
 * parms for the open command are now reorderd to get the commandhandler
 * working correctly with it.
 * 
 * 51    20-02-01 2:36p Martin
 * pause and message send a read catalog because the script no longer
 * generates it
 * 
 * 50    12-02-01 2:39p Martin
 * all commands except those that open a catlog-like window use Execute to
 * execute their action. This ensures that the thread in which the script
 * runs remains the same. Catlog-likes must run in the main thread
 * 
 * 49    9-02-01 10:37a Martin
 * filenames with spaces in it where not correctly parsed for the cd
 * command
 * 
 * 48    22-01-01 11:15a Martin
 * before changing dir first the current dir has to be set
 * 
 * 47    23-11-00 2:23p Martin
 * ilwis.css included in the files to be copied
 * 
 * 46    10-11-00 12:21p Martin
 * md and cd used an obsolete way to set the current dir.
 * 
 * 45    9-11-00 11:52a Martin
 * used sendmessage ILW_EXECUTE to execute the open. Prevents any crashes
 * from MDIObjects that are opened (thread problems)
 * 
 * 44    11/07/00 3:48p Martin
 * added help file for copying to the getobjectstructure
 * 
 * 43    11/02/00 10:11a Martin
 * changed the oldgetobjectstructure to a new form (old was from weeks
 * back)
 * 
 * 42    9-10-00 11:23a Martin
 * md command has now a script counterpart. This is done forestall
 * directory documents use
 * 
 * 41    19-09-00 3:41p Martin
 * added a function to convert a paramstring to paramnumber
 * 
 * 40    11-09-00 11:32a Martin
 * added function for ObjectStructure
 * 
 * 39    5/09/00 15:19 Willem
 * Added SetReadyOnly function to be able to change the read/write status
 * of the datafile
 * 
 * 38    8-08-00 15:35 Koolhoven
 * message cmd now gives a always on top message box
 * 
 * 37    19-07-00 3:46p Martin
 * added ! to get the extarnal command working
 * 
 * 36    17-05-00 13:06 Koolhoven
 * protected against undefined iNrParams
 * 
 * 35    10-05-00 3:59p Martin
 * number of parms is now a member of script
 * 
 * 34    12-04-00 12:44p Martin
 * script have now a -dir parm to set the working directory for scripts
 * 
 * 33    4/10/00 12:05p Wind
 * added flag to set the directory to start the script (-dir=path)
 * 
 * 32    8-03-00 16:21 Wind
 * change directory problem
 * 
 * 31    8-03-00 12:00 Wind
 * added special parameter types ptFILENAME and ptCOLUMN
 * 
 * 30    25/02/00 9:20 Willem
 * Removed second parameter from Execute call
 * 
 * 29    22-02-00 13:54 Wind
 * change flag noask to quiet
 * 
 * 28    22-02-00 12:05 Wind
 * move commands copy and copyfile to commandhandler
 * 
 * 27    21-02-00 16:36 Wind
 * moved more commands to command handler
 * 
 * 26    16-02-00 17:41 Wind
 * more commands moved from script to commandhandler
 * 
 * 25    16-02-00 17:27 Wind
 * moved commands from script to commandhandler
 * 
 * 24    8-02-00 18:04 Wind
 * removed some 'update catalogs'
 * 
 * 23    7-02-00 17:05 Wind
 * added show with -noask option
 * 
 * 22    1-02-00 10:26a Martin
 * Script commands are not stored in the History
 * 
 * 21    14-01-00 17:19 Koolhoven
 * Post reread Catalog as thread message
 * 
 * 20    4-01-00 16:18 Wind
 * accept quoted name when copying object
 * 
 * 19    22-12-99 18:13 Koolhoven
 * Derived ElementContainer from FileName
 * it has ElementMap* as a member.
 * In ObjectInfo::ReadElement and ObjectInfo::WriteElement the ElementMap
 * is used when available instead of the file.
 * ElementMap is initialized in MapCompositionDoc::Serialize to load/store
 * a MapView.
 * Further has IlwisObjectPtr now a SetFileName() function to prevent
 * everythere the const_casts of fnObj.
 * 
 * 18    2-12-99 12:02 Wind
 * accept double quotes as single quotes in parameter list of script
 * 
 * 17    29-11-99 2:47p Martin
 * added getters/setter for default values for scirpt params
 * 
 * 16    29-11-99 10:25 Wind
 * adapted for UNC (use now SetCurrentDirectory and GetCurrentDirectory)
 * 
 * 15    29-10-99 12:51 Wind
 * case sensitive stuff
 * 
 * 14    25-10-99 13:14 Wind
 * making thread save (2); not yet finished
 * 
 * 13    22-10-99 12:54 Wind
 * thread save access (not yet finished)
 * 
 * 12    20-10-99 15:53 Wind
 * debugged for parameters with spaces
 * 
 * 11    9/29/99 10:34a Wind
 * added case insensitive string comparison
 * 
 * 10    16-09-99 16:11 Koolhoven
 * ScriptWindow has now a tabwindow and shows in the second tab the
 * parameters to enter.
 * When a script is run with parameters they are asked from the user
 * ScriptDoc has been adapted to recognize both views..
 * 
 * 9     15-09-99 12:11 Koolhoven
 * When a script has parameters a form will prompt the user for them
 * 
 * 8     9/08/99 12:56p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 7     9/08/99 10:25a Wind
 * adpated to use of quoted file names
 * 
 * 6     7/02/99 12:17p Visser
 * import (for importASCII table was in comment
 * 
 * 5     5/21/99 9:33a Martin
 * changed a WinExec in a IlwisApp->Execute(..)
 * 
 * 4     3/12/99 3:05p Martin
 * Added support for case insensitive
 * 
 * 3     3/08/99 3:26p Martin
 * //->/*
 * 
 * 2     3/08/99 3:15p Martin
 * Some error in flags with GetFileAttributes
// Revision 1.15  1998/10/08 08:35:46  Wim
// Added closeall as command
//
// Revision 1.14  1998-09-17 13:08:36+01  Wim
// closetbl now removes the not used entries in the Array<Table> atbl
// to prevent the limitless growing of this array and errormessages
// as described in bug 95.
//
// Revision 1.13  1998-09-16 18:22:46+01  Wim
// 22beta2
//
// Revision 1.12  1998/05/12 19:37:02  Willem
// Added support for import and export commands from the script
//
// Revision 1.11  1997/09/11 14:13:55  Wim
// No strict checking in DelObject()
//
// Revision 1.10  1997-09-11 12:29:58+02  Wim
// UpdateCatalog() switched to no force
//
// Revision 1.9  1997-09-09 14:43:48+02  Wim
// Revised CreateGrf() function
//
// Revision 1.8  1997-08-18 20:53:45+02  Wim
// Skip objects in use, do not quit
//
// Revision 1.7  1997-08-18 20:52:34+02  Wim
// DelObject with and without force changed
//
// Revision 1.6  1997-08-18 20:27:11+02  Wim
// Force Catalogue to read again when needed
//
// Revision 1.5  1997-08-15 16:25:55+02  Wim
// InvalidLineError writing corrected
//
// Revision 1.4  1997-08-12 18:52:01+02  Wim
// Allow to calculate a histogram
//
// Revision 1.3  1997-07-31 11:18:41+02  Wim
// cd now also accepts \ and ..
// breakdepcol added
//
// Revision 1.2  1997-07-30 19:45:26+02  Wim
// Changed scanning of "vr=lo:hi:step", also allow now "vr=::step",
// this creates a valuerange with an invalid range, so that the range will
// be set by the application and only the stepsize will be overruled by the user.
//
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
    SetPointer(new ScriptPtr(FileName(fn)));
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
    _unlink(fnScript.sFullName().scVal());
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
      NotFoundError(fn);
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
    ErrorObject(SDATErrRightCurlyExpected, 0).Show();  // '}'
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
  throw ErrorObject(WhatError(String(SDATErrUnknownCommand_S.scVal(), sComm), errScript));
}  

void ScriptPtr::DomainValueRangeExpected(const FileName& fnObj)
{
  throw ErrorObject(WhatError(SDATErrDomainVRExpected, errScript+4));
} 

void ScriptPtr::InvalidLineError(const String& sLine, const FileName& fnObj) {
  throw ErrorObject(WhatError(String(SDATErrInvalidLine_S.scVal(), sLine), errScript+2), fnObj);
} 

void ScriptPtr::SortDomainError(const String& sDomain, const FileName& fnObj)
{
  throw ErrorSortDomain(sDomain, fnObj, errScript+1);
}

void ScriptPtr::NotDomainGroupError(const String& sDomain, const FileName& fnObj)
{
  throw ErrorObject(
    WhatError(String(SDATErrDomainGroupExpected_S.scVal(), sDomain),
		errScript+1), fnObj);
}

void ScriptPtr::InvalidDirError(const String& sDir, const FileName& fnObj)
{
  throw ErrorObject(WhatError(String(SDATErrInvalidDir_S.scVal(), sDir), errScript+1), fnObj);
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
		SetCurrentDirectory(getEngine()->sGetCurDir().scVal());
		Directory dir(sDir);
		if (sDir == "\\")
			SetCurrentDirectory("\\");
		else if (sDir == "..")
			SetCurrentDirectory("..");
		else {
			word iFileAtt;
			iFileAtt=GetFileAttributes(dir.sFullPath().scVal());
			bool fExist = 0xffff != iFileAtt;
			if (!fExist || !(iFileAtt & FILE_ATTRIBUTE_DIRECTORY))
				InvalidDirError(sDir, fnObj);
			SetCurrentDirectory(dir.sFullPath().scVal());
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
		MessageBox(0, sCurrLine.sTrimSpaces().scVal(), sName().scVal(), MB_OK|MB_TOPMOST);
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
		const char *s = sCmd.scVal();	
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
	ReadElement("Params", s.scVal(), sPar);
	return sPar;
}

String ScriptPtr::sDefaultValue(int i)
{
	String sVal;
	ReadElement("Params", String("DefaultValue%d", i).scVal(), sVal);
	return sVal;
}


ScriptPtr::ParamType ScriptPtr::ptParam(int i)
{
	String s("Type%d", i);
	String sPar;
	ReadElement("Params", s.scVal(), sPar);
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
  if (0 == ReadElement("Params", s.scVal(), fExt))
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
	if ( sDefault != "" ) WriteElement("Params", String("DefaultValue%d", i).scVal(), sDefault);
}

void ScriptPtr::SetParam(int i, ParamType pt, const String& sQuestion, bool fIncExt, const String& sDefault)
{
	String sParam("Param%d", i);
	WriteElement("Params", sParam.scVal(), sQuestion);
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
	WriteElement("Params", sType.scVal(), s);
	SetDefaultParam(i, sDefault);
	String sExt("Ext%d", i);
	WriteElement("Params", sExt.scVal(), fIncExt);
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
