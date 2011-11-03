///////////////////////////////////////////////////////////
//  PThematicData.cpp
//  Implementation of the Class PThematicData
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PThematicData.h"


PThematicData::PThematicData(){

}

PThematicData::PThematicData(const std::string& domain) {
	try{
		FileName fn(name);
		if ( fn.sExt == "")
			fn.sExt = ".dom";
		if ( fn.fExist())
			dm = Domain(fn);
		else
			dm = Domain(fn,0,dmtCLASS);
	} catch (const ErrorObject& err) {
		err.Show();
	}
}


PThematicData::~PThematicData(){

}

bool PThematicData::isValid(const std::string& s) const{
	if ( isValid()) {
		return dm->pdsrt()->iOrd(s) != iUNDEF;
	}
	return false;
}

bool PThematicData::isValid() const {
	return dm.fValid() && dm->pdsrt() != 0;
}


bool PThematicData::isEqual(const PThematicData& nd) const {
	return false;
}

std::string PThematicData::getType() const{
	return "thematic";
}

void PThematicData::addItem(const std::string& n){
	if ( isValid()) {
		dm->pdsrt()->iAdd(n,"");
	}
}