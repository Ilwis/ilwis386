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
#pragma warning( disable : 4786 )

#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Appforms.hs"
#include "Headers\toolspch.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\strng.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include <afxpriv.h>
#include "Engine\DataExchange\MAPIMP.H"
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\DataExchange\POLIMP.H"
#include "Engine\DataExchange\PNTIMP.H"
#include "Engine\DataExchange\Tblimp.h"
#include "Headers\messages.h"
#include "Engine\Base\System\Appcont.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ExternalCommand.h"
#include "Engine\Stereoscopy\StereoPair.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Engine\Base\File\ObjectCopier.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"

struct WinAndParm 
{ // struct for passing parms to import, export, copy and delete threads
	WinAndParm(): handle(0), s(0), sPath(0) {}
	~WinAndParm() { delete s; delete sPath; }
	HWND handle;
	String* s;
	String* sPath;
};

LRESULT Cmdcopy(CWnd *wndOwner, const String& sPar)
{
	WinAndParm* wp = new WinAndParm;
	wp->handle = wndOwner->m_hWnd;
	wp->s = new String(sPar);
	wp->sPath = new String(getEngine()->getContext()->sGetCurDir());
	AfxBeginThread(CommandHandler::CmdCopyFileInThread, (LPVOID)(wp));

	return -1;
}

static void UpdateCatalog(FileName *fpn=NULL)
{
	getEngine()->PostMessage(ILW_READCATALOG, 0, (LPARAM)fpn);
}

BaseCommandHandler::BaseCommandHandler() 
: wndOwner(0)
{
	commands["exit"] = &BaseCommandHandler::CmdExit;
	commands["help"] = &BaseCommandHandler::CmdHelp;
} 

BaseCommandHandler::~BaseCommandHandler()
{
	
}

void BaseCommandHandler::SetOwner(CFrameWnd* wnd)
{
	ASSERT(wnd != 0);
	wndOwner = wnd;
}

void BaseCommandHandler::AddCommand(string sCmd, CommandFunction cf)
{
	commands[sCmd] = cf;
}

LRESULT BaseCommandHandler::fExecute(const String& sCmd)
{
	String cc = sCmd;
	String head = cc.sHead(" "); 
	String sCom = head.toLower();
	size_t iSize = min(sCom.size() + 1, sCmd.size());
	String sParm = sCmd.substr(iSize); // iSize cannot be larger than sCmd.size()
	sParm = sParm.sTrimSpaces();
	bool routToClient = sCom == "copy" && getEngine()->hasClient();
	CommandIter ci = commands.find(sCom);
	if (!routToClient && ci != commands.end()) // known command ?
	{
		const CommandPair &cp = *ci;
		if (cp.second) // anything usefull ??
		{
			CommandFunction cf = cp.second;
			String sCmd = sParm.sTrimSpaces();
			(this->*cf)(sCmd);
			return true;
		}
		else
			throw ErrorObject(String(SMSErrNoCommandHandler_S.scVal(), sCmd)); // ??later here loadModule/ GetProcAddress ??
	} else {
		ReroutPost(sCmd);
		return true;
	}
	return false;
}

void BaseCommandHandler::CmdExit(const String& s)
{
	if (wndOwner)
		getEngine()->SendMessage(WM_CLOSE, 0, 0);
}

void BaseCommandHandler::CmdHelp(const String& s)
{
	ReroutPost("help " + s);
}

UINT CommandHandler::CalcObjectAndShow(LPVOID sFile)
{
	try
	{
		FileName fn(* ((String *)sFile));
		delete sFile; // sFile has been made outside the function and must be deleted here
		getEngine()->InitThreadLocalVars();		
		IlwisObject obj = IlwisObject::obj(fn);
		getEngine()->SetCurDir(fn.sPath());
		if (obj.fValid())
		{
			obj->Calc();
			String sCmd = "show " + fn.sFullNameQuoted(true);
			char* str = sCmd.sVal();
			getEngine()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);

			FileName *fn2 = new FileName(fn);
			UpdateCatalog(fn2);
			getEngine()->RemoveThreadLocalVars();
			return TRUE;
		}
		getEngine()->RemoveThreadLocalVars();
		return FALSE;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		getEngine()->RemoveThreadLocalVars();
	}
	return FALSE;
}

UINT CommandHandler::FreezeObjectAndShow(LPVOID sFile)
{
	try
	{
		FileName fn(* ((String *)sFile));
		delete sFile; // sFile has been made outside the function and must be deleted here
		getEngine()->InitThreadLocalVars();		
		IlwisObject obj = IlwisObject::obj(fn);
		getEngine()->SetCurDir(fn.sPath());
		if (obj.fValid())
		{
			obj->Calc();
			obj->BreakDependency();
			String sC = "show " + fn.sFullNameQuoted(true);
			getEngine()->Execute(sC);

			FileName *fn2 = new FileName(fn);
			UpdateCatalog(fn2);
			getEngine()->RemoveThreadLocalVars();
			return TRUE;
		}
		getEngine()->RemoveThreadLocalVars();
		return FALSE;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		getEngine()->RemoveThreadLocalVars();
	}
	return FALSE;
}

UINT CommandHandler::FreezeObjectInThread(LPVOID lp)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
	try
	{
		String *sCmd =  (String*) lp;
		getEngine()->InitThreadLocalVars();
		ParmList pm(*sCmd);
		delete sCmd; // sCmd has been made outside the function and must be deleted here
		Directory dir(pm.sGet(pm.iFixed()-1));
		getEngine()->SetCurDir(dir.sFullPath());
		BreakDepObjects(pm.sGet(0), 0); // no tranquilizer

		// Now tell mainwindow to update the catalog
		FileName* pfn = new FileName(pm.sGet(0));
		UpdateCatalog(pfn);

		getEngine()->RemoveThreadLocalVars();
		return TRUE;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		getEngine()->RemoveThreadLocalVars();
	}
	
	return FALSE;
}


UINT CommandHandler::CalcObjectInThread(LPVOID lp)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
	try
	{
		String *sCmd =  (String*) lp;
		ParmList pm(*sCmd);
		delete sCmd; // sCmd has been made outside the function and must be deleted here
		getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(pm.sGet(pm.iFixed()-1));
		CalcObjects(pm.sGet(0), 0); // no tranquilizer

		// Now tell mainwindow to update the catalog
		FileName* pfn = new FileName(pm.sGet(0));
		UpdateCatalog(pfn);
		
		getEngine()->RemoveThreadLocalVars();
		return TRUE;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		getEngine()->RemoveThreadLocalVars();
	}
	
	return FALSE;
}

UINT CommandHandler::UpdateObjectInThread(LPVOID lp)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
	try
	{
		String *sCmd =  (String*) lp;
		ParmList pm(*sCmd);
		delete sCmd; // sCmd has been made outside the function and must be deleted here
		getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(pm.sGet(pm.iFixed()-1));
		UpdateObjects(pm.sGet(0), 0); // no tranquilizer

		// Now tell mainwindow to update the catalog
		FileName* pfn = new FileName(pm.sGet(0));
		UpdateCatalog(pfn);
	
		getEngine()->RemoveThreadLocalVars();
		return TRUE;
	}
	catch(ErrorObject& err)
	{
		err.Show();
		getEngine()->RemoveThreadLocalVars();
	}
	
	return FALSE;
}



UINT CommandHandler::RunScriptInThread(LPVOID lp)
{
	try
	{
		String* sScr = (String*) lp;
		String sScript = *sScr;
		delete sScr; // sScr has been made outside the function and must be deleted here
		ParmList pl(sScript);
		String s1 = pl.sGet(0);
		FileName fn(s1);
		getEngine()->InitThreadLocalVars();
		getEngine()->SetCurDir(fn.sPath());
		// scripts will only update the catalog under special circumstances, so generally no.
		bool *fNoUpdate = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
		*fNoUpdate = true;
		Script scr(fn);
		if (scr.fValid()) 
		{		
			// make background jobs easier interruptable
			AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
			String sExpr;
			char* sTail = sScript.strchrQuoted(' ');
			if (sTail)
				sExpr = sTail + 1;
			scr->Exec(sExpr);
		}
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}

	UpdateCatalog(NULL);
	getEngine()->RemoveThreadLocalVars();
	return FALSE;
}

struct FileNameAndString
{
	FileName* fnObj;
	String*  sExpr;
};

bool CommandHandler::ImportObject(FileName fnObj, String sExpr)
{
	bool fRet = false;
	String sObjType = IlwisObjectPtr::sParseFunc(sExpr);
	if (fCIStrEqual(sObjType, "map")) 
	{
		Map mp(fnObj, sExpr);
		fRet = mp.fValid();
	}
	else if (fCIStrEqual(sObjType, "maplist")) 
	{
		MapList mpl(fnObj, sExpr);
		fRet = mpl.fValid();
	}
	else if (fCIStrEqual(sObjType, "table")) 
	{
		Table tbl(fnObj, sExpr);
		fRet = tbl.fValid();
	}
	return fRet;
}

UINT CommandHandler::ImportObjectInThread(LPVOID lp)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	BOOL fRet = FALSE;
	try
	{
		FileNameAndString* fns = (FileNameAndString*) lp;
		getEngine()->InitThreadLocalVars();
		String sExpr = *(fns->sExpr);
		FileName fnObj = *(fns->fnObj);
		delete fns->sExpr;
		delete fns->fnObj;
		delete fns; 
	//	getEngine()->SetCurDir(fnObj.sPath());
		bool *fNoUpdate = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
		*fNoUpdate = true;
		fRet = ImportObject(fnObj, sExpr);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	UpdateCatalog(NULL);
	getEngine()->RemoveThreadLocalVars();
	return fRet;
}

//---[ CommandHandler ]-------------------------------------------------------------------------

CommandHandler::CommandHandler() 
{
	Init();
}

CommandHandler::~CommandHandler()
{

}

void CommandHandler::Init()
{
	//datamanagment
	commands["cd"							  ] = (CommandFunction) &CommandHandler::CmdChangeDir;
	commands["opendir"				  ] = (CommandFunction) &CommandHandler::CmdOpenDir;
	commands["md"							  ] = (CommandFunction) &CommandHandler::CmdMakeDir;
	commands["mkdir"					  ] = commands["md"];
	commands["rd"							  ] = (CommandFunction) &CommandHandler::CmdRemoveDir;
	commands["rmdir"					  ] = commands["rd"];
	commands["create"					  ] = (CommandFunction) &CommandHandler::CmdCreate;
	commands["del"						  ] = (CommandFunction) &CommandHandler::CmdDel;
	commands["copy"						  ] = (CommandFunction) &CommandHandler::Cmdcopy;

	commands["createpyramidlayers"]= (CommandFunction) &CommandHandler::CmdCreatePyramidFiles;
	commands["deletepyramidlayers"]= (CommandFunction) &CommandHandler::CmdDeletePyramidFiles;	

	commands["calc"							] = (CommandFunction) &CommandHandler::CalcObject;
	commands["calculate"				] = commands["calc"];
	commands["makeuptodate" 		] = (CommandFunction) &CommandHandler::UpdateObject;
	commands["update" 		      ] = commands["makeuptodate"];
	commands["breakdep" 			  ] = (CommandFunction) &CommandHandler::BreakDepObject;
	commands["reldiskspace"		  ] = (CommandFunction) &CommandHandler::RelDiskSpaceObject;
	commands["reldisksp"		    ] = commands["reldiskspace"];
	commands["script"						] = (CommandFunction) &CommandHandler::CmdScript;
	commands["run"							] = (CommandFunction) &CommandHandler::CmdRunScript;
	commands["setdescr"         ] = (CommandFunction) &CommandHandler::CmdSetDescr;
							
//	commands["mapcalc"					] = (CommandFunction) &CommandHandler::CmdMapCalc;
	//commands["maplistspectra"			] = (CommandFunction) &CommandHandler::CmdSpectra;	

	commands["import14"					] = (CommandFunction) &CommandHandler::CmdImport14;
	commands["convert14"        ] = (CommandFunction) &CommandHandler::CmdConvert14;
	commands["setreadonly"      ] = (CommandFunction) &CommandHandler::CmdSetReadOnly;
	commands["setreadwrite"     ] = (CommandFunction) &CommandHandler::CmdSetReadWrite;
	commands["closeall"         ] = (CommandFunction) &CommandHandler::CmdCloseAll;
	//commands["rename"           ] = (CommandFunction) &CommandHandler::CmdRenameObject;
	commands["delfile"          ] = (CommandFunction) &CommandHandler::CmdDelFile;
	commands["delcol"           ] = (CommandFunction) &CommandHandler::CmdDelColumn;
	commands["calccol"          ] = (CommandFunction) &CommandHandler::CmdCalcColumn;
	commands["updatecol"        ] = (CommandFunction) &CommandHandler::CmdUpdateColumn;
	commands["breakdepcol"      ] = (CommandFunction) &CommandHandler::CmdBreakDepColumn;
	commands["domidtoclass"     ] = (CommandFunction) &CommandHandler::CmdDomIDToClass;
	commands["domclasstoid"     ] = (CommandFunction) &CommandHandler::CmdDomClassToID;
	commands["dompictoclass"    ] = (CommandFunction) &CommandHandler::CmdDomPicToClass;

	commands["setgrf"] = (CommandFunction) &CommandHandler::CmdSetGrf;
	commands["setcsy"] = (CommandFunction) &CommandHandler::CmdSetCsy;
	commands["setatttable"] = (CommandFunction) &CommandHandler::CmdSetAttTable;
	commands["setdom"] = (CommandFunction) &CommandHandler::CmdSetDom;
	commands["setvr"] = (CommandFunction) &CommandHandler::CmdSetValRange;
	commands["changedom"] = (CommandFunction) &CommandHandler::CmdChangeDom;
	commands["mergedom"] = (CommandFunction) &CommandHandler::CmdMergeDom;
	commands["additemtodomain"] = (CommandFunction) &CommandHandler::CmdAddItemToDomain;
	commands["additemtodomaingroup"] = (CommandFunction) &CommandHandler::CmdAddItemToDomainGroup;
	commands["showexpressionerror"] = (CommandFunction) &CommandHandler::CmdShowExpressionError;

	commands["cr2dim"] = (CommandFunction) &CommandHandler::CmdCreate2DimTable;
	commands["crtbl"] = (CommandFunction) &CommandHandler::CmdCreateTable;
	commands["crmap"] = (CommandFunction) &CommandHandler::CmdCreateMap;
	commands["crpntmap"] = (CommandFunction) &CommandHandler::CmdCreatePointMap;
	commands["crsegmap"] = (CommandFunction) &CommandHandler::CmdCreateSegMap;
	commands["crmaplist"] = (CommandFunction) &CommandHandler::CmdCrMapList;	
	commands["crdom"] = (CommandFunction) &CommandHandler::CmdCreateDom;
	commands["crgrf"] = (CommandFunction) &CommandHandler::CmdCreateGrf;
	commands["crrpr"] = (CommandFunction) &CommandHandler::CmdCreateRpr;
	commands["appmetadata"]= (CommandFunction) &CommandHandler::CmdAppMetaData;



	commands["testingdbconnection"] = (CommandFunction) &CommandHandler::CmdTestingDBConnection;
}

class ErrorInvalidCalcCommand
{
  public:
    ErrorInvalidCalcCommand() {}
};

// main execution function for the command handler.
LRESULT CommandHandler::fExecute(const String& sCmd)
{
	try
	{
		if (sCmd == "") return true;

		if (sCmd[0] != '*') {
			ReroutPost("commandline " + sCmd);
		}


		String sComd = sCmd.sTrimSpaces();

		if (sComd[0] == '!' || ( sComd[0] == '*' && sCmd[1] == '!') ) // external commands
		{
			int iWhere = sComd[0] == '!' ? 1 : 2;
			return fCmdExtern(sComd.substr(iWhere));
		}

		if ( sComd[0] == '?' || ( sComd[0] == '*' && sCmd[1] == '?' )) // simple calc
		{
			int iWhere = sComd[0] == '?' ? 1 : 2;
			return fCmdSimpleCalc(sComd.substr(iWhere));
		}

		if ((sComd[1] == ':') && (sComd.length()==2)) // change drive of the from 'e:'
			return fExecute("cd " + sComd);

		if (tolower(sComd[0]) == 'c' && tolower(sComd[1]) == 'd' // allow cd without space
				&& (sComd[2] == '.' || sComd[2] == '\\'))
			return fExecute("cd " + sComd.substr(2));

		if ( sComd[sComd.length()-1] == ';')  // command to the script parser
		{
			CmdScript(sComd.substr(0, sComd.size() - 1));
			return true;
		}

		try {
			// this is an incorrect check if we allow file names which include '='
			return fCmdCalc(sComd);
		}
		catch (const ErrorInvalidCalcCommand&)
		{
			bool fQuiet = sComd[0] == '*';
			if (fQuiet)
				sComd = String((char*)&sComd[1]); // skip *
			if (fCIStrEqual(sComd.sLeft(8), "tabcalc "))  // can't have noask flag, so handle separately
			{
				CmdTabCalc((char*)&sComd[8]);
				return true;
			}
			FileName fn(sComd);
			String sOldCommand = sComd; // retained for error reporting

			// Slight workaround to make some old DDE scripts work: Import and Export don't like the -quiet parameter.
			if (fQuiet && sComd.sLeft(6) != "import" && sComd.sLeft(6) != "export")
				sComd &= " -quiet";

			if (BaseCommandHandler::fExecute(sComd))
				return true;

			if ("" == fn.sExt) 
			{
				fn.sExt = ".isl";
				if (fn.fExist())
				{
					if (fQuiet)
						CmdRunScript(String("*%S",sComd));
					else
						return fExecute(String("run %S", sComd));
					return true;
				}
			}
			throw ErrorObject(String(SMSErrIncorrectCmd_S.scVal(), sOldCommand));
		}
	}
	catch(StopScriptError& err)
	{
		throw err;
	}
	catch(ErrorNotFound& err)
	{
		bool *fNoFindErrorDisplay = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
		if (*fNoFindErrorDisplay)
			throw err;
		else
			err.Show();
	}
	catch(ErrorObject& err)
	{
		bool *fNoErrorDisplay = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWANYERROR));
		if (*fNoErrorDisplay)
			throw err;
		else
			err.Show();
	}
	return false;
}

void CommandHandler::CmdChangeDir(const String& sDir)
{
	if (sDir != "") 
	{
		String sDirLoc = sDir.sUnQuote();
		size_t iPos;
		if ( (iPos = sDirLoc.find("-quiet", sDirLoc.size() - 8)) != string::npos)
		sDirLoc = sDirLoc.substr(0, iPos)	;		
		SetCurrentDirectory(getEngine()->sGetCurDir().scVal());				
		Directory dir(sDirLoc);
		if (!SetCurrentDirectory(dir.sFullPath().scVal())) {
			String sErr("ChangeDirError: %i", GetLastError());
			throw ErrorObject(sErr);
		}
		char sPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, sPath);
		getEngine()->SetCurDir(sPath);
		ReroutPost("dirchanged");
	}
}

void CommandHandler::CmdShowExpressionError(const String& sErr) {
	throw ErrorObject(sErr);
}

void CommandHandler::CmdOpenDir(const String& sDir)
{
	if (sDir != "") 
	{
		String sDirLoc = sDir.sUnQuote();

		size_t iPos;
		if ( (iPos = sDirLoc.find("-quiet", sDirLoc.size() - 8)) != string::npos)
			sDirLoc = sDirLoc.substr(0, iPos - 1);
		SetCurrentDirectory(getEngine()->sGetCurDir().scVal());
		Directory dir(sDirLoc);		
		if (SetCurrentDirectory(dir.sFullPath().scVal()) == 0 )
		{
			String sErr(SMSErrorCouldNotChangeDirectory);
			throw ErrorObject(sErr);
		}
		
		char sPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, sPath);
		//Directory dir2(String(sPath));
		getEngine()->SendMessage(ILWM_OPENCATALOG, (WPARAM)&dir); 
	}
}

bool CommandHandler::fCmdExtern(const String& sCmd)
{
	// the 'old' syntax will "block" means ILWIS waits for completion
	// of the external command before moving on
	// if this is unwanted (e.g. from action.def), call with "!!" instead of "!'
	bool fBlockWhileExecuting = true;
	String sCmdStripped;
	if ((sCmd.length() > 0) && sCmd[0] == '!')
	{
		fBlockWhileExecuting = false;
		sCmdStripped = sCmd.sRight(sCmd.length() - 1); // strip the '!'
	}
	else
		sCmdStripped = sCmd;
	ParmList p(sCmdStripped);
	bool fIsQuoted = p.sGet(0)[0] == '\'';
	String sCommand = p.sGet(0).sUnQuote();

	String sParameters ("");
	for (int i=1; i < p.iSize(); ++i)
	{
		String sParameter = p.sGet(i);
		if (sParameter[0] == '\'')
			sParameter = "\"" + sParameter.sUnQuote() + "\"";
		sParameters += ((i > 1) ? " " : "") + sParameter;
	}
	
	FileName fn(sCommand); // "probe" if it is executable or not

	if ((fn.sExt.length() == 0) || fCIStrEqual(fn.sExt, ".exe") || fCIStrEqual(fn.sExt, ".com") || fCIStrEqual(fn.sExt, ".bat"))
	{
		// yes, executable - call CreateProcess
		String rest(sCmd.substr(sCommand.size() + (fIsQuoted ? 2 : 0)));
		ExternalCommand ec(sCommand, rest.sTrimSpaces());
		ec.Execute(fBlockWhileExecuting); // throws ErrorObject
	}
	else
	{
		int hInst = (int)ShellExecute(0, "open", sCommand.scVal(), NULL, "", SW_SHOWNORMAL);
		switch(hInst)
		{
			case 0:
				throw ErrorMemAlloc();
				break;
			case ERROR_BAD_FORMAT:
				throw ErrorObject(String(SMSErrExecuteError_S.scVal() , sCmd));
				break;
			case ERROR_FILE_NOT_FOUND:
				throw ErrorObject(String(SMSErrExternalCommand_S.scVal(), sCmd));
				break;
			case ERROR_PATH_NOT_FOUND:
				throw ErrorDirNotFound(FileName(sCmd).sPath());
		}
	}

	return true;
}

void CommandHandler::CmdMakeDir(const String& str)
{
	ParmList p(str);
	String sDir = p.sGet(0);
	SetCurrentDirectory(getEngine()->sGetCurDir().scVal());
	if ("" == sDir) 
	{
		String *sDir2 = (String *)ReroutSend(wndOwner, "mkdir " + sDir);
		sDir = *sDir2;
		delete sDir2;
	}
	String  sNewDir = sDir.sUnQuote();
	Directory  dir(sNewDir);
	if (!dir.fValid())	
		return;	
	if (!CreateDirectory(sNewDir.c_str(), NULL))
		throw WindowsError(GetLastError());

	ReroutPost("dirchanged");
	CmdOpenDir(sDir);
}

void CommandHandler::CmdRemoveDir(const String& str)
{
	ParmList p(str);
	String sDir = p.sGet(0).sUnQuote();
	SetCurrentDirectory(getEngine()->sGetCurDir().scVal());
	if ("" == sDir) 
	{
		String *sDir2 = (String *)ReroutSend(wndOwner, "rmdir " + sDir);
		sDir = *sDir2;
		delete sDir2;
	}
	Directory  dir(sDir);
	if ( dir.fValid())
	{
		if (_rmdir(sDir.c_str()))
			throw DosError(errno);
	}

	ReroutPost("dirchanged");
}

String CommandHandler::sDataBaseName(const String& sDb)
{
	size_t iWhere = sDb.rfind('\\');
	if ( iWhere != string::npos)
	{
		size_t iWhere2 = sDb.rfind('\\', iWhere - 1);
		if ( iWhere2 == string::npos ) iWhere2 = 0;
		String sDbName = sDb.substr(iWhere2, iWhere - iWhere2 );
		return sDbName.sUnQuote();
	}
	return "";
}

void CommandHandler::CmdScript(const String& sScript)
{
	if (sScript == "") return;

	Script::Exec(sScript);
}

void CommandHandler::CalcObject(const String& sFile)
{
	if (sFile == "") return;

	String *s = new String("%S %S", sFile, getEngine()->sGetCurDir());
	AfxBeginThread(CommandHandler::CalcObjectInThread, (LPVOID)s); 
}

void CommandHandler::BreakDepObject(const String& sF)
{
	if (sF == "") return;

	String *s = new String("%S %S", sF, getEngine()->sGetCurDir());
	AfxBeginThread(CommandHandler::FreezeObjectInThread, (LPVOID)s); 
}

void CommandHandler::RelDiskSpaceObject(const String& sF)
{
	if (sF == "") return;
	ParmList p(sF);
	String sFile = p.sGet(0);
	FileName fn(sFile);
	IlwisObject obj = IlwisObject::obj(fn);
	if (obj.fValid())
	{
		obj->DeleteCalc();

		// Now tell mainwindow to update the catalog
		if ( wndOwner) {
			FileName* pfn = new FileName(fn);
			UpdateCatalog(pfn);
		}
	}
}

void CommandHandler::UpdateObject(const String& sF)
{
	if (sF == "")
		return;

	ParmList p(sF);
	String sFile = p.sGet(0);
	String *s = new String("%S %S", sFile, getEngine()->sGetCurDir());
	AfxBeginThread(CommandHandler::UpdateObjectInThread, (LPVOID)s); 
}

void CommandHandler::CmdCreate(const String& s)
{
	TextInput ti(s);
	TokenizerBase tokenizer(&ti);
	Token tok = tokenizer.tokGet();
	String sType = tok.sVal();
	String sDom, sGrf, sCsy, sMpl, sMpr;
	String sVal;
	for (;;)
	{
		Token tok = tokenizer.tokGet();
		if (tok.sVal() == "")
			break;

		sVal = tok.sVal();
		FileName fn(sVal);
		if (fn.sExt == ".dom")
			sDom = sVal;
		else if (fn.sExt == ".grf")
			sGrf = sVal;
		else if (fn.sExt == ".csy")
			sCsy = sVal;
		else if (fn.sExt == ".mpl")
			sMpl = sVal;
		else if (fn.sExt == ".mpr")
			sMpr = sVal;
		}

	if("map" == sType)
		CreateMap(sDom, sGrf);
	else if ("seg" == sType)
		CreateSeg(sDom, sCsy);
	else if ("pnt" == sType)
		CreatePnt(sDom, sCsy);
	else if ("tbl" == sType)
		CreateTbl(sDom);
	else if ("tb2" == sType)
		CreateTb2(sDom);
	else if ("dom" == sType)
		CreateDom();
	else if ("grf" == sType)
		CreateGrf(sCsy);
	else if ("rpr" == sType)
		CreateRpr(sDom);
	else if ("fil" == sType)
		CreateFil();
	else if ("csy" == sType)
		CreateCsy();
	else if ("mpl" == sType)
		CreateMpl();
	else if ("sms" == sType)
		CreateSms(sMpr, sMpl);
	else if ("fun" == sType || "function" == sType)
		CreateFun();
	else if ("isl" == sType || "script" == sType)
		CreateIsl();
	else if ("dir" == sType)
		CmdMakeDir(sVal);
	else if ("ioc" == sType)
		CreateIoc(s);
	else if ("ilo" == sType || "lay" == sType || "layout" == sType)
		CmdLayout(sVal);
	else 
		MessageBox(0, SMSErrImpCreateCmd.scVal(), SMSErrCreate.scVal(), MB_OK | MB_ICONEXCLAMATION);
}

void CommandHandler::CmdLayout(const String& sCmd)
{
	ReroutPost("layout " + sCmd);
}

bool CommandHandler::fCmdSimpleCalc(const String& sCmd)
{
	ReroutPost("simplecalc " + sCmd);		
	return true;
}

static char* sComposed[]  = { ":=", 0}; // hmmm


bool CommandHandler::fCmdCalc(const String& sCmd)
{
	TextInput ip(sCmd);
	TokenizerBase tokenize(&ip);
	tokenize.SetComposed(sComposed);

	bool fShow = false;
	bool fAskOverwrite = true;
	bool fQuiet = false;
	bool fCalcInThread = true;
	Token tok = tokenize.tokGet();
	String sType = tok.sVal();
	if (sType == "$")
	{
		fShow = true;
		tok = tokenize.tokGet();
		sType = tok.sVal();
	}
	else if (sType == "*")
	{
		fShow = fAskOverwrite = false;
		fQuiet = true;
		fCalcInThread = false;
		tok = tokenize.tokGet();
		sType = tok.sVal();
	}
	else if (sType[0] == '$')
	{
		fShow = true;
		String sType = sType.substr(1);
	}

	tok = tokenize.tokGet();
	String sObject = tok.sVal();
	if ((sObject == "=") || (sObject == ":=") || (sObject == "{"))
	{
		sObject = sType;
		sType = "map";
	}
	else
		tok = tokenize.tokGet();

	if (sType == "table")
		sType = "tbl";

	Domain dom;
	ValueRange vr;
	bool fDom = tok.sVal() == "{";
	if (fDom)
		fDom = ScriptPtr::fScanDomainValueRange(tok, tokenize, dom, vr);
	if ((tok.sVal() != "=") && (tok.sVal() != ":="))
	{
		throw ErrorInvalidCalcCommand();
	}
	bool fFreeze = tok.sVal() == ":=";
	String sExpres = tokenize.sCurrentLine(); // rest of line
	sExpres = sExpres.sTrimSpaces();
	String sObj;
	if (sObject[0] == '$')
		sObj = sObject.substr(1);
	else
		sObj = sObject;
	FileName fn(sObj);

	if (fn.sExt == ".mpr")
		sType = "map";
	else if (fn.sExt == ".mpl")
		sType = "mpl";
	else if (fn.sExt == ".tbt")
		sType = "tbl";
	else if (fn.sExt == ".mps")
		sType = "seg";
	else if (fn.sExt == ".mpa")
		sType = "pol";
	else if (fn.sExt == ".mpp")
		sType = "pnt";
	else if (fn.sExt == ".mat")
		sType = "mat";
	else if (fn.sExt == ".ioc")
		sType = "ioc";

	size_t iBracket = sExpres.find('(');
	if (iBracket!=-1) 
	{
		char* c = sExpres.sVal();
		while (isalnum(*c)) ++c;
		// if non alpha numerical characters are before the bracket
		// presume a mapcalc statement
		if (c == sExpres.substr(iBracket)) 
		{
			String appName = sExpres.sHead("(");
			appName.toLower();
			ApplicationInfo *info = getEngine()->modules.getAppInfo(appName);
			if ( info && info->metadata){
				ApplicationQueryData query;
				query.queryType = "OUTPUTTYPE";
				query.expression = sExpres;
				ApplicationMetadata md = (info->metadata)(&query);
				if ( md.returnType != IlwisObject::iotANY) {
					switch( md.returnType){
						case IlwisObject::iotPOINTMAP :
							sType = "pnt"; break;
						case IlwisObject::iotPOLYGONMAP :
							sType = "pol"; break;
						case IlwisObject::iotSEGMENTMAP :
							sType = "seg"; break;
						case IlwisObject::iotRASMAP :
							sType = "map"; break;
						case IlwisObject::iotMAPLIST :
							sType = "mpl"; break;
						case IlwisObject::iotTABLE :
							sType = "tbl"; break;
						case IlwisObject::iotMATRIX :
							sType = "mat"; break;
						case IlwisObject::iotOBJECTCOLLECTION :
							sType = "col"; break;
						case IlwisObject::iotSTEREOPAIR :
							sType = "stp"; break;
					};
				}
			}
			else if (fCIStrEqual(sExpres.sLeft(8) , "MapList(") || fCIStrEqual(sExpres.sLeft(4) , "Map(") ||
				fCIStrEqual(sExpres.sLeft(6) , "Table("))
			{
				if (fCalcInThread)
				{
					FileNameAndString* fns = new FileNameAndString();
					fns->fnObj = new FileName(fn); 
					fns->sExpr = new String(sExpres);
					AfxBeginThread(CommandHandler::ImportObjectInThread, (LPVOID)(fns)); 
					return true;
				}
				else
					return CommandHandler::ImportObject(fn, sExpres);
			}
			else if (fCIStrEqual(sExpres.sLeft(14) , "PolygonMapList")) 
				sType = "ioc";
			else if (fCIStrEqual(sExpres.sLeft(7) , "MapList")) 
				sType = "mpl";
			else if (fCIStrEqual(sExpres.sLeft(3) , "Map"))
				sType = "map";
			else if (fCIStrEqual(sExpres.sLeft(5) , "Table")) 
			{
				sType = "tbl";
				if (fCIStrEqual(sExpres.sLeft(14) , "TableHistogram"))
					fAskOverwrite = false;
			}
			else if (fCIStrEqual(sExpres.sLeft(10) , "SegmentMap"))
				sType = "seg";
			else if (fCIStrEqual(sExpres.sLeft(10) , "PolygonMap"))
				sType = "pol";
			else if (fCIStrEqual(sExpres.sLeft(8) , "PointMap"))
				sType = "pnt";
			else if (fCIStrEqual(sExpres.sLeft(6) , "Matrix"))
				sType = "mat";
			else if (fCIStrEqual(sExpres.sLeft(10) , "StereoPair"))
				sType = "stp";

		}
	}
	if ("map" == sType)
		fn.sExt = ".mpr";
	else if ("mpl" == sType)
		fn.sExt = ".mpl";
	else if ("tbl" == sType) 
	{
		fn.sExt = ".tbt";
		if (fCIStrEqual(sExpres.sLeft(17) , "TableHistogramPnt"))
			fn.sExt = ".hsp";
		else if (fCIStrEqual(sExpres.sLeft(17) , "TableHistogramSeg"))
			fn.sExt = ".hss";
		else if (fCIStrEqual(sExpres.sLeft(17) , "TableHistogramPol"))
			fn.sExt = ".hsa";
		else if (fCIStrEqual(sExpres.sLeft(14) , "TableHistogram"))
			fn.sExt = ".his";
	}
	else if ("seg" == sType)
		fn.sExt = ".mps";
	else if ("pol" == sType)
		fn.sExt = ".mpa";
	else if ("pnt" == sType)
		fn.sExt = ".mpp";
	else if ("mat" == sType)
		fn.sExt = ".mat";
	else if ("stp" == sType)
		fn.sExt = ".stp";
	else if ("ioc" == sType)
		fn.sExt = ".ioc";
	else{
		throw ErrorInvalidCalcCommand();
	}
	
	if (!fn.fValid()) {
		throw ErrorObject( String(SMSErrInvalidOutObj_s.scVal(), sObj.sVal()));
	}
	if (fAskOverwrite && fn.fExist()) {
		String sErr(SAFMsgAlreadyExistsOverwrite_S.scVal(), fn.sFullNameQuoted(true));
		int iRet=wndOwner->MessageBox(sErr.scVal(), SAFMsgAlreadyExists.scVal(), MB_YESNO|MB_ICONEXCLAMATION);
		if (iRet!=IDYES)
			return false;
		else
		{
			Tranquilizer trq;
			DeleteObjects(wndOwner, String("%S -force -quiet", fn.sFullNameQuoted()), &trq);
		}
	}
	
	bool fOk = false;
	if ("map" == sType) {
		fn.sExt = ".mpr";
		Map map(fn, sExpres);
		fOk = map.fValid();
		if (fOk) {
			if (!map->fDependent()) {
				String sExpr("MapCalculate(%S)", sExpres);
				map = Map(fn, sExpr);
				fOk = map.fValid();
			}
			if (fOk) {
				if (dom.fValid() && map->fDomainChangeable())
					map->SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));
				else if (map->fValueRangeChangeable() && vr.fValid())
					map->SetValueRange(vr);
				IlwisSettings settings("DefaultSettings");
				bool fShowRasterDef = settings.fValue("ShowRasterDefForm", true);
				if (!fQuiet && fShowRasterDef) {
					String parms("%S,%d,%d", map->fnObj.sFullName(), !fShow, fFreeze);
					bool fOk = ReroutSend(wndOwner, "showdefform " + parms);
//					MapDefForm frm(wndOwner, map, !fShow, fFreeze);
//					//fOk = frm.fOkClicked();
//					if (!fShow)
//						fShow = frm.fShow;
					if (!fOk)
						map->fErase = true;
					wndOwner->SetFocus();
				}
			}
		}
	}
	else if ("mpl" == sType) {
		MapList mpl(fn, sExpres);
		fOk = mpl.fValid();
	}
	else if ("tbl" == sType) {
		Table tbl(fn, sExpres);
		fOk = tbl.fValid();
	}
	else if ("seg" == sType) {
		fn.sExt = ".mps";
		SegmentMap map(fn, sExpres);
		fOk = map.fValid();
		if (fOk)
			if (dom.fValid() && map->fDomainChangeable())
				map->SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));
			else if (map->fValueRangeChangeable() && vr.fValid())
				map->SetValueRange(vr);
	}
	else if ("pol" == sType) {
		fn.sExt = ".mpa";
		PolygonMap map(fn, sExpres);
		fOk = map.fValid();
		if (fOk)
			if (dom.fValid() && map->fDomainChangeable())
				map->SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));
			else if (map->fValueRangeChangeable() && vr.fValid())
				map->SetValueRange(vr);
	}    
	else if ("pnt" == sType) {
		fn.sExt = ".mpp";
		PointMap map(fn, sExpres);
		fOk = map.fValid();
		if (fOk)
			if (dom.fValid() && map->fDomainChangeable())
				map->SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));
			else if (map->fValueRangeChangeable() && vr.fValid())
				map->SetValueRange(vr);
	}
	else if ("mat" == sType) {
		fn.sExt = ".mat";
		MatrixObject mat(fn, sExpres);
		fOk = mat.fValid();
	}
	else if ("stp" == sType) {
		fn.sExt = ".stp";
		StereoPair stp(fn, sExpres);
		fOk = stp.fValid();
	} else if ("ioc" == sType) {
		fn.sExt = ".ioc";
		ObjectCollection col(fn, sExpres);
		fOk = col.fValid();
	}
	else {
		throw ErrorObject( SMSErrInvalidCmdLine.scVal());
	}
	if (fOk)
	{
		if ( fFreeze ) {
			if (fCalcInThread) {
				String s = fn.sFullName(true);
				if ( fShow ) {
					AfxBeginThread(CommandHandler::FreezeObjectAndShow, (LPVOID)(new String(s)));
				}
				else {
					AfxBeginThread(CommandHandler::FreezeObjectInThread, (LPVOID)(new String(s)));
				}
			}
			else {
				IlwisObject obj = IlwisObject::obj(fn);
				if (obj.fValid())
				{
					obj->Calc();
					obj->BreakDependency();
				}
				if ( fShow ) {
					String sC= "show " + fn.sFullPathQuoted(true);
					return fExecute(sC);
				}
			}
		}
		else if (fShow)
		{
			String sC= "show " + fn.sFullPathQuoted(true);
			return fExecute(sC);
		}
	}
	return false;
}


void CommandHandler::CmdRunScript(const String& sScript)
{
	if (sScript == "") return;
	// need to take the first identifier and add current path to it (if no path specified)
	ParmList pl(sScript);
	String s1 = pl.sGet(0);
	bool fRunInThread =  s1[0] == '*' ? false : true;
	if ( s1[0] == '*' )
		s1 = s1.substr(1);
	FileName fn(s1);
	Script scr(fn);
	if (scr.fValid()) {
		String sExpr;
		char* sTail = sScript.strchrQuoted(' ');
		if (sTail)
			sExpr = sTail + 1;
		if (pl.iFixed() == 1 && scr->iParams()) // no parms specified
		{
			ReroutSend(wndOwner, "run " + s1);
		}
		else {
			String sCurDir;
			if (pl.fExist("dir")) 
				sCurDir = pl.sGet("dir");
			else
				sCurDir = getEngine()->sGetCurDir();
			TextInput ti(sScript);
			TokenizerBase tkb(&ti);
			FileName fnScr(tkb.tokGet().sVal(), ".isl", true); // adds current path 
			String sScr("%S %S -dir=%S", fnScr.sFullNameQuoted(true), tkb.sCurrentLine(), sCurDir.sQuote());
			if ( fRunInThread)
				AfxBeginThread(CommandHandler::RunScriptInThread, (LPVOID)(new String(sScr))); 
			else
			{
				String sExpr;
				Script scr(fn);
				char* sTail = sScript.strchrQuoted(' ');
				if (sTail)
					sExpr = sTail + 1;
				scr->Exec(sExpr);
			}
			
		}
	}
}

void CommandHandler::ResetDir()
{
	String sDir = getEngine()->sGetCurDir();
	SetCurrentDirectory(sDir.c_str());
}


void CommandHandler::CmdDel(const String& sCmd)
{
	String sFn;
	if (sCmd == "") {
		String *sF;
		LRESULT res = ReroutSend(wndOwner, "del");
		if ( res == -1)
			return ;
		sF = (String *)res;
		sFn = *sF;
		delete sF;
	}
	else 
		sFn = sCmd;
	WinAndParm *wp = new WinAndParm;
	wp->handle = wndOwner->m_hWnd;
	wp->s = new String(sFn);
	wp->sPath = new String(getEngine()->sGetCurDir());
	AfxBeginThread(DeleteObjectInThread, (LPVOID)(wp));
}

UINT CommandHandler::DeleteObjectInThread(void * data)
{
	WinAndParm *wap = (WinAndParm *)data;
	CWnd *wnd = CWnd::FromHandle(wap->handle);
	getEngine()->InitThreadLocalVars();  
	getEngine()->SetCurDir(*(wap->sPath));
	String sCmd = *(wap->s);

	delete wap; // made in another thread
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	*fDoNotShowError = true;

	try
	{
		Tranquilizer trq;
		DeleteObjects(wnd, sCmd, &trq);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}

	getEngine()->RemoveThreadLocalVars();

	return 0;
}
  
// Read the domain name from the command line, or if it is not present
// determine it from the input fnDeafult
// Also read the domain type from the command line and update input/output dmtDefault
// if the domain type is not present on the command line dmt is unchanged by the function
static FileName DetermineDomainName(DomainType& dmtDefault, const FileName& fnDefault, ParmList& prml)
{
	DomainType dmtUser = Domain::dmt(prml.sGet("dmt"));
	if (dmtUser != dmtNONE)
		dmtDefault = dmtUser;

	FileName fnDom;
	if ((dmtDefault == dmtCLASS) || (dmtDefault == dmtID))
	{
		if (prml.fExist("dom"))
		{
			fnDom = prml.sGet("dom");
			fnDom.Dir(fnDefault.sPath());
		}  
		else // default (Wim 18/8/97)
			fnDom = FileName(fnDefault, ".dom", true);
	}
	return fnDom;
}

static void import14(const FileName& fnInp, const FileName& fnNew, ParmList& prml)
{
	bool fForce = false;
	if (prml.fExist("force"))
		fForce = prml.fGet("force");

	if (fCIStrEqual(fnInp.sExt, ".mpd"))
	{
		FileName fnOut(fnNew, ".mpr", true);
		if (!fForce && File::fExist(fnOut))
			return ;

		int iMapType, iScale;
		bool fInf, fCol, fGrfNone;
		FileName fnGrf;
		MapImport::GetImportInfo(fnInp, iMapType, iScale, fInf, fCol, fGrfNone, fnGrf, fnNew.sPath());
		DomainType dmt;
		if (iMapType == 0)
			dmt = dmtBIT;
		else if (fInf)
		{
			if (fCol)
				dmt = dmtCLASS;
			else if (iMapType == 2) 
				dmt = dmtID; 
			else 
				dmt = dmtCLASS;  
		}
		else
		{
			if (fCol)
				dmt = dmtPICTURE; 
			else if (iMapType == 1)
				dmt = dmtIMAGE; 
			else
				dmt = dmtVALUE;
		}
		FileName fnDom = DetermineDomainName(dmt, fnOut, prml);

		MapImport::import(fnInp, fnOut, dmt, fnDom, fnGrf, "");
	}
	else if (fCIStrEqual(fnInp.sExt, ".seg"))
	{
		FileName fnOut(fnNew, ".mps", true);
		if (!fForce && File::fExist(fnOut))
			return;

		DomainType dmt;
		ValueRange vr;
		ArrayLarge<String> asCodes;
		SegmentMapImport::GetImportInfo(fnInp, dmt, vr, asCodes);

		FileName fnDom = DetermineDomainName(dmt, fnOut, prml);

		SegmentMapImport::import(fnInp, fnOut, dmt, fnDom, vr, CoordSystem(), "");
	}
	else if (fCIStrEqual(fnInp.sExt, ".pol"))
	{
		FileName fnOut(fnNew, ".mpa", true);
		if (!fForce && File::fExist(fnOut))
			return ;

		DomainType dmt;
		ValueRange vr;
		Array<String> asCodes;
		PolygonMapImport::GetImportInfo(fnInp, dmt, vr, asCodes);

		FileName fnDom = DetermineDomainName(dmt, fnOut, prml);

		PolygonMapImport::import(fnInp, fnOut, dmt, fnDom, vr, CoordSystem(), "");
	}
	else if (fCIStrEqual(fnInp.sExt, ".pnt"))
	{
		FileName fnOut(fnNew, ".mpp", true);
		if (!fForce && File::fExist(fnOut))
			return ;

		bool fAttTablePossible, fNameCol;
		PointMapImport::GetImportInfo(fnInp, fNameCol, fAttTablePossible);
		FileName fnAttTable;
		if (fAttTablePossible)
			if (prml.fExist("att"))
			{
				fnAttTable = prml.sGet("att");
				fnAttTable.sExt = ".tbt";
				if (!fForce && File::fExist(fnAttTable))
					fnAttTable = FileName();
			}
			DomainType dmt = dmtNONE;
			if (fNameCol)
				dmt = dmtID;

			FileName fnDom = DetermineDomainName(dmt, fnOut, prml);
			if (dmt == dmtNONE) 
				return;

			PointMapImport::import(fnInp, fnOut, dmt, fnDom, CoordSystem(), fnAttTable, "");
	}
	else if (fCIStrEqual(fnInp.sExt, ".tbl"))
	{
		FileName fnOut(fnNew, ".tbt", true);
		if (!fForce && File::fExist(fnOut))
			return ;

		bool fNameCol;
		TableImport::GetImportInfo(fnInp, fNameCol);
		DomainType dmt = dmtNONE;
		if (fNameCol)
			dmt = dmtCLASS;

		FileName fnDom = DetermineDomainName(dmt, fnOut, prml);
		if ((dmt != dmtCLASS) && (dmt != dmtID) && (dmt != dmtNONE))
			return;

		TableImport::import(fnInp, fnOut, ".tbt", dmt, fnDom, "");
	}
}


// format 1: import a single data file:
//    import14  inpdata14 [outdata] [-force] [-dmt=domtype] [-dom=domname] [-grf=grfname]
// were:
//    if outdata parm is not there, name  inpdata14 is taken
//    -force flag will overwrite existing data
//
// format 2: import more than one data file (use wildcard mask)
//    import14 inpmask [outdir] [-force]
// were:
//    if outdir parm is not there, current dir is taken
//    -force flag will overwrite existing data
//
void CommandHandler::CmdImport14(const String& sCmd)
{
	// sCmd is command tail
	// Do not display the import form when no arguments are specified
	// User should specify Import14 command fully
	if (sCmd.length() != 0)
	{
		ParmList prml(sCmd);
		Array<FileName> afn;
		Array<String> asExt;
		asExt &= ".mpd";  // only allow raster import
		String sInpMask = prml.sGet(0);
		// Check if the command specifies wildcards
		if (sInpMask.find('*') == String::npos && sInpMask.find('?') == String::npos)
		{
			// Handle case without wildcards
			FileName fn14(sInpMask);
			FileName fnNew = prml.sGet(1);
			if (!fnNew.fValid()) 
				fnNew = fn14;

			import14(fn14, fnNew, prml);

			return;
		}

		// Handle case with wildcards
		File::GetFileNames(sInpMask, afn, &asExt);  
		if (afn.iSize() == 0)  // if there are no .MPD files we are done 
			return;
		
		String sOutDir = prml.sGet(1);
		for (unsigned int i=0; i < afn.iSize(); ++i)
		{
			try
			{
				FileName fnOut = afn[i];
				if (sOutDir.length() != 0)
					fnOut.Dir(sOutDir);

				import14(afn[i], fnOut, prml);
			}
			catch (ErrorObject& err) 
			{
				err.Show();
			}
		}
	}
}

void CommandHandler::CmdConvert14(const String& sCmd)
{
	throw ErrorObject(String(SMSErrOperationNotAvailable_s.scVal(), "convert14"));
}

void CommandHandler::CmdSetDescr(const String& sCmd)
{
	ParmList pl(sCmd);	
	if ( pl.iFixed() < 2 )
		throw ErrorObject(SMSErrNotEnoughParameters);
	String sFile = pl.sGet(0);
	String sRest = sCmd.substr(sFile.size(), sCmd.size());
	IlwisObject object = IlwisObject::obj(pl.sGet(0));
	size_t iPos;
	if ( (iPos = sRest.find("-quiet", sRest.size() - 8)) != string::npos)
		sRest = sRest.substr(0, iPos);
	object->SetDescription(sRest);
	object->Updated();
}


UINT CommandHandler::CreatePyrInThread(LPVOID p)
{
	String *str = (String *)p;
	getEngine()->InitThreadLocalVars();
	CreatePyramidFiles(*str);
	getEngine()->RemoveThreadLocalVars();
	delete str;
	return 1;
}

void CommandHandler::CreatePyramidFiles(const String& str)
{
	String sCommand = str;
	if ( sCommand == "" )
		sCommand = "*.mpr";
	ParmList pl(str);
	if (pl.iFixed() == 0)
	{
		Parm *pm = new Parm(0, String("*.mpr"));
		pl.Add(pm);
	}
	for (int iFile=0; iFile < pl.iFixed(); ++iFile)
	{
		CFileFind finder;		
		FileName fn(pl.sGet(iFile), ".mpr", false);

		BOOL fFound = finder.FindFile(fn.sFullName().scVal());
		while ( fFound )
		{
			fFound = finder.FindNextFile();
			FileName fn(finder.GetFileName());
			if (finder.IsDirectory())
				continue;
			if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotRASMAP )
			{
				Map mp(fn);
				if ( mp->fReadOnly() == false )
					mp->CreatePyramidLayer();
			}
			else
			{
				if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotMAPLIST )
				{
					MapList ml(fn);
					if ( ml->fReadOnly() == false)
						ml->CreatePyramidLayers();
				}
			}
		}
	}
}

void CommandHandler::CmdCreatePyramidFiles(const String& sCmd)
{
	CWaitCursor cur;
	CreatePyramidFiles(sCmd);
}

void CommandHandler::CmdDeletePyramidFiles(const String& sCmd)
{
	String sCommand = sCmd;
	if ( sCommand == "" )
		sCommand = "*.mpr";
	ParmList pl(sCommand);
	if (pl.iFixed() == 0)
	{
		Parm *pm = new Parm(0, String("*.mpr"));
		pl.Add(pm);
	}
	Tranquilizer trq;
	for (int iFile=0; iFile < pl.iFixed(); ++iFile)
	{
		CFileFind finder;
		FileName fn(pl.sGet(iFile), ".mpr", false);

		BOOL fFound = finder.FindFile(fn.sFullName().scVal());
		while ( fFound )
		{
			fFound = finder.FindNextFile();
			FileName fn(finder.GetFileName());
			if (finder.IsDirectory())
				continue;
			if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotRASMAP )
			{
				Map mp(fn);
				if ( mp->fReadOnly() == false )
					mp->DeletePyramidFile();
			}
			else
			{
				if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotMAPLIST )
				{
					MapList ml(fn);
					if ( ml->fReadOnly() == false)
						ml->DeletePyramidFiles();
				}
			}
		}
	}
}

void CommandHandler::CmdTestingDBConnection(const String& sCmd)
{
	getEngine()->PostMessage(WM_COMMAND, ID_FILE_IMPORT_CONNECT_DATABASE);

}

void BaseCommandHandler::ReroutPost(const String& s) {
	if ( wndOwner != 0) {
		size_t len = s.size();
		char * str = new char[len + 1];
		strcpy(str, s.scVal());
		getEngine()->PostMessage(ILWM_CMDHANDLERUI, 0, (LPARAM)str);
	}
}

LRESULT BaseCommandHandler::ReroutSend(CWnd *owner, String s) {
	if ( owner != 0) {
		size_t len = s.size();
		char * str = new char[len + 1];
		strcpy(str, s.scVal());
		return getEngine()->SendMessage(ILWM_CMDHANDLERUI, 0, (LPARAM)str);
	}
	return 0;
}

static void SortDomainError(const String& sDomain)
{
	throw ErrorSortDomain(sDomain, FileName(), 0);
}

static void NotDomainGroupError(const String& sDomain)
{
	throw ErrorObject(
		WhatError(String(SDATErrDomainGroupExpected_S.scVal(), sDomain), 0), 
		FileName());
}

void CommandHandler::CmdDelFile(const String& sFiles)
{
	Array<FileName> afn;
	File::GetFileNames(sFiles, afn);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
		_unlink(afn[i].sFullName(true).scVal());
	UpdateCatalog();
}

void CommandHandler::CmdDelColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	Table tbl(col->fnTbl);
	col = tbl->col(col->sName());
	if (!col->fReadOnly() && !col->fOwnedByTable()) 
	{
		tbl->LoadData();
		col->fErase = true;
		tbl->RemoveCol(col);
	}
}

void CommandHandler::CmdCalcColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	col->Calc();
}

void CommandHandler::CmdUpdateColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	col->MakeUpToDate();
}

void CommandHandler::CmdBreakDepColumn(const String& sCol)
{
  Column col = Column(sCol);
  col->BreakDependency();
}

void CommandHandler::CmdDomClassToID(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomClassToIDInThread, (LPVOID)(s));
}

void CommandHandler::CmdDomIDToClass(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomIDToClassInThread, (LPVOID)(s));
}

void CommandHandler::CmdDomPicToClass(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomPicToClassInThread, (LPVOID)(s));
}

UINT CommandHandler::CmdDomClassToIDInThread(void *p)
{
	getEngine()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainSort *pdsrt = dm->pdsrt();
		if (0 == pdsrt)
			return 0;

		dm.ClassToIdent();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		

	getEngine()->RemoveThreadLocalVars();

	return 0;
}

UINT CommandHandler::CmdDomPicToClassInThread(void *p)
{
	getEngine()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainPicture *pdp = dm->pdp();
		if (0 == pdp)
			return 0;

		dm.PictureToClass();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		
	
	getEngine()->RemoveThreadLocalVars();

	return 0;
}

UINT CommandHandler::CmdDomIDToClassInThread(void *p)
{
	getEngine()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainSort *pdsrt = dm->pdsrt();
		if (0 == pdsrt)
			return 0;

		dm.IdentToClass();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		

	getEngine()->RemoveThreadLocalVars();

	return 0;
}

void CommandHandler::CmdSetGrf(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; 
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	GeoRef gr(pm.sGet(1));
	for (unsigned int i=0; i < afn.iSize(); ++i)
	{
		Map mp(afn[i]);
		if (!mp->fReadOnly())
		{
			mp->SetGeoRef(gr);
			if (!gr->fGeoRefNone())
				mp->SetSize(gr->rcSize());
		}
	}
}

void CommandHandler::CmdSetCsy(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".grf"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".mpr";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	CoordSystem csy(pm.sGet(1));
	String sMapExtensions(".mpr.mpp.mps.mpa");
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly())
				mp->SetCoordSystem(csy);
		}
		else if (fCIStrEqual(sExt, ".grf"))
		{
			GeoRef grf(fn);
			if (!grf->fReadOnly())
				grf->SetCoordSystem(csy);
		}
	}
}

void CommandHandler::CmdSetAttTable(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	asExt &= ".dom";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	String sMapExtensions(".mpr.mpp.mps.mpa");
	if (pm.fExist(1))
	{
		Table tblAtt(pm.sGet(1));
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				if (!mp->fReadOnly())
					mp->SetAttributeTable(tblAtt);
			}
			else if (fCIStrEqual(sExt, ".dom")) {
				Domain dm(fn);
				if (!dm->fReadOnly())
				{
					DomainSort* pdsrt = dm->pdsrt();
					if (0 != pdsrt)
						pdsrt->SetAttributeTable(tblAtt);
				}
			}
		}
	}
	else
	{
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				if (!mp->fReadOnly())
					mp->SetNoAttributeTable();
			}
			else if (fCIStrEqual(sExt, ".dom"))
			{
				Domain dm(fn);
				if (!dm->fReadOnly()) {
					DomainSort* pdsrt = dm->pdsrt();
					if (0 != pdsrt)
						pdsrt->SetNoAttributeTable();
				}
			}
		}
	}
}

void CommandHandler::CmdSetDom(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	Domain dm(pm.sGet(1));
	ValueRange vr;
	if (pm.iFixed() == 3)
		vr = ValueRange(pm.sGet(2));
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");

	String sMapExtensions(".mpr.mpp.mps.mpa");
	if (fForce)
	{
		for (unsigned int i=0; i < afn.iSize(); ++i)
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				ObjectInfo::WriteElement("BaseMap", "Domain", fn, dm->sName(true, fn.sPath()));
				if (vr.fValid())
					ObjectInfo::WriteElement("BaseMap", "ValueRange", fn, vr->sRange(true));
			}
			else if (fCIStrEqual(fn.sExt, ".tbt"))
				ObjectInfo::WriteElement("Table", "Domain", fn, dm->sName(true, fn.sPath()));
		}  
		return;
	}

	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly())
			{
				if (!mp->fDependent() || mp->fDomainChangeable() )
					mp->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
			}
		}
	}
}

void CommandHandler::CmdSetValRange(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	ValueRange vr(pm.sGet(1));

	String sMapExtensions(".mpr.mpp.mps.mpa");
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly()) {
				if (!mp->fDependent() || mp->fDomainChangeable() )
					mp->SetValueRange(vr);
			}
		}
	}
}

void CommandHandler::CmdChangeDom(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(1));
	ValueRange vr;
	if (pm.iFixed() == 3)
		vr = ValueRange(pm.sGet(2));
	Column colAtt;
	if (pm.fExist("colatt"))
		colAtt = Column(pm.sGet("colatt"));
	if (!pm.fExist("col"))
	{
		Array<FileName> afn;
		Array<String> asExt;
		asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
		File::GetFileNames(pm.sGet(0), afn, &asExt);
		String sMapExtensions(".mpr.mpp.mps.mpa");
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				mp->fConvertTo(DomainValueRangeStruct(dm, vr), colAtt);
			}
		}
	}
	else {
		Column col(pm.sGet("col"));
		if (col.fValid())
			col->fConvertTo(DomainValueRangeStruct(dm, vr), colAtt);
	}  
}

void CommandHandler::CmdMergeDom(const String& s)
{
	ParmList pm(s);
	try
	{
		Domain dm1(pm.sGet(0));
		if (dm1->fReadOnly())
			return; // warning ??
		Domain dm2(pm.sGet(1));
		DomainSort* pdsrt1 = dm1->pdsrt();
		DomainSort* pdsrt2 = dm2->pdsrt();
		if (0 == pdsrt1)
			SortDomainError(dm1->sName(true));
		if (0 == pdsrt2)
			SortDomainError(dm2->sName(true));
		pdsrt1->Merge(pdsrt2);
	}
	catch (const ErrorObject&) {
		return;
	}
}

void CommandHandler::CmdAddItemToDomain(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(0));
	if (dm->fReadOnly())
		return; // warning ??
	DomainSort* pdsrt = dm->pdsrt();
	if (0 == pdsrt)
		SortDomainError(dm->sName(true));
	String sName = pm.sGet(1);
	// remove " or '
	if ((sName[0] == '\"') ||(sName[0] == '\''))
		sName = sName.sRight(sName.length()-1);
	if ((sName[sName.length()-1] == '\"') || (sName[sName.length()-1] == '\''))
		sName = sName.sLeft(sName.length()-1);
	String sCode;
	if (pm.fExist(2))
		sCode = pm.sGet(2);
	if ( sCode.length() > 0) {
		if ((sCode[0] == '\"') ||(sCode[0] == '\''))
			sCode = sCode.sRight(sCode.length()-1);
		if ((sCode[sCode.length()-1] == '\"') || (sCode[sCode.length()-1] == '\''))
			sCode = sCode.sLeft(sCode.length()-1);
	}
	long iNew = pdsrt->iAdd(sName);
	if (pm.fExist(2) || (sCode.length() != 0))
		pdsrt->SetCode(iNew, sCode);
}

void CommandHandler::CmdAddItemToDomainGroup(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(0));
	if (dm->fReadOnly())
		return; // warning ??
	DomainGroup* pdgrp = dm->pdgrp();
	if (0 == pdgrp)
		NotDomainGroupError(dm->sName(true));
	double rLim = pm.sGet(1).rVal();
	String sName = pm.sGet(2);
	// remove " or '
	if ((sName[0] == '\"') ||(sName[0] == '\''))
		sName = sName.sRight(sName.length()-1);
	if ((sName[sName.length()-1] == '\"') || (sName[sName.length()-1] == '\''))
		sName = sName.sLeft(sName.length()-1);
	String sCode;
	if (pm.fExist(3))
		sCode = pm.sGet(3);
	if ((sCode[0] == '\"') ||(sCode[0] == '\''))
		sCode = sCode.sRight(sCode.length()-1);
	if ((sCode[sCode.length()-1] == '\"') || (sCode[sCode.length()-1] == '\''))
		sCode = sCode.sLeft(sCode.length()-1);
	long iNew = pdgrp->iAdd(sName);
	if (pm.fExist(3) || (sCode.length() != 0))
		pdgrp->SetCode(iNew, sCode);
	pdgrp->SetUpperBound(iNew, rLim);
}

void CommandHandler::CmdRenameObject(const String& s)
{
	ParmList pm(s);
	if (pm.iFixed() != 2)
		return;
	FileName fnObject(pm.sGet(0));
	FileName fnNew(pm.sGet(1));
	IlwisObject obj = IlwisObject::obj(fnObject);
	obj->Rename(fnNew);
}  
  
void CommandHandler::CmdCreateTable(const String& s)
{
	if ( s == "" ) return;
	ParmList pm(s);
	FileName fnTbl(pm.sGet(0), ".tbt", true);
	if (File::fExist(fnTbl))
		if (!DeleteFile(fnTbl.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnTbl.sRelative()));
	String sDom = pm.sGet(1);
	if ( sDom == "")
		return;
	long iNr = sDom.iVal();
	if (iNr > 0) {
		Domain dm("None");
		Table tbl(fnTbl, dm);
		tbl->iRecNew(iNr);
	}
	else {
		Domain dm(sDom);
		Table tbl(fnTbl, dm);
	}
}

void CommandHandler::CmdCreateMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mpr", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	String sGrf = pm.sGet(1);
	String sDom = pm.sGet(2);
	GeoRef grf(sGrf);
	Domain dm(sDom);
	Map map(fnMap, grf, grf->rcSize(), dm);
	map->FillWithUndef();
}

void CommandHandler::CmdCrMapList(const String& sCmd)
{
	ParmList pm(sCmd);
	FileName fnMapList(pm.sGet(0), ".mpl", true);
	if (File::fExist(fnMapList))
		return;
	Array<FileName> arfnMaps;
	for(int i=1; i < pm.iFixed(); ++i) 
	{
		arfnMaps &= FileName(pm.sGet(i));
	}		
	MapList mpl(fnMapList, arfnMaps);
}

void CommandHandler::CmdCreatePointMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mpp", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	Domain dm;
	CoordSystem csy;
	
	if (pm.iFixed() < 4)
	{
		CoordSystem csy(pm.sGet(1));
		dm = Domain (pm.sGet(2));
	}
	else
	{
		String sCsy = pm.sGet(1);		
		double rXT = pm.sGet(2).rVal();
		double rYL = pm.sGet(3).rVal();
		double rXR = pm.sGet(4).rVal();
		double rYB = pm.sGet(5).rVal();		
		dm = Domain(pm.sGet(6));		
		csy = CoordSystem(sCsy);
		csy->cb = CoordBounds(Coord(rXT, rYL), Coord(rXR, rYB));
	}	
	PointMap map(fnMap, csy, csy->cb, dm);	
}

void CommandHandler::CmdCreateSegMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mps", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	Domain dm;
	CoordSystem csy;
	
	if (pm.iFixed() < 4)
	{
		CoordSystem csy(pm.sGet(1));
		dm = Domain (pm.sGet(2));
	}
	else
	{
		String sCsy = pm.sGet(1);		
		double rXT = pm.sGet(2).rVal();
		double rYL = pm.sGet(3).rVal();
		double rXR = pm.sGet(4).rVal();
		double rYB = pm.sGet(5).rVal();		
		dm = Domain(pm.sGet(6));		
		csy = CoordSystem(sCsy);
		csy->cb = CoordBounds(Coord(rXT, rYL), Coord(rXR, rYB));
	}
	SegmentMap map(fnMap, csy, csy->cb, dm);			
}

void CommandHandler::CmdCreateDom(const String& s)
{
	ParmList pm(s);
	FileName fnDom(pm.sGet(0), ".dom", true);
	if (File::fExist(fnDom))
		if (!DeleteFile(fnDom.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnDom.sRelative()));
	String sType = pm.sGet("type");
	DomainType dmt = dmtNONE;
	if ((fCIStrEqual(sType , "class")) || (sType == "")) 
		dmt = dmtCLASS;
	else if (fCIStrEqual(sType , "id"))
		dmt = dmtID;
	else if (fCIStrEqual(sType , "group"))
		dmt = dmtGROUP;
	else if (fCIStrEqual(sType , "value"))
		dmt = dmtVALUE;
	switch (dmt) 
	{
		case dmtVALUE: 
		{
			double rMin = pm.sGet("min").rVal();
			double rMax = pm.sGet("max").rVal();
			double rStep;
			String s = pm.sGet("prec");
			if (s.length() == 0)
				rStep = 1;
			else
				rStep = s.rVal();
			if (rStep < 0)
				rStep = 0;
			Domain(fnDom, rMin, rMax, rStep);
		}
		break;
		case dmtCLASS: 
		case dmtID: 
		{
			String sPrefix;
			sPrefix = pm.sGet("prefix");
			if (sPrefix.length() == 0) {
				if (dmt == dmtID)
					sPrefix = "id";
				else
					sPrefix = "class";
			}
			String s = pm.sGet("items");
			long iNr = s.iVal();
			if (iNr <= 0)
				iNr = 0;
			Domain(fnDom, iNr, dmt, sPrefix);
		}
		break;
		case dmtGROUP: 
		case dmtPICTURE: 
		{
			String s = pm.sGet("items");
			long iNr = s.iVal();
			if (iNr <= 0)
				iNr = 0;
			Domain(fnDom, iNr, dmt);
		}
		break;
		default :
			return;
	}
}

void CommandHandler::CmdCreateRpr(const String& s)
{
	ParmList pm(s);
	FileName fnRpr(pm.sGet(0), ".rpr", true);
	if (File::fExist(fnRpr))
		if (!DeleteFile(fnRpr.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnRpr.sRelative()));
	Domain dm(pm.sGet(1));
	Representation(fnRpr, dm);
}

void CommandHandler::CmdCreateGrf(const String& s)
{
	ParmList pm(s);
	FileName fnGrf(pm.sGet(0), ".grf", true);
	if (File::fExist(fnGrf))
		if (!DeleteFile(fnGrf.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnGrf.sRelative()));
	GeoRef gr;
	RowCol rc;
	rc.Row = pm.sGet(1).iVal();
	rc.Col = pm.sGet(2).iVal();
	String sType = pm.sGet("type");
	CoordSystem cs;
	//  if (pm.fGet("crdsys"))
	String sCsys = pm.sGet("crdsys");
	if (sCsys.length() > 0)
		cs = CoordSystem(sCsys);
	if ((fCIStrEqual(sType , "corners")) || (sType == "")) {
		bool fCoC = pm.fGet("centercorners");
		double rMinX=0, rMinY=0, rMaxX=0, rMaxY=0;
		if (cs.fValid()) {
			rMinX = cs->cb.MinX();
			rMinY = cs->cb.MinY();
			rMaxX = cs->cb.MaxX();
			rMaxY = cs->cb.MaxY();
		}
		String s = pm.sGet("lowleft");
		if (s.length() > 0) {
			char* str = s.sVal();
			if (str[0] == '(')
				str++;
			if (2 != sscanf(str, "%lg,%lg", &rMinX, &rMinY))
				return;
		}
		s = pm.sGet("upright");
		if (s.length() > 0) {
			char* str = s.sVal();
			if (str[0] == '(')
				str++;
			if (2 != sscanf(str, "%lg,%lg", &rMaxX, &rMaxY))
				return;
		}
		else {
			s = pm.sGet("pixsize");
			if (s.length() > 0) {
				double rPixSize = s.rVal();
				if (rPixSize < 0)
					rPixSize = 0;
				rMaxX = rMinX + (rc.Col - (int)fCoC) * rPixSize;
				rMaxY = rMinY + (rc.Row - (int)fCoC) * rPixSize;
			}
		}
		gr.SetPointer(new GeoRefCorners(fnGrf, cs, rc, !fCoC, Coord(rMinX, rMinY), Coord(rMaxX, rMaxY)));
	}
}


void CommandHandler::CmdCreate2DimTable(const String& s)
{
	ParmList pm(s);
	String sDom1 = pm.sGet(1);
	String sDom2 = pm.sGet(2);
	Domain dm1(sDom1);
	Domain dm2(sDom2);
	Domain dmVal(pm.sGet(3));
	ValueRange vr;
	if (pm.fExist(4))
		vr = ValueRange(pm.sGet(4));
	Table2Dim tbl2dim(FileName(pm.sGet(0)), dm1, dm2, DomainValueRangeStruct(dmVal, vr));
}

void CommandHandler::CalcObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".his"; asExt &= ".hsp"; asExt &= ".hss"; asExt &= ".hsa";
	asExt &= ".mpl"; asExt &= ".mat"; asExt &= ".stp";
	File::GetFileNames(sFileMask, afn, &asExt);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if (ObjectInfo::fDependent(fn)) {
			String sExt = fn.sExt;
			if ((0 != trq) && (afn.iSize() > 1)) {
				String sCalc("calc %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			IlwisObject obj = IlwisObject::obj(fn);
			if (obj.fValid())
				obj->Calc();
		}
	}
}

void CommandHandler::DeleteObjects(CWnd *owner, const String& sCommand, Tranquilizer* trq)
{
	class Prot
	{
	public:
		Prot()
		{
			bool *fDoNotShowError = (bool*)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			*fDoNotShowError = true;
			bool *fDoNotLoadGDB = (bool*)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
			*fDoNotLoadGDB = true;	
		}
		~Prot()
		{
			bool* fDoNotShowError = (bool*)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			*fDoNotShowError = false;
			bool* fDoNotLoadGDB = (bool*)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
			*fDoNotLoadGDB = false;								
		}
	};

	try
	{
		Prot prot;

		ObjectStructure *osStruct = NULL;
		LRESULT result = ReroutSend(owner, "deleteobjects " + sCommand + " -noassociated");
		if ( result != -1)
			osStruct = (ObjectStructure *)result;
		if (osStruct != 0)
		{
			// get the files from osStruct into lstFiles
			list<String> lstFiles;
			osStruct->GetUsedFiles(lstFiles, false);
			FileName fnUpdate;
			// loop through all files in lstFiles and delete them
			for (list<String>::iterator cur = lstFiles.begin(); cur != lstFiles.end(); ++cur)
			{
				FileName fn(*cur);
				if ( fnUpdate == FileName() ) // used to determine the directory that gets the update message;
					fnUpdate = fn;
				
				// Check the existance of the file to remove.
				// The list of files to delete can contain duplicates
				if (File::fExist(fn))
				{
					// if the file to remove is an object collection, make sure
					// that all containing object remove their reference to the collection
					if (IlwisObject::iotObjectType(fn) == IlwisObject::iotOBJECTCOLLECTION)
					{
						try
						{
							ObjectCollection oc(fn);
							if (oc.fValid())
								oc->RemoveAllObjects();
						}
						catch (...)
						{
							// do nothing
						}
					}

					ObjectInfo::RemoveFileFromLinkedCollections(fn);
					DWORD iAttrib = GetFileAttributes( fn.sFullPath().scVal() );
					SetFileAttributes( fn.sFullPath().scVal(), iAttrib & ~FILE_ATTRIBUTE_READONLY );
					BOOL fRet = DeleteFile(fn.sFullPath().scVal());
				}
			}
			if (lstFiles.size() > 0)
			{
				FileName *fnUpd = new FileName(fnUpdate.sPath());
				UpdateCatalog(fnUpd);
			}
		}
		delete osStruct;
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
}

void CommandHandler::BreakDepObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".mat"; asExt &= ".mpl"; asExt &= ".stp"; asExt&=".ioc";
	File::GetFileNames(sFileMask, afn, &asExt);
	if (fForce)
	{
		for (unsigned int i=0; i < afn.iSize(); ++i)
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt;
			if ((0 != trq) && (afn.iSize() > 1))
			{
				String sCalc("breakdep %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			
			if (fCIStrEqual(fn.sExt, ".mpr"))
				ObjectInfo::WriteElement("Map", "Type", fn, "MapStore");
			else if (fCIStrEqual(fn.sExt, ".mps"))
				ObjectInfo::WriteElement("SegmentMap", "Type", fn, "SegmentMapStore");
			else if (fCIStrEqual(fn.sExt, ".mpa"))
				ObjectInfo::WriteElement("PolygonMap", "Type", fn, "PolygonMapStore");
			else if (fCIStrEqual(fn.sExt, ".mpp"))
				ObjectInfo::WriteElement("PointMap", "Type", fn, "PointMapStore");
		}
		return;
	}
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if ((0 != trq) && (afn.iSize() > 1)) {
			String sCalc("breakdep %S (%S%S)", sFileMask, fn.sFile, sExt);
			if (trq->fText(sCalc))
				return;
		}
		if (ObjectInfo::fDependent(fn)) 
		{
			IlwisObject obj = IlwisObject::obj(fn);
			if (!obj->fReadOnly())
				obj->BreakDependency();
		}
	}
}

void CommandHandler::UpdateObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".mpl"; asExt &= ".mat"; asExt &= ".stp";
	File::GetFileNames(sFileMask, afn, &asExt);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if (ObjectInfo::fDependent(fn)) 
		{
			if ((0 != trq) && (afn.iSize() > 1)) {
				String sCalc("update %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			IlwisObject obj = IlwisObject::obj(fn);
			if (obj.fValid())
				obj->MakeUpToDate();
		}
	}
}


void CommandHandler::CmdSetReadOnly(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	IlwisObject::GetAllObjectExtensions(asExt);
	File::GetFileNames(pm.sGet(0), afn, &asExt);  
	for (unsigned int i=0; i < afn.iSize(); ++i) {
		IlwisObject obj = IlwisObject::obj(afn[i]);
		obj->SetReadOnly(true);
	}  
}

void CommandHandler::CmdSetReadWrite(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	IlwisObject::GetAllObjectExtensions(asExt);
	File::GetFileNames(pm.sGet(0), afn, &asExt);  
	for (unsigned int i=0; i < afn.iSize(); ++i) {
		IlwisObject obj = IlwisObject::obj(afn[i]);
		obj->SetReadOnly(false);
	}  
}

void CommandHandler::CmdCloseAll(const String&)
{
	getEngine()->PostMessage(WM_COMMAND, ID_CLOSEALL, 0);
}

void CommandHandler::CmdTabCalc(const String& sCmd)
{
	TextInput ip(sCmd);
	TokenizerBase tkn(&ip);
	tkn.SetComposed(sComposed);
	Token tok = tkn.tokGet();
	FileName fnObject(tok.sVal());
	Table tbl(FileName(fnObject.sFullNameQuoted(true), ".tbt", false));
	tok = tkn.tokGet();
	String sCol = tok.sVal();
	tok = tkn.tokGet();
	if (tok == "[") {
		Calculator calc(tbl, String("%S[%S",sCol,tkn.sCurrentLine()));
		calc.ExecColumnIndexStore();
		return;
	}
	Domain dm;
	ValueRange vr;
	if (tok == "{")
		if (!ScriptPtr::fScanDomainValueRange(tok, tkn/**tkn.tknb*/, dm, vr))
			return;
	bool fBreakDep = tok == ":=";
	String sRestOfLine = tkn.sCurrentLine();
	tbl->LoadData();
	Column col(tbl, sCol, sRestOfLine);
	if (!col->fDependent())
		return;
	if (dm.fValid() && col->fDomainChangeable())
		col->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
	else if (col->fValueRangeChangeable() && vr.fValid())
		col->SetValueRange(vr);
	col->Calc();
	col->CalcMinMax();  // on special request of Jelle in tblwind.c, so why not here?
	if (fBreakDep)
		col->BreakDependency();
}

UINT CommandHandler::CmdCopyFileInThread(void *p)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	WinAndParm* wp = (WinAndParm*)(p);
	getEngine()->getContext()->InitThreadLocalVars();
	getEngine()->getContext()->SetCurDir(*(wp->sPath));
	String s(wp->s->scVal());

	delete wp;

	try
	{
    Tranquilizer trq;
    // trq.SetTitle(String(SMAPTextImportRasterFrom14.scVal(), fn.sFile));
    trq.Start();
		CopyFiles(s, &trq);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	getEngine()->getContext()->RemoveThreadLocalVars();
	return 1;
}

void CommandHandler::Cmdcopy(const String& sPar)
{
	WinAndParm* wp = new WinAndParm;
	wp->handle = NULL;
	wp->s = new String(sPar);
	wp->sPath = new String(getEngine()->getContext()->sGetCurDir());
	AfxBeginThread(CommandHandler::CmdCopyFileInThread, (LPVOID)(wp));

}

void CommandHandler::CopyObjects(const String& sCommand, Tranquilizer* trq, CWnd* wnd, bool fOnlyDirectory)
{
	ParmList pm(sCommand);
	if ( pm.iFixed() < 1 )
		return;
	bool fBreakDep = pm.fGet("breakdep");
	bool fQuiet = pm.fGet("quiet");
	String sTo = pm.sGet(pm.iFixed()-1);
	FileName fnTo = FileName(sTo);
	vector<FileName> afnFiles(pm.iFixed()-1);
	for (int iParm = 0; iParm < pm.iFixed()-1; iParm++) 
		afnFiles[iParm] = FileName(pm.sGet(iParm));
	
	if ( afnFiles.size() == 0 )
		throw ErrorObject(SMSErrorNoFilesToCopy);
	
	if ( fnTo.sExt == "" && Directory::fIsExistingDir( Directory(sTo)))
	{
		ObjectCopier copier(afnFiles, Directory(sTo));
		copier.Copy(fBreakDep);
	}
	else
	{
		if ( fnTo.sExt == "")
			fnTo.sExt = afnFiles[0].sExt;
		ObjectCopier copier( afnFiles[0], fnTo);
		copier.Copy(fBreakDep); 
	}	 

}

void CommandHandler::CopyFiles(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sTo = pm.sGet(1);
	FileName fnTo = FileName(sTo);
	if (fnTo.sExt == "" && Directory::fIsExistingDir( Directory(sTo)))
	{ // copy to other subdir
		String sFileMask = pm.sGet(0);
		String sDirFrom = FileName(sFileMask).sPath();
		String sDirTo = fnTo.sFullPath();
		if (sDirTo.length() == 0)
			sDirTo = fnTo.sPath();
		if (sDirTo.length() == 0)
			sDirTo = sTo;
		for (FilenameIter iter(pm.sGet(0)); iter.fValid(); ++iter)
		{
			String sFile("%S%s", sDirFrom, (*iter).c_str());
			FileName fnFrom = FileName(sFile);
			FileName fnTo = fnFrom;
			fnTo.Dir(sDirTo);
			if (!File::fCopy(fnFrom, fnTo, *trq))
				break;
		}
	}
	else  // copy in current dir
	{
		FileName fnFrom(pm.sGet(0));
		FileName fnTo = fnFrom;
		fnTo.sFile = pm.sGet(1).sUnQuote();
		File::Copy(fnFrom, fnTo);
	}
	UpdateCatalog(NULL);
}

void CommandHandler::CreateMap(const String& sDom, const String& sGrf)
{
	ReroutPost("createmap "+ sDom + "," + sGrf);
}

void CommandHandler::CreateSeg(const String& sDomain, const String& sCsys)
{
	ReroutPost("createseg "+ sDomain + "," + sCsys);

}

void CommandHandler::CreatePol(const String& sDomain, const String& sCsys)
{
	ReroutPost("createpol "+ sDomain + "," + sCsys);

}

void CommandHandler::CreatePnt(const String& sDomain, const String& sCsys)
{
	ReroutPost("createpnt "+ sDomain + "," + sCsys);
}

void CommandHandler::CreateTbl(const String& sDomain)
{
	ReroutPost("createtbl " + sDomain);
}

void CommandHandler::CreateTb2(const String& sDomain)
{
	ReroutPost("createtb2 " + sDomain);
}

void CommandHandler::CreateDom()
{
	ReroutPost("createdom");
}

void CommandHandler::CreateRpr(const String& sDomain)
{
	ReroutPost("createrpr " + sDomain);
}

void CommandHandler::CreateCsy()
{
	ReroutPost("createcsy");
}

void CommandHandler::CreateFil()
{
	ReroutPost("createfilter");
}

void CommandHandler::CreateIsl()
{
		ReroutPost("createisl ");
}

void CommandHandler::CreateFun()
{
	ReroutPost("createfun ");
}

void CommandHandler::CreateMpl()
{
	ReroutPost("creatempl ");
}

void CommandHandler::CreateSms(const String& sMpr, const String& sMpl)
{
	ReroutPost("createsms " + sMpr + "," + sMpl);
}

void CommandHandler::CreateGrf(const String& sCsy)
{
	ReroutPost("creategrf " + sCsy);
}

void CommandHandler::CreateIoc(const String& sN)
{
	ReroutPost("createioc " + sN);
}

void CommandHandler::CmdAppMetaData(const String& sN)
{
	String appName = sN.sHead(" ");
	String output = sN.sTail(" ");
	ApplicationInfo *info = getEngine()->modules.getAppInfo(appName);
	if ( info == NULL) 
		return;
   if ( info->metadata != NULL) {
		ApplicationQueryData query;
		query.queryType = "WPSMETADATA";
		ApplicationMetadata amd = (info->metadata)(&query);
		if ( amd.wpsxml == "")
			return;
		String xml = amd.wpsxml;
		if ( output == "")
			ReroutPost(String("simplecalc Text %S",xml));
		else {
			if ( output.toLower() != "stdout" && output.toLower() != "stdout -quiet") {
				File file(FileName(output),facCRT);
				file.Write(xml);
			} else {
				std::cout << xml;
			}
		}
	}

}






