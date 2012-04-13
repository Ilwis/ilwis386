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

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\DataExchange\SHPFILE.H"
#include <geos/algorithm/CGAlgorithms.h>

void Shapefile::SwabLong(long& l) {
  _swab((char*)&l, (char*)&l, 4);
  long i = l & 0x0000ffff;
  l = (l >> 16) & 0x0000ffff; // make sure to not introduce new zeroes
  l = l | ( (long)i << 16 );
}

Shapefile::File_Header::File_Header() {
  iFileCode = 9994;
  for (short i = 0; i < 5; i++)
    iUnused1[i] = 0;
  iFileLength = 50;
  iVersion = 1000;
  for (int i = 0; i < 8; i++)
    iUnused2[i] = 0;
}


void Shapefile::SetFileHeader() {
  SwabLong(FileHeader.iFileCode);
  SwabLong(FileHeader.iFileLength);
}

void Shapefile::SetIndexFileHeader() {
  FileHeader.iFileLength = 50 + 4 * iRec;
  SwabLong(FileHeader.iFileLength);
}

Shapefile::Shapefile(File& fileshp)
  : fileSHP(fileshp)
{
  iRec = 0;
}

void Shapefile::Update() {
  RecordHeader.iRecordNumber = ++iRec;
  SwabLong(RecordHeader.iRecordNumber);
  IndexRecord.iOffset = FileHeader.iFileLength;
  SwabLong(IndexRecord.iOffset);
}

void Shapefile::SetFileBounds(const Coord& cdMin, const Coord& cdMax) {
  FileHeader.rBoundMinX = cdMin.x;
  FileHeader.rBoundMinY = cdMin.y;
  FileHeader.rBoundMaxX = cdMax.x;
  FileHeader.rBoundMaxY = cdMax.y;
}

void Shapefile::SetContentLength(long iConLen) {
  Update();
  FileHeader.iFileLength += iConLen + 4;
  long iContentLength = iConLen;
  SwabLong(iContentLength);
  IndexRecord.iContentLength = iContentLength;
  RecordHeader.iContentLength = iContentLength;
}

Shapefile_Point::Shapefile_Point(const PointMap& pntmap, File& fileSHP)
  : Shapefile(fileSHP)
{
  FileHeader.iShapeType = 1;
  Coord cdMin, cdMax;
  pntmap->Bounds(cdMin, cdMax);
  SetFileBounds(cdMin, cdMax);
  long iContentLength = 10;
  SwabLong(iContentLength);
  IndexRecord.iContentLength = iContentLength;
  RecordHeader.iContentLength = iContentLength;
}

Shapefile_Point::Point::Point()
{
  iShapeType = 1;
}

void Shapefile_Point::PointWrite() {
  fileSHP.Write(sizeof(RecordHeader), &RecordHeader);
  fileSHP.Write(4, &aPoint.iShapeType);
  fileSHP.Write(16, &aPoint.cd);
}

void Shapefile_Point::Update(const Coord& crd) {
  Shapefile::Update();
  aPoint.cd = crd;
  FileHeader.iFileLength += 14;
  PointWrite();
}


Shapefile_Arc::Shapefile_Arc(const SegmentMap& smIn, File& fileSHP)
  : Shapefile(fileSHP), _sm(smIn)
{
  FileHeader.iShapeType = 3;
  CoordBounds cb = _sm->cbGetCoordBounds();
  SetFileBounds(cb.cMin, cb.cMax );
}

Shapefile_Arc::Arc::Arc()
{
  iShapeType = 3;
  iNumParts = 1;
  iBeginPoint = 0;
}

void Shapefile_Arc::ArcWrite() {
  fileSHP.Write(sizeof(RecordHeader), &RecordHeader);
  fileSHP.Write(4, &aArc.iShapeType);
  fileSHP.Write(32, &aArc.rBox[0]);
  fileSHP.Write(4, &aArc.iNumParts);
  fileSHP.Write(4, &aArc.iNumPoints);
  fileSHP.Write(4, &aArc.iBeginPoint);
  double *buf = new double[aArc.iNumPoints * 2];
  for(int i = 0; i<	aArc.iNumPoints; ++i) {
	  double x = aArc.crdbuf->getAt(i).x;
	  double y = aArc.crdbuf->getAt(i).y;
	  buf[2*i] = x;
	  buf[2*i+1] = y;
  }
  double xx = buf[1];
  fileSHP.Write(16 * aArc.iNumPoints, buf);
  delete [] buf;
}

void Shapefile_Arc::Update(const ILWIS::Segment* seg) {
  CoordBounds cb = seg->cbBounds();
  Coord& crd = cb.cMin;
  aArc.rBox[0] = crd.x;
  aArc.rBox[1] = crd.y;
  crd = cb.cMax;
  aArc.rBox[2] = crd.x;
  aArc.rBox[3] = crd.y;
  long iNr;
  aArc.crdbuf = seg->getCoordinates();
  aArc.iNumPoints = aArc.crdbuf->size();
  iNr = 24 + aArc.iNumPoints * 8;
  SetContentLength(iNr);
  ArcWrite();
}


Shapefile_Pol::Shapefile_Pol(const PolygonMap& pm, File& fileSHP)
  : Shapefile(fileSHP), _pm(pm)
{
	FileHeader.iShapeType = 5;
	iShapeType = 5;
	CoordBounds cb = _pm->cbGetCoordBounds();
	SetFileBounds(cb.cMin, cb.cMax );
	//aiBeginPoint.Resize(1000);
}

void Shapefile_Pol::PolWrite(const vector<Coordinate>& coords, int iNumParts, int iNumPoints)
{
	fileSHP.Write(sizeof(RecordHeader), &RecordHeader);
	fileSHP.Write(4, &iShapeType);
	fileSHP.Write(32, &rBox[0]);
	fileSHP.Write(4, &iNumParts);
	fileSHP.Write(4, &iNumPoints);
	long iBeginPoint;
	for (long l = 0; l < iNumParts; ++l)
	{
		iBeginPoint = aiBeginPoint[l];
		fileSHP.Write(4, &iBeginPoint);
	}
	for (long l = 0; l < iNumPoints; ++l)
	{
		const Coord crd = coords[l];
		fileSHP.Write(16, &crd);
	}

}

void Shapefile_Pol::Update(const ILWIS::Polygon* pol)
{
	const CoordBounds cb = pol->cbBounds();
	Coord crd = cb.cMin;
	rBox[0] = crd.x;
	rBox[1] = crd.y;
	crd = cb.cMax;
	rBox[2] = crd.x;
	rBox[3] = crd.y;
	long iNrPointSeg = 0;
	vector<Coordinate> coords;
	aiBeginPoint.resize(0);
	const LineString *extRing = pol->getExteriorRing();
	CoordinateSequence * seq = extRing->getCoordinates();
	bool isCC1 = geos::algorithm::CGAlgorithms::isCCW(seq);
	if (isCC1)
		CoordinateSequence::reverse(seq);
	aiBeginPoint.push_back(0);
	for(int i =0; i < seq->size(); ++i) {
		coords.push_back(seq->getAt(i));
	}
	for(int hole = 0; hole < pol->getNumInteriorRing(); ++hole) {
		aiBeginPoint.push_back(coords.size());
		const LineString *intRing = pol->getInteriorRingN(hole);
		CoordinateSequence * intseq = intRing->getCoordinates();
		bool isCC2 = geos::algorithm::CGAlgorithms::isCCW(intseq);
		if (!isCC2)
			CoordinateSequence::reverse(intseq);
		for(int i =0; i < intseq->size(); ++i) {
			coords.push_back(intseq->getAt(i));
		}
		delete intseq;
	}
	//reverse(coords.begin(), coords.end());
	delete seq;

	
	//iNumPoints = 0;
	//iNumParts = 0;
	//Topology topStart = pol.topStart();
	//Topology topCur = topStart;
	//bool fIsIsland, fAtBeginning;
	//long l, iBufSize = cbuf.iSize();
	//do
	//{
	//	Topology top = topCur;
	//	do
	//	{
	//		topCur.GetCoords(iNrPointSeg, cbufseg);
	//		while (iNumPoints + iNrPointSeg > iBufSize)
	//		{
	//			if (iBufSize > 56000L)
	//				ErrorTooMuchPoints(_pm->sName());
	//			cbuf.Append(8000);
	//			iBufSize = cbuf.iSize();
	//		}
	//		for (l = 0; l < iNrPointSeg; l++ )
	//			cbuf[iNumPoints++] = cbufseg[l];

	//		Coord crdFirst = topCur.crdBegin();
	//		Coord crdLast = topCur.crdEnd();
	//		topCur = topCur.topNext();
	//		Coord crdBegin = topCur.crdBegin();
	//		Coord crdEnd = topCur.crdEnd();
	//		fIsIsland = crdBegin != crdFirst &&
	//			crdBegin != crdLast &&
	//			crdEnd != crdFirst &&
	//			crdEnd != crdLast;
	//		fAtBeginning = top.iCurr() == topCur.iCurr();
	//	}
	//	while (top.iCurr() != topCur.iCurr() && !fIsIsland &&
	//		topStart.iCurr() != topCur.iCurr());   // added extra check to stop when reaching the start

	//	// make sure there is enough room in the aiBeginPoint container
	//	if (iNumParts >= aiBeginPoint.iSize())
	//		aiBeginPoint.Append(aiBeginPoint.iSize());
	//	aiBeginPoint[iNumParts++] = iNumPoints;
	//}
	//while (topStart.iCurr() != topCur.iCurr());
	long iConLen = (44 + 4 * aiBeginPoint.size() + 16 * coords.size()) / 2;
	SetContentLength(iConLen);
	PolWrite(coords,aiBeginPoint.size(),coords.size());
}
