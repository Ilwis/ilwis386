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
/* conv.h
   Interface setup by Wim Koolhoven
   (c) Ilwis System Development ITC
   Last change:  WN   25 Mar 98   17:01 
*/

#ifndef ILW_CONVERT
#define ILW_CONVERT
#include "Headers\toolspch.h"

enum ImportRasterPixelStructure { irpsBIT = 1, irpsBYTE, irpsFLOAT };
enum ImportRasterFileStructure { irfsBANDINTERLEAVED = 1, irfsBANDSEQUENTIAL, irfsPIXELINTERLEAVED };
// iNrBytesPerPixel means nr of bits if irpsBIT is specified.

extern "C" {
void _export ImportWithHeader(const FileName& fnFile, const FileName& fnObject,
                              long iHeaderSize, long iNrBands, long iNrCols,
                              ImportRasterPixelStructure, int iNrBytesPerPixel, ImportRasterFileStructure,
                              bool fUseAs, bool fByteSwap, bool fCreateMPR, const String& sDescr = "");
void _export Export(const FileName& fnObject, const FileName& fnFile, const String& sExtFormat, const String& sMethod="");
}

// the String versions can be called from the user interface
void _export Import(const String& sExpr);
void _export Export(const String& sExpr);
void _export Import(const FileName& fnFile, const FileName& fnObject, const String& sExtFmt, const String& sOptions = "", CWnd* w=0);

#endif // ILW_CONVERT



