///////////////////////////////////////////////////////////
//  PBoundingBox.h
//  Implementation of the Class PBoundingBox
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Headers\toolspch.h"
#include "PCoordinate.h"

class PBoundingBox
{

public:
	PBoundingBox();
	PBoundingBox(double minX, double minY, double maxX, double maxY);
	virtual ~PBoundingBox();

	virtual void add(const PBoundingBox& box);
	double getMaxX() const;
	double getMaxY() const;
	double getMinX() const;
	double getMinY() const;
	void setMaxX(double v) ;
	void setMaxY(double v) ;
	void setMinX(double v) ;
	void setMinY(double v) ;
	
	virtual bool isValid() const;
	double getWidth() const;
	double getHeight() const;
	bool isInside(double X, double Y) const;
protected:
	void extend(double X, double Y);
private:
	double minX, minY, maxX, maxY;

};

