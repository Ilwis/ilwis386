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
/* $Log: /ILWIS 3.0/Import_Export/CONVERR.H $
 * 
 * 4     9-09-99 2:39p Martin
 * ported 2.22 stuff
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.7  1998/09/17 09:11:32  Wim
// 22beta2
//
// Revision 1.6  1998/06/05 14:00:22  Willem
// Added a new error message for errors in numeric values
//
// Revision 1.5  1998/05/12 19:32:42  Willem
//  Added ErrorObject: FailedCreateColumn is thrown when a table cannot
//  create a column.
//
// Revision 1.4  1998/03/17 18:09:24  Willem
// Added Error function for coordinates
//
// Revision 1.3  1997/08/07 23:44:42  Willem
// Addition of several Import/Export error objects and functions.
//
// Revision 1.2  1997/08/07 06:55:54  Wim
// Did not compile, so added the string in a #define.
// Should be added to conv.s instead
//
// Revision 1.1  1997/08/01 15:29:07  Willem
// Initial revision
//
/*
  Converr.h: Error objects for conversions
  by Willem Nieuwenhuis, 1 August 97
  ILWIS Department ITC
	Last change:  WN   17 Mar 98   12:11 pm
*/

#ifndef CONVERR_H
#define CONVERR_H

#include "Engine\DataExchange\CONV.H"
#include "Headers\Hs\CONV.hs"

#define errUNEXEOF     10001
#define errFORMERROR   10002
#define errINTERNAL    10003
#define errUSERABORT   10004
#define errPOLYGONIZE  10005
#define errCOORDEXPECT 10006
#define errNONUMERIC   10007
#define errUNRECTAG    10008
#define errTOOMUCH     10010
#define errCREATECOL   10021
#define errTOOMANYCLASS 10030

/* Definition of Import/Export error objects; do not use directly,
   but use the function after the class definition instead.
*/

class ErrorTooMuchPoints : public ErrorObject {
public:
  ErrorTooMuchPoints(const WhereError& where)
    : ErrorObject(WhatError(SCVErrTooMuchPoints, errTOOMUCH), where) {}
};

class ErrorUnrecognizedTag : public ErrorObject {
public:
  ErrorUnrecognizedTag(const WhereError& where, const String& sTag)
    : ErrorObject(WhatError(String(SCVErrUnrecognizedTag_S.c_str(), sTag), errUNRECTAG), where) {}
};    

class ErrorUnexpectedEOF : public ErrorObject {
public:
  ErrorUnexpectedEOF(const WhereError& where)
    : ErrorObject(WhatError(SCVErrUnexpectedEOF, errUNEXEOF), where) {}
};

class ErrorInFormat : public ErrorObject {
public:
  ErrorInFormat(const WhereError& where) 
    : ErrorObject(WhatError(SCVErrFormatViolation, errFORMERROR), where) {}
};

class ErrorInternal : public ErrorObject {
public:
  ErrorInternal(const WhereError& where) 
    : ErrorObject(WhatError(SCVErrInternalFailure, errINTERNAL), where) {}
};

class AbortedByUser : public ErrorObject {
public:
  AbortedByUser(const WhereError& where)
    : ErrorObject(WhatError(SCVWarnAbortedByUser, errUSERABORT), where) {}
};

class PolygonizationFailed : public ErrorObject {
public:
  PolygonizationFailed(const WhereError& where)
    : ErrorObject(WhatError(SCVErrPolygonizeFailed, errPOLYGONIZE), where) {}
};

class ExpectedCoordinate : public ErrorObject {
public:
  ExpectedCoordinate(const WhereError& where)
    : ErrorObject(WhatError(SCVErrCoordExpected, errCOORDEXPECT), where) {}
};

class FailedCreateColumn : public ErrorObject {
public:
  FailedCreateColumn(const WhereError& where)
    : ErrorObject(WhatError(SCVErrCreateColFailed, errCREATECOL), where) {}
};

class ErrorNoNumeric : public ErrorObject {
public:
  ErrorNoNumeric(const WhereError& where)
    : ErrorObject(WhatError(SCVErrNoNumeric, errNONUMERIC), where) {}
};

class ErrorExportTooManyClasses : public ErrorObject {
public:
  ErrorExportTooManyClasses(const WhereError& where)
    : ErrorObject(WhatError(SCVErrTooManyClasses, errTOOMANYCLASS), where) {}
};


/* ------ Error functions based on the objects above ------- */

void TooMuchPoints(const String& sErr);
void UnexpectedEOF(const String& sErr);
void FormatProblem(const String& sErr);
void InternalProblem(const String& sErr);
void UserAbort(const String& sErr);
void PolygonizationFailure(const String& sErr);
void CoordExpected(const String& sErr);
void CreateColError(const String& sErr);
void NumericExpected(const String& sErr);
void TooManyClassesForExport(const String& sErr);
void UnrecognizedTag(const String& sErr, const String& sTag);

#endif CONVERR_H


