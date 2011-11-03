///////////////////////////////////////////////////////////
//  PPCoordinateSystem.h
//  Implementation of the Class PPCoordinateSystem
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "engine\spatialreference\coordsys.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"
#include "PCRSBoundingBox.h"
#include "PDatum.h"
#include "PProjection.h"
#include "PSRObject.h"

class PCoordinateSystem : public PSRObject
{

public:
	PCoordinateSystem();
	PCoordinateSystem(const string& name, bool setWorkingDir=false );
	virtual ~PCoordinateSystem();

	void convertBB(PCRSBoundingBox bb);
	PCoordinate convertCrd(const PCoordinateSystem& source, const PCoordinate& crd) const;
	PCRSBoundingBox convertBB(const PCoordinateSystem& sourceCsy, const PCRSBoundingBox& bb) const;
	bool isProjected() const;
	bool isValid() const;
	PDatum getDatum() const;
	void setDatum(const PDatum& dat);
	PEllipsoid getEllipsoid() const;
	//void setEllipsoid(const& PEllipsoid ell);
	PProjection getProjection() const;
	void setProjection(const PProjection& proj);
	std::string getName() const;
protected:
	CoordSystem getIlwisCsy() const;
private:
	
	CoordSystem csy;

};

