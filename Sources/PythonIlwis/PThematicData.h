///////////////////////////////////////////////////////////
//  PThematicData.h
//  Implementation of the Class PThematicData
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PDataType.h"
#include "Engine\Domain\dmsort.h"

class PThematicData : public PDataType
{

public:
	PThematicData();
	PThematicData(const std::string& domain);
	virtual ~PThematicData();
	std::string getType() const ;
	bool isValid(const std::string& s) const ;
	bool isValid() const ;
	bool isEqual(const PThematicData& nd) const;
	void addItem(const std::string& n);

private:
	Domain dm; // dm sort

};

