///////////////////////////////////////////////////////////
//  PPoint.cpp
//  Implementation of the Class PPoint
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PPoint.h"


PPoint::PPoint(){

}

PPoint::PPoint(Feature *f, const BaseMap& bp) : PFeature(f, bp){
}


PPoint::~PPoint(){

}

vector<PCoordinate> PPoint::getCoordinates() const{

	if ( isValid()) {
		Coord c = *(CPOINT(feature)->getCoordinate());
		vector<PCoordinate> coords;
		coords.push_back(PCoordinate(c));
		return coords;
	}
	return vector<PCoordinate>();
}


int PPoint::getType(){

	return Feature::ftPOINT;
}


void PPoint::setCoordinates(vector<PCoordinate> crds){
	if ( isValid() && crds.size() > 0) {
		CPOINT(feature)->setCoord(crds[0].getIlwisCoord());
	}

}

