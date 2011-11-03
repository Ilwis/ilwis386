///////////////////////////////////////////////////////////
//  PCoverageList.h
//  Implementation of the Class PCoverageList
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PCoverage.h"
#include "PCoverageCollection.h"

class PCoverageList : public PCoverageCollection
{

public:
	PCoverageList();
	virtual ~PCoverageList();

	void addCoverage(PCoverage cov, PCoverage index);

};

