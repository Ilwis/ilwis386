///////////////////////////////////////////////////////////
//  PGeoReference.cpp
//  Implementation of the Class PGeoReference
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PGeoReference.h"


PGeoReference::PGeoReference(){
}

PGeoReference::PGeoReference(const string& name, bool setWorkingDir) : RootObject(name, setWorkingDir){
	FileName fn(name);
	if ( fn.sExt == "")
		fn.sExt = ".grf";
	grf = GeoRef(fn);
}

PGeoReference::~PGeoReference(){

}

PPixel PGeoReference::coordToPixel(PCoordinate location){

	return  PPixel();
}


PCoordinateSystem PGeoReference::getCoordinateSystem(){

	return  PCoordinateSystem();
}


PCoordinate PGeoReference::pixelToCoordinate(PPixel location){

	return  PCoordinate();
}


void PGeoReference::setCoordinateSystem(PCoordinateSystem csy){

}

bool PGeoReference::isValid() const {
	return grf.fValid();
}

bool PGeoReference::isReadOnly() const {
	if (isValid())
		return grf->fReadOnly();
	return true;
}