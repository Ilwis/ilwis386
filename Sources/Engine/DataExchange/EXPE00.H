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
/*
  Expe00.h
  Export to ArcInfo .E00
  by Willem Nieuwenhuis 15 oct 1996
  (c) Ilwis System Development ITC
  Last change:  WN   22 Jul 97    5:00 pm
*/

#ifndef EXPE00_H
#define EXPE00_H

#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Base\DataObjects\valrange.h"

#define SCVFormatSegAttr  "%S.AAT%*cXX%4i%4i%4i%10li"
#define SCVFormatPolAttr  "%S.PAT%*cXX%4i%4i%4i%10li"
#define SCVFormatBoundary "%S.BND%*cXX   4   4  16         1"
#define SCVFormatMinMax   "%s              4-1%4i4-1  12 3 60-1  -1  -1-1%20i"
#define SCVFormatTicks    "%S.TIC%*cXX   3   3  12"
#define SCVFormatTickDef  "%s             4-1%4i4-1  %9s  -1  -1-1%20i"
#define SCVFormatArcStart "%10li%10li%10li%10li%10li%10li%10li"
#define SCVFormatHistory  "%4i%02i%02i%02i%02i%4i%6i%6i%s"
#define SCVFormatPolBound " %li %S"
#define SCVFormatPolTopo  "%10li%10li%10li%10li%10li%10li"
#define SCVFormatPolTopoS "%10li%10li%10li"
#define SCVUndefFloat     "-3.370000037275507030+38" // -MAXFLOAT
#define SCVUndefLong      "-2147483647"   // -MAXLONG

struct Triple {
  long iArc;
  long iNode;
  long iLeftPol;
  Triple() : iArc(0), iNode(0), iLeftPol(0) {}
  Triple(long iA, long iNd, long iLP) : iArc(iA), iNode(iNd), iLeftPol(iLP) {}
};

struct ColInfo {
  short iType;      // Arc/Info type: 1=date, 2=string, 3=byte?, 4=short?, 5=long, 6=real
  short iNum;       // #digits for internal storage in Arc/Info
  short iDec;       // #decimals for internal storage in Arc/Info
  short iForm;      // #characters in interchange file (.E00)
  short iStrSize;   // Size specifier of the field info
  String sName;     // Name of the Field/Column
};

//#pragma option -Jgd
//static Array<Triple>  trpDummy;
//static Array<ColInfo> aciDummy;
//#pragma option -Jgx

class E00 {
  File* filE00;
  FileName fnE00;
  FileName fnIlwis;
  long iCurCoverage;
  long iTotalArcs;
  RowColBuf rcb;
  CoordBuf cbuf;
  DomainValueRangeStruct dvs;
  String sBuild;
  String sBuildCoords(Coord, Coord crd2 = crdUNDEF);
  String sBuildTick(short, Coord);
  void WriteLabel(const Coord&, long);
  void WriteBounds(CoordBounds&);
  void WriteTicks(CoordBounds&);
  ColInfo ciColDetails(const DomainValueRangeStruct&);
  void GetAttributeInfo(Table&, Array<ColInfo>&, short&);
  void WritePATFieldInfo(Array<ColInfo>&, long, short);
  void WriteArcFieldInfo(Array<ColInfo>&, long, short);
  String sBuildUndef(ColInfo&, long, Column&);
  String sBuildValue(ColInfo&, long, Column&);
  void WriteAAT(SegmentMap&);
  void WritePAT(PointMap&);
  void WritePAT(PolygonMap&);
  Tranquilizer& trq;
  ArrayLarge<long> aiSegIndex;      // link each ARC in .E00 file to Ilwis Segment (needed to link to attributes)
  ArrayLarge<long> aiArcIndex;      // link each ARC in .E00 file to Ilwis Segment (needed to link to attributes)
protected:
	void DoubleConvert(const double& rVal, int iPrecision, char *pcRes);
	String DoubleToStr(const double& rVal, bool fDoublePrec);
	String CoordToStr(const Coord& crd, bool fDoublePrec);

  void PutName();
  void PutSegment(ILWIS::Segment*);
  void PutPolygonLeader();
  void StartArcFile();            // ARC file: Segment Coordinates and Topology
  void EndFileMarker();           // write " -1 0 0 0 0 0 0"
  void StartLabFile();            // LAB file: Points or Polygon labels
  void EndLabFileMarker();        // write " -1 0.0 0.0"
  void PutCentroids()  {}         // CNT file: Polygon Centroid Coords
  void PutLabels()     {}         // LAB file: Label Point Coordinates and Topology
  void PutHistory();              // LOG file: Coverage History
  void PutProjection() {}         // PRJ file: Projection Parameters
  void PutSpacial();              // SIN file: Spacial Index
  void PutTolerance()  {}         // TOL file: Tolerance parameters
  void PutInfo(BaseMap&);         // IFO file: General info (boundary, attribute)
  void WriteTrailer();
public:
  E00(const FileName&, const FileName&, Tranquilizer&);
  ~E00();
  bool DoSegments();
  bool DoPolygons();
  bool DoPoints();
};

#endif


