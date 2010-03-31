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
/* $Log: /ILWIS 3.0/Import_Export/CONVERR.CPP $
 * 
 * 4     9-09-99 2:39p Martin
 * ported 2.22 stuff
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.5  1998/09/17 09:11:32  Wim
// 22beta2
//
// Revision 1.4  1998/06/05 13:59:28  Willem
// Added a new error message for errors in numeric values
//
// Revision 1.3  1998/05/12 19:16:28  Willem
// Added ErrorObject: FailedCreateColumn is thrown when a table cannot
// create a column.
//
// Revision 1.2  1998/03/17 18:09:56  Willem
// Added error function for coordinates.
//
// Revision 1.1  1997/08/07 23:42:18  Willem
// Initial revision
//
/*
  Converr.c: Error objects for conversions
  by Willem Nieuwenhuis, 1 August 97
  ILWIS Department ITC
	Last change:  WN   17 Mar 98   12:08 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\CONVERR.H"

void UnrecognizedTag(const String& sErr, const String& sTag) {
  throw ErrorUnrecognizedTag(sErr, sTag);
}

void TooMuchPoints(const String& sErr) {
  throw ErrorTooMuchPoints(sErr);
}

void UnexpectedEOF(const String& sErr) {
  throw ErrorUnexpectedEOF(sErr);
}

void FormatProblem(const String& sErr) {
  throw ErrorInFormat(sErr);
}

void InternalProblem(const String& sErr) {
  throw ErrorInternal(sErr);
}

void UserAbort(const String& sErr) {
  throw AbortedByUser(sErr);
}

void PolygonizationFailure(const String& sErr) {
  throw PolygonizationFailed(sErr);
}

void CoordExpected(const String& sErr) {
  throw ExpectedCoordinate(sErr);
}

void CreateColError(const String& sErr) {
  throw FailedCreateColumn(sErr);
}

void NumericExpected(const String& sErr) {
  throw ErrorNoNumeric(sErr);
}

void TooManyClassesForExport(const String& sErr) {
  throw ErrorExportTooManyClasses(sErr);
}


