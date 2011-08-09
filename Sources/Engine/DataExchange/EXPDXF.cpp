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
/* $Log: /ILWIS 3.0/Import_Export/EXPDXF.cpp $
 * 
 * 6     6-03-02 10:32 Willem
 * Prevented indefinitely looping which can appear because of polygon
 * structure not conforming to specification
 * 
 * 5     22/03/00 17:09 Willem
 * - Replaced obsolete function Tranq::Message() with MessageBox()
 * - string comparison are now case-insensitive
 * - leading and trailing spaces are now removed from domain items
 * 
 * 4     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
// Revision 1.9  1997/11/12 21:42:24  Willem
// The closing point for a closed polyline is now also written to the DXF file
// although it should not be necessary (autocad can handle both cases), but some
// applications make different assumptions.
//
// Revision 1.8  1997/09/30 17:12:02  Willem
// Added 3D flags for value Domain export.
//
// Revision 1.7  1997/09/30 10:12:07  Wim
// On request of Moshe put values as Z variable
//
// Revision 1.6  1997-09-29 23:44:19+02  Willem
// Removed layer tables for DomainValue. Changed internal use of Domain to DomainValueRangeStruct.
// This was necessary to get the ValueRange of the map and not the default ValueRange of the domain.
//
// Revision 1.5  1997/09/27 19:09:37  Willem
// A layer table is now also written for valuedomains. Also added tranquilizer in
// building the layer table.
//
/* expdxf.c
   Export AutoCad DXF
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WN   12 Nov 97    5:30 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"

//#pragma option -Jgd
//static ArrayLarge<RowCol> arcDummy;
//#pragma option -Jgx

#define ISPOLYGON true
#define ISSEGMENT false

/* Used Class Tree:
    DXFExporter             // Abstract base class
      DXFFromPointMap       //
      DXFFromSegmentMap     //   Derived classes taking care of the export
      DXFFromPolygonMap     //

    DXFFromIlwis            // Wrapper class, containing one DXFExporter*
  -------
  The base class defines the functions to build a DXF file. It has implementations
  for utility functions and a virtual function WriteEntities(). This virtual function
  will write the actual data. It is called by the DXFExporter::ExportToDXF() function, 
  that will also write the header and optionally a layer table.
  The WriteEntities() function is implemented in the three derived classes.
  The DXFFromIlwis class determines upon creation which type of data is to be
  exported and will allocate an appropriate (derived) class pointer.
  --------------------
  revision history:
  12/2/97: first version
  18/2/97: remove spaces from segment/point/polygon code retrieved from domain
           before writing to DXF
*/

class DXFExporter {
  friend class DXFFromIlwis;
protected:
  DXFExporter(const FileName& fnFile);
  virtual ~DXFExporter();
  void ExportToDXF();

// The utility functions
  void WriteLn(const String&);
  void WriteHeader();
  bool WriteLayerTable(Array<String>&);
  void WritePolylineHeader(const String&, bool);

  virtual bool BuildValLayer(Array<String>&) = 0;
  virtual void WriteEntities() = 0;

  void SetupName(String& sName);
  void SetDomain(const DomainValueRangeStruct&);
  void SetBounds(const CoordBounds&);
  void SetTranq(Tranquilizer *);
  bool fUpdate(long, long);
  void SetText(const String&);
  DomainValueRangeStruct dvs()  { return _dvs; }
  CoordBuf cbuf;
  RowColBuf rcb;
private:
  File* fileDXF;
  DomainValueRangeStruct _dvs;
  CoordBounds _cb;
  Tranquilizer *_trq;
};

class DXFFromPolygonMap : public DXFExporter {
public:
  DXFFromPolygonMap(const FileName& fnFile, const FileName& fnObject);
private:
  virtual bool BuildValLayer(Array<String>&);
  virtual void WriteEntities();
  void WritePolygon(const ILWIS::Polygon*);
  void WriteRing(CoordinateSequence *seq, const ILWIS::Polygon* pol);
  PolygonMap _pm;
};

class DXFFromSegmentMap : public DXFExporter {
public:
  DXFFromSegmentMap(const FileName& fnFile, const FileName& fnObject);
private:
  virtual bool BuildValLayer(Array<String>&);
  virtual void WriteEntities();
  void WriteSegment(const ILWIS::Segment*);
  SegmentMap _sm;
};

class DXFFromPointMap : public DXFExporter {
public:
  DXFFromPointMap(const FileName& fnFile, const FileName& fnObject);
private:
  virtual bool BuildValLayer(Array<String>&);
  virtual void WriteEntities();
  void WritePoint(const long);
  PointMap _pt;
};

class DXFFromIlwis {
  DXFExporter *pdxf;
public:
  DXFFromIlwis(const FileName&, const FileName&, Tranquilizer*);
  ~DXFFromIlwis();
  void Export();
};

// Implementation

// The base class DXFExporter
DXFExporter::DXFExporter(const FileName& fnFile) {
  fileDXF = new File(fnFile, facCRT);
  rcb.Size(1000);
  cbuf.Size(1000);
}

DXFExporter::~DXFExporter() {
  if (fileDXF) delete fileDXF;
}

void DXFExporter::SetText(const String& sMsg) {
  _trq->SetText(sMsg);
}

void DXFExporter::SetTranq(Tranquilizer *trq) {
  _trq = trq;
}

bool DXFExporter::fUpdate(long iPos, long iHighest) {
  return _trq->fUpdate(iPos, iHighest);
}

void DXFExporter::SetupName(String& sName) {
  String sLocal = sName.sTrimSpaces();
  if (sLocal == "?")
    sLocal = "Undefined";
  else
    for (short ii = 0; ii < sLocal.length(); ii++)
      if (sLocal[ii] == ' ') sLocal[ii] = '_';
  sName = sLocal;
}

void DXFExporter::SetDomain(const DomainValueRangeStruct& dvs) {
  _dvs = dvs;
}

void DXFExporter::SetBounds(const CoordBounds& cb) {
  _cb = cb;
}

void DXFExporter::WriteLn(const String& sLine) {
  fileDXF->WriteLnAscii(sLine);
}

void DXFExporter::WriteHeader() {
  WriteLn("0");
  WriteLn("SECTION");
  WriteLn("2");
  WriteLn("HEADER");
  WriteLn("9");
  WriteLn("$ACADVER");
  WriteLn("1");
  WriteLn("AC1006");
  WriteLn("9");
  WriteLn("$INSBASE");
  WriteLn("10");
  WriteLn("0.0");
  WriteLn("20");
  WriteLn("0.0");
  WriteLn("30");
  WriteLn("0.0");
  WriteLn("9");
  WriteLn("$EXTMIN");
  WriteLn("10");
  WriteLn(String("%15.6lf", _cb.MinX()));
  WriteLn("20");
  WriteLn(String("%15.6lf", _cb.MinY()));
  WriteLn("9");
  WriteLn("$EXTMAX");
  WriteLn("10");
  WriteLn(String("%15.6lf", _cb.MaxX()));
  WriteLn("20");
  WriteLn(String("%15.6lf", _cb.MaxY()));
  WriteLn("0");
  WriteLn("ENDSEC");
}

bool DXFExporter::WriteLayerTable(Array<String>& asLayer) {
  DomainSort* pdsort = dvs().dm()->pdsrt();
  DomainValue* pdv = dvs().dm()->pdv();
  DomainValueInt* pdi = dvs().dm()->pdvi();
  if (pdsort /* || pdv*/) {
    WriteLn("0");
    WriteLn("SECTION");
    WriteLn("2");
    WriteLn("TABLES");
    WriteLn("0");
    WriteLn("TABLE");
    WriteLn("2");
    WriteLn("LAYER");

    WriteLn("70");
    long iNrItems;
    if (pdsort)
      iNrItems = pdsort->iSize();
    else
      iNrItems = asLayer.iSize();
    WriteLn(String("%i", iNrItems + 1));
    SetText(TR("Writing layer table"));

    WriteLn("0");
    WriteLn("LAYER");
    WriteLn("2");
    WriteLn("0");    // default empty layer 0
    WriteLn("62");
    WriteLn("7");
    WriteLn("70");
    WriteLn("0");
    WriteLn("6");
    WriteLn("CONTINUOUS");

    String sLocal;
	long i = 1;
    for (; i <= iNrItems; i++) {
      WriteLn("0");
      WriteLn("LAYER");
      WriteLn("2");
      if (pdsort) {
        sLocal = (dvs().sValueByRaw(i)).sTrimSpaces();
        SetupName(sLocal);
        WriteLn(sLocal);
      }
      else
        WriteLn(asLayer[i - 1]);
      WriteLn("62");
      WriteLn(String("%i", i % 255));  // think up a colour
      WriteLn("70");
      WriteLn("0");
      WriteLn("6");
      WriteLn("CONTINUOUS");
    }
    WriteLn("0");
    WriteLn("ENDTAB");
    WriteLn("0");
    WriteLn("ENDSEC");
    if (fUpdate(i, iNrItems))
      return true;
  }
  return false;
}

void DXFExporter::WritePolylineHeader(const String& sName, bool fIsPolygon) {
  WriteLn("0");
  WriteLn("POLYLINE");
  WriteLn("66");     // Vertices follow flag
  WriteLn("1");
  WriteLn("70");     // Polyline flags:
  if (dvs().fValues()) {
    if (fIsPolygon)
      WriteLn("9");    //    this will be a closed polyline
    else
      WriteLn("8");    //    this will be a open polyline
  }
  else {
    if (fIsPolygon)
      WriteLn("1");    //    this will be a closed polyline
    else
      WriteLn("0");    //    this will be a open polyline
  }
  WriteLn("8");
  String sLocal = sName;
  SetupName(sLocal);
  WriteLn(sLocal);
}

static int iCompCodes(const String* s1, const String* s2) {
  return _stricmp((*s1).c_str(), (*s2).c_str());
}

void DXFExporter::ExportToDXF() {
  WriteHeader();
  Array<String> asLayer, asUnique;
  if (BuildValLayer(asLayer))
    return;

  if (asLayer.iSize() > 0) {
    typedef int (*iFncCmp)(const void*, const void*);
    qsort(asLayer.buf(), asLayer.iSize(), sizeof(String), (iFncCmp)iCompCodes);
    String sLast = asLayer[0];
    asUnique &= sLast;
    for (long i = 1; i < asLayer.iSize(); i++)
      if (sLast != asLayer[i]) {
        sLast = asLayer[i];
        asUnique &= sLast;
      }
  }

  WriteLayerTable(asUnique);
  WriteEntities();
}

// DXFFromSegmentMap members implementation
DXFFromSegmentMap::DXFFromSegmentMap(const FileName& fnObject, const FileName& fnFile)
             : DXFExporter(fnFile)
{
  _sm = SegmentMap(fnObject);
  SetDomain(_sm->dvrs());
  SetBounds(_sm->cb());
}

bool DXFFromSegmentMap::BuildValLayer(Array<String>& asLayer) {
  asLayer.Reset();
  if (dvs().fValues()) {
    asLayer &= _sm->fnObj.sFile;
    return false;
  }
  SetText(TR("Building layer table for values"));
  long iNrSegs = _sm->iFeatures();
  for (int i=0 ; i < _sm->iFeatures(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *) _sm->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
	  if (fUpdate(i, iNrSegs))
		  return true;
	  asLayer &= seg->sValue(_sm->dvrs());
  }
  fUpdate(iNrSegs, iNrSegs);
  return false;
}

void DXFFromSegmentMap::WriteSegment(const ILWIS::Segment* seg) {
  CoordinateSequence *seq = seg->getCoordinates();
  String sLocal = (dvs().sValueByRaw(seg->iValue())).sTrimSpaces();
  if (dvs().fValues())
    sLocal = _sm->fnObj.sFile;
  SetupName(sLocal);
  WritePolylineHeader(sLocal, ISSEGMENT);
  for (long i = 0; i < seq->size(); i++) {
    WriteLn("0");
    WriteLn("VERTEX");
    WriteLn("8");
    WriteLn(sLocal);
    WriteLn("10");
    WriteLn(String("%15.6lf", seq->getAt(i).x));
    WriteLn("20");
    WriteLn(String("%15.6lf", seq->getAt(i).y));
    if (dvs().fValues()) {
      WriteLn("30");
      WriteLn(String("%15.6lf", seg->rValue()));
      WriteLn("70");
      WriteLn("32");
    }
  }
  WriteLn("0");
  WriteLn("SEQEND");
  delete seq;
}

void DXFFromSegmentMap::WriteEntities() {
  WriteLn("0");
  WriteLn("SECTION");
  WriteLn("2");
  WriteLn("ENTITIES");
  long iSegCnt = 0;
  long iNrSegs = _sm->iFeatures();
  SetText(TR("Writing segments"));
  int i=0;
  for ( ; i < _sm->iFeatures(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *) _sm->getFeature(i);
	  if ( seg == NULL || !seg->fValid())
		  continue;
	  if (fUpdate(iSegCnt, iNrSegs))
		  return;
	  WriteSegment(seg);
  }
  fUpdate(i, iNrSegs);
  WriteLn("0");
  WriteLn("ENDSEC");
  WriteLn("0");
  WriteLn("EOF");
}

// DXFFromPolygonMap members implementation
DXFFromPolygonMap::DXFFromPolygonMap(const FileName& fnObject, const FileName& fnFile)
             : DXFExporter(fnFile)
{
  _pm = PolygonMap(fnObject);
  SetDomain(_pm->dvrs());
  SetBounds(_pm->cb());
}

bool DXFFromPolygonMap::BuildValLayer(Array<String>& asLayer) {
  asLayer.Reset();
  if (dvs().fValues()) {
    asLayer &= _pm->fnObj.sFile;
    return false;
  }
  SetText(TR("Building layer table for values"));
  long iNrPols = 0;
  long iTotPol = _pm->iFeatures();
  for (int i=0 ; i < _pm->iFeatures(); ++i) {
	  ILWIS::Polygon *pol = (ILWIS::Polygon *) _pm->getFeature(i);
	  if ( pol == NULL || !pol->fValid())
		  continue;
	  if (pol->rArea() > 0)
		  asLayer &= pol->sValue(_pm->dvrs());
	  if (fUpdate(i, iTotPol))
		  return true;
  }
  fUpdate(iTotPol, iTotPol);
  return false;
}

void DXFFromPolygonMap::WriteRing(CoordinateSequence *seq, const ILWIS::Polygon* pol) {
	String sLocal = (dvs().sValueByRaw(pol->iValue())).sTrimSpaces();
	if (dvs().fValues())
		sLocal = _pm->fnObj.sFile;
	SetupName(sLocal);
	for(int i = 0; i < seq->size(); ++i) {
		double x = seq->getAt(i).x;
		double y = seq->getAt(i).y;
		WriteLn("0");
		WriteLn("VERTEX");
		WriteLn("8");
		WriteLn(sLocal);
		WriteLn("10");
		WriteLn(String("%15.6lf", x));
		WriteLn("20");
		WriteLn(String("%15.6lf", y));
		if (dvs().fValues()) {
			WriteLn("30");
			WriteLn(String("%15.6lf", pol->rValue()));
			WriteLn("70");
			WriteLn("32");
		}
	}
}

/*
  WritePolygon transforms an Ilwis polygon to ONE closed DXF Polyline. If the
  polygon contains islands, a check is made if the island is also a valid polygon
  (code <> Undef). When it is valid the island is accessed and traversed separately and
  therefore can be skipped here. If the code is UNDEF then the island is handled and will
  be written (separate from the containing polygon) as a closed POLYLINE.
*/
void DXFFromPolygonMap::WritePolygon(const ILWIS::Polygon* pol) {
	const LineString *extRing = pol->getExteriorRing();
	CoordinateSequence *seq = extRing->getCoordinates();
	WriteRing(seq,pol);
	for(int hole=0; hole < pol->getNumInteriorRing(); ++hole) {
		const LineString *intRing = pol->getInteriorRingN(hole);
		delete seq;
		seq = NULL;
		seq = intRing->getCoordinates();
		WriteRing(seq, pol);
	}
	delete seq;

	//bool fIsIsland;
	//ArrayLarge<Coord> acrdBuf;
	//long i, iNrCoord, iNrPolPoints = 0;
	//Topology topStart = pol.topStart();
	//Topology topCur = topStart;
	//do {
	//	if (iNrPolPoints > 0)
	//		acrdBuf.Resize(0);
	//	iNrPolPoints = 0;
	//	Topology top = topCur;
	//	do {
	//		topCur.GetCoords(iNrCoord, cbuf);
	//		for (i = 0; i < iNrCoord - 1; i++ ) {  // - 1: put node only once
	//			acrdBuf &= cbuf[i];
	//			iNrPolPoints++;
	//		}
	//		Coord crdFirst = topCur.crdBegin();
	//		Coord crdLast = topCur.crdEnd();
	//		topCur = topCur.topNext();
	//		Coord crdBegin = topCur.crdBegin();
	//		Coord crdEnd = topCur.crdEnd();
	//		fIsIsland = crdBegin != crdFirst &&
	//			crdBegin != crdLast &&
	//			crdEnd != crdFirst &&
	//			crdEnd != crdLast;
	//	} 
	//	while ((top.iCurr() != topCur.iCurr()) && !fIsIsland &&
	//		topStart.iCurr() != topCur.iCurr());   // added extra check to stop when reaching the start

	//	fIsIsland = fIsIsland && (topStart.iCurr() != top.iCurr()); // containing pol is no island
	//	if (!fIsIsland || 
	//		(fIsIsland && (topCur.polLeft().iCurr() == iUNDEF)) )
	//	{
	//		acrdBuf &= cbuf[i];       //
	//		iNrPolPoints++;         // close polygon by adding the last node
	//		if (iNrPolPoints > 0) {
	//			String sLocal = (dvs().sValueByRaw(pol.iRaw())).sTrimSpaces();
	//			if (dvs().fValues())
	//				sLocal = _pm->fnObj.sFile;
	//			SetupName(sLocal);
	//			WritePolylineHeader(sLocal, ISPOLYGON);
	//			for (i = 0; i < iNrPolPoints; i++) {
	//				WriteLn("0");
	//				WriteLn("VERTEX");
	//				WriteLn("8");
	//				WriteLn(sLocal);
	//				Coord cCur = acrdBuf[i];
	//				WriteLn("10");
	//				WriteLn(String("%15.6lf", cCur.x));
	//				WriteLn("20");
	//				WriteLn(String("%15.6lf", cCur.y));
	//				if (dvs().fValues()) {
	//					WriteLn("30");
	//					WriteLn(String("%15.6lf", pol.rValue()));
	//					WriteLn("70");
	//					WriteLn("32");
	//				}
	//			}
	//			WriteLn("0");
	//			WriteLn("SEQEND");
	//		}
	//	}
	//}
	//while (topStart.iCurr() != topCur.iCurr() );
}

void DXFFromPolygonMap::WriteEntities() {
  WriteLn("0");
  WriteLn("SECTION");
  WriteLn("2");
  WriteLn("ENTITIES");
  long iNrPols = 0;
  long iTotPol = _pm->iFeatures();
  SetText(TR("Writing polygons"));
  for (int i=0 ; i < _pm->iFeatures(); ++i) {
	  ILWIS::Polygon *pol = (ILWIS::Polygon *) _pm->getFeature(i);
	  if ( pol == NULL || !pol->fValid())
		  continue;
	  if (pol->rArea() > 0)
		  WritePolygon(pol);
	  iNrPols++;
	  if (fUpdate(iNrPols, iTotPol))
		  return;
  }
  fUpdate(iTotPol, iTotPol);
  WriteLn("0");
  WriteLn("ENDSEC");
  WriteLn("0");
  WriteLn("EOF");
}

// DXFFromPointMap members implementation
DXFFromPointMap::DXFFromPointMap(const FileName& fnObject, const FileName& fnFile)
             : DXFExporter(fnFile)
{
  _pt = PointMap(fnObject);
  SetDomain(_pt->dvrs());
  SetBounds(_pt->cb());
}

bool DXFFromPointMap::BuildValLayer(Array<String>& asLayer) {
  asLayer.Reset();
  if (dvs().fValues()) {
    asLayer &= _pt->fnObj.sFile;
    return false;
  }
  SetText(TR("Building layer table for values"));
  for (long i = 1; i <= _pt->iFeatures(); i++) {
    asLayer &= _pt->sValue(i, 0);
    if (i % 25 == 1)
      if (fUpdate(i, _pt->iFeatures()))
        return true;
  }
  fUpdate(_pt->iFeatures(), _pt->iFeatures());
  return false;
}

void DXFFromPointMap::WritePoint(const long iRec) {
  Coord cCur = _pt->cValue(iRec);
  WriteLn("0");
  WriteLn("POINT");
  WriteLn("8");
  String sLocal = _pt->sValue(iRec);
  if (dvs().fValues())
    sLocal = _pt->fnObj.sFile;
  SetupName(sLocal);
  WriteLn(sLocal);
  WriteLn("10");
  WriteLn(String("%15.6lf", cCur.x));
  WriteLn("20");
  WriteLn(String("%15.6lf", cCur.y));
  if (dvs().fValues()) {
    WriteLn("30");
    WriteLn(String("%15.6lf", _pt->rValue(iRec)));
  }
}

void DXFFromPointMap::WriteEntities() {
  WriteLn("0");
  WriteLn("SECTION");
  WriteLn("2");
  WriteLn("ENTITIES");
  SetText(TR("Writing points"));
  for (long i = 1; i <= _pt->iFeatures(); i++) {
    WritePoint(i);
    if (i % 25 == 1)
      if (fUpdate(i, _pt->iFeatures()))
        return;
  }
  fUpdate(_pt->iFeatures(), _pt->iFeatures());
  WriteLn("0");
  WriteLn("ENDSEC");
  WriteLn("0");
  WriteLn("EOF");
}

DXFFromIlwis::DXFFromIlwis(const FileName& fnObject, const FileName& fnFile, Tranquilizer* trq) {
	pdxf = 0;
  if (fCIStrEqual(fnObject.sExt, ".mpp"))
    pdxf = new DXFFromPointMap(fnObject, fnFile);
  else if (fCIStrEqual(fnObject.sExt, ".mps"))
    pdxf = new DXFFromSegmentMap(fnObject, fnFile);
  else if (fCIStrEqual(fnObject.sExt, ".mpa"))
    pdxf = new DXFFromPolygonMap(fnObject, fnFile);
  if (pdxf)
    pdxf->SetTranq(trq);
}

DXFFromIlwis::~DXFFromIlwis() {
  if (pdxf) delete pdxf;
}

void DXFFromIlwis::Export() {
  if (pdxf)
    pdxf->ExportToDXF();
}

void ImpExp::ExportDXF(const FileName& fnObject, const FileName& fnFile) {
  try {
    trq.SetTitle(TR("Exporting to AutoCAD DXF"));
    trq.SetText(TR("Processing..."));

    DXFFromIlwis(fnObject, fnFile, &trq).Export();

  }
  catch (ErrorObject& err) {
    err.Show();
  }
}


