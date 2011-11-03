///////////////////////////////////////////////////////////
//  PGridCoverage.cpp
//  Implementation of the Class PGridCoverage
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PGridCoverage.h"


PGridCoverage::PGridCoverage(){

}

PGridCoverage::PGridCoverage(const string& name, bool setWorkingDir) : PCoverage(name, setWorkingDir){
	try{
		FileName fn(name);
		if ( fn.sExt == "")
			fn.sExt = ".mpr";
		bmp = BaseMap(fn);
	} catch (const ErrorObject& err) {
		err.Show();
	}
}

PGridCoverage::~PGridCoverage(){

}

PGeoReference PGridCoverage::getGeoReference() const{

	if (isValid() && mp()->gr().fValid()) {
		PGeoReference grf(mp()->gr()->fnObj.sFullPath(), false);
		return grf;
	}

	return  PGeoReference();
}


PGridBoundingBox PGridCoverage::getGridBoundingBox() const{
	if ( isValid())
		return PGridBoundingBox(mp()->gr()->rcSize());
	return PGridBoundingBox();
}


double PGridCoverage::getd(const PPixel& location) const{
	double v = rUNDEF;

	if ( !isValid())
		return v;

	if ( location.isValid()) {
		v = mp()->rValue(location.getRC());
	}

	return v;

}

void PGridCoverage::getGrid(const PExtent& extent, PGrid& result) const{
	if ( !isValid())
		return;

	int xsz = extent.getXDim();
	if ( extent.getXDim() == 0)
		xsz = mp()->rcSize().Col;

	result.resize(xsz, extent.getYDim());
	for( long y=0; y < extent.getYDim(); ++y) {
		mp()->GetLineVal(extent.getBase().getY(),result.get(y,0),extent.getBase().getX(), xsz);
	}

}

PGridCoverage PGridCoverage::operator+(const PGridCoverage& cov) {
	return PGridCoverage();
}

PGridCoverage PGridCoverage::operator-(const PGridCoverage& cov) {
	return PGridCoverage();
}

PGridCoverage PGridCoverage::operator*(const PGridCoverage& cov) {
	return PGridCoverage();
}

PGridCoverage PGridCoverage::operator/(const PGridCoverage& cov) {
	return PGridCoverage();
}

MapPtr *PGridCoverage::mp() {
	return (MapPtr *)bmp.ptr();
}

MapPtr *PGridCoverage::mp() const{
	return (MapPtr *)bmp.ptr();
}
