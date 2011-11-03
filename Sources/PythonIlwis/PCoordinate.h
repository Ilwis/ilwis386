///////////////////////////////////////////////////////////
//  PCoordinate.h
//  Implementation of the Class PCoordinate
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once
#include "Headers\toolspch.h"

using namespace std;

class PCoordinate
{

public:
	//python interface
	PCoordinate();
	PCoordinate(const PCoordinate& coord);
	PCoordinate(double x, double y, double z=rUNDEF);
	virtual ~PCoordinate();

	double getX() const;
	double getY() const;
	double getZ() const;
	void setX(double v);
	void setXYZ(double y, double x, double z = 0);
	void setY(double v);
	void setZ(double v);
	virtual bool isValid() const;
	bool isNear(const PCoordinate& crd, double tolerance);
	bool isEqual(const PCoordinate& crd);
	virtual string toString(long precission=0) const;

	//other
	PCoordinate(const Coord& crd);
	Coord getIlwisCoord() const;

private:
	Coord c;


};

