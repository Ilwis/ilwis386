///////////////////////////////////////////////////////////
//  PFeatureCoverage.h
//  Implementation of the Class PFeatureCoverage
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PFeature.h"
#include "PCoordinate.h"
#include "PCRSBoundingBox.h"
#include "PCoverage.h"

class PFeatureCoverage : public PCoverage
{

public:
	//python interface
	PFeatureCoverage();
	PFeatureCoverage(const std::string& name, bool setWorkingDir=true);
	virtual ~PFeatureCoverage();

	PFeature newFeature();
	vector<PFeature> getFeatures(const PCoordinate& location, double tolerance=rUNDEF, const std::string& query="") const;
	vector<PFeature> getFeatures(const PCRSBoundingBox& box, double tolerance, const string& query) const;
	PFeature getFeature(long index) const;
	int getNumberOfFeatures() const;
	void removeFreature(const PFeature& f);
	bool isEqual() const;
protected:
	PFeature createFeature(Feature *f) const;
};

