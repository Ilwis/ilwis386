///////////////////////////////////////////////////////////
//  PCoordinate.cpp
//  Implementation of the Class PCoordinate
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PCoordinate.h"


PCoordinate::PCoordinate() {
}

PCoordinate::PCoordinate(const PCoordinate& coord) {
	c.x = coord.getX();
	c.y = coord.getY();
	c.z = coord.getZ();
}

PCoordinate::PCoordinate(const Coord& coord) {
	c = coord;
}

PCoordinate::PCoordinate(double x, double y, double z) {
	c = Coord(x,y,z);
}

PCoordinate::~PCoordinate(){

}

double PCoordinate::getX() const{

	return c.x;
}

double PCoordinate::getY() const{

	return c.y;
}

double PCoordinate::getZ() const{

	return c.z;
}

void PCoordinate::setX(double v){
	c.x = v;
}


void PCoordinate::setXYZ(double x, double y, double z){
	c = Coord(x,y,z);
}


void PCoordinate::setY(double v){
	c.y = v;
}


void PCoordinate::setZ(double v){
	c.z = v;
}

bool PCoordinate::isValid() const {
	return !c.fUndef();
}

bool PCoordinate::isEqual(const PCoordinate& crd) {
	if ( isValid() && crd.isValid())
		return crd.getIlwisCoord() == c;
	return false;
}

bool PCoordinate::isNear(const PCoordinate& crd, double tolerance) {
	if ( isValid() && crd.isValid())
		c.fNear(crd.getIlwisCoord(), tolerance);
	return false;
}

Coord PCoordinate::getIlwisCoord() const {
	return c;
}

string PCoordinate::toString(long precission) const{
	if ( c.z == rUNDEF) {
		return String("%.*f,%.*f", precission, c.x, precission, c.y);
	}
	return String("%.*f,%.*f,%.*f", precission, c.x, precission, c.y, precission, c.z);
}