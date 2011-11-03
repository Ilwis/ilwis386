///////////////////////////////////////////////////////////
//  PPixel.h
//  Implementation of the Class PPixel
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once
#include "Headers\toolspch.h"

using namespace std;

class PPixel
{

public:
	PPixel();
	PPixel(long x, long y, long z=iUNDEF);

	long getX() const;
	long getY() const;
	long getZ() const;
	void setX(long v);
	void setXY(long y, long x);
	void setXYZ(long y, long x, long z);
	void setY(long v);
	void setZ(long v);
	bool isValid() const;
	bool isEqual(const PPixel& pix) const;
	bool isNear2D(const PPixel& pix, long tolerance) const;
	string toString() const;

	//other
	PPixel(const RowCol& rc);
	RowCol getRC() const;
protected:
private:
	RowCol rc;
	long z;


};

