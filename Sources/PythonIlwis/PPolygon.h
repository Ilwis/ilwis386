///////////////////////////////////////////////////////////
//  PPolygon.h
//  Implementation of the Class PPolygon
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PFeature.h"

using namespace std;

class PPolygon : public PFeature
{

public:
	//python interfACE
	PPolygon();
	virtual ~PPolygon();

	vector<PCoordinate> getCoordinates();
	int getType();
	void setShell(const vector<PCoordinate>& shell);
	void addHole(const vector<PCoordinate>& shell);
	vector<PCoordinate> getShell() const;

	//other
	PPolygon(Feature *f, const BaseMap& bp);

};

