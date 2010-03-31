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
/* ErrorObject
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK   28 Sep 98    3:59 pm
*/

#ifndef ERROROBJ_H
#define ERROROBJ_H
#include "Headers\Hs\DAT.hs"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

//typedef int Error;

// oke
#define errOK 0
// not created:
#define errCREATE 1001
// not created in system dir
#define errCREATESYS 1021
// not created, still open
#define errCREATEOPEN 1022
// not created, read only file
#define errCREATEACC 1023
// not opened, not found
#define errOPEN 1002
// read error
#define errREAD 1003
// write error
#define errWRITE 1004
// access error
#define errACCESS 1005
// copy to itself
#define errCOPYSELF 1006
// invalid name (first char)
#define errNAME 1007
// memory alloc error
#define errMEMALL 1008
// invlaid type in section
#define errTYPESECTION 1009
// invalid expresion
#define errEXPRESSION 1010
// invalid name
#define errNOATTTBL 1011
// invalid app name
#define errAPPNAME 1012
// disk full
#define errDISKFULL 1013
// not this type
#define errNOTTHISTYPE 1014

#define errDomainSortElementExists 2410


typedef int IlwisError;

class IMPEXP WhatError
{
  friend class IMPEXP ErrorObject;
public:
  WhatError() {}
  WhatError(IlwisError e)
    { err = e; }
  WhatError(const String& sError, IlwisError e)
    { sErr = sError;  err = e; }
  virtual String sErrorCode() const;
  virtual String sWhat() const;
  String sErr;
  IlwisError err;
};

class IMPEXP WhereError
{
  friend class IMPEXP ErrorObject;
public:
  WhereError()
    {}
  WhereError(const FileName& filnam)
    : fn(filnam) {}
  WhereError(const FileName& filnam, const String& sTitl)
    : fn(filnam), _sTitle(sTitl) {}
  WhereError(const String& sNam)
    : sName(sNam) {}
  WhereError(const FileName& filnam, const String& sSubName, const String& sTitl)
    : fn(filnam), sName(sSubName), _sTitle(sTitl) {}
  String sWhere() const;
  void SetTitle(const String& sTtl)
    { _sTitle = sTtl; }
  bool fTitle() const
    { return _sTitle.length() != 0; }
  const String& sTitle() const
    { return _sTitle; }
private:
  FileName fn;
  String sName;
  String _sTitle;
};

class IMPEXP WhereErrorColumn : public WhereError
{
public:
  WhereErrorColumn(const FileName& fnTbl, const String& sColName, const String& sTitl = "");
};

class IMPEXP ErrorObject
{
public:
  ErrorObject();
  ErrorObject(const WhatError& whaterr);
  ErrorObject(const WhatError& whaterr, const WhereError& whereerr);
  ErrorObject(const String& sErr, int err=shUNDEF);
  ErrorObject(const String& sErr, String longMessage);
  virtual ~ErrorObject();
  String sShowError() const;
  virtual void Show(const String& sTitle = "") const;
  void SetTitle(const String& sTitle)
   { where.SetTitle(sTitle); }
  IlwisError err() const { return what.err; }
  String sWhat() const { return what.sWhat(); } 
protected:
  WhereError where;
  WhatError what;
  String longMessage;
};

void MemAllocError();
void IMPEXP NotFoundError(const FileName&);
void IMPEXP ColumnNotFoundError(const FileName& fnTbl, const String& sCol);
void IMPEXP NotCreatedError(const FileName&);
void IMPEXP NotCreatedErrorSystem(const FileName&);
void IMPEXP NotCreatedErrorOpen(const FileName&);
void IMPEXP NotCreatedErrorAccess(const FileName&);
void IMPEXP InvalidTypeError(const FileName&, const char*, const String&);
void IMPEXP NotThisObjectTypeError(const FileName&, const char*);
void IMPEXP NotImplementedError();

class IMPEXP ErrorInvalidType : public ErrorObject
{
public:
  ErrorInvalidType(const WhereError& where, const String& sSect,
                   const String& sType);
};

class IMPEXP ErrorNotThisObjectType: public ErrorObject
{
public:
  ErrorNotThisObjectType(const WhereError& where, const String& sType);
};

class IMPEXP ErrorMemAlloc : public ErrorObject
{
public:
  ErrorMemAlloc() : ErrorObject(WhatError(errMEMALL)) {}
  ErrorMemAlloc(const WhereError& where) : ErrorObject(errMEMALL, where) {}
};


class IMPEXP ErrorNotFound : public ErrorObject
{
public:
  ErrorNotFound(const WhereError& where) : ErrorObject(errOPEN, where) {}
};

class IMPEXP ErrorRename : public ErrorObject
{
public:
  ErrorRename(const WhereError& where);
};

class IMPEXP ErrorDirNotFound : public ErrorObject
{
public:
  ErrorDirNotFound(const String& sDir);
};

class IMPEXP ErrorNotCreated : public ErrorObject
{
public:
  ErrorNotCreated(const WhereError& where) : ErrorObject(errCREATE, where) {}
};

class IMPEXP ErrorNotCreatedSystem : public ErrorObject
{
public:
  ErrorNotCreatedSystem(const WhereError& where) : ErrorObject(errCREATESYS, where) {}
};

class IMPEXP ErrorNotCreatedOpen : public ErrorObject
{
public:
  ErrorNotCreatedOpen(const WhereError& where) : ErrorObject(errCREATEOPEN, where) {}
};

class IMPEXP ErrorNotCreatedAccess : public ErrorObject
{
public:
  ErrorNotCreatedAccess(const WhereError& where) : ErrorObject(errCREATEACC, where) {}
};

class IMPEXP ErrorCopySelf : public ErrorObject
{
public:
  ErrorCopySelf(const WhereError& where) : ErrorObject(errCOPYSELF, where) {}
};

class IMPEXP ErrorRead : public ErrorObject
{
public:
  ErrorRead(const WhereError& where) : ErrorObject(errREAD, where) {}
};

class IMPEXP ErrorWrite : public ErrorObject
{
public:
  ErrorWrite(const WhereError& where) : ErrorObject(errWRITE, where) {}
};

class IMPEXP ErrorExpression : public ErrorObject
{
public:
//ErrorExpression(const WhereError& where, const String& sExpr);
  ErrorExpression(const String& sExpr, const String& sSyntax);
};

void IMPEXP ExpressionError(const String& sExpr, const String& sSyntax);

class IMPEXP ErrorAppName : public ErrorObject
{
public:
  ErrorAppName(const WhereError& where, const String& sAppName);
};

void IMPEXP AppNameError(const FileName&, const String& sAppName);

class IMPEXP ErrorNoAttTable: public ErrorObject
{
public:
  ErrorNoAttTable(const WhereError& where, IlwisError err = 0) :
    ErrorObject(err == 0 ? errNOATTTBL : err, where) {}
};

class IMPEXP ErrorPerc: public ErrorObject
{
public:
  ErrorPerc(double rVal, const WhereError& where, IlwisError err, double rPercMax=100);
};

void IMPEXP PercError(double rVal, const FileName&, IlwisError err, double rPercMax=100);

class IMPEXP ErrorValueDomain: public ErrorObject
{
public:
  ErrorValueDomain(const String& sDomain, const WhereError& where, IlwisError err);
};

class IMPEXP ErrorIdentifierDomain: public ErrorObject
{
public:
  ErrorIdentifierDomain(const String& sDomain, const WhereError& where, IlwisError err);
};

void IMPEXP ValueDomainError(const String& sDomain, const FileName&, IlwisError err); 
void IMPEXP ValueDomainError(const String& sDomain, const String&, IlwisError err);
void IMPEXP ValueOrSortDomainError(const String& sDomain, const String&, IlwisError err);
void IMPEXP IdentifierDomainError(const String& sDomain, const FileName&, IlwisError err);

class IMPEXP ErrorGeoRefNone: public ErrorObject
{
public:
  ErrorGeoRefNone(const WhereError& where, IlwisError err);
};

class IMPEXP ErrorMapWithoutGeoRefCorners: public ErrorObject
{
public:
  ErrorMapWithoutGeoRefCorners(const WhereError& where, IlwisError err);
};

class IMPEXP ErrorSortDomain: public ErrorObject
{
public:
  ErrorSortDomain(const String& sDomain, const WhereError& where, IlwisError err);
};

class IMPEXP ErrorClassDomain: public ErrorObject
{
public:
  ErrorClassDomain(const String& sDomain, const WhereError& where, IlwisError err);
};


class IMPEXP ErrorImageDomain: public ErrorObject
{
public:
  ErrorImageDomain(const String& sDomain, const WhereError& where, IlwisError err);
};

class IMPEXP ErrorCoordSystem: public ErrorObject
{
public:
  ErrorCoordSystem(const String& sCoordSys, const WhereError& where, IlwisError err);
};

void IMPEXP IncompatibleDomainsError(const String& sDom1, const String& sDom2, const WhereError& where, IlwisError err);

class IMPEXP ErrorIncompatibleDomains: public ErrorObject
{
public:
  ErrorIncompatibleDomains(const String& sDomain1, const String& sDomain2, const WhereError& where, IlwisError err);
};

class ErrorNotUnique : public ErrorObject {
public:
	ErrorNotUnique(const String& sErr)
	: ErrorObject(WhatError(sErr, errDomainSortElementExists), WhereError(String("DomainSort"))) {}
};

class IMPEXP ErrorIncompatibleGeorefs: public ErrorObject
{
public:
  ErrorIncompatibleGeorefs(const String& sGeoRef1, const String& sGeoRef2, const WhereError& where, IlwisError err);
};

void IMPEXP IncompatibleCoordSystemsError(const String& sCsy1, const String& sCsy2, const WhereError& where, IlwisError err);

class IMPEXP ErrorIncompatibleCoordSystems: public ErrorObject
{
public:
  ErrorIncompatibleCoordSystems(const String& sCsy1, const String& sCsy2, const WhereError& where, IlwisError err);
};

class IMPEXP ErrorIncompatibleMapSize: public ErrorObject
{
public:
  ErrorIncompatibleMapSize(const String& sMap1, const String& sMap2, const WhereError& where, IlwisError err);
};

class IMPEXP WhatErrorFloatingPoint: public WhatError
{
public:
  WhatErrorFloatingPoint(IlwisError e) :
    WhatError(e) {}
  virtual String sWhat() const;
};

class IMPEXP ErrorFloatingPoint: public ErrorObject
{
public:
  ErrorFloatingPoint(int e);
  ErrorFloatingPoint(const WhereError& whereerr, int e);
};

class IMPEXP ErrorUserAbort: public ErrorObject
{
public:  
  ErrorUserAbort();
  virtual void Show(const String& sTitle = "") const;
};

void IMPEXP UserAbort();

class IMPEXP ErrorDummy: public ErrorObject
{
public:  
  ErrorDummy();
  virtual void Show(const String& sTitle = "") const;
};

void IMPEXP DummyError();

class IMPEXP ErrorDiskFull: public ErrorObject {
public:
  ErrorDiskFull(const String& sFilNam);
};

class WindowsError : public ErrorObject
{
  public:
    _export WindowsError(DWORD iErrorId);
		_export WindowsError(const String& s);
};

class DosError : public ErrorObject
{
  public:
    _export DosError(int iErrorId);
};

class  AssertionError : public ErrorObject
{
  public:
    _export AssertionError(const String& err) : ErrorObject(err) {}
};

class InvariantError : public AssertionError
{
  public:
    _export InvariantError() : AssertionError(SDATErrInvariantError) {};
};

class PostPreConditionError : public AssertionError
{
  public:
    _export PostPreConditionError() : AssertionError(SDATErrPrePostCondError) {}
};

class NonImplementableFunction : public ErrorObject
{
	public:
		_export NonImplementableFunction(const String& s) : ErrorObject(s) {}
};

class RegistryError : public WindowsError
{
public:
	_export RegistryError(DWORD iError) : WindowsError(iError) {}
	_export RegistryError(const String& sError) : WindowsError(sError) {}
};

// this exception is meant to be able to throw error from within a script and stop the script
// a special catch block is needed. As it is different from the regular catch block (for ErrorObject)
// you can pass the excpetion to higher levels.
class StopScriptError : public ErrorObject
{
public:
	_export StopScriptError(const String& sErr) : ErrorObject(sErr) {}
};

void IMPEXP DiskFullError(const String& sFilNam);

void IMPEXP FileAlreadyExistError(const FileName& fn);

#endif







