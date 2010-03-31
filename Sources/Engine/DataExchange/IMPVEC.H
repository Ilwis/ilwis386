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
/* impvec.h
   Import to Ilwis vector
   by Willem Nieuwenhuis, march 17, 1998
   ILWIS Department ITC
   Last change:  WN   12 May 98   11:22 am
*/

#ifndef IMPORTVEC_H
#define IMPORTVEC_H

#include "Engine\DataExchange\Convloc.h"
#include "Engine\Map\basemap.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\tbl.h"

enum kind { KNDClass, KNDID, KNDValue, KNDNone };
enum ObjectKind { okSEGMENT, okPOLYGON, okPOINT, okRASTER, okTABLE, okNONE };
enum AttrDomainType { adtVALUE, adtSTRING, adtBOOL, adtCLASS, adtID, adtNONE,
                      adtTABLECLASS, adtTABLEID, adtUNKNOWN };
enum AttrColType { atctCLASS, atctID, atctVALUE, atctSTRING, atctBOOL };

struct FieldInfo {
  FieldInfo();
  
  String          sName;
  short           iWidth;
  short           iDecimal;
  short           iFieldSize;   // number of characters in the E00 file
  AttrColType     atctCol;
  double          rMin;
  double          rMax;
};

struct AttribInfo {
public:
  AttribInfo();

  String          sName(short);
  AttrColType     atctType(short);
  short           iWidth(short);
  short           iPrecision(short);

  Array<FieldInfo> fields;

  long            iNrRecords;
  short           iRecordSize;
  short           iNrActiveFields;
  short           iNrFields;
  short           iMaxFieldLength;
  bool            fOuterIncluded;
};


struct TObjectInfo {
  TObjectInfo();

  bool            fValid;       // true: object is found in external format
  BaseMap         mp;
  CoordBounds     cbMap;
  CoordSystem     csMap;
  AttribInfo      atiInfo;
  Table           tblAttrib;
//  MinMax          mmMap;        // meant for raster only
  RowCol          rcSize;       // Rows and Cols for raster
  double          rMin, rMax;   // ValueRange for raster
  bool            fIntType;     // Raster map type: true = integer, false = float
  LongArrayLarge  aiCodes;      // storage for all codes
  LongArrayLarge  aiIndexCodes;   // storage for all index codes
  
  long            iCount;       // Count number of elements;
  long            iCur;         // Current element
  long            iIDMax;       // keep track of highest ArcInfo ID
  long            iMaxIdx;      // maximum of index.
  String          sPrefix;
  bool            fCreatePolygons;  // Disable creation of polygons when true
  bool            fHasAttTable;
  bool            fError;       // indicate if the input contains erroneous info
                                // this is only use for polygon info here
  bool            fUnique;      // it is (almost) impossible to be sure that the
                                // codes in vectors represent numbers; therefore this flag is
                                // used only to decide between a Class or ID domain
};

#endif IMPORTVEC_H


