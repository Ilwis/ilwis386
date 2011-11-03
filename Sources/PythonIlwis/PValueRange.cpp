///////////////////////////////////////////////////////////
//  PValueRange.cpp
//  Implementation of the Class PValueRange
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PValueRange.h"


PValueRange::PValueRange(){
	range.rLo() = -1e300;
	range.rHi() = 1e300;
}

PValueRange::PValueRange(double min, double max){
	range.rLo() = min;
	range.rHi() = max;
}

PValueRange::~PValueRange(){

}

double PValueRange::getMaxValue() const{

	return  range.rHi();
}


double PValueRange::getMinValue() const{

	return  range.rLo();
}


bool PValueRange::isValid(double v) const{
	return isValid() && v >= range.rLo() && v <= range.rHi();

}


void PValueRange::setMaxValue(double v){
	range.rHi() = v;

}


void PValueRange::setMinValue(double v){
	range.rLo() = v;

}

bool PValueRange::isValid() const {
	return range.fValid();
}

bool PValueRange::isEqual(const PValueRange& r) const{
	return isValid() && r.isValid() && r.range == range;
}