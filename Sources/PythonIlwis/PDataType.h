///////////////////////////////////////////////////////////
//  PDataType.h
//  Implementation of the Class PDataType
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"
#include "PValueRange.h"

class PDataType
{
public:
	PDataType();
	PDataType(const std::string& name);
	virtual ~PDataType();

	std::string getName() const;
	virtual std::string getType() const { return sUNDEF;}
	virtual bool isValid(double v) const { return false;}
	virtual bool isValid(const std::string& s) const { return false; }
	virtual bool isValid() const { return false;}
	virtual bool isEqual() const { return false;}
protected:
	std::string name;
};

