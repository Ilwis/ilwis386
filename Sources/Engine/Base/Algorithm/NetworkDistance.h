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
 Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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

 Created on: 2012-10-09
 ***************************************************************/

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