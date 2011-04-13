#pragma once

#include "Engine\Map\Feature.h"
#include "Geos\Geos.h"

namespace geos {
	namespace index {
		namespace quadtree{
		class Quadtree;
		}
	}
}
class Mask;

namespace ILWIS {

	class _export Point : public geos::geom::Point, public Feature {
public:
	Point(geos::index::quadtree::Quadtree *tree,const geos::geom::Point *pnt=NULL);
	Point(geos::index::quadtree::Quadtree *tree, const Coord& crd);
	Point(geos::index::quadtree::Quadtree *tree, CoordinateSequence *seq);
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
	LPoint(geos::index::quadtree::Quadtree *tree, geos::geom::Point *pnt=NULL);
	LPoint(geos::index::quadtree::Quadtree *tree, const Coord& crd, long v);
	LPoint(geos::index::quadtree::Quadtree *tree, CoordinateSequence *seq, long v);
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
	RPoint(geos::index::quadtree::Quadtree *tree, geos::geom::Point *pnt=NULL);
	RPoint(geos::index::quadtree::Quadtree *tree, const Coord& crd, double v);
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