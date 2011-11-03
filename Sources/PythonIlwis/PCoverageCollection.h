///////////////////////////////////////////////////////////
//  PCoverageCollection.h
//  Implementation of the Class PCoverageCollection
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PCoverage.h"
#include "RootObject.h"

class PCoverageCollection : public RootObject
{

public:
	PCoverageCollection();
	virtual ~PCoverageCollection();
	PCoverage *m_Coverage;

	void addCoverage(PCoverage cov);
	int getCount();
	PCoverage getCoverage(int index);
	void removeCoverage(int index);

};
