///////////////////////////////////////////////////////////
//  PGeoReference.h
//  Implementation of the Class PGeoReference
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\SpatialReference\Gr.h"
#include "PPixel.h"
#include "PCoordinate.h"
#include "PCoordinateSystem.h"
#include "RootObject.h"

class PGeoReference : public RootObject
{

public:
	PGeoReference();
	PGeoReference(const string& name, bool setWorkingDir);
	virtual ~PGeoReference();

	PPixel coordToPixel(PCoordinate location);
	PCoordinateSystem getCoordinateSystem();
	PCoordinate pixelToCoordinate(PPixel location);
	void setCoordinateSystem(PCoordinateSystem csy);
	bool isValid() const;
	bool isReadOnly() const;
private:
	GeoRef grf;

};

