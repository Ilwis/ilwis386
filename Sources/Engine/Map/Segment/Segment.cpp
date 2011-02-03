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
// $Log: /ILWIS 3.0/SegmentMap/Segment.cpp $
 * 
 * 28    23-05-05 12:33 Retsios
 * [bug=6575] Do not attempt to gather the coordinates of an invalid
 * segment as it will cause a crash.
 * 
 * 27    7-06-04 16:36 Retsios
 * [bug=6471] Improved the fValid check so that a segment with invalid id
 * can't propagate.
 * 
 * 26    20-11-02 22:22 Hendrikse
 * debugged fValid with if (id > sm->iSeg()) (last seg was not drawn !!)
 * 
 * 25    15-10-02 10:13 Koolhoven
 * ILWIS::Segment::fValid() now also checks whether id is not >= than total nr of
 * segments in map (bug 6063)
 * 
 * 24    5-04-01 15:44 Koolhoven
 * Delete(fDel) now only changes count of deleted segments if really
 * something changes
 * 
 * 23    7-03-01 17:46 Koolhoven
 * protect fIntersects() against partial purely horizonatl or vertical
 * segments so that calculated x or y is always smaller or larger than the
 * line
 * 
 * 22    16-02-01 13:16 Koolhoven
 * added function sValueAndNumber() to be used in the SegmentEditor for
 * reporting purposes
 * 
 * 21    5-02-01 19:03 Koolhoven
 * in fSelfCheck() protect against pure horizontal or vertical parts where
 * the rX or rY would almost always be < or > when itself
 * 
 * 20    30-11-00 18:24 Koolhoven
 * in segSplit() second segment now also incorporates splitted coordinate
 * in coordinate bounds
 * 
 * 19    22-11-00 15:18 Koolhoven
 * segSplit() now returns the newly created segment instead of itself
 * (needed to be sure the correct segment is deleted in case of Remove
 * False Polygon)
 * 
 * 18    21-11-00 18:40 Koolhoven
 * segSplit() did not call sm->Updated()
 * 
 * 17    16-10-00 16:59 Hendrikse
 * removed the 1000 coords limitation 
 * ( CoordBuf crdBuf;(1000);) in fSelCheck() and fIntersects()
 * 
 * 16    8/15/00 5:36p Wind
 * prevent against column access when they do not exist.
 * 
 * 15    27-07-00 11:18 Koolhoven
 * fIntersects() and fSelfCheck() returned the coordinate with X and Y
 * switched
 * 
 * 14    16-06-00 20:49 Koolhoven
 * use Column members of SegmentMapStore instead of enum ColumnTypes with
 * indirections
 * 
 * 13    16-06-00 17:46 Koolhoven
 * segSplit() takes care of DomainUniqueID
 * 
 * 12    4/06/00 12:25p Hendrikse
 * simplified and debugged Densify() 
 * replaced for inst crdBufOut by crdBufTemp to Size it properly at return
 * time
 * 
 * 11    4/03/00 5:51p Hendrikse
 * added cBufOut.Size(iCrdIn) to use the buf contents properly
 * removed some more 1000 limits and improved a var name iCrdIn
 * 
 * 10    14-03-00 13:12 Hendrikse
 * removed the 1000 crds limitation and consequences
 * 
 * 9     15/02/00 14:07 Willem
 * PutVal now uses the correct ValueRange to calculate iRaw
 * 
 * 8     20-01-00 10:50a Martin
 * uses columnPtr to access data, changed column layout
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 6     15-12-99 15:40 Koolhoven
 * segSplit now works
 * 
 * 5     15-12-99 10:31 Koolhoven
 * in fValid() put single lock after check on 0==sm
 * 
 * 4     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 3     29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 2     9/08/99 12:02p Wind
 * comments
*/
// Revision 1.11  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.10  1997/09/23 13:23:10  Wim
// segSplit creates a new segment so should update sm->pms->alSegBuf
//
// Revision 1.9  1997-09-19 17:52:19+02  Wim
// Added buffering with alSegBuf
//
// Revision 1.8  1997-09-15 11:22:43+02  Wim
// Do not use WriteCode() in segSplit() because
// PutRaw() or PutVal() are used which are much better
//
// Revision 1.7  1997-09-04 18:24:07+02  Dick
// In ILWIS::Segment::Density acrdBuf.Reset() instead of Resize(0) to regain memory Dick
//
// Revision 1.6  1997/08/11 09:56:45  Wim
// In SelfCheck() initialize fChanged on false.
//
// Revision 1.5  1997-08-04 17:05:16+02  Wim
// Use string for mask with all values
//
// Revision 1.4  1997-08-04 16:27:05+02  Wim
// Added ILWIS::Segment::fInMask(mask) function
//
// Revision 1.3  1997-08-04 16:16:00+02  Wim
// fSelfCheck() deletes segments with only 1 coordinate
// (it should not be possible to create these)
//
// Revision 1.2  1997-08-01 10:06:50+02  Wim
// fCheck() with segment of two equal points only will now return overlapping itself.
//
#define ILWSEGMENT_C
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Table\Col.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
//#include "Engine\Base\Algorithm\Tunnel.h"
#include "Engine\Base\Algorithm\Clipline.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Base\mask.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Domain\DomainUniqueID.h"

#define EPS10 1.e-10

using namespace ILWIS;

ILWIS::Segment::Segment(geos::geom::LineString *line) :
	geos::geom::LineString(NULL, new GeometryFactory(new PrecisionModel()))
{
  if ( line != NULL)
	  PutCoords(line->getCoordinates());
  ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
  fAcceptDeleted = false;
}

Segment::~Segment() {
}

//CoordBounds ILWIS::Segment::crdBounds() const 
//{
//	ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
//	Geometry *geom = getEnvelope();
//	const CoordinateSequence *seq = geom->getCoordinates();
//	CoordBounds cb;
//	for(int i = 0; i < seq->size(); ++i) {
//		Coord crd(seq->getAt(i));
//		cb += crd;
//	}
//	delete seq;
//	return cb;
//}

Coord ILWIS::Segment::crdBegin() 
{
	return Coord(*(getStartPoint()->getCoordinate()));
}

Coord ILWIS::Segment::crdEnd() 
{
  return Coord(*(getEndPoint()->getCoordinate()));
}

double ILWIS::Segment::rAzim(bool fEnd) const  // azimuth at begin or end
{
  double dx, dy;
  const CoordinateSequence *buf = getCoordinates();
  //GetCoords(iNr, buf);
  Coord crd0, crd1;
  if (fEnd) {
	  int i = buf->size() - 1;
    crd0 = buf->getAt(i);
    do {
      i--;
      crd1 = buf->getAt(i);
      dx = crd1.x - crd0.x;
      dy = crd1.y - crd0.y;
    } while (dx == 0 && dy == 0); 
  }
  else {
    int i = 0;
    crd0 = buf->getAt(i);
    do {
      i++;
      crd1 = buf->getAt(i);
      dx = crd1.x - crd0.x;
      dy = crd1.y - crd0.y;
    } while (dx == 0 && dy == 0); 
  }

  double r = atan2(dy,dx);
  if (r < 0)
    r += 2 * M_PI;
  if (r < 0)
    r += 2 * M_PI;  // error !!!!!
  delete buf;
  return r;
}

void ILWIS::Segment::PutCoords(CoordinateSequence* sq) {
	ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	points.release();
	//CoordinateArraySequence *sss = new CoordinateArraySequence();
	points = CoordinateSequence::AutoPtr(sq);
}

void ILWIS::Segment::PutCoords(long iNr, const CoordBuf& crdBuf) 
{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	cb = CoordBounds();
	points.release();
	vector<Coordinate> *crds = new vector<Coordinate>();
	crds->resize(iNr);
	for (int i = 0; i < iNr; ++i) {
		(*crds)[i] = Coordinate(crdBuf[i].x,crdBuf[i].y, crdBuf[i].z);
	}
	points =  CoordinateSequence::AutoPtr(new CoordinateArraySequence(crds));
}

void ILWIS::Segment::Clip(const CoordBounds& cbClip,
                   Array<CoordBuf>& acrdBuf) const
{
	long iCrdOut;
	Coord cFirst, cNext;
	CoordBuf cBufOut;
	acrdBuf.Resize(0);                      // initialize the array of coord buffers
	CoordinateSequence *cBufIn = getCoordinates();
	cBufOut.Size(cBufIn->size());
	iCrdOut = 0;
	bool fLLCB; // true if line leaves clipping box
	for (long i = 1; i < cBufIn->size(); ++i ) {
		cFirst = cBufIn->getAt(i-1);
		cNext = cBufIn->getAt(i);
		fLLCB = ((cbClip.fContains(cFirst)) && (!cbClip.fContains(cNext))); // Line Leaves cb box
		if ( fClipLine(cbClip, cFirst, cNext))
		{
			cBufOut[iCrdOut].x = cFirst.x ;
			cBufOut[iCrdOut].y = cFirst.y ;
			iCrdOut++;
			// determine all parts of Segment inside the coordbounds cbClip
			// and for each part put the coordinates in cBufOut
			if (( fLLCB ) || (i == (cBufIn->size()-1)))
			{
				cBufOut[iCrdOut].x = cNext.x ;
				cBufOut[iCrdOut].y = cNext.y ;
				iCrdOut++;
				// for each part fill a new segment buffer:
				CoordBuf cb(iCrdOut);                 // define new ccordbuf of correct size

				for ( int j = 0; j < iCrdOut ; j++) {
					cb[j] = cBufOut[j];                 // and fill it with the coords after clipping
				}
				acrdBuf &= cb;                        // append new coordbuffer for new segment having
				// a size defined by iCrdOut
				iCrdOut = 0;
			} //
			// else   new segmentbuf not yet created
		}
	} // end for i = 1; i < iCrdIn
	delete cBufIn;
}

void ILWIS::Segment::Densify(const double rDistance, CoordBuf& crdBufOut) const
{
	CoordBuf crdBufTemp;
	long iCrdIn;                
	double rNewCoordsDistance = abs(rDistance);
	double dx, dy, rDx, rDy;
	CoordinateSequence *crdBufIn = getCoordinates(); 
	long iCrdOut = 0; // initialize counter of coords in new segment
	long iInsertablePoints;// per leg of 'old' segment
	long iInsertedPoints;  // counts for each 'old' leg again from 0	
	crdBufTemp.Size((int)(rLength()/rNewCoordsDistance) + crdBufIn->size());
	for (iCrdIn = 0; iCrdIn < crdBufIn->size() - 1; iCrdIn++) { 
		crdBufTemp[iCrdOut] = crdBufIn->getAt(iCrdIn); // copy the input-vertex coord to crdBufOut
		iCrdOut++;
		dx = crdBufIn->getAt(iCrdIn + 1).x - crdBufIn->getAt(iCrdIn).x;   // determine for each old leg
		dy = crdBufIn->getAt(iCrdIn + 1).y - crdBufIn->getAt(iCrdIn).y;   // its x and y components
		double rLegLength = sqrt(dx*dx + dy*dy) ;        // the length of this leg 
		if ( rLegLength < EPS10 ) continue;       // if its length ~ 0 it is skipped,
		rDx = rNewCoordsDistance * dx / rLegLength; // compute the new leg vector
		rDy = rNewCoordsDistance * dy / rLegLength; // components along that leg
		iInsertedPoints = 0;
		iInsertablePoints = (long)floor(rLegLength / rNewCoordsDistance);
		if (iInsertablePoints > 0 ) {                   // densification of current leg required
			while (iInsertedPoints < iInsertablePoints) {       
				crdBufTemp[iCrdOut].x = crdBufTemp[iCrdOut - 1].x + rDx;
				crdBufTemp[iCrdOut].y = crdBufTemp[iCrdOut - 1].y + rDy;
				iCrdOut++;
				iInsertedPoints++;	
			}
		}
	}
	crdBufTemp[iCrdOut] = crdBufIn->getAt(crdBufIn->size() - 1); //copy last vertex (end-node)
	iCrdOut++;
	crdBufOut.Size(iCrdOut);
	for (int i = 0; i < iCrdOut; i++) 
		crdBufOut[i] = crdBufTemp[i];
	delete crdBufIn;
}


bool ILWIS::Segment::fValid() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  return !fDeleted();
}

bool ILWIS::Segment::fInMask(const DomainValueRangeStruct& dvrs, const Mask& mask) const
{
  if (dvrs.fValues()) {
    return mask.fInMask(sValue(dvrs));
  }
  else
    return mask.fInMask(iValue());
}

double ILWIS::Segment::rLength() const
{
	return getLength();
}

void ILWIS::Segment::Split(long iAfter, Coord crdAt, CoordBuf& crdBufBefore, CoordBuf& crdBufAfter)
{
	CoordBounds cbBefore, cbAfter;
	int iSz = points->size();

	CoordinateSequence *crdBufCurrent = getCoordinates();

	cbBefore += crdAt;
	cbAfter += crdAt;
	crdBufBefore[iAfter + 1] = crdBufAfter[0] = crdAt;

	for(long i=0; i < iSz; ++i)
	{
		if (i <= iAfter)
		{
			cbBefore += crdBufCurrent->getAt(i);
			crdBufBefore[i] = crdBufCurrent->getAt(i);
		}			
		else
		{
			cbAfter += crdBufCurrent->getAt(i);
			crdBufAfter[i - iAfter] = crdBufCurrent->getAt(i);
		}

	}
	delete crdBufCurrent;
}



bool ILWIS::Segment::fSelfCheck(long& iFirst, long& iSecond, Coord& crdAt)
{
  CoordinateSequence * crdBuf = getCoordinates();
  if (crdBuf->size() == 1) {
    Delete(true);
	delete crdBuf;
    return true;
  }
  if (crdBuf->size() == 2 && crdBuf->getAt(0) == crdBuf->getAt(1)) {
    iFirst = -1;
	crdAt = crdBuf->getAt(0);
	delete crdBuf;
    return false;
  }
  Coord crdA1, crdA2, crdB1, crdB2, crdDA, crdDB;
  int a, b;
  double rCA, rCB, rD;
  CoordBounds cb;

  // first check double points: always auto correct!
  bool fChanged = false;
  CoordBuf crdBufTmp;//(1000);
	crdBufTmp.Size(crdBuf->size());
  int i, j = 1;
  crdBufTmp[0] = crdA1 = crdBuf->getAt(0);
  for (i = 1; i < crdBuf->size(); ++i) {
    crdA2 = crdBuf->getAt(i);
    if (crdA1 == crdA2) {
      fChanged = true;
      continue;
    }
    crdBufTmp[j++] = crdA1 = crdA2;
  }
  if (fChanged) {
    PutCoords(j,crdBufTmp);
	delete crdBuf;
    crdBuf = getCoordinates();
  }

  crdA2 = crdBuf->getAt(0);
  for (a = 1; a < crdBuf->size() - 1; ++a) {
    crdA1 = crdA2;
    crdA2 = crdBuf->getAt(a);
    cb = CoordBounds(crdA1, crdA2);
    cb.Check();
    crdDA = crdA2 - crdA1;
    rCA = crdA1.y * crdDA.x - crdA1.x * crdDA.y;
	crdB2 = crdBuf->getAt(a+1);
    for (b = a + 2; b < crdBuf->size(); ++b) {
      crdB1 = crdB2;
      crdB2 = crdBuf->getAt(b);
      if (crdB1.y > cb.MaxY() && crdB2.y > cb.MaxY())
        continue;
      if (crdB1.x > cb.MaxX() && crdB2.x > cb.MaxX())
        continue;
      if (crdB1.y < cb.MinY() && crdB2.y < cb.MinY())
        continue;
      if (crdB1.x < cb.MinX() && crdB2.x < cb.MinX())
        continue;
      crdDB = crdB2 - crdB1;
      rCB = crdB1.y * crdDB.x - crdB1.x * crdDB.y;
      rD = crdDA.x * crdDB.y - crdDB.x * crdDA.y;
      if (abs(rD) < 1E-6) { // parallel
        if (a == 1 && b == crdBuf->size() - 1) // Fst = Lst node
          continue;
        if (crdDA.x == 0) { // vertical
          if ((crdB2.y < crdA1.y) ^ (crdB2.y < crdA2.y)) {
            iFirst = -1;
            crdAt = crdB2;
            return false;
          }
          if ((crdA2.y < crdB1.y) ^ (crdA2.y < crdB2.y)) {
            iFirst = -1;
            crdAt = crdA2;
			delete crdBuf;
            return false;
          }
        }
        else {
          double r = crdA1.y + double(crdB2.x - crdA1.x) * crdDA.y / crdDA.x;
          if (abs(r - crdB2.y) < 1e-6) {
            iFirst = -1;
            crdAt = crdB2;
			delete crdBuf;
            return false;
          }
        }
      }
      else { // not parallel
        if (a == 1 && b == crdBuf->size() - 1 && crdA1 == crdB2) // closed segment
          continue;
        double rX, rY;
        rY = (rCA * crdDB.y - rCB * crdDA.y) / rD;
        if (rY < cb.MinY() - 1e-6 || rY > cb.MaxY() + 1e-6)
          continue;
        if (rY < crdB1.y && rY < crdB2.y)
          continue;
        if (rY > crdB1.y && rY > crdB2.y)
          continue;
        rX = (rCA * crdDB.x - rCB * crdDA.x) / rD;
        if (rX < cb.MinX()  - 1e-6 || rX > cb.MaxX() + 1e-6)
          continue;
        if (rX < crdB1.x && rX < crdB2.x)
          continue;
        if (rX > crdB1.x && rX > crdB2.x)
          continue;
        iFirst = a - 1;
        iSecond = b - 1;
        crdAt = Coord(rX, rY);
		delete crdBuf;
        return false;
      }
    }
  }
  delete crdBuf;
  return true;
}

bool ILWIS::Segment::fIntersects(bool& fOverlay, long& iAft, Coord& crdAt,
                          ILWIS::Segment* s, long& iAft2, Tranquilizer& trq)
{
  CoordinateSequence *crdBuf = getCoordinates();
  CoordinateSequence *crdBufS = s->getCoordinates();
  CoordBounds cbS = s->cbBounds();

  CoordBounds cbA, cbB;
  Coord crdA1, crdA2, crdB1, crdB2, crdDA, crdDB;
  int a, b;
  double rCA, rCB, rD;

  crdA2 = crdBuf->getAt(0);
  for (a = 1; a < crdBuf->size(); ++a) {
	  if (trq.fAborted()){
	  delete crdBuf;
	  delete crdBufS;
      return true;
	}
    crdA1 = crdA2;
    crdA2 = crdBuf->getAt(a);
    cbA = CoordBounds(crdA1, crdA2);
    cbA.Check();
    if (!cbA.fContains(cbS))
      continue;
    crdDA = crdA2 - crdA1;
    rCA = double(crdA1.y) * crdDA.x - double(crdA1.x) * crdDA.y;

	crdB2 = crdBufS->getAt(0);
	for (b = 1; b < crdBufS->size(); ++b) {
      crdB1 = crdB2;
	  crdB2 = crdBufS->getAt(b);
      if (a == 1) {
        if (b == 1)
          if (crdA1 == crdB1)  // node Fst Fst
            continue;
		if (b == crdBufS->size() - 1)
          if (crdA1 == crdB2)  // node Fst Lst
            continue;
      }
      if (a == crdBuf->size() - 1) {
        if (b == 1)
          if (crdA2 == crdB1)  // node Lst Fst
            continue;
		if (b == crdBufS->size() - 1)
          if (crdA2 == crdB2)  // node Lst Lst
            continue;
      }

      cbB = CoordBounds(crdB1, crdB2);
      cbB.Check();
      if (!cbB.fContains(cbA))
        continue;
      crdDB = crdB2 - crdB1;
      rCB = double(crdB1.y) * crdDB.x - double(crdB1.x) * crdDB.y;

      rD = double(crdDA.x) * crdDB.y - double(crdDB.x) * crdDA.y;
      if (abs(rD) < 1E-6) { // parallel
        if (crdDA.x == 0) { // vertical
          if ((crdB2.y < crdA1.y) ^ (crdB2.y < crdA2.y)) {
            fOverlay = true;
            iAft = a - 1;
            iAft2 = b - 1;
            crdAt = crdB2;
			delete crdBuf;
			delete crdBufS;
            return true;
          }
          if ((crdA2.y < crdB1.y) ^ (crdA2.y < crdB2.y)) {
            fOverlay = true;
            iAft = a - 1;
            iAft2 = b - 1;
            crdAt = crdA2;
			delete crdBuf;
			delete crdBufS;
            return true;
          }
        }
        else {
          double r = crdA1.y + double(crdB2.x - crdA1.x) * crdDA.y / crdDA.x;
          if (abs(r - crdB2.y) < 1e-6) {
            fOverlay = true;
            iAft = a - 1;
            iAft2 = b - 1;
            crdAt = crdB2;
			delete crdBuf;
			delete crdBufS;
            return true;
          }
        }
      }
      else { // not parallel
        double rY, rX;
        rY = (rCA * crdDB.y - rCB * crdDA.y) / rD;
        if ((rY < cbA.MinY() || rY > cbA.MaxY()) && cbA.height() > 0)
          continue;
        if (rY < crdB1.y && rY < crdB2.y)
          continue;
        if (rY > crdB1.y && rY > crdB2.y)
          continue;
        rX = (rCA * crdDB.x - rCB * crdDA.x) / rD;
        if ((rX < cbA.MinX() || rX > cbA.MaxX()) && cbA.width() > 0)
          continue;
        if (rX < crdB1.x && rX < crdB2.x)
          continue;
        if (rX > crdB1.x && rX > crdB2.x)
          continue;
        fOverlay = false;
        iAft = a - 1;
        iAft2 = b - 1;
        crdAt = Coord(rX, rY);
		delete crdBuf;
		delete crdBufS;
        return true;
      }
    }
  }
  delete crdBuf;
  delete crdBufS;
  return false;
}

Feature::FeatureType ILWIS::Segment::getType() const {
	return ftSEGMENT;
}

void ILWIS::Segment::getBoundaries(vector<geos::geom::CoordinateSequence*>& boundaries) const{
	boundaries.push_back(getCoordinates());

}

//-----[LSEGMENT]-----------------------------------------------------------------------
ILWIS::LSegment::LSegment(geos::geom::LineString *line) : ILWIS::Segment(line){

}

void ILWIS::LSegment::PutVal(long iRaw)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
 
  value = iRaw;
}

Geometry *ILWIS::LSegment::clone() const {
	LSegment *seg = new LSegment();
	seg->PutCoords(getCoordinates());
	seg->PutVal(value);
	return seg;
}

void ILWIS::LSegment::PutVal(double rVal)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  if ( rVal == rUNDEF)
	  value = iUNDEF;
  else
	value = rVal;	
}

void ILWIS::LSegment::PutVal(const DomainValueRangeStruct& dvs, const String& sV)
{
	if( dvs.fRawAvailable()) {
		value = dvs.iRaw(sV);
	}
	else {
		value = sV.iVal();
	}
}

long ILWIS::LSegment::iValue() const
{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	return value; 
}

double ILWIS::LSegment::rValue() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  if ( value == iUNDEF)
	return rUNDEF;
  return (double)value;
}

String ILWIS::LSegment::sValue(const DomainValueRangeStruct& dvrs, short iWidth, short iDec) const
{
   ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  if (dvrs.fUseReals()) 
     return dvrs.sValue(rValue(),0);
  else if (dvrs.dm().fValid())
     return dvrs.sValueByRaw(iValue(),0);
  else
     return sUNDEF;
}

void ILWIS::LSegment::segSplit(long iAfter, Coord crdAt, ILWIS::Segment **seg)
{
	CoordBuf cbBefore(iAfter + 2), cbAfter(points->size() - iAfter);
	Split(iAfter, crdAt, cbBefore, cbAfter);
	if ( seg != NULL) {
		(*seg)->PutCoords(cbAfter.iSize(), cbAfter);
		(*seg)->PutVal(iValue());
	}
	PutCoords(cbBefore.iSize(), cbBefore);
	
}

//---[RSEGMENT]---------------------------------------------------------
ILWIS::RSegment::RSegment(geos::geom::LineString *line) : ILWIS::Segment(line){
}

Geometry *ILWIS::RSegment::clone() const {
	RSegment *seg = new RSegment();
	seg->PutCoords(getCoordinates());
	seg->PutVal(value);
	return seg;
}

void ILWIS::RSegment::PutVal(long iRaw)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
 
  value = iRaw;
}

void ILWIS::RSegment::PutVal(double rVal)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  if ( rVal == iUNDEF)
	  value = rUNDEF;
  else
	value = rVal;	
}

void ILWIS::RSegment::PutVal(const DomainValueRangeStruct& dvs, const String& sV)
{
	value = sV.rVal();
}

long ILWIS::RSegment::iValue() const
{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	if ( value == rUNDEF)
		return iUNDEF;
	return (long)value; 
}

double ILWIS::RSegment::rValue() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
  return value;
}

String ILWIS::RSegment::sValue(const DomainValueRangeStruct& dvs, short iWidth, short iDec) const
{
  return String("%f",value);
}

void ILWIS::RSegment::segSplit(long iAfter, Coord crdAt, ILWIS::Segment **seg)
{
	CoordBuf crdBufBefore(iAfter + 2), crdBufAfter(points->size() - iAfter);
	Split(iAfter, crdAt, crdBufBefore, crdBufAfter);
	if ( seg != NULL) {
		(*seg)->PutCoords(crdBufAfter.iSize(), crdBufAfter);
		(*seg)->PutVal(rValue());
	}
	PutCoords(crdBufBefore.iSize(), crdBufBefore);
	
}

