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
/* impdxf.h
   Import AutoCad DXF
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WN   24 Jun 97    5:59 pm
*/

#ifndef IMPORTDXF_H
#define IMPORTDXF_H

#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\IMPVEC.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\dmsort.h"

// enum kind { KNDClass, KNDID, KNDNone };

/*struct IlwPolInfo {
  IlwPolInfo();
  long            iFirstTop;
  short           iNrTops;
  String          sCode;
  double          rArea;     // when area < 0 the points must be stored backwards
  CoordBounds     cbPol;     // to detect islands
  LongArrayLarge  aiIncludedPols;
};

*/

class AutoCadDXF {
public:
  AutoCadDXF(const FileName& fnFile, const Tranquilizer*);
  void MarkErase(BaseMap& bm, bool fRemove);
  bool fScanHeader(CoordSystem&, CoordBounds&);
  bool fScanTable();
  bool fScanBlock();
  bool fScanInformation(CoordBounds&, CoordSystem&, Array<String>&,
                       RangeReal&, bool fBoundsOK);
  bool fScanSection(CoordBounds&, CoordSystem&, Array<String>&, RangeReal&);
  bool SetupAllMaps(const FileName&, const FileName&);
  String sSegmentCode(); // { return _sSegCode; }
  long iLinesRead()     { return _iLins; }
  long iLinesTotal()    { return _iLinesTotal; }
  long iSegmentsRead()  { return _iNrSeg; }
  void ReadGroup();              // get a DXF group from the DXF file into _dg
  short iGroupCode()    { return _dg.iCode; }
  String sGroupValue()  { return _dg.sValue; }
  ~AutoCadDXF();
  String sReason;
  void ReadSection();            // Read a DXF section
  void SetAllMaps(SegmentMap&, PolygonMap&, PointMap&);
  bool fCancelled;
  long iSegments;               // > 0 if DXF file contains segments
  long iPolygons;               // > 0 if DXF file contains polygons
  long iPoints;                 // > 0 if DXF file contains points
private:
  struct DXFGroup {
    short iCode;
    String sValue;
  };
  double rGroupValue();
  long iGroupValue();
  Coord cDXF();
  void FlushPoint();
  void FlushSegment();
  void FlushPolygon();
  bool fScanDXFInfo(CoordBounds&, CoordSystem&, Array<String>&, RangeReal&);
  void AddCoord(const Coord&);
  void AddBulge(const Coord&, const Coord&, double);

  bool fIsDXFBinary();
  void ScanLine(String&, CoordBounds&, RangeReal&);
  void ScanArc(String&, CoordBounds&, RangeReal&);
  bool fScanPolyline(String&, CoordBounds&, RangeReal&);    // return value: true = polygon, false = segment
  bool fScanLWPolyline(String&, CoordBounds&, RangeReal&);  // DXF14; return value: true = polygon, false = segment
  void ScanVertex(CoordBounds&, RangeReal&);
  void ScanBulge(CoordBounds&, const Coord&, const Coord&, double);
  void ScanPoint(String&, CoordBounds&, RangeReal&);
  void UpdateCodes(const String&, Array<String>&, bool&);

  void UpdateMinMax(double&, double&);
  void UpdateMinMax(RangeReal&);
  void ReadHeader();
  void ReadLayer();              // read representation information
  void ReadTables();             // Read Domain information
  void ReadInformation();        // read sequences of DXF groups (BLOCK, ENTITIES)
  void ReadBlocks();             // read Blocks section (just skips it here)
  void ReadUnknownSection();

  bool fGetLine();               // get a line, ie 2 Coord's
  bool fGetArc();                // get an Arc
  bool fGetPoint();              // get a Point
  bool fGetPolyline();           // get a polyline (more efficient storage than lines)
  bool fGetLWPolyline();         // get a compact polyline (DXF14; more efficient storage than lines)
  bool fGetText();               // Get Annotation text, skipped for now (30 may 1996)

  bool fGetVertex();             // get one Coord or Value
  bool fEndSequence();           // end of a sequence (fe POLYLINE)

  void GetNextLine();            // get line from SMT list

  bool fLineOptimize;            // set if detected that two or more LINE's are connected
  bool fIsPrevLine;              // set if previous item is 'LINE'
  bool fSegIsPol;
  bool fPolyLine;                // Differentiate between LINE and POLYLINE
  bool fStartPoly;               // Expect the first VERTEX in a POLYLINE
  bool fUseID;                   // if no domain info found use ID's
  bool fValues;
  bool fDecimals;
  SegmentMap _sm;
  PolygonMap _pm;
  PointMap _pt;
  Coord cVertexPrev, cStart;
  CoordArrayLarge _acrd;
  CoordBuf _cb;
  RowColBuf _rcb;
  short iNrTopPol;
  long iOuterPolygon;
  File* fileDXF;
  String sLine, _sSegCode;
  String sOptimCode;             // both Optim variables are use to keep track of the
  double rOptimHeight;           // correct value of the segment
  String sLineType, sTrqText;
  long _iLinesTotal;
  long _iLins, _iNrSeg, _iNrPol, _iNrPts;
  long iMaxPolCoord, iNrCoord;
  short iTextHeight, iTextColor;
  bool fBoundsOK;
  bool fTopTooLong;
  double rBulge, rPrevBulge;
  bool fIsValue;                 // fIsValue: true if all codes are numeric
  bool fUseHght;
  bool fIsMacText;               // end of file marker is CR only
  double _rMin, _rMax;           // the boundary values of a potential value domain
  double rHeight;
  DXFGroup _dg;
  Tranquilizer* _trq;
};

static int iCompCodes(const String* s1, const String* s2);


#endif


