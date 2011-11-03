///////////////////////////////////////////////////////////
//  PFeature.cpp
//  Implementation of the Class PFeature
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PFeature.h"


PFeature::PFeature(){

}

PFeature::PFeature(Feature *f, const BaseMap& bp){
	feature = f;
	bmp = bp;
	if ( bp->fTblAtt())
		attTbl = bp->tblAtt();
}

PFeature::~PFeature(){

}

string PFeature::getId() const{

	if ( feature)
		return feature->getGuid();
	return  sUNDEF;
}


int PFeature::getType(){

	return PFeature::ftFEATURE;
}


double PFeature::getd(const std::string& field) const {
	if ( field == "")
		return feature->rValue();
	else if ( attTbl.fValid()) {
		Column col = attTbl->col(field);
		if ( col.fValid()) {
			return col->rValue(feature->iValue());
		}
	}
	return rUNDEF;

}

string PFeature::gets(const std::string& field) const {
	if ( field == "")
		return feature->sValue(bmp->dvrs());
	else if ( attTbl.fValid()) {
		Column col = attTbl->col(field);
		if ( col.fValid()) {
			return col->sValue(feature->iValue());
		}
	}
	return sUNDEF;

}

void PFeature::setd(double v, const std::string& field){
	if ( field == "") {
		feature->PutVal(v);
	} else if ( attTbl.fValid()) {
		Column col = attTbl->col(field);
		if ( col.fValid()) {
			return col->PutVal(feature->iValue(),v);
		}
	}
}

void PFeature::sets(const std::string& v, const std::string& field){
	if ( field == "") {
		feature->PutVal(bmp->dvrs(), v);
	} else if ( attTbl.fValid()) {
		Column col = attTbl->col(field);
		if ( col.fValid()) {
			return col->PutVal(feature->iValue(),v);
		}
	}
}



void PFeature::setCoordinates(vector<PCoordinate> crds){
}

vector<PCoordinate> PFeature::getCoordinates() const {
	return vector<PCoordinate>();
}


bool PFeature::isValid(const std::string& field) const {
	if ( feature && bmp.fValid()) {
		if ( field == "")
			return feature->fValid();
		else if ( attTbl.fValid())
			return attTbl->col(field).fValid();
	}
	return false;
}

bool PFeature::isReadOnly(const std::string& field) const{
	if ( isValid(field)) {
		if ( field == "")
			return bmp->fReadOnly();
	}else if ( attTbl.fValid()) {
		Column col = attTbl->col(field);
		if ( col.fValid()) {
			return col->fReadOnly();
		}
	}
	return true;
}

