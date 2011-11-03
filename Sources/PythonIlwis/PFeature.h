///////////////////////////////////////////////////////////
//  PFeature.h
//  Implementation of the Class PFeature
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PCoordinate.h"
#include "Engine\Map\Feature.h"
#include "Engine\Map\basemap.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include <vector>

using namespace std;

class PFeature
{

public:
	//python interface
	enum FeatureType{ftPOINT, ftLINE, ftPOLYGON, ftFEATURE};
	PFeature();
	virtual ~PFeature();

	virtual vector<PCoordinate> getCoordinates() const;
	string getId() const;
	virtual int getType();
	double getd(const std::string& field="") const;
	std::string gets(const std::string& field="") const;
	virtual void setCoordinates(vector<PCoordinate> crds);
	void setd(double v, const std::string& field="");
	void sets(const std::string& v, const std::string& field="");
	bool isValid(const std::string& field = "") const;
	bool isReadOnly(const std::string& field="") const;

	//other
protected:
	PFeature(Feature *f, const BaseMap& bp);

	Feature *feature;
	Table attTbl;
	BaseMap bmp;

};

