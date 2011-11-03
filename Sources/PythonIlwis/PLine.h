///////////////////////////////////////////////////////////
//  PLine.h
//  Implementation of the Class PLine
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PFeature.h"

using namespace std;

class PLine : public PFeature
{

public:
	//python interface
	PLine();
	virtual ~PLine();

	vector<PCoordinate> getCoordinates();
	int getType();
	void setCoordinates(vector<PCoordinate> crds);

	//other
	PLine(Feature *f, const BaseMap& bp);;

};

