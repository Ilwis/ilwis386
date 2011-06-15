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
/* app/appcont.h
// Ilwis Application Context Interface for ILWIS 2.0
// march 1994, by Jelle Wind
// (c) Computer Department ITC
Last change:  WK   28 Sep 98    4:23 pm
*/
#ifndef ILWAPPCONT_H
#define ILWAPPCONT_H

class MessageHandling;
class SysVarList;
class StringManager;
class IlwAppRootWnd;
class IlwisApplication;

#include "Engine\Base\DataObjects\Parm.h"
#include "Engine\Base\System\SYSVAR.H"
#include "Engine\Base\System\HTP.H"
//#ifndef _Windows
//#endif

// winExec is same as WinExec, but can show error message
class CommandHandler;
class ModuleMap;

class IlwisAppContext 
{
public:
	enum ThreadLocalVars{tlvDONOTSHOWFINDERROR, tlvCURDIR, tlvDONOTLOADGDB, tlvNOUPDATECATALOG, tlvDONOTSHOWANYERROR, tlvSERVERMODE, tlvNONE};
	_export IlwisAppContext(const String& sProg, const String& sCmdLine);
	_export ~IlwisAppContext();
	void UseYield(bool f) { _fUseYield = f; }
	bool fUseYield() const { return _fUseYield; }
	String sProg() { return _sProg; } // program name (full path)
	_export LRESULT Execute(const String& command);
	_export FileName fnGetSearchPath(unsigned int iIndex);
	_export void AddSearchPath(const FileName& fn);
	_export void RemoveSearchPath(const FileName& fn);
	_export String getLanguageExtension() const { return languageExtension;}
	_export void setLanguageExtension(const String& ext) {languageExtension=ext;}
	CommandHandler * ComHandler() { return commHandler; }
	struct ThreadVar
	{
		ThreadVar(DWORD iId, ThreadLocalVars tv, void *V) ;
		DWORD iThreadId;
		ThreadLocalVars tvVar;
		void *pValue;

		bool operator==(const struct ThreadVar tv2) const;
	};	
	long iProcess() { return (long)((__int64)AfxGetInstanceHandle()) % 10000; }  // process Id
	MessageHandling *mh() { return _mh; }
	SysVarList *svl() { return _svl; }
	StringManager *sm() { return _sm; }
	String htp() { return help; }
	void setHelpItem(const String& htp) { help = htp; }
	String _export sIlwDir();
	String _export sStdDir();
	String _export sCurDir();
	String sStartUpDir();
	void _export SetCurDir(const String& sDir);
	String _export sGetCurDir() const;
	_export void *pGetThreadLocalVar(ThreadLocalVars tvType);
	_export void SetThreadLocalVar(ThreadLocalVars tvType, void *var);
	_export void InitThreadLocalVars();
	_export void RemoveThreadLocalVars();
	_export void ChangeSystemVar(const String& sOpt, const String& sValue);

	FileName _export fnUserINI();
	MessageHandling *_mh;
private:
	SysVarList *_svl;
	StringManager *_sm;
	vector<ThreadVar> ThreadVars;
	vector<FileName> searchPaths;
	bool _fUseYield;  // flag used in Yield
	bool fYldActive;
	long _iProcess;  // place holder for process Id
	String _sProg;   // place holder for program name
	String help;
	CommandHandler *commHandler;
	String languageExtension;
public:  
	IlwAppRootWnd* rwWinRoot() const { return winRoot; }
private:  
	IlwAppRootWnd* winRoot;
};


#endif // ILWAPPCONT_H







