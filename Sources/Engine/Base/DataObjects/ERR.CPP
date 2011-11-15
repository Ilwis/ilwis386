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

*/
#define ERR_C
#include "Headers\toolspch.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Hs\DAT.hs"
#include "Headers\Hs\ILWISGEN.hs"


String WhereError::sWhere() const
{
  String sObj, sSubObj;
  if (sName.length() != 0)
    sObj = sName;
  if (fn.sFile.length() != 0) {
    if (sObj.length() != 0)
      sSubObj = sObj;
    sObj = fn.sFullPath();
  }
  String s;
  if (sSubObj.length() != 0) {
    s = sSubObj;
    s &= " in ";
  }
  s &= sObj;
  return s;
}

String WhatError::sErrorCode() const
{
  if ( err!= shUNDEF)
    return String("(ILWIS Error %04i)", err);
  return "";  
}

String WhatError::sWhat() const
{
  if (sErr.length() != 0)
    return sErr;
  if (err == errOK) return "";
  String sTmp;
  switch (err) {
    case errMEMALL:
      sTmp = TR("Insufficient Memory"); break;
    case errCREATE:
      sTmp = TR("Creation error"); break;
    case errCREATESYS:
      sTmp = TR("Creation error");
      sTmp &= "; ";
      sTmp &= TR("Not allowed in system folder"); break;
      break;
    case errCREATEOPEN:
      sTmp = TR("Creation error"); 
      sTmp &= "; ";
      sTmp &= TR("Object still open");
      break;
    case errCREATEACC:
      sTmp = TR("Creation error"); 
      sTmp &= "; ";
      sTmp &= TR("File is readonly");
      break;
    case errOPEN:
      sTmp = TR("Find error"); break;
    case errREAD:
      sTmp = TR("Read error"); break;
    case errWRITE:
      sTmp = TR("Write error"); break;
    case errACCESS:
      sTmp = TR("Access violation error"); break;
    case errCOPYSELF :
      sTmp = TR("Can not copy to self"); break;
    case errNOATTTBL :
      sTmp = TR("No attribute table"); break;
    default:
      sTmp = TR("Unknown Error");
  }
  return sTmp;
}

WhereErrorColumn::WhereErrorColumn(const FileName& fnTbl, const String& sColName, const String& sTitle)
  : WhereError(fnTbl, String("Column %S", sColName), sTitle)
{
}

ErrorObject::ErrorObject()
{
  what = WhatError(0);   
}

ErrorObject::ErrorObject(const WhatError& whaterr)
{
  what = whaterr;
}

ErrorObject::ErrorObject(const WhatError& whaterr, const WhereError& whereerr)
{
  what = whaterr;
  where = whereerr;
}

ErrorObject::ErrorObject(const String& sErr, int err)
{
  what = WhatError(sErr, err); 
}

ErrorObject::ErrorObject(const String& sErr, String _message) {
  what = WhatError(sErr, shUNDEF);
  longMessage = _message;
}

String ErrorObject::sShowError() const
{
  String sWhere, sWhat, sCode;

  try {
    sWhere = where.sWhere();
  }
  catch (...) {
  }

  try {
    sWhat = what.sWhat();
  }
  catch (...) {
    sWhat = SDATErrAstErrorAst;
  }

  sCode = what.sErrorCode();

  String sErr;
  if (sWhere.length() != 0)
    sErr = String("%S\n%S\n%S", sWhere, sWhat, sCode);
  else {
    if (what.err != 0)
      sErr = String("%S\n%S", sWhat, sCode);
  }
  return sErr;
}

void ErrorObject::Show(const String& sTitl) const
{
	CWnd* wnd = CWnd::GetFocus();
	String sTtl = sTitl;
	if (sTtl.length() == 0)
		sTtl = where.sTitle();
	try {
		String sErr = sShowError();
		if (sErr.length() == 0)
			return;
		bool *fServerMode = (bool*)getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvSERVERMODE);
		if ( *fServerMode == false) {
			MessageBeep(MB_ICONHAND);
			MessageBox(0,sErr.sVal(), sTtl.sVal(), MB_ICONHAND|MB_OK|MB_TASKMODAL|MB_TOPMOST);
		}
		if ( longMessage != "") {
			String txt("simplecalc Text %S",longMessage);
			getEngine()->Execute(txt);
		}
		try {
			getEngine()->getLogger()->LogLine(sErr);
			if ( getEngine()->fStayResident() == false)
				getEngine()->Execute(String("exit"));

		}
		catch (const ErrorObject&) {
			// do nothing (readonly logfile or something)
		}
  }
  catch (...) {
    MessageBox(0, SDATErrErrorInErrorhandling.sVal(), SDATErrFatalILWISError.sVal(), MB_SYSTEMMODAL|MB_ICONHAND|MB_OK);
  }
  if (wnd && ::IsWindow(wnd->m_hWnd) && wnd->m_hWnd)
		wnd->SetFocus();
}

ErrorObject::~ErrorObject()
{
}

/*#define FPE_DENORMAL        0x82
#define FPE_UNEMULATED      0x87
#define FPE_SQRTNEG         0x88
#define FPE_STACKOVERFLOW   0x8a
#define FPE_STACKUNDERFLOW  0x8b
#undef  FPE_EXPLICITGEN
#define FPE_EXPLICITGEN     0x8c    /* raise( SIGFPE ); */


String WhatErrorFloatingPoint::sWhat() const
{
  switch( err ) {
    case FPE_INVALID:
      return TR("Invalid");
    case FPE_DENORMAL:
		return TR("Denormal");
    case FPE_ZERODIVIDE:
		return TR("Divide by zero");
    case FPE_OVERFLOW:
      return TR("Overflow");
    case FPE_UNDERFLOW:
      return TR("Underflow");
    case FPE_INEXACT:
      return TR("Inexact");
    case FPE_UNEMULATED:
      return TR("Unemulated");
    case FPE_SQRTNEG:
      return TR("Square root of negative number");
    case FPE_STACKOVERFLOW:
      return TR("Stack overflow");
    case FPE_STACKUNDERFLOW:
      return TR("Stack underflow");
    case FPE_EXPLICITGEN:
      return TR("Exception raised");
  }
  return SDATErrUnknown;
}

ErrorInvalidType::ErrorInvalidType(const WhereError& where,
                                   const String& sSect,
                                   const String& sType)
: ErrorObject(WhatError(String("%S '%S': '%S'",
                        TR("Invalid type in section"),
                        sSect, sType), errTYPESECTION), where)
{}

ErrorNotThisObjectType::ErrorNotThisObjectType(const WhereError& where,
                                   const String& sType)
: ErrorObject(WhatError(String("%S %S", TR("Object is not a"), 
                        sType), errNOTTHISTYPE), where)
{}

/*
ErrorExpression::ErrorExpression(const WhereError& where, const String& sExpr)
: ErrorObject(WhatError(String("Invalid expression: '%S'", sExpr), errEXPRESSION), where)
{}*/

static ErrorMemAlloc errMemAlloc;

void _export MemAllocError()
{
	MessageBox(0, TR("Out of menory").c_str(), TR("Error").c_str(), MB_ICONHAND|MB_SYSTEMMODAL|MB_OK);
  throw errMemAlloc;
}

void NotFoundError(const FileName& fn)
{
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	if ( fDoNotShowError == 0 || *fDoNotShowError == false)
		throw ErrorNotFound(fn);
	else
		throw ErrorDummy();
}

void ColumnNotFoundError(const FileName& fnTbl, const String& sCol)
{
  throw ErrorNotFound(WhereErrorColumn(fnTbl, sCol));
}

void NotCreatedError(const FileName& fn)
{
  throw ErrorNotCreated(fn);
}

void NotCreatedErrorSystem(const FileName& fn)
{
  throw ErrorNotCreatedSystem(fn);
}

void NotCreatedErrorOpen(const FileName& fn)
{
  throw ErrorNotCreatedOpen(fn);
}

void NotCreatedErrorAccess(const FileName& fn)
{
  throw ErrorNotCreatedAccess(fn);
}

void InvalidTypeError(const FileName& fn , const char* str, const String& sType)
{
  throw ErrorInvalidType(fn, str, sType);
}

void NotThisObjectTypeError(const FileName& fn , const char* str)
{
  throw ErrorNotThisObjectType(fn, str);
}

void NotImplementedError()
{
  throw ErrorObject(SDATErrNotImplemented, -1);
}

ErrorExpression::ErrorExpression(const String& sExpr, const String& sSyntax)
: ErrorObject(WhatError(String(TR("Invalid expression %S %S").c_str(), sExpr, sSyntax), errEXPRESSION), where)
{}

void ExpressionError(const String& sExpr, const String& sSyntax)
{
  throw ErrorExpression(sExpr, sSyntax);
}

ErrorAppName::ErrorAppName(const WhereError& where, const String& sAppName)
: ErrorObject(WhatError(String(TR("Invalid application: %").c_str(), sAppName), errAPPNAME), where)
{}

void AppNameError(const FileName& fn, const String& sAppName)
{
  throw ErrorAppName(fn,sAppName);
}

ErrorDirNotFound::ErrorDirNotFound(const String& sDir)
: ErrorObject(WhatError(String(TR("Folder not found %S").c_str(), sDir), errOPEN))
{
}

ErrorPerc::ErrorPerc(double rPerc, const WhereError& where, IlwisError err, double rPercMax)
: ErrorObject(WhatError(String(SDATErrPercOutRange_gg.sVal() , rPerc, rPercMax), err), where)
{
}

void PercError(double rPerc, const FileName& fn, IlwisError err, double rPercMax)
{
  throw ErrorPerc(rPerc, fn, err, rPercMax);
}

ErrorValueDomain::ErrorValueDomain(const String& sDomain, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(String(TR("Domain value expected %S").c_str(), sDomain), err), where)
{}

void ValueDomainError(const String& sDomain, const FileName& fn, IlwisError err)
{
  throw ErrorValueDomain(sDomain, fn, err);
}

void ValueDomainError(const String& sDomain, const String& s, IlwisError err)
{
  throw ErrorValueDomain(sDomain, s, err);
}

void ValueOrSortDomainError(const String& sDomain, const String& s, IlwisError err)
{
	throw ErrorObject(WhatError(String(TR("Value/Class domain type error %S").c_str(), sDomain), err), s);
}

ErrorSortDomain::ErrorSortDomain(const String& sDomain, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(
  String(SDATErrDomClassIDGroup_S.sVal(), sDomain),
		err), where)
{}

ErrorClassDomain::ErrorClassDomain(const String& sDomain, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(
  String(SDATErrDomClassExpected_S.sVal(), sDomain),
		err), where)
{}

ErrorImageDomain::ErrorImageDomain(const String& sDomain, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(
  String(SDATErrDomImageExpected_S.sVal(), sDomain),
		err), where)
{
}

ErrorIdentifierDomain::ErrorIdentifierDomain(const String& sDomain, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(String(TR("Domain ID expected %S").c_str(), sDomain), err), where)
{}

void IdentifierDomainError(const String& sDomain, const FileName& fn, IlwisError err)
{
  throw ErrorIdentifierDomain(sDomain, fn, err);
}

ErrorCoordSystem::ErrorCoordSystem(const String& sCoordS, const WhereError& where, IlwisError err)
: ErrorObject(WhatError(
			  String(TR("Wrong coordinate system %S").c_str(), sCoordS),	err), where)
{
}

ErrorIncompatibleGeorefs::ErrorIncompatibleGeorefs(const String& sGeoRef1, const String& sGeoRef2, const WhereError& where,
IlwisError err)
: ErrorObject(WhatError(
			  String(TR("Incompatible georefs %S %S").c_str(), sGeoRef1, sGeoRef2), err), where)
{
}

ErrorIncompatibleDomains::ErrorIncompatibleDomains(const String& sDom1, const String& sDom2, const WhereError& where,
IlwisError err)
: ErrorObject(WhatError(
			  String(TR("Incompatible domains %S %S").c_str(), sDom1, sDom2), err), where)
{
}

void IncompatibleDomainsError(const String& sDom1, const String& sDom2, const WhereError& where, IlwisError err)
{
  throw ErrorIncompatibleDomains(sDom1, sDom2, where, err);
}

void IncompatibleCoordSystemsError(const String& sCsy1, const String& sCsy2, const WhereError& where, IlwisError err)
{
  throw ErrorIncompatibleCoordSystems(sCsy1, sCsy2, where, err);
}

ErrorIncompatibleCoordSystems::ErrorIncompatibleCoordSystems(const String& sCsy1, const String& sCsy2, const WhereError& where,
IlwisError err)
: ErrorObject(WhatError(
			  String(TR("Incompatible coordinate systems %S %S").c_str(), sCsy1, sCsy2), err), where)
{}

ErrorIncompatibleMapSize::ErrorIncompatibleMapSize(const String& sMap1, const String& sMap2, const WhereError& where,
IlwisError err)
: ErrorObject(WhatError(
			  String(TR("Incompatible map sizes %S %S").c_str(), sMap1, sMap2), err), where)
{}

ErrorGeoRefNone::ErrorGeoRefNone(const WhereError& where, IlwisError err)
: ErrorObject(WhatError(String(SDATErrGeoRefCoordNeeded), err), where)
{}

ErrorMapWithoutGeoRefCorners::ErrorMapWithoutGeoRefCorners(const WhereError& where, IlwisError err)
: ErrorObject(WhatError(String(SDATErrDTMNeedsGRCorners), err), where)
{}

ErrorFloatingPoint::ErrorFloatingPoint(int e)
: ErrorObject(WhatErrorFloatingPoint(e))
{ where.SetTitle(SDATErrFloatingPointError); }

ErrorFloatingPoint::ErrorFloatingPoint(const WhereError& whereerr, int e)
: ErrorObject(WhatErrorFloatingPoint(e), whereerr)
{ if (!where.fTitle())
    where.SetTitle(SDATErrFloatingPointError);
}

ErrorUserAbort::ErrorUserAbort()
: ErrorObject()
{
}

void UserAbort()
{
  throw ErrorUserAbort();
}

void ErrorUserAbort::Show(const String& ) const
{
  // do nothing
}

ErrorDummy::ErrorDummy()
: ErrorObject()
{
}

void DummyError()
{
  throw ErrorDummy();
}

void ErrorDummy::Show(const String& ) const
{
  // do nothing
}

ErrorDiskFull::ErrorDiskFull(const String& sFileName)
: ErrorObject(WhatError(String(TR("Disk full")), errDISKFULL), sFileName)
{}

void DiskFullError(const String& sFileName)
{
  throw ErrorDiskFull(sFileName);
}

ErrorRename::ErrorRename(const WhereError& where)
: ErrorObject(WhatError(String(SDATErrRename), 1000), where)
{
}

void FileAlreadyExistError(const FileName& fn)
{
  throw ErrorObject(WhatError(String(SDATErrFileAlreadyExists), errCREATE), fn);
}

WindowsError::WindowsError(const DWORD iErrorId) : ErrorObject()
{
    char pcMessage[200];
    int err=::FormatMessage( 
                  FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  iErrorId,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  pcMessage,
                  200,
                  NULL);

	String sMessage("Windows Error code %d \n\n %s\n", iErrorId, pcMessage);
  what = WhatError(sMessage, shUNDEF);
  
}

WindowsError::WindowsError(const String& sError) : ErrorObject(sError)
{}

DosError::DosError(int iErrorId) : ErrorObject()
{
  String sMessage("%s\n", strerror(iErrorId));
  what = WhatError(sMessage, shUNDEF);
}








