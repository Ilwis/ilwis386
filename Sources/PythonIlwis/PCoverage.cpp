///////////////////////////////////////////////////////////
//  PCoverage.cpp
//  Implementation of the Class PCoverage
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PCoverage.h"


PCoverage::PCoverage(){

}

PCoverage::PCoverage(const std::string& name, bool setWorkingDir) : RootObject(name, setWorkingDir){
}

PCoverage::~PCoverage(){

}

PCoordinateSystem PCoverage::getCoordinateSystem(){

	if ( isValid()) {
		try{
		CoordSystem cs;
		IlwisObjectPtr *ptr = bmp->cs().pointer();
		cs.SetPointer(ptr);
		String name = cs->fnObj.sFullPath();
		return PCoordinateSystem(name, false);
		} catch (const ErrorObject& err) {
			err.Show();
		}

	}
	return  PCoordinateSystem();
}


PDataType PCoverage::getDataType(){

	return  PDataType();
}


/**
 * Returns the extents of the coverage in terms of the attached PCoordinateSystem
 */
PCRSBoundingBox PCoverage::getCRSBoundingBox(){
	if ( isValid() ) {
		if ( bmp->cs().fValid()) {
			PCRSBoundingBox box(bmp->cb());
			return box;
		}
	}

	return  PCRSBoundingBox();
}


int PCoverage::getType(){

	return 0;
}


PValue PCoverage::getValue(PCoordinate location){

	return  PValue();
}

bool PCoverage::isValid() const {
	return bmp.fValid();
}

bool PCoverage::isReadOnly() const {
	if ( !isValid())
		return true;
	return bmp->fReadOnly();
}
