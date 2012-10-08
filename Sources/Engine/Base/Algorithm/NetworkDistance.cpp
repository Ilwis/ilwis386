#include "NetworkDistance.h"
#include "Engine\Table\tbl2dim.h"
#include "geos\headers\geos\algorithm\distance\DistanceToPoint.h"
#include "geos\headers\geos\algorithm\distance\PointPairDistance.h"
#include "geos\headers\geos\linearref\LengthIndexedLine.h"

NetworkDistance::NetworkDistance(const PointMap& _pmFrom, const PointMap& _pmTo, const SegmentMap& _smVia, const bool fProject, Tranquilizer & trq)
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

	rDistanceOD.resize(iFromFeatures);

	vector<LineString*> * vlsVia = (vector<LineString*> *)spatialIndex.queryAll();
	Dijkstra dijkstra (*vlsVia);
	delete vlsVia;

	if (iFromFeatures < iToFeatures) {
		for(long i = 0; i < iFromFeatures; ++i) {
			const Geometry * geomFrom = pmFrom->getFeature(i);
			rDistanceOD[i].resize(iToFeatures);
			dijkstra.init(geomFrom);
			for(long j = 0; j < iToFeatures; ++j) {
				const Geometry * geomTo = pmTo->getFeature(j);
				rDistanceOD[i][j] = dijkstra.getNetworkDistance(geomTo);
			}
			trq.fUpdate(step++, iterations);
		}
	} else {
		for(long i = 0; i < iFromFeatures; ++i)
			rDistanceOD[i].resize(iToFeatures);
		for(long j = 0; j < iToFeatures; ++j) {
			const Geometry * geomTo = pmTo->getFeature(j);
			dijkstra.init(geomTo);
			for(long i = 0; i < iFromFeatures; ++i) {
				const Geometry * geomFrom = pmFrom->getFeature(i);
				rDistanceOD[i][j] = dijkstra.getNetworkDistance(geomFrom);
			}
			trq.fUpdate(step++, iterations);
		}
	}
}

NetworkDistance::~NetworkDistance()
{
}

void NetworkDistance::CopyToTable2Dim(Table2DimPtr & ptr)
{
	for (long i = 0; i < pmFrom->iFeatures(); ++i)
		for (long j = 0; j < pmTo->iFeatures(); ++j)
			ptr.PutVal(i+1, j+1, rDistanceOD[i][j]);
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

	geos::linearref::LengthIndexedLine lineNearest (lsNearest); // index it
	double projectedPos = lineNearest.indexOf(ptDistNearest.getCoordinate(0));

	if (projectedPos != lineNearest.getStartIndex() && projectedPos != lineNearest.getEndIndex()) {
		LineString * AP = (LineString*)lineNearest.extractLine(lineNearest.getStartIndex(), projectedPos);
		LineString * PB = (LineString*)lineNearest.extractLine(projectedPos, lineNearest.getEndIndex());
		spatialIndex.remove(lsNearest->getEnvelopeInternal(), lsNearest); // remove the large segment
		spatialIndex.insert(AP->getEnvelopeInternal(), AP); // add the first split part
		spatialIndex.insert(PB->getEnvelopeInternal(), PB); // add the second split part
	}
	LineString * PP = 0;
	if (rDistNearest != rUNDEF && rDistNearest != 0) {
		vector<Coordinate> ptsProjectionLine = ptDistNearest.getCoordinates();
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
			final_distances[i] = rUNDEF;
		}

		long startpoint = nodeFrom->second;

		final_distances[startpoint] = 0;
		A[startpoint] = true;
		pair<multimap<long, pair<long, double>>::iterator, multimap<long, pair<long, double>>::iterator> range = graph.equal_range(startpoint);
		for (multimap<long, pair<long, double>>::iterator it = range.first; it != range.second; ++it) {
			final_distances[it->second.first] = it->second.second;
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
						final_distances[it->second.first] = final_distances[y] + it->second.second;
					} else
						final_distances[it->second.first] = min(final_distances[it->second.first], final_distances[y] + it->second.second);
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
			return final_distances[(*nodeTo).second];
		else
			return rUNDEF;
	} else
		return rUNDEF;
}