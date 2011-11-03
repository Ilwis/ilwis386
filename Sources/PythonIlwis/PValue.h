///////////////////////////////////////////////////////////
//  PValue.h
//  Implementation of the Class PValue
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include <string>

using namespace std;

class PValue
{
public:
	PValue();
	virtual ~PValue();

	int iValue();
	double rValue();
	string sValue();

};

