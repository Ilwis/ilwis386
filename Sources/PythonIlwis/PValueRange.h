///////////////////////////////////////////////////////////
//  PValueRange.h
//  Implementation of the Class PValueRange
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"

class PValueRange
{

public:
	PValueRange();
	PValueRange(double min, double max);
	virtual ~PValueRange();

	double getMaxValue() const;
	double getMinValue() const;
	bool isValid(double v) const;
	void setMaxValue(double v);
	void setMinValue(double v);
	bool isValid() const;
	bool isEqual(const PValueRange& r) const;
private:
	RangeReal range;

};

