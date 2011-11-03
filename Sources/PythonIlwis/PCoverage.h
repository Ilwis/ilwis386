///////////////////////////////////////////////////////////
//  PCoverage.h
//  Implementation of the Class PCoverage
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\basemap.h"
#include "PCRSBoundingBox.h"
#include "PValue.h"
#include "PCoordinate.h"
#include "PBoundingBox.h"
#include "PCoordinateSystem.h"
#include "RootObject.h"
#include "PDataType.h"

/**
 * The base class for all layers
 */
class PCoverage : public RootObject
{

public:
	PCoverage();
	PCoverage(const std::string& name, bool setWorkingDir);
	virtual ~PCoverage();

	PCoordinateSystem getCoordinateSystem();
	PDataType getDataType();
	PCRSBoundingBox getCRSBoundingBox();
	int getType();
	PValue getValue(PCoordinate location);
	bool isValid() const;
	bool isReadOnly() const;

protected:
	BaseMap bmp;

};

