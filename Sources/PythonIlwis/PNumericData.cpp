///////////////////////////////////////////////////////////
//  PNumericData.cpp
//  Implementation of the Class PNumericData
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PNumericData.h"


PNumericData::PNumericData(){

}

PNumericData::PNumericData(const std::string& n, const PValueRange& r) : PDataType(n), range(r){
}

PNumericData::~PNumericData(){
}

bool PNumericData::isUndefined(double v) {
	return v == rUNDEF || v == iUNDEF || v == shUNDEF;
}

std::string PNumericData::getType() const{
	return "numeric";
}

bool PNumericData::isValid(double v) const{
	return !isUndefined(v) && isValid() && range.isValid(v);
}

bool PNumericData::isValid(const std::string& s) const{
	String val(s);
	double v = val.rVal();
	return isValid(v);
}

bool PNumericData::isValid() const{
	return range.isValid();
}

PValueRange PNumericData::getRange() const{
	return range;
}

bool PNumericData::isEqual(const PNumericData& nd) const {
	return isValid() && nd.isValid() && range.isEqual(nd.getRange());
}
