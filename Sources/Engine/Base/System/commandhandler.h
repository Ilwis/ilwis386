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
#pragma once

#pragma warning( disable : 4786 )

#include "Engine\Base\System\LOGGER.H"

class BaseCommandHandler;
class CommandHandler;
class Tranquilizer;

struct IlwisFileInfo {
	IlwisFileInfo() { size = 0; status = true; check = 0; }

	String id() const {
		if ( location != "")
			return location + name;
		return name;
	}

	//bool operator<(const IlwisFileInfo& inf) {
	//	return id() < inf.id();
	//}
	bool operator==(const IlwisFileInfo& inf) const{
		bool ok = id() == inf.id();
		ok = ok && size == inf.size;
		ok = ok && modifiedTime == inf.modifiedTime;
		ok = ok && check == inf.check;
		return ok;
	}
	bool operator!=(const IlwisFileInfo& inf) const{
		return ! (operator==(inf));
	}

	String location;
	String name;
	long size;
	ILWIS::Time modifiedTime;
	short check;
	bool status;
};

class IMPEXP BaseCommandHandler
{
protected:

	BaseCommandHandler();
	static void ReroutPost(const String& s);
	static LRESULT ReroutSend(CWnd *owner, String s);
	static CFrameWnd *wndOwner;
public:
  virtual LRESULT fExecute(const String& sCmd);
  	void AddCommand(const String& cmd, CommandFunc cf, MetaDataFunc mdFunc=0);
	virtual ~BaseCommandHandler();
  void SetOwner(CFrameWnd*);
  virtual void init();
private:
  static void CmdExit(const String& s);
  static void CmdHelp(const String& s);
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
	static void _export gatherFromFolder(const string& root, const string& folder, vector<IlwisFileInfo>& files);
	void init();
	
protected:
	String sDataBaseName(const String& sName);
	
private:
	static void CmdShowExpressionError(const String& sErr);
	static void CmdRunScript(const String& s);
	bool fCmdCalc(const String& s);
	bool fCmdSimpleCalc(const String& s);
	static void CmdCreate(const String& s);
	static void CmdLayout(const String&);
	static void UpdateObject(const String& fn);
	static void RelDiskSpaceObject(const String& fn);
	static void CalcObject(const String& fn);
	static void BreakDepObject(const String& sFile);
	static void PropObject(const String& sPar);
	static void PopupHelpObject(const String& sPar);
	static void ExecuteFlag(const ParmList& parms);
	static void CmdScript(const String& sScript);
	bool fCmdExtern(const String& sCmd);
	static void CmdChangeDir(const String& sDir);
	static void CmdOpenDir(const String& sDir);
	static void Cmdcopy(const String& sPar);
	static void CmdCopyFile(const String& sPar);
	static void CmdMakeDir(const String& sDir);
	static void CmdRemoveDir(const String& sDir);
	static void CmdShow(const String& sPar);
	static void CmdDel(const String& sCmd);
	static void CmdImport14(const String& sCmd);
	static void CmdConvert14(const String& sCmd);
	static void CmdEdit(const String& sCmd);
	static void CmdSetDescr(const String& sCmd);
	static void CmdCrMapList(const String& sCmd);
	static void CmdCreatePyramidFiles(const String& sCmd);
	static void CmdDeletePyramidFiles(const String& sCmd);
	int  OpenMapListColorComp();
	int  OpenAnaglyph();
	static void ResetDir();
	static void OpenBaseMap();
	static void EditObject(const FileName& fn);
	static void EditBaseMap();
	int  OpenMapListSlideShow();

	static void CreateMap(const String& sDom, const String& sGrf);
	static void CreateSeg(const String& sDomain, const String& sCsys);
	static void CreatePol(const String& sDomain, const String& sCsys);
	static void CreatePnt(const String& sDomain, const String& sCsys);
	static void CreateTbl(const String& sDomain);
	static void CreateTb2(const String& sDomain);
	static void CreateDom();
	static void CreateRpr(const String& sDomain);
	static void CreateCsy();
	static void CreateFil();
	static void CreateIsl();
	static void CreateMpl();
	static void CreateSms(const String& sMpr, const String& sMpl);
	static void CreateGrf(const String& sCsy);
	static void CreateFun();
	static void CreateIoc(const String& s);
	
	static void CmdSetReadOnly(const String& s);
	static void CmdSetReadWrite(const String& s);
	static void CmdCloseAll(const String& s);
	static void CmdTabCalc(const String& s);
	static void CmdRenameObject(const String& s);
	static void CmdDelFile(const String& s);
	static void CmdDelColumn(const String& s);
	static void CmdCalcColumn(const String& s);
	static void CmdUpdateColumn(const String& s);
	static void CmdBreakDepColumn(const String& s);
	static void CmdDomClassToID(const String& sDom);
	static void CmdDomIDToClass(const String& sDom);
	static void CmdDomPicToClass(const String& sDom);
	static void CmdSetGrf(const String& s);
	static void CmdSetCsy(const String& s);
	static void CmdSetAttTable(const String& s);
	static void CmdSetDom(const String& s);
	static void CmdChangeDom(const String& s);
	static void CmdSetValRange(const String& s);
	static void CmdAddItemToDomain(const String& s);
	static void CmdAddItemToDomainGroup(const String& s);
	static void CmdMergeDom(const String& s);
	static void CmdCreate2DimTable(const String& s);
	static void CmdCreateTable(const String& s);
	static void CmdCreateMap(const String& s);
	static void CmdCreatePointMap(const String& s);
	static void CmdCreateSegMap(const String& s);
	static void CmdCreateDom(const String& s);
	static void CmdCreateGrf(const String& s);
	static void CmdCreateRpr(const String& s);
	static void CmdDir(const String& sCmd);
	static void CmdAppMetaData(const String& sN);
	static void CmdSend(const String& sN);
	static void CmdZip(const String& filename);
	static void CmdUpdateIlwis(const String& expr);
	static void CreatePyramidFiles(const String& str);
	static void CmdAddToMapList(const String& sN);

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
//	static UINT CmdCopyFileInThread(void *p);

	static UINT UpdateIlwis(void *p);
	static void cleanupDownloadDir(const String& path);
	
};


