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
/* implin.h
   Import Arc/Info LIN file
   by Willem Nieuwenhuis, march 13, 1998
   ILWIS Department ITC
   Last change:  WN   18 Mar 98    4:10 pm
*/

#ifndef IMPORTLIN_H
#define IMPORTLIN_H

#include "Engine\DataExchange\IMPVEC.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Domain\dmsort.h"

class ArcInfoLin {
public:
  ArcInfoLin(const FileName& fnFile, Tranquilizer&, CWnd*);
  long ScanInfo();
  void Convert(const FileName&);
  ~ArcInfoLin();
private:
  void MarkErase(bool);
  bool fArcInfoSegment();  // return true if logical EOF
  String sSegmentCode()    { return _sSegCode; }
  long iLinesRead()        { return _iLins; }
  long iSegmentsRead()     { return _iNrSeg; }
  long iTotSeg()           { return _iTotSegs; }
  void WriteSegment(long iCode, long iNrCoord);
  void SetupSegMap(const FileName&);
  bool fArcInfoCoord(Coord&);    // read world coordinate from file
  String sArcInfoCode();         //
  void GetNextLine();            // get line from ArcInfo list
  void MakeColumn(const FieldInfo&, Table&);
  ValueRange vrField(const FieldInfo&);
  void MakeTable(const Domain&, Table&);
  void AddDomainToAttrib(TObjectInfo&);
  void AddCode(long iCode, TObjectInfo&);
  File*             fileArcInfo;
  Tranquilizer&     trq;
  TObjectInfo       oi;
  CoordBuf          cbuf;
  RowColBuf         rcbuf;
  String            sLine, _sSegCode;
  String            sReason;
  FileName          fnIlwis;
  long              _iLins, _iNrSeg;
  long              _iTotSegs;
  bool              fScanning;
  bool              fInSegment;
	CWnd              *m_wnd;
};

#endif IMPORTLIN_H


