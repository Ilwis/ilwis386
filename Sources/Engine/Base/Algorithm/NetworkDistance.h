#ifndef NETWORKDISTANCE_H
#define NETWORKDISTANCE_H

#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"

#include <geos/index/quadtree/Quadtree.h>

class _export NetworkDistance
{
public:
  NetworkDistance(const PointMap& _pmFrom, const PointMap& _pmTo, const SegmentMap& _smVia, const bool fProject, Table2DimPtr & ptr, Tranquilizer & trq);
  NetworkDistance(const PointMap& _pmFrom, const PointMap& _pmTo, const SegmentMap& _smVia, const bool fProject, Table2DimPtr & ptr, SegmentMap & segMap, Domain & dm, Tranquilizer & trq);
  ~NetworkDistance();

private:
  void projectOnNetwork(const PointMap& pmFrom, const PointMap& pmTo, geos::index::quadtree::Quadtree & spatialIndex, double rInitialSearchRadius);
  LineString * computeProjection(const Geometry * pointNotOnNetworkNode, geos::index::quadtree::Quadtree & spatialIndex, double rInitialSearchRadius);
  const PointMap& pmFrom;
  const PointMap& pmTo;
  const SegmentMap& smVia;
};

class SortCoordinates {
public:
	bool operator()(const Coordinate & left, const Coordinate & right) const {
		return left.compareTo(right) < 0;
	}
};

class SortByVector { 
public:
	SortByVector(vector<pair<double, long>> & _values)
	: values(_values)
	{
	}

	bool operator()(const long &left, const long &right) const { 
	    return values[left].first > values[right].first; // move smallest value to the end, for easy pop_back()
	} 
private:
	vector<pair<double, long>> & values;
};

class Dijkstra
{
public:
	Dijkstra(vector<LineString*> & _vlsVia);
	double getNetworkDistance(const Geometry *pointTo);
	CoordinateArraySequence * getShortestPath(const Geometry *pointTo);
	void init(const Geometry *pointFrom); 
private:
	map<Coordinate, long, SortCoordinates> nodeNames;
	vector<Coordinate> coordinateList;
	multimap<long, pair<long, double>> graph;
	vector<pair<double, long>> final_distances;
	bool fValid;
};

#endif // NETWORKDISTANCE_H