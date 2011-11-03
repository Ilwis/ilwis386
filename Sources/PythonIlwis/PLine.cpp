///////////////////////////////////////////////////////////
//  PLine.cpp
//  Implementation of the Class PLine
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PLine.h"
#include "Engine\Map\Segment\Seg.h"


PLine::PLine(){

}

PLine::PLine(Feature *f, const BaseMap& bp) : PFeature(f, bp){
}

PLine::~PLine(){

}

vector<PCoordinate> PLine::getCoordinates(){

	if ( isValid()) {
		CoordinateSequence *seq = (CSEGMENT(feature)->getCoordinates());
		vector<PCoordinate> coords;
		for(int i = 0; i < seq->size(); ++i)
			coords.push_back(PCoordinate(PCoordinate(Coord(seq->getAt(i)))));
		delete seq;
		return coords;
	}
	return vector<PCoordinate>();
}


int PLine::getType(){

	return PFeature::ftLINE;
}


void PLine::setCoordinates(vector<PCoordinate> crds){
	if ( isValid()) {
		CoordinateSequence *seq = new CoordinateArraySequence();
		for(int i = 0; i < crds.size(); ++i)
			seq->add(Coord(crds.at(i).getIlwisCoord()));
		CSEGMENT(feature)->PutCoords(seq);
	}
}