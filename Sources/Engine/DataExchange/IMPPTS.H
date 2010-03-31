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
/* imppts.h
   Import Arc/Info .PTS file (point data)
   by Willem Nieuwenhuis, march 18, 1998
   ILWIS Department ITC
   Last change:  WN   23 Mar 98    2:44 pm
*/

#ifndef IMPORTPTS_H
#define IMPORTPTS_H

#include "Engine\DataExchange\IMPVEC.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\dmsort.h"

class ArcInfoPts {
public:
  ArcInfoPts(const FileName& fnFile, Tranquilizer&);
  long ScanInfo();
  void Convert(const FileName&);
  ~ArcInfoPts();
protected:
  long iLinesRead()        { return _iLins; }
  long iTotPoints()        { return _iTotPoints; }
  long iPointsRead()       { return _iPoint; }
  String sPointCode()      { return sCode; }
private:
  void MarkErase(bool);
  void MakeColumn(const FieldInfo&, Table&);
  ValueRange vrField(const FieldInfo&);
  void MakeTable(const Domain&, Table&);
  void AddDomainToAttrib(TObjectInfo&);

  void SetupPointMap(const FileName&);
  void AddCode(long iCode, TObjectInfo&);
  void WritePoint(long, Coord&);
  bool fArcInfoCoord();          // read world coordinate from file
  void GetNextLine();            // get line from ArcInfo list
  Tranquilizer&     trq;
  String            sReason;
  FileName          fnIlwis;
  TObjectInfo       oi;
  File*             fileArcInfo;
  String            sLine;
  String            sCode;
  long              _iLins, _iPoint, _iTotPoints;
  bool              fScanning;
};

#endif IMPORTPTS_H


