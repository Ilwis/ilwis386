///////////////////////////////////////////////////////////
//  PFeatureCoverage.cpp
//  Implementation of the Class PFeatureCoverage
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PFeatureCoverage.h"
#include "PPoint.h"
#include "PLine.h"
#include "PPolygon.h"


PFeatureCoverage::PFeatureCoverage(){

}

PFeatureCoverage::PFeatureCoverage(const string& name, bool setWorkingDir) : PCoverage(name,setWorkingDir) {
	try{
		FileName fn(name);
		bmp = BaseMap(fn);
	} catch(const ErrorObject& err) {
		err.Show();
	}
}



PFeatureCoverage::~PFeatureCoverage(){

}

vector<PFeature> PFeatureCoverage::getFeatures(const PCoordinate& location, double tolerance, const std::string& query) const{
	vector<PFeature> features;
	if ( isValid()) {
		vector<Geometry *> geoms = bmp->getFeatures(location.getIlwisCoord(),tolerance);
		for(int i = 0; i < geoms.size(); ++i) {
			features.push_back(createFeature( CFEATURE(geoms[i])));
		}
	}
	return  features;
}

PFeature PFeatureCoverage::createFeature(Feature *f) const{
	if ( f) {
		Feature::FeatureType ft = f->getType();
		if (  ft == Feature::ftPOINT)
			return PPoint(f, bmp);
		if ( ft == Feature::ftSEGMENT)
			return PLine(f, bmp);
		if ( ft == Feature::ftPOLYGON)
			return PPolygon(f, bmp);
	}
	return PFeature();
	
}

vector<PFeature> PFeatureCoverage::getFeatures(const PCRSBoundingBox& box, double tolerance, const string& query) const{

	return  vector<PFeature>();
}


void PFeatureCoverage::removeFreature(const PFeature& f){
	if ( isValid()) {
		bmp->removeFeature(f.getId());
	}
}

PFeature PFeatureCoverage::newFeature() {
	if ( isValid()) {
		Feature *f = bmp->newFeature();
		return createFeature(f);
	}
	return PFeature();
}

PFeature PFeatureCoverage::getFeature(long index) const {
	if ( isValid()) {
		return createFeature(CFEATURE(bmp->getFeature(index)));
	}
	return PFeature();
}

int PFeatureCoverage::getNumberOfFeatures() const {
	if ( isValid()) {
		return bmp->iFeatures();
	}
	return iUNDEF;
}





