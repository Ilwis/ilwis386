///////////////////////////////////////////////////////////
//  PPoint.h
//  Implementation of the Class PPoint
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PFeature.h"
#include "Engine\Map\Point\ilwPoint.h"

using namespace std;

class PPoint : public PFeature
{

public:
	//python interface
	PPoint();
	virtual ~PPoint();

	vector<PCoordinate> getCoordinates() const;
	int getType();
	void setCoordinates(vector<PCoordinate> crds);

	//other
	PPoint(Feature *f, const BaseMap& bp);

};

