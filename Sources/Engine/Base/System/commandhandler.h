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
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#pragma warning( disable : 4786 )

#include "Engine\Base\System\LOGGER.H"

class BaseCommandHandler;
class CommandHandler;
class Tranquilizer;

typedef void (BaseCommandHandler::*CommandFunction)(const String&);
typedef void (*AdditionalCommand)(const String& expression);
typedef map<string, CommandFunction> CommandMap;
typedef CommandMap::iterator CommandIter;
typedef pair<string, CommandFunction> CommandPair;

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif




class IMPEXP BaseCommandHandler
{
protected:
	BaseCommandHandler();
	void ReroutPost(const String& s);
	static LRESULT ReroutSend(CWnd *owner, String s);
	CommandMap commands;
	map<String, AdditionalCommand> additionalCommands;
	CFrameWnd *wndOwner;
public:
  virtual LRESULT fExecute(const String& sCmd);
  		void AddCommand(const String& cmd, AdditionalCommand);
	virtual ~BaseCommandHandler();
  void SetOwner(CFrameWnd*);
private:
  void CmdExit(const String& s);
  void CmdHelp(const String& s);
  void executeAdditional(const String& sCmd);
};

class CommandHandler: public BaseCommandHandler
{
public:
	CommandHandler();
	~CommandHandler();
	
	virtual LRESULT _export fExecute(const String& s);
	
	static UINT _export CalcObjectAndShow(LPVOID sFile);
	static void _export CalcObjects(const String& sCmd, Tranquilizer* trq);
	static void _export DeleteObjects(CWnd *owner, const String& sCmd, Tranquilizer* trq);
	static void _export UpdateObjects(const String& sCmd, Tranquilizer* trq);
	static void _export BreakDepObjects(const String& sCmd, Tranquilizer* trq);
	static void _export CopyObjects(const String& sCmd, Tranquilizer* trq, CWnd* wnd=0, bool fOnlyDirectory=false);
	static void _export CopyFiles(const String& sCmd, Tranquilizer* trq);
	static UINT CmdCopyFileInThread(void *p);
	
protected:
	void Init();
	String sDataBaseName(const String& sName);
	
private:
	void CmdShowExpressionError(const String& sErr);
	void CmdRunScript(const String& s);
	bool fCmdCalc(const String& s);
	bool fCmdSimpleCalc(const String& s);
	void CmdCreate(const String& s);
	void CmdLayout(const String&);
	void UpdateObject(const String& fn);
	void RelDiskSpaceObject(const String& fn);
	void CalcObject(const String& fn);
	void BreakDepObject(const String& sFile);
	void PropObject(const String& sPar);
	void PopupHelpObject(const String& sPar);
	void ExecuteFlag(const ParmList& parms);
	void CmdScript(const String& sScript);
	bool fCmdExtern(const String& sCmd);
	void CmdChangeDir(const String& sDir);
	void CmdOpenDir(const String& sDir);
	void Cmdcopy(const String& sPar);
	void CmdCopyFile(const String& sPar);
	void CmdMakeDir(const String& sDir);
	void CmdRemoveDir(const String& sDir);
	void CmdShow(const String& sPar);
	void CmdDel(const String& sCmd);
	void CmdImport14(const String& sCmd);
	void CmdConvert14(const String& sCmd);
	void CmdEdit(const String& sCmd);
	void CmdSetDescr(const String& sCmd);
	void CmdCrMapList(const String& sCmd);
	void CmdCreatePyramidFiles(const String& sCmd);
	void CmdDeletePyramidFiles(const String& sCmd);
	int  OpenMapListColorComp();
	int  OpenAnaglyph();
	void ResetDir();
	void OpenBaseMap();
	void EditObject(const FileName& fn);
	void EditBaseMap();
	int  OpenMapListSlideShow();

	void CreateMap(const String& sDom, const String& sGrf);
	void CreateSeg(const String& sDomain, const String& sCsys);
	void CreatePol(const String& sDomain, const String& sCsys);
	void CreatePnt(const String& sDomain, const String& sCsys);
	void CreateTbl(const String& sDomain);
	void CreateTb2(const String& sDomain);
	void CreateDom();
	void CreateRpr(const String& sDomain);
	void CreateCsy();
	void CreateFil();
	void CreateIsl();
	void CreateMpl();
	void CreateSms(const String& sMpr, const String& sMpl);
	void CreateGrf(const String& sCsy);
	void CreateFun();
	void CreateIoc(const String& s);
	
	void CmdSetReadOnly(const String& s);
	void CmdSetReadWrite(const String& s);
	void CmdCloseAll(const String& s);
	void CmdTabCalc(const String& s);
	void CmdRenameObject(const String& s);
	void CmdDelFile(const String& s);
	void CmdDelColumn(const String& s);
	void CmdCalcColumn(const String& s);
	void CmdUpdateColumn(const String& s);
	void CmdBreakDepColumn(const String& s);
	void CmdDomClassToID(const String& sDom);
	void CmdDomIDToClass(const String& sDom);
	void CmdDomPicToClass(const String& sDom);
	void CmdSetGrf(const String& s);
	void CmdSetCsy(const String& s);
	void CmdSetAttTable(const String& s);
	void CmdSetDom(const String& s);
	void CmdChangeDom(const String& s);
	void CmdSetValRange(const String& s);
	void CmdAddItemToDomain(const String& s);
	void CmdAddItemToDomainGroup(const String& s);
	void CmdMergeDom(const String& s);
	void CmdCreate2DimTable(const String& s);
	void CmdCreateTable(const String& s);
	void CmdCreateMap(const String& s);
	void CmdCreatePointMap(const String& s);
	void CmdCreateSegMap(const String& s);
	void CmdCreateDom(const String& s);
	void CmdCreateGrf(const String& s);
	void CmdCreateRpr(const String& s);
	void CmdDir(const String& sCmd);
	void CmdAppMetaData(const String& sN);

	// this command is only for testing purposes, it will show the connect db dialog
	void CmdTestingDBConnection(const String& sCmd);
	
	static bool ImportObject(FileName fnObj, String sExpr);
	
	static UINT CalcObjectInThread(LPVOID sFile);
	static UINT UpdateObjectInThread(LPVOID sFile);
	static UINT RunScriptInThread(LPVOID sScr);
	static UINT FreezeObjectAndShow(LPVOID sFile);
	static UINT FreezeObjectInThread(LPVOID sFile);
	static UINT CmdCopyInThread(LPVOID p);
//	static UINT CmdCopyFileInThread(LPVOID p);
	static UINT DeleteObjectInThread(void * data);
	static UINT ImportObjectInThread(LPVOID lp);
	static UINT ExportObjectInThread(LPVOID lp);
	static UINT CmdDomClassToIDInThread(void *p);
	static UINT CmdDomPicToClassInThread(void *p);
	static UINT CmdDomIDToClassInThread(void *p);
	static UINT CreatePyrInThread(LPVOID p);

	static void CreatePyramidFiles(const String& s);
	
};

#endif //COMMAND_HANDLER_H
