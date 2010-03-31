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
/* Script, ScriptPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  WK   23 Jun 98    5:45 pm
*/

#ifndef ILWSCRIPT_H
#define ILWSCRIPT_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Table\tbl.h"

class Token;
class TokenizerBase;

class ScriptTokenizer;

class DATEXPORT ScriptPtr: public IlwisObjectPtr
{
	friend class DATEXPORT Script;
public:
	virtual ~ScriptPtr();
	virtual void Store();
	virtual String sType() const;
	virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	
	const String& sScript() { return _sScript; }
	void _export SetScript(const String& sScript);
	void Exec(const Array<String>* asParms = 0);
	void _export Exec(const String& sParmList);
	void ExecAction(Token& tok, TokenizerBase& tkn);

	static bool _export fScanDomainValueRange(Token& tok, TokenizerBase& tkn, Domain& dm, ValueRange& vr);
	virtual void Rename(const FileName& fnNew);
	void Pause(ParmList& prml);

	// for automation of script forms
	// nr of parameters to be supplied by user
	int _export iParams();
	int _export SetParms(int iN);
	// string to display to user
	String _export sParam(int i);
	enum ParamType { 
		ptSTRING=1, ptVALUE, ptDOMAIN, ptRPR, ptGEOREF, ptCOORDSYS,
		ptRASMAP, ptSEGMAP, ptPOLMAP, ptPNTMAP, ptTABLE,
		ptMAPVIEW, ptMAPLIST, ptTBL2D, ptANNTXT, ptSMS, ptMATRIX,
		ptFILTER, ptFUNCTION, ptSCRIPT, ptFILENAME, ptCOLUMN, ptANY}; 
	// type of question to ask the user
	ParamType _export ptParam(int i);
  bool _export fParamIncludeExtension(int i);
	String _export sParamType(ParamType pt);	
	void _export SetParams(int iNr);
	void _export SetParam(int i, ParamType pt, const String& sQuestion, bool fIncExt=true, const String& sDefault="");
	void _export SetDefaultParam(int i, const String& sDefault);
	String _export sDefaultValue(int i);
	void _export GetObjectStructure(ObjectStructure& os);

protected:
	ScriptPtr(const String&);
	ScriptPtr(const FileName&);
	ScriptPtr(const FileName&, const String& sScript);

private:
	FileName fnScript;
	int iParms;
	String _sScript;
	Tranquilizer trq;
	Array<Table> atbl;

public:
	static void UnknownCommandError(const String& sComm, const FileName& fnObj);
	static void InvalidLineError(const String& sLine, const FileName& fnObj);
	static void SortDomainError(const String& sDomain, const FileName& fnObj);
	static void InvalidDirError(const String& sDir, const FileName& fnObj);
	static void DomainValueRangeExpected(const FileName& fnObj);
	static void NotDomainGroupError(const String& sDomain, const FileName& fnObj);
};

class DATEXPORT Script: public IlwisObject
{
	static IlwisObjectPtrList lstScript;
protected:
	Script(IlwisObjectPtr* ptr);
public:
	_export Script();
	_export Script(const FileName& filename);
	_export Script(const Script& scrpt);
	_export Script(const FileName& filename, const String& sScript);
	_export Script(const String& sScript);
	void operator = (const Script& scrpt) { SetPointer(scrpt.pointer()); }
	ScriptPtr* ptr() const { return static_cast<ScriptPtr*>(pointer()); }
	ScriptPtr* operator -> () const { return ptr(); }
	static void _export Exec(const String& sScriptLine);
	static ScriptPtr* pGet(const FileName& fn);
};

#endif




