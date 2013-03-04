#pragma once

#include "Engine\Map\Feature.h"
#include <Geos.h>

namespace geos {
	namespace index {
		namespace QuadTree{
		class QuadTree;
		}
	}
	namespace geom{
		class GeometryFactory;
	}
}
class Mask;

namespace ILWIS {

	class _export Point : public geos::geom::Point, public Feature {
public:
	Point(QuadTree *tree,const geos::geom::Point *pnt=NULL);
	Point(QuadTree *tree, const Coord& crdnew ,GeometryFactory *);
	Point(QuadTree *tree, CoordinateSequence *seq);
	~Point();
	static CoordinateSequence* create(const Coord& c);
	void setCoord(const Coord& c);
	virtual Geometry *clone()const=0;
	virtual bool fValid() const;
	virtual FeatureType getType() const;
	virtual	void getBoundaries(vector<CoordinateSequence*>& boundaries) const;
};

class _export LPoint : public Point {
public:
	LPoint(QuadTree *tree, geos::geom::Point *pnt=NULL);
	LPoint(QuadTree *tree, const Coord& crd, long v, GeometryFactory *);
	LPoint(QuadTree *tree, CoordinateSequence *seq, long v);
	~LPoint();
	String sValue(const DomainValueRangeStruct& dvs, short iWidth=-1, short iDec=-1) const;
	long iValue() const;
	double rValue() const;
    void PutVal(long iV);
	void PutVal(double rV);
    void PutVal(const DomainValueRangeStruct& dvs, const String &sVal);
	Geometry *clone() const;
	bool fInMask(const DomainValueRangeStruct& dvs, const Mask& mask) const;

	private:
		long value;
};

class _export RPoint : public Point {
public:
	RPoint(QuadTree *tree, geos::geom::Point *pnt=NULL);
	RPoint(QuadTree *tree, const Coord& crd, double v, GeometryFactory *);
	~RPoint();
	String sValue(const DomainValueRangeStruct& dvs, short iWidth=-1, short iDec=-1) const;
	double rValue() const;
	long iValue() const;
    void PutVal(double rV);
	void PutVal(long iV);
    void PutVal(const DomainValueRangeStruct& dvs,const String &sVal);
	Geometry *clone() const;
	bool fInMask(const DomainValueRangeStruct& dvs, const Mask& mask) const;

	private:
		double value;
};
};