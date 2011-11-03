///////////////////////////////////////////////////////////
//  PDatum.h
//  Implementation of the Class PDatum
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\SpatialReference\DATUM.H"
#include "PSRObject.h"
#include "PEllipsoid.h"
#include "PLatLon.h"

class Datum;

using namespace std;

class PDatum : public PSRObject
{

public:
	// python interface
	PDatum();
	PDatum(const string& ellipsoid, const string& area);
	PDatum(const string& ellipsoid, double dx, double dy, double dz);
	PDatum(const string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS);
	PDatum(const string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS,double X0, double Y0, double Z0);
	PLatLon fromWGS84(const PLatLon& ll);
	PLatLon toWGS84(const PLatLon& ll);
	int getEPSG() const;
	std::string getName() const;
	std::string getArea() const;
	PEllipsoid getEllipsoid() const;

	bool isEqual(const PDatum& dat) const;
	bool isValid() const;
	virtual ~PDatum();
	// other
	PDatum( Datum& dat);
	Datum *getDatum() const;

private:
	Datum *datum;

};

