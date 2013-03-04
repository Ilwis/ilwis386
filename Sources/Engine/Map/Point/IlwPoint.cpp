#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\mask.h"
#include "Engine\Map\Point\ilwPoint.h"
#include <geos/geom/Envelope.h>

#define _HAS_ITERATOR_DEBUGGING 0

using namespace ILWIS;

ILWIS::Point::Point(QuadTree *tree, const geos::geom::Point *pnt) : geos::geom::Point( ILWIS::Point::create(Coord()), new GeometryFactory(new PrecisionModel())), Feature(tree){
	if ( pnt) {
		Coordinate *crd = const_cast<Coordinate *>(getCoordinate());
		crd->x = pnt->getCoordinate()->x;
		crd->y = pnt->getCoordinate()->y;
		crd->z = pnt->getCoordinate()->z;
		envelope = computeEnvelopeInternal();
		if ( spatialIndex)
			spatialIndex->insert(this);
	}
}

ILWIS::Point::~Point() {
}
ILWIS::Point::Point(QuadTree *tree,const Coord& c, GeometryFactory *fact)  : geos::geom::Point( ILWIS::Point::create(c), fact), Feature(tree)
{
	envelope = computeEnvelopeInternal();
	if ( spatialIndex && !c.fUndef())
		spatialIndex->insert(this);
}

ILWIS::Point::Point(QuadTree *tree, CoordinateSequence *seq) : 
geos::geom::Point(seq,new GeometryFactory(new PrecisionModel())),
Feature(tree)
{
	envelope = computeEnvelopeInternal();
	if ( spatialIndex)
		spatialIndex->insert(this);
}

CoordinateSequence* ILWIS::Point::create(const Coord& c) {
	vector<Coordinate> *crds = new vector<Coordinate>(1);
	crds->at(0) = c;
	CoordinateSequence *seq = new CoordinateArraySequence(crds);
	return seq;
}

void ILWIS::Point::setCoord(const Coord& c) {
	if ( spatialIndex)
		spatialIndex->remove(this);
	const Coordinate *c1 = getCoordinate();
	Coordinate *crd = const_cast<Coordinate *>(c1);
	crd->x = c.x;
	crd->y = c.y;
	crd->z = c.z;
	spatialIndex->insert(this);
}

bool ILWIS::Point::fValid() const {
	Coord *crd = (Coord*)(getCoordinate());
	return !crd->fUndef();
}

Feature::FeatureType ILWIS::Point::getType() const {
	return ftPOINT;
}

void ILWIS::Point::getBoundaries(vector<CoordinateSequence*>& boundaries) const{
	boundaries.push_back(getCoordinates());
}

//--------------------------------------------------------------
ILWIS::LPoint::LPoint(QuadTree *tree, geos::geom::Point *pnt) : ILWIS::Point(tree, pnt), value(iUNDEF) {
	
}

ILWIS::LPoint::LPoint(QuadTree *tree, CoordinateSequence *seq, long v) : ILWIS::Point(tree, seq) {
}

ILWIS::LPoint::LPoint(QuadTree *tree, const Coord& c, long v, GeometryFactory *fact=0) : ILWIS::Point(tree, c,fact), value(v) {
}

ILWIS::LPoint::~LPoint() {
}

long ILWIS::LPoint::iValue() const {
	return value;
}

double ILWIS::LPoint::rValue() const {
	if ( value == iUNDEF)
		return rUNDEF;
	return value;
}

 String ILWIS::LPoint::sValue(const DomainValueRangeStruct& dvs, short iWidth, short iDec) const {
	if ( iWidth == 0)
		return String("%d",value);
	if ( iWidth < 0)
		iWidth = dvs.dm()->iWidth();
	return  String("%*d", -iWidth, value);
}

 void ILWIS::LPoint::PutVal(long iV) {
	value = iV;
}

void ILWIS::LPoint::PutVal(double rV) {
	if ( rV == rUNDEF)
		value = iUNDEF;
	else
		value = (long)rV;
}

 void ILWIS::LPoint::PutVal(const DomainValueRangeStruct& dvs, const String &sVal){
	 value = dvs.iRaw(sVal);
  }

 Geometry *ILWIS::LPoint::clone() const {
	 ILWIS::LPoint *p = new ILWIS::LPoint(spatialIndex, *getCoordinate(), iValue(), 0);
	 return p;
 }

bool ILWIS::LPoint::fInMask(const DomainValueRangeStruct& dvs, const Mask& mask) const
{
	return mask.fInMask(iValue());
}

//-------------------------------------------------------------------
 ILWIS::RPoint::RPoint(QuadTree *tree,geos::geom::Point *pnt) : ILWIS::Point(tree, pnt), value(rUNDEF) {

 }
 ILWIS::RPoint::RPoint(QuadTree *tree,const Coord& c, double v, GeometryFactory *fact) : ILWIS::Point(tree, c, fact), value(v) {
}

ILWIS::RPoint::~RPoint() {
}

double ILWIS::RPoint::rValue() const {
	return value;
}

long ILWIS::RPoint::iValue() const {
	if ( value == rUNDEF)
		return iUNDEF;
	return value;
}

 String ILWIS::RPoint::sValue(const DomainValueRangeStruct& dvs, short iWidth, short iDec) const {
	if ( iWidth == 0)
		return String("%f",value);
	if ( iWidth < 0)
		iWidth = dvs.dm()->iWidth();
	return  String("%*f", -iWidth, value);
}

 void ILWIS::RPoint::PutVal(double rV) {
	value = rV;
}

 void ILWIS::RPoint::PutVal(long iV) {
	 if ( iV == iUNDEF)
		 value = rUNDEF;
	 else
		value = (double)iV;
}

 void ILWIS::RPoint::PutVal(const DomainValueRangeStruct& dvs, const String &sVal){
	 value = sVal.rVal(); 
}

Geometry *ILWIS::RPoint::clone() const {
	ILWIS::RPoint *p = new ILWIS::RPoint(spatialIndex, *getCoordinate(), rValue(), 0);
	return p;
}

bool ILWIS::RPoint::fInMask(const DomainValueRangeStruct& dvs, const Mask& mask) const
{
    String sVal = sValue(dvs);
    return mask.fInMask(sVal);
}

