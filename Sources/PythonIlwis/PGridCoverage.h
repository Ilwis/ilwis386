///////////////////////////////////////////////////////////
//  PGridCoverage.h
//  Implementation of the Class PGridCoverage
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\Map\Raster\Map.h"
#include "pgrid.h"
#include "PCoverage.h"
#include "PGeoReference.h"
#include "PGridBoundingBox.h"
#include <vector>

using namespace std;

class PGridCoverage : public PCoverage
{

public:
	PGridCoverage();
	PGridCoverage(const string& name, bool setWorkingDir=true);
	virtual ~PGridCoverage();

	PGeoReference getGeoReference() const;
	PGridBoundingBox getGridBoundingBox() const;
	double getd(const PPixel& location) const;
	void getGrid(const PExtent& extent, PGrid& result) const;

	PGridCoverage operator+(const PGridCoverage& cov);
	PGridCoverage operator-(const PGridCoverage& cov);
	PGridCoverage operator*(const PGridCoverage& cov);
	PGridCoverage operator/(const PGridCoverage& cov);

private:
	MapPtr *mp();
	MapPtr *mp() const;

};
