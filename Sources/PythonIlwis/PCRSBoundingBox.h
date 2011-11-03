///////////////////////////////////////////////////////////
//  PCRSBoundingBox.h
//  Implementation of the Class PCRSBoundingBox
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PBoundingBox.h"

class PCRSBoundingBox : public PBoundingBox
{

public:
	//python interface
	PCRSBoundingBox();
	virtual ~PCRSBoundingBox();
	double getMaxZ() const;
	double getMinZ() const;
	void setMaxZ(double v) ;
	void setMinZ(double v) ;	
	virtual bool isValid() const;
	void add(const PCoordinate& coord);
	bool isInside(const PCoordinate& coord);
	PCRSBoundingBox& operator+(const PCRSBoundingBox& box);

	//other
	PCRSBoundingBox(const CoordBounds& bb);
	CoordBounds toCoordBounds() const;


private:
	double minZ, maxZ;

};

