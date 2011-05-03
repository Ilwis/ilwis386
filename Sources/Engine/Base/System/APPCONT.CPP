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
/* appcont.c
// Implementation of Ilwis Application Context for ILWIS 2.0
// march 1994, by Jelle Wind
// (c) Computer Department ITC
	Last change:  WK   28 Sep 98    4:54 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\System\commandhandler.h"
//#include "Headers\stdafx.h"


//static String* psCurDir = 0;
static String _sStartUpDir;
static __declspec(thread) char _sCurDir[_MAX_DIR];

//HINSTANCE hInstApp;
const int ModAd = 20935;
unsigned char Enschede[] = "Enschede";
unsigned char VerKey[8] = { 0xD8, 0xE7, 0x1B, 0xF6, 0x57, 0xE2, 0xD0, 0x53 };

#ifndef _Windows
#define HTASK int;
#endif

IlwisAppContext::IlwisAppContext(const String& sProg, const String& sCmdLine)
: winRoot(0)
{
  if (sProg.length() == 0)
  {
      char sTemp[MAX_PATH];
      ::GetModuleFileName(NULL, sTemp, MAX_PATH);
      _sProg = String(sTemp);
  }
  else {
    _sProg = sProg;
  }
//  _obm = new IlwObjectManager();
  _svl = new SysVarList();
  IlwisSettings settings("DefaultSettings");
  languageExtension = settings.sValue("LanguageExtension",".eng");
  FileName fn(_sProg);
  String sDir = fn.sDrive & fn.sDir;
  _svl->Add(new Parm("IlwDir", sDir)); //"c:\\ilwis"));
  String sTmp = sDir & "system";
  _svl->Add(new Parm("IlwStdDir", sTmp)); //"c:\\ilwis\\system"));
  sTmp = sDir & "temp";
  _svl->Add(new Parm("IlwTmpDir", sTmp)); //"c:\\ilwis\\temp"));
  sTmp = "";
  _svl->Add(new Parm("Temp", sTmp));
  char sLang[5];//, sLng[5];
  _svl->Add(new Parm("SysProgName", String("ilwis")));
  _svl->Add(new Parm("verbose", true));
  _svl->Add(new Parm("timer", false));
  _svl->Add(new Parm("form", true));
//  *pl = new ParmList(sCmdLine);
//  _svl->Fill(*pl);
   { char sDir[_MAX_DIR];
     GetCurrentDirectory(_MAX_DIR, sDir);
     strcpy(_sCurDir, sDir);
   }
  _sm = new StringManager();
  commHandler = new CommandHandler();
}


IlwisAppContext::~IlwisAppContext()
{
  delete _svl;
  delete _sm;
  delete commHandler;
}


String IlwisAppContext::sCurDir()
{ 
	return sGetCurDir(); 
}

String IlwisAppContext::sStartUpDir()
{ return _sStartUpDir; }

String IlwisAppContext::sStdDir()
  { return svl()->sGet("IlwStdDir"); }

String IlwisAppContext::sIlwDir()
  { return svl()->sGet("IlwDir"); }

FileName IlwisAppContext::fnUserINI()
{
  FileName fn("ilwiscfg.ini"); //"user.ini");
  String sProj = sStartUpDir(); //sCurDir(); // svl()->sGet("projectdir");
  fn.Dir(sProj);
  return fn;
}

IlwisAppContext::ThreadVar::ThreadVar(DWORD iId, ThreadLocalVars tv, void *V) : iThreadId(iId), tvVar(tv), pValue(V) 
{
}

void IlwisAppContext::InitThreadLocalVars()
{
	bool *fVar = new bool;
	*fVar = false;
	bool *fDoNotLoad = new bool;
	*fDoNotLoad = false;
	bool *fNoUpdate = new bool(false);
	bool *fDispNoErr = new bool(false);
	bool *fServerMode = new bool(false);
	SetThreadLocalVar(tlvDONOTSHOWFINDERROR, fVar);
	SetThreadLocalVar(tlvDONOTLOADGDB, fDoNotLoad);
	SetThreadLocalVar(tlvNOUPDATECATALOG, fNoUpdate);
	SetThreadLocalVar(tlvDONOTSHOWANYERROR, fDispNoErr);
	SetThreadLocalVar(tlvSERVERMODE, fServerMode);

	char sDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, sDir);
	String *sCurDir = new String(sDir);
	SetThreadLocalVar(tlvCURDIR, sCurDir);

}

void IlwisAppContext::SetThreadLocalVar(ThreadLocalVars tvType, void *Var)
{
	DWORD id = ::GetCurrentThreadId();	
	ThreadVar tv(id, tvType, Var);

	for( vector<ThreadVar>::iterator cur=ThreadVars.begin();  cur != ThreadVars.end(); ++cur)	
	{
		ThreadVar vr = *cur;
		if ( (*cur) == tv )
		{
			switch (vr.tvVar )
			{
				case tlvCURDIR:
					delete (String *)((*cur).pValue);
					break;
				case tlvDONOTSHOWFINDERROR:
				case tlvDONOTLOADGDB:
				case tlvNOUPDATECATALOG:					
				case tlvDONOTSHOWANYERROR:
				case tlvSERVERMODE:
					delete (bool *)((*cur).pValue);
					break;
			}			
			(*cur).pValue = Var;
			return;
		}	
	}		
	ThreadVars.push_back(tv);
}

void *IlwisAppContext::pGetThreadLocalVar(ThreadLocalVars tvType)
{
	DWORD id = ::GetCurrentThreadId();	
	ThreadVar tv(id, tvType, 0);	
	vector<ThreadVar>::iterator where = find(ThreadVars.begin(), ThreadVars.end(), tv);
	if ( where == ThreadVars.end() )
		return 0;

	return (*where).pValue;
}

bool IlwisAppContext::ThreadVar::operator==(const struct ThreadVar tv2) const
{ 
		return iThreadId == tv2.iThreadId && tvVar == tv2.tvVar; 
}

void IlwisAppContext::RemoveThreadLocalVars()
{
	DWORD id = ::GetCurrentThreadId();
	if (ThreadVars.size() == 0)
		return;
	for( vector<ThreadVar>::iterator cur=ThreadVars.begin();  cur != ThreadVars.end();)
	{
		if ( (*cur).iThreadId == id )
		{
			switch ((*cur).tvVar)
			{
				case tlvCURDIR:
					delete (String *)((*cur).pValue);
					break;
				case tlvDONOTSHOWFINDERROR:
				case tlvDONOTLOADGDB:
				case tlvNOUPDATECATALOG:
				case tlvDONOTSHOWANYERROR:
				case tlvSERVERMODE:
					delete (bool *)((*cur).pValue);
					break;
					
			}				
			cur = ThreadVars.erase(cur);
		}
		else
			++cur;
	}		

}

String IlwisAppContext::sGetCurDir() const
{
	String *s = ((String *)(const_cast<IlwisAppContext *>(this))->pGetThreadLocalVar(tlvCURDIR));


	if ( s )
		return *s;

	return String();

}

void IlwisAppContext::SetCurDir(const String& sDir)
{
	String *s = (String *)(pGetThreadLocalVar(tlvCURDIR));
	if ( s )
	{
		if ( sDir[sDir.size() - 1 ] != '\\')
			*s = sDir + '\\';
		else
			*s = sDir;
	}
	else
	{
		s = new String(sDir);
		SetThreadLocalVar(tlvCURDIR, s);
	}
}

LRESULT IlwisAppContext::Execute(const String& command) {
	return commHandler->fExecute(command);
}

UINT winExec(LPCSTR cmd, UINT nShow)
{
  char temp[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, temp);
  unsigned int wRet = WinExec(cmd,nShow);
  if (wRet < 32) {
    char sErr[80];
    if (8 == wRet)
      strcpy(sErr, "Insufficient Memory");
    else
      sprintf(sErr, "WinExec Error Code = %d", wRet);
    MessageBox(0, sErr, "WinExec Failure", MB_ICONSTOP|MB_OK|MB_TASKMODAL);
  }
  return wRet;
}

void IlwisAppContext::RemoveSearchPath(const FileName& fn)
{
	vector<FileName>::iterator here = find(searchPaths.begin(), searchPaths.end(), fn);
	if ( here != searchPaths.end() )
		searchPaths.erase(here);
}			

void IlwisAppContext::AddSearchPath(const FileName& fn)
{
	searchPaths.push_back(fn);
}

FileName IlwisAppContext::fnGetSearchPath(unsigned int iIndex)
{
	if ( iIndex >= searchPaths.size()) return FileName();
	return searchPaths[iIndex];
}

void IlwisAppContext::ChangeSystemVar(const String& sOpt, const String& sValue) {
	_svl->Remove(sOpt);
	_svl->Add(new Parm(sOpt,sValue));
}











