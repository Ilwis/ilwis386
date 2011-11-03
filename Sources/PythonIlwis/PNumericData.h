///////////////////////////////////////////////////////////
//  PNumericData.h
//  Implementation of the Class PNumericData
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PDataType.h"

class PNumericData : public PDataType
{

public:
	PNumericData();
	PNumericData(const std::string& n, const PValueRange& r);
	virtual ~PNumericData();

	std::string getType() const;
	bool isValid(double v) const;
	bool isValid(const std::string& s) const;
	bool isValid() const;
	static bool isUndefined(double v);
	PValueRange getRange() const;
	bool isEqual(const PNumericData& nd) const;

private:
	PValueRange range;


};

