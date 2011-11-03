///////////////////////////////////////////////////////////
//  PEllipsod.h
//  Implementation of the Class Ellipsod
//  Created on:      27-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\SpatialReference\Ellips.H"
#include "PSRObject.h"
#include "PLatLon.h"

class PEllipsoid : public PSRObject {
public:
	//python interface
	PEllipsoid();
	PEllipsoid(const string& ell);
	PEllipsoid(double axis, double flattening);
	virtual ~PEllipsoid();

	bool isSpherical() const;
	bool isEqual(const PEllipsoid& ell) const;
	bool isValid() const;
	double getDistance(const PLatLon& begin, const PLatLon& end) const;
	double getAzimuth(const PLatLon& begin, const PLatLon& end) const;
	double getMajorAxis() const;
	double getMinorAxis() const;
	double getFlattening() const;
	double getExcentricity() const;

	//other
	Ellipsoid *getIlwisEllipsoid() const;

private:
	Ellipsoid *ell;


};