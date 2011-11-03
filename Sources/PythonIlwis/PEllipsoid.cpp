///////////////////////////////////////////////////////////
//  PEllipsod.cpp
//  Implementation of the Class Ellipsod
//  Created on:      27-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PEllipsoid.h"

PEllipsoid::PEllipsoid() : ell(0){
}

PEllipsoid::PEllipsoid(const string& ellips){
	ell = new Ellipsoid(ellips);
}

PEllipsoid::PEllipsoid(double axis, double flattening){
	ell = new Ellipsoid(axis, flattening);
}

PEllipsoid::~PEllipsoid() {
	delete ell;
}

bool PEllipsoid::isSpherical() const{
	if ( isValid())
		return ell->fSpherical();
	return false;
}

bool PEllipsoid::isEqual(const PEllipsoid& ellips) const{
	if ( isValid()) {
		return ell->fEqual(*(ellips.getIlwisEllipsoid()));
	}
	return false;
}

bool PEllipsoid::isValid() const{
	if ( ell) {
		return true;
	}
	return false;
}

double PEllipsoid::getDistance(const PLatLon& begin, const PLatLon& end) const{
	if ( isValid() && begin.isValid() && end.isValid())
		return ell->rEllipsoidalDistance(begin.getLatLon(), end.getLatLon());
	return rUNDEF;
}

double PEllipsoid::getAzimuth(const PLatLon& begin, const PLatLon& end) const{
	if ( isValid() && begin.isValid() && end.isValid())
		return ell->rEllipsoidalAzimuth(begin.getLatLon(), end.getLatLon());
	return rUNDEF;
}

Ellipsoid *PEllipsoid::getIlwisEllipsoid() const {
	return ell;
}

double PEllipsoid::getMajorAxis() const{
	if ( isValid())
		return ell->a;
	return rUNDEF;
}

double PEllipsoid::getMinorAxis() const{
	if ( isValid())
		ell->b;
	return rUNDEF;
}

double PEllipsoid::getFlattening() const{
	if ( isValid())
		ell->f;
	return rUNDEF;
}

double PEllipsoid::getExcentricity() const{
	if ( isValid())
		ell->e;
	return rUNDEF;
}
