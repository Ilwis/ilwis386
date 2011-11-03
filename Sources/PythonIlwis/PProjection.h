///////////////////////////////////////////////////////////
//  PProjection.h
//  Implementation of the Class PProjection
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\SpatialReference\prj.h"
#include "PEllipsoid.h"
#include "PDatum.h"
#include "PSRObject.h"

class PProjection : public PSRObject
{

public:
	// python interface
	enum ProjectionParameter{ppUnknown, ppFalseEasting, ppFalsNorthing, ppCentralMeridian, ppLatitudeOfTrueScale,ppCentralParallel,ppStandardParallel_1, ppStandardParallel_2,
		ppNorthernHemisphere,ppHeight,ppTiltedProjPlane,ppTiltOfPlane,ppAzimuthYAxis, ppAzimuthCentralLine, ppPoleObliqueCylinder,ppNorthOriented, ppScale, ppZone};

	PProjection();
	PProjection(const std::string& name) ;
	PProjection(const std::string& name, const std::string ellipsoid) ;
	virtual ~PProjection();

	double getProjectionParameter(ProjectionParameter id) const;
	void setProjecttionParameter(ProjectionParameter id, double v);
	PEllipsoid getEllipsoid() const;
	int getEPSG() const;
	string getName() const;
	bool isEqual(const PProjection& dat) const;
	bool isValid() const;

	//other
	ProjectionParamValue mapProjectionParameterToIlwis(PProjection::ProjectionParameter pp) const;
	ProjectionParameter mapProjectionParamValueToPythonEnum(ProjectionParamValue pp);
	Projection getIlwisProjection() const;

private:
	Projection prj;

};

