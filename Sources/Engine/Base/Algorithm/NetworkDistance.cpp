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

#include "NetworkDistance.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "geos\headers\geos\algorithm\distance\DistanceToPoint.h"
#include "geos\headers\geos\algorithm\distance\PointPairDistance.h"
#include "geos\headers\geos\linearref\LengthIndexedLine.h"

NetworkDistance::NetworkDistance(const PointMap& _pmFrom, const PointMap& _pmTo, const SegmentMap& _smVia, const bool fProject, Table2DimPtr & ptr, Tranquilizer & trq)
: pmFrom(_pmFrom)
, pmTo(_pmTo)
, smVia(_smVia)
{
	geos::index::quadtree::Quadtree spatialIndex;
	for (int i=0; i<smVia->iFeatures(); ++i) {
		Geometry * geom = smVia->getFeature(i);
		spatialIndex.insert(geom->getEnvelopeInternal(), geom);
	}

	const long iFromFeatures = pmFrom->iFeatures();
	const long iToFeatures = pmTo->iFeatures();
	const long iterations = min(iFromFeatures, iToFeatures) + (fProject ? 1 : 0);
	long step = 0;

	trq.fUpdate(step++, iterations);

	if (fProject) {
		double rInitialSearchRadius = sqrt(smVia->cb().width() * smVia->cb().width() + smVia->cb().height() * smVia->cb().height()) / 100.0 / sqrt(2.0);
		projectOnNetwork(pmFrom, pmTo, spatialIndex, rInitialSearchRadius);
		trq.fUpdate(step++, iterations);
	}

	vector<LineString*> * vlsVia = (vector<LineString*> *)spatialIndex.queryAll();
	Dijkstra dijkstra (*vlsVia);
	delete vlsVia;

	if (iFromFeatures < iToFeatures) {
		for(long i = 0; i < iFromFeatures; ++i) {
			const Geometry * geomFrom = pmFrom->getFeature(i);
			dijkstra.init(geomFrom);
			for(long j = 0; j < iToFeatures; ++j) {
				const Geometry * geomTo = pmTo->getFeature(j);
				ptr.PutVal(i+1, j+1, dijkstra.getNetworkDistance(geomTo));
			}
			trq.fUpdate(step++, iterations);
		}
	} else {
		for(long j = 0; j < iToFeatures; ++j) {
			const Geometry * geomTo = pmTo->getFeature(j);
			dijkstra.init(geomTo);
			for(long i = 0; i < iFromFeatures; ++i) {
				const Geometry * geomFrom = pmFrom->getFeature(i);
				ptr.PutVal(i+1, j+1, dijkstra.getNetworkDistance(geomFrom));
			}
			trq.fUpdate(step++, iterations);
		}
	}
}

NetworkDistance::NetworkDistance(const PointMap& _pmFrom, const PointMap& _pmTo, const SegmentMap& _smVia, const bool fProject, Table2DimPtr & ptr, SegmentMap & segMap, Domain & dm, Tranquilizer & trq)
: pmFrom(_pmFrom)
, pmTo(_pmTo)
, smVia(_smVia)
{
	geos::index::quadtree::Quadtree spatialIndex;
	for (int i=0; i<smVia->iFeatures(); ++i) {
		Geometry * geom = smVia->getFeature(i);
		spatialIndex.insert(geom->getEnvelopeInternal(), geom);
	}

	const long iFromFeatures = pmFrom->iFeatures();
	const long iToFeatures = pmTo->iFeatures();
	const long iterations = min(iFromFeatures, iToFeatures) + (fProject ? 1 : 0);
	long step = 0;

	trq.fUpdate(step++, iterations);

	if (fProject) {
		double rInitialSearchRadius = sqrt(smVia->cb().width() * smVia->cb().width() + smVia->cb().height() * smVia->cb().height()) / 100.0 / sqrt(2.0);
		projectOnNetwork(pmFrom, pmTo, spatialIndex, rInitialSearchRadius);
		trq.fUpdate(step++, iterations);
	}

	DomainUniqueID * pdUid = dm->pdUniqueID();
	long iSegRecord = 0; // increase before adding first segment; segment count starts at 1

	vector<LineString*> * vlsVia = (vector<LineString*> *)spatialIndex.queryAll();
	Dijkstra dijkstra (*vlsVia);
	delete vlsVia;

	if (iFromFeatures < iToFeatures) {
		for(long i = 0; i < iFromFeatures; ++i) {
			const Geometry * geomFrom = pmFrom->getFeature(i);
			dijkstra.init(geomFrom);
			for(long j = 0; j < iToFeatures; ++j) {
				const Geometry * geomTo = pmTo->getFeature(j);
				ptr.PutVal(i+1, j+1, dijkstra.getNetworkDistance(geomTo));
				CoordinateArraySequence * coordinates = dijkstra.getShortestPath(geomTo);
				if (coordinates != 0) {
					pdUid->iAdd();
					ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
					segCur->PutCoords(coordinates); // segCur becomes the "owner" of coordinates
					segCur->PutVal(++iSegRecord);
				}
			}
			trq.fUpdate(step++, iterations);
		}
	} else {
		for(long j = 0; j < iToFeatures; ++j) {
			const Geometry * geomTo = pmTo->getFeature(j);
			dijkstra.init(geomTo);
			for(long i = 0; i < iFromFeatures; ++i) {
				const Geometry * geomFrom = pmFrom->getFeature(i);
				ptr.PutVal(i+1, j+1, dijkstra.getNetworkDistance(geomFrom));
				CoordinateArraySequence * coordinates = dijkstra.getShortestPath(geomFrom);
				if (coordinates != 0) {
					pdUid->iAdd();
					ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
					segCur->PutCoords(coordinates); // segCur becomes the "owner" of coordinates
					segCur->PutVal(++iSegRecord);
				}
			}
			trq.fUpdate(step++, iterations);
		}
	}
}

NetworkDistance::~NetworkDistance()
{
}

LineString * NetworkDistance::computeProjection(const Geometry * pointNotOnNetworkNode, geos::index::quadtree::Quadtree & spatialIndex, double rInitialSearchRadius)
{
	Coordinate crd = *(pointNotOnNetworkNode->getCoordinate());
	double rDistNearest = rUNDEF;
	LineString * lsNearest = 0;
	geos::algorithm::distance::PointPairDistance ptDistNearest;

	vector<void *> lineStrings;
	double rSearchRadius = rInitialSearchRadius;
	while (rDistNearest == rUNDEF) {
		geos::geom::Envelope env(crd.x - rSearchRadius, crd.x + rSearchRadius, crd.y - rSearchRadius, crd.y + rSearchRadius);
		spatialIndex.query(&env, lineStrings);
		for(int i = 0; i < lineStrings.size(); ++i) {
			LineString * ls = (LineString*)lineStrings[i];
			if (!ls)
				continue;
			geos::algorithm::distance::PointPairDistance ptDist;
			geos::algorithm::distance::DistanceToPoint::computeDistance(*ls, crd, ptDist);
			double distance = ptDist.getDistance();
			if (distance > rSearchRadius)
				continue;
			if (rDistNearest == rUNDEF || rDistNearest > distance) {
				rDistNearest = distance;
				lsNearest = ls;
				ptDistNearest = ptDist;
			}
			if (distance == 0.0)
				break;
		}
		rSearchRadius *= 2;
	}

	vector<Coordinate> ptsProjectionLine;
	ptsProjectionLine.push_back(crd);
	geos::linearref::LengthIndexedLine lineNearest (lsNearest); // index it
	double projectedPos = lineNearest.indexOf(ptDistNearest.getCoordinate(0));
	if (projectedPos == lineNearest.getStartIndex())
		ptsProjectionLine.push_back(*(lsNearest->getCoordinate()));
	else if (projectedPos == lineNearest.getEndIndex())
		ptsProjectionLine.push_back(lsNearest->getCoordinateN(lsNearest->getNumPoints() - 1));
	else {
		LineString * AP = (LineString*)lineNearest.extractLine(lineNearest.getStartIndex(), projectedPos);
		LineString * PB = (LineString*)lineNearest.extractLine(projectedPos, lineNearest.getEndIndex());
		spatialIndex.remove(lsNearest->getEnvelopeInternal(), lsNearest); // remove the large segment
		spatialIndex.insert(AP->getEnvelopeInternal(), AP); // add the first split part
		spatialIndex.insert(PB->getEnvelopeInternal(), PB); // add the second split part
		ptsProjectionLine.push_back(*(PB->getCoordinate())); // add the first point of the second split part to PP
	}
	LineString * PP = 0;
	if (rDistNearest != rUNDEF && rDistNearest != 0) {
		CoordinateSequence * csProjectionLine = lsNearest->getFactory()->getCoordinateSequenceFactory()->create(&ptsProjectionLine);
		PP = lsNearest->getFactory()->createLineString(csProjectionLine->clone());
	}

	return PP; // add the projected part at a later stage, so that it is not accidentally included in computation of projections
}

void NetworkDistance::projectOnNetwork(const PointMap& pmFrom, const PointMap& pmTo, geos::index::quadtree::Quadtree & spatialIndex, double rInitialSearchRadius)
{
	TRACE("Computing projections;\n");
	clock_t start = clock();

	vector<LineString*> projectionLines;
	map<Coordinate, long, SortCoordinates> processedNodes;
	for(long i = 0; i < pmFrom->iFeatures(); ++i) {
		const Geometry * geomFrom = pmFrom->getFeature(i);
		const Coordinate & crd = *(geomFrom->getCoordinate());
		map<Coordinate, long, SortCoordinates>::iterator node = processedNodes.find(crd);
		if (node == processedNodes.end()) {
			LineString * PP = computeProjection(geomFrom, spatialIndex, rInitialSearchRadius);
			if (PP != 0)
				projectionLines.push_back(PP);
			processedNodes[crd] = 1;
		}
	}
	for (long i = 0; i < pmTo->iFeatures(); ++i) {
		const Geometry * geomTo = pmTo->getFeature(i);
		const Coordinate & crd = *(geomTo->getCoordinate());
		map<Coordinate, long, SortCoordinates>::iterator node = processedNodes.find(crd);
		if (node == processedNodes.end()) {
			LineString * PP = computeProjection(geomTo, spatialIndex, rInitialSearchRadius);
			if (PP != 0)
				projectionLines.push_back(PP);
			processedNodes[crd] = 1;
		}
	}

	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Computed projections in %2.2f milliseconds;\n", duration);

	for (long i = 0; i < projectionLines.size(); ++i)
		spatialIndex.insert(projectionLines[i]->getEnvelopeInternal(), projectionLines[i]);
}

Dijkstra::Dijkstra(vector<LineString*> & vlsVia)
: fValid(false)
{
	clock_t start = clock();

	long nodes = 0;
	// first find the nr of distinct nodes in the network
	for (long i = 0; i < vlsVia.size(); ++i) {
		LineString * ls = vlsVia[i];
		const CoordinateSequence* lineStringNodes = ls->getCoordinatesRO();
		Coordinate crdFrom;
		Coordinate crdTo;
		long nodeFrom;
		long nodeTo;
		for (long j = 0; j < lineStringNodes->size(); ++j) {
			crdTo = lineStringNodes->getAt(j);
			map<Coordinate, long, SortCoordinates>::iterator it = nodeNames.find(crdTo);
			if (it == nodeNames.end()) {
				nodeTo = nodes++;
				nodeNames[crdTo] = nodeTo;
				coordinateList.push_back(crdTo);
			} else
				nodeTo = it->second;
			if (j > 0 && nodeFrom != nodeTo) { // if we wanted to walk from A to A, the distance covered is 0 .. for now i'd say that if this occurs, it is an error in the segment-map with the road-network
				double weight = 1.0;
				double dx = crdTo.x - crdFrom.x;
				double dy = crdTo.y - crdFrom.y;
				double length = sqrt(dx * dx + dy * dy);
				graph.insert(pair<long, pair<long, double>>(nodeFrom, pair<long, double>(nodeTo, length * weight)));
				graph.insert(pair<long, pair<long, double>>(nodeTo, pair<long, double>(nodeFrom, length * weight))); // redundancy, but not for 1-way streets
			}
			crdFrom = crdTo;
			nodeFrom = nodeTo;
		}
	}

	final_distances.resize(nodes);

	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Dijkstra-init run in %2.2f milliseconds;\n", duration);

}

	/*

	Dijkstra pseudo, reference http://nl.wikipedia.org/wiki/Kortstepad-algoritme

	foreach v  V(G) do d(v) = rUNDEF;
	A := startpoint
	d(a) := 0
	X := []
	foreach z : z directly connected to startpoint do 
		X := X union {z}
		d(z) := weighted_distance(startpoint, z); 
		//
		// X en d zijn nu geïnitialiseerd
		//
	while not(X = []) do
	//
	// X is nog niet leeg
	//
		y : (y member_of X)  (d(y) = MIN {d(y')|y' member_of X}
		//
		// y is dus het element van X met de laagste waarde van d(v) -- dit is de definitieve waarde van d(y)
		//
		A := A union {y}
		X := X\{y}
		//
		// y is nu verplaatst van X naar A
		//
			foreach z: z directly connected to (y) and not (z member_of A) do 
				if not (z member_of X) then
					X := X union {z} 
					d(z) := d(y) + weighted_distance(y ,z)
				else 
					//
					// dus z member_of X
					// 
					d(z) := MIN{d(z), d(y) + weighted_distance(y, z)}
	*/

void Dijkstra::init(const Geometry *pointFrom)
{
	fValid = false;
	map<Coordinate, long, SortCoordinates>::iterator nodeFrom = nodeNames.find(*(pointFrom->getCoordinate()));
	if (nodeFrom != nodeNames.end())
	{		
		// Dijkstra implementation
		clock_t start = clock();

		long nodes = nodeNames.size();
		vector<bool> A;
		vector<long> X;
		A.resize(nodes);
		for (long i = 0; i < nodes; ++i) {
			A[i] = false;
			final_distances[i] = pair<double, long>(rUNDEF, -1);
		}

		long startpoint = nodeFrom->second;

		final_distances[startpoint] = pair<double, long>(0, -1);
		A[startpoint] = true;
		pair<multimap<long, pair<long, double>>::iterator, multimap<long, pair<long, double>>::iterator> range = graph.equal_range(startpoint);
		for (multimap<long, pair<long, double>>::iterator it = range.first; it != range.second; ++it) {
			final_distances[it->second.first] = pair<double, long>(it->second.second, startpoint);
			X.push_back(it->second.first);
		}

		SortByVector sbv (final_distances);

		while (X.size() > 0) {
			sort(X.begin(), X.end(), sbv); // by distance to startpoint, shortest last, for easy pop_back
			long y = X[X.size() - 1];
			X.pop_back();
			A[y] = true;
			pair<multimap<long, pair<long, double>>::iterator, multimap<long, pair<long, double>>::iterator> range = graph.equal_range(y);
			for (multimap<long, pair<long, double>>::iterator it = range.first; it != range.second; ++it) {
				if (!A[it->second.first]) {
					if (find(X.begin(), X.end(), it->second.first) == X.end()) {
						X.push_back(it->second.first);
						final_distances[it->second.first] = pair<double, long>(final_distances[y].first + it->second.second, y);
					} else if (final_distances[it->second.first].first > final_distances[y].first + it->second.second)
						final_distances[it->second.first] = pair<double, long>(final_distances[y].first + it->second.second, y);
				}
			}
		}

		clock_t end = clock();
		double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("Dijkstra run in %2.2f milliseconds;\n", duration);

		fValid = true;
	}
}

double Dijkstra::getNetworkDistance(const Geometry *pointTo)
{
	if (fValid) {
		map<Coordinate, long, SortCoordinates>::iterator nodeTo = nodeNames.find(*(pointTo->getCoordinate()));
		if (nodeTo != nodeNames.end())
			return final_distances[(*nodeTo).second].first;
		else
			return rUNDEF;
	} else
		return rUNDEF;
}

CoordinateArraySequence * Dijkstra::getShortestPath(const Geometry *pointTo)
{
	if (fValid) {
		map<Coordinate, long, SortCoordinates>::iterator nodeTo = nodeNames.find(*(pointTo->getCoordinate()));
		if (nodeTo != nodeNames.end()) {
			vector<Coordinate> * path = new vector<Coordinate>();
			long currentNode = (*nodeTo).second;
			while (currentNode != -1) {
				path->insert(path->begin(), coordinateList[currentNode]);
				currentNode = final_distances[currentNode].second;
			}
			if (path->size() > 1) // minimum 2 points for a segment; when path = 1 point, then pointTo was the same as pointFrom
				return new CoordinateArraySequence(path); // CoordinateArraySequence will be the owner, and will delete it at destruction
			else {
				delete path;
				return 0;
			}
		} else
			return 0;
	} else
		return 0;
}