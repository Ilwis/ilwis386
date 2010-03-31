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
// $Log: /ILWIS 3.0/PolygonMap/POLYGON.cpp $
 * 
 * 13    8-09-04 18:16 Hendrikse
 * inserted a safer  if (iPtAmount > 99)  in the function
 * :crdFindPointInPol() const    to debug b6550  , to prevent overflow of
 * double rPt[100] and rDist[100]
 * 
 * 12    7-06-04 16:35 Retsios
 * Improved readability of function PolygonTable()
 * 
 * 11    16/03/01 16:21 Willem
 * Replaced the long with a double in FindInterSect() function.
 * 
 * 10    13-03-01 11:40 Koolhoven
 * TopStart() checked the wrong column to determine if it was deleted
 * 
 * 9     25-07-00 10:28a Martin
 * added acces function and member to indicate that a polygonmap is (non)
 * topological
 * 
 * 8     19-06-00 11:30 Koolhoven
 * replaced reference to SegmentMapStore::ct... with PolygonMapStore::ct..
 * 
 * 7     20-01-00 10:49a Martin
 * uses columPtr to acces data
 * 
 * 6     17-01-00 11:22a Martin
 * TableVector removed, now PolygonMapStore
 * 
 * 5     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 4     10-12-99 11:50a Martin
 * removed internal coordinates and replaced them by true coords
 * 
 * 3     9/08/99 1:11p Wind
 * comments
 * 
 * 2     9/08/99 11:57a Wind
 * comments
*/
// Revision 1.4  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.3  1997/08/04 15:04:19  Wim
// Use string for mask with all value maps
//
// Revision 1.2  1997-08-04 16:41:44+02  Wim
// Added ILWIS::Polygon::fInMask(mask)
//
/* ILWIS::Polygon
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    4 Aug 97    5:04 pm
*/

#include "Engine\Base\AssertD.h"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\Algorithm\Clipline.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Headers\geos\algorithm\InteriorPointArea.h"
#include "Headers\Hs\polygon.hs"

// for the time being the interface works with the 1.4 datastructures
// this will soon change to a more structured tableinterface

//------------------------------------------------------------

ILWIS::Polygon::Polygon() : geos::geom::Polygon(NULL,new vector<Geometry *>(),new GeometryFactory(new PrecisionModel()))
{
}

ILWIS::Polygon::Polygon(geos::geom::Polygon *pol) :  
geos::geom::Polygon(NULL,new vector<Geometry *>(),new GeometryFactory(new PrecisionModel())) 
{
	if ( pol != NULL) {
		addBoundary(makeRing(pol->getExteriorRing()));
		for(int island=0; island < pol->getNumInteriorRing(); ++island) {
			addHole(makeRing(pol->getInteriorRingN(island)));
		}
	}
}

LinearRing *ILWIS::Polygon::makeRing(const LineString *line) const {
	CoordinateSequence *seq = line->getCoordinates();
	return new LinearRing(seq, new GeometryFactory());
}

void ILWIS::Polygon::addBoundary(LinearRing *ring) {
	if ( shell != NULL)
		delete shell;
	shell = ring;
}

void ILWIS::Polygon::addHole(LinearRing * ring) {
	for(int i = 0; i < holes->size(); ++i) {
		LinearRing *r = (LinearRing *)holes->at(i);
		if ( r->equalsExact(ring))
			throw ErrorObject("Adding duplicate hole to polygon");
	}
	holes->push_back(ring);
}


CoordBounds ILWIS::Polygon::cbBounds() const // bounding rectangle
{
	ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
	Geometry *geom = getEnvelope();
	CoordinateSequence *seq = geom->getCoordinates();
	CoordBounds cb;
	for(int i = 0; i < seq->size(); ++i) {
		Coord crd(seq->getAt(i));
		cb += crd;
	}
	delete seq;
	return cb;	
}

double ILWIS::Polygon::rArea() const {
	return getArea();
}

double ILWIS::Polygon::rLen() const{
	return getLength();
}

bool ILWIS::Polygon::fValid() const
{
	return shell != NULL && shell->getNumPoints() > 2;  
}

bool ILWIS::Polygon::fInMask(const DomainValueRangeStruct& dvrs, const Mask& mask) const
{
  if (dvrs.fValues()) {
    String sVal = sValue(dvrs);
    return mask.fInMask(sVal);
  }
  else
    return mask.fInMask(iValue());
}

bool ILWIS::Polygon::fContains(Coord crd) const
{
	ILWIS::LPoint pnt(crd,iUNDEF);
	return contains(&pnt);
}


Coord	 ILWIS::Polygon::crdGravity() const // center of gravity
{
  Coord crd;
  CoordBounds cb = cbBounds();
  crd.y = cb.MinY() / 2 + cb.MaxY() / 2;
  crd.x = cb.MinX() / 2 + cb.MaxX() / 2;
  return crd;
}

Coord ILWIS::Polygon::crdFindPointInPol2() const {
	const geos::geom::LineString *ring = getExteriorRing();
	const CoordinateSequence *seq = ring->getCoordinates();
	if ( seq->size() < 3){ 
		delete seq;
		return Coord();
	}
	Coord c1 = seq->getAt(0);
	delete seq;
	Coord c2(crdFindPointInPol());
	if ( (c1.x - c2.x) == 0) {
		return Coord(c1.x, c1.y + 0.01 *( c2.y - c1.y));
	}
	double a = (c1.y - c2.y) / (c1.x - c2.x);
	double b = c1.y - (a * c1.x);
	double dx = c1.x + 0.01 * ( c2.x - c1.x);
	Coord c3(dx, a *dx + b);
	return c3;

}

Coord ILWIS::Polygon::crdFindPointInPol() const {   
	return Coord(*(getInteriorPoint()->getCoordinate()));
}

Geometry * ILWIS::Polygon::copy(ILWIS::Polygon *p) const {
	const geos::geom::LineString *ring = getExteriorRing();
	p->addBoundary(new LinearRing(ring->getCoordinates(), new GeometryFactory()));
	for(int i = 0; i < getNumInteriorRing(); ++i) {
		ring = getInteriorRingN(i);
		p->addHole(new LinearRing(ring->getCoordinates(), new GeometryFactory()));
	}
	return p;
}

Feature::FeatureType ILWIS::Polygon::getType() const {
	return ftPOLYGON;
}


//--------[ILWIS::LPolygon]--------------------------------------------------------
ILWIS::LPolygon::LPolygon(geos::geom::Polygon *pol) : ILWIS::Polygon(pol), value(0) {
}

Geometry * ILWIS::LPolygon::clone() const{
	ILWIS::LPolygon *p = new ILWIS::LPolygon();
	p = (ILWIS::LPolygon *)copy(p);
	p->PutVal(value);
	return p;
}

void ILWIS::LPolygon::PutVal(long iRaw)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
 
  value = iRaw;
}

void ILWIS::LPolygon::PutVal(double rVal)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  if ( rVal == rUNDEF)
	  value = iUNDEF;
  else
	value = rVal;	
}

void ILWIS::LPolygon::PutVal(const DomainValueRangeStruct& dvs, const String& sV)
{
	if( dvs.fRawAvailable()) {
		value = dvs.iRaw(sV);
	}
	else {
		value = sV.iVal();
	}
}

long ILWIS::LPolygon::iValue() const
{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
	return value; 
}

double ILWIS::LPolygon::rValue() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  if ( value == iUNDEF)
	  return rUNDEF;
  return (double)value;
}

String ILWIS::LPolygon::sValue(const DomainValueRangeStruct& dvrs, short iWidth, short iDec) const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  if (dvrs.fUseReals()) 
     return dvrs.sValue(rValue(),iWidth);
  else if (dvrs.dm().fValid())
     return dvrs.sValueByRaw(iValue(),iWidth,iDec);
  else
     return sUNDEF;
}

//---[ILWIS::RPolygon]-------------------------------------------------
ILWIS::RPolygon::RPolygon(geos::geom::Polygon *pol) : ILWIS::Polygon(pol), value(0) {
}
void ILWIS::RPolygon::PutVal(long iRaw)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  if ( iRaw == iUNDEF)
	  value = rUNDEF;
  else
	value = iRaw;
}

void ILWIS::RPolygon::PutVal(double rVal)
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  
  value = rVal;	
}

void ILWIS::RPolygon::PutVal(const DomainValueRangeStruct& dvs, const String& sV)
{
	if( dvs.fRawAvailable()) {
		value = dvs.iRaw(sV);
	}
	else {
		value = sV.rVal();
	}
}

long ILWIS::RPolygon::iValue() const
{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
	if ( value == rUNDEF)
		return iUNDEF;
	return (long)value; 
}

double ILWIS::RPolygon::rValue() const
{
  ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  return value;
}

String ILWIS::RPolygon::sValue(const DomainValueRangeStruct& dvrs, short iWidth, short iDec) const
{
   ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE);
  if (dvrs.fUseReals()) 
     return dvrs.sValue(rValue(),iWidth);
  else if (dvrs.dm().fValid())
     return dvrs.sValueByRaw(rValue() + 1,iWidth,iDec);
  else
     return sUNDEF;
}

Geometry * ILWIS::RPolygon::clone() const{
	ILWIS::RPolygon *p = new ILWIS::RPolygon();
	p = (ILWIS::RPolygon *)copy(p);
	p->PutVal(value);
	return p;
}

