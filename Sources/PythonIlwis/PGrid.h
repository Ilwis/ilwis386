///////////////////////////////////////////////////////////
//  Grid.h
//  Implementation of the Class Grid, Grid1D, Grid2D, Grid3D
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "Engine\Map\Raster\Map.h"
#include "PValue.h"
#include "PPixel.h"
#include <vector>

class PExtent {
public:
	enum Dimensions{t1D, t2D, t3D,tUNKNWON};
	PExtent(int line, int begin=0, int end=0);
	PExtent(const PPixel& base, int xlength=0, int ylength=0, int zlength=0);

	PPixel getBase() const;
	long getXDim() const;
	long getYDim() const;
	long getZDim() const;
	bool isValid(PExtent::Dimensions) const;
	Dimensions getDimension() const;

private:
	PPixel base;
	long xDim,yDim,zDim;

};

class PGrid {
public:
	//python interface
	enum Axis{aX,aY,aZ};
	PGrid(const PGrid& grid);
	PGrid(long xsize=0, long ysize=1, long zsize=1);
	double getd(const PPixel& px);
	double getd(long x, long y=iUNDEF, long z=iUNDEF);
	//std::string gets(const PPixel& px);
	//std::string gets(long x, long y=iUNDEF, long z=iUNDEF);
	void setd(double v, long x, long y=0, long z=0);
//	void sets(const PPixel& px, double v);
	long dimSize(Axis axis) const;
	void resize(long xsize, long ysize=1, long zsize=1);

	//other
	RealBuf& get(long y, long z=0);
private:
	double getd(long x, long y=iUNDEF, long z=iUNDEF) const;
	vector< vector< RealBuf >> values;
	RealBuf *cache;
	long lasty;
	RealBuf dummy;

};



