///////////////////////////////////////////////////////////
//  PSRObject.h
//  Implementation of the Class PSRObject
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"

using namespace std;

class PSRObject
{

public:
	PSRObject();
	virtual ~PSRObject();

	virtual int getEPSG() const;
	virtual string getName() const;
};

