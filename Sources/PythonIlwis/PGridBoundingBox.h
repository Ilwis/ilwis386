///////////////////////////////////////////////////////////
//  PGridBoundingBox.h
//  Implementation of the Class PGridBoundingBox
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PBoundingBox.h"
#include "PPixel.h"

class PGridBoundingBox : public PBoundingBox
{

public:
	//Python interface
	PGridBoundingBox();
	virtual ~PGridBoundingBox();

	void add(const PPixel& pix);
	bool isInside(const PPixel& pix);
	PGridBoundingBox& operator+(const PGridBoundingBox& box);

	//other
	PGridBoundingBox(RowCol rc);

};

