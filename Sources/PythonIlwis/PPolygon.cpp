///////////////////////////////////////////////////////////
//  PPolygon.cpp
//  Implementation of the Class PPolygon
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PPolygon.h"
#include "Engine\Map\Polygon\POL.H"


PPolygon::PPolygon(){

}

PPolygon::PPolygon(Feature *f, const BaseMap& bp) : PFeature(f, bp){
}


PPolygon::~PPolygon(){

}

vector<PCoordinate> PPolygon::getCoordinates(){
	if ( isValid()) {
		CoordinateSequence *seq = (CPOLYGON(feature)->getCoordinates());
	
		vector<PCoordinate> coords;
		for(int i = 0; i < seq->size(); ++i)
			coords.push_back(PCoordinate(PCoordinate(Coord(seq->getAt(i)))));
		delete seq;
		return coords;
	}
	return vector<PCoordinate>();
}


int PPolygon::getType(){

	return Feature::ftPOLYGON;
}


vector<PCoordinate> PPolygon::getShell() const {
	if ( isValid()) {
		CoordinateSequence *seq = (CPOLYGON(feature)->getExteriorRing()->getCoordinates());
		vector<PCoordinate> coords;
		for(int i = 0; i < seq->size(); ++i)
			coords.push_back(PCoordinate(PCoordinate(Coord(seq->getAt(i)))));
		delete seq;
		return coords;
	}
	return vector<PCoordinate>();
}

void PPolygon::setShell(const vector<PCoordinate>& shell){
	if ( isValid()) {
		CoordinateSequence *seq = new CoordinateArraySequence();
		for(int i = 0; i < shell.size(); ++i)
			seq->add(Coord(shell.at(i).getIlwisCoord()));
		geos::geom::LinearRing *ring = new geos::geom::LinearRing(seq, new geos::geom::GeometryFactory());
		CPOLYGON(feature)->addBoundary(ring);
	}

}

void PPolygon::addHole(const vector<PCoordinate>& shell){
	if ( isValid()) {
		CoordinateSequence *seq = new CoordinateArraySequence();
		for(int i = 0; i < shell.size(); ++i)
			seq->add(Coord(shell.at(i).getIlwisCoord()));
		geos::geom::LinearRing *ring = new geos::geom::LinearRing(seq, new geos::geom::GeometryFactory());
		CPOLYGON(feature)->addHole(ring);
	}
}