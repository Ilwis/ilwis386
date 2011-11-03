///////////////////////////////////////////////////////////
//  PTable.h
//  Implementation of the Class PTable
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"
#include "RootObject.h"
#include "PDataType.h"

class PTable : public RootObject
{

public:
	PTable();
	virtual ~PTable();

	double getd(int index, const std::string& field);
	void setd(int index, const std::string& field, double value);
	std::string gets(int index, const std::string& field);
	void sets(int index, const std::string& field,const std::string& value);
private:
	vector<PDataType> types;
	
};

