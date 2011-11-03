#include "pgrid.h"


PExtent::PExtent(int line, int begin, int end){
	base = PPixel(begin, line, 0);
	xDim = end - begin;
	yDim = 1;
	zDim = 1;
	
}

PExtent::PExtent(const PPixel& b, int xlength, int ylength, int zlength) : xDim(xlength), yDim(ylength), zDim(zlength), base(b) {
}

PPixel PExtent::getBase() const{
	return base;
}

long PExtent::getXDim() const{
	return xDim;
}

long PExtent::getYDim() const{
	return yDim;
}

long PExtent::getZDim() const{
	return zDim;
}

bool PExtent::isValid(PExtent::Dimensions type) const{

	if ( xDim != 0 && base.getY() != 0 && type == PExtent::t1D) // get line
		return true;
	else if ( yDim != 0 && base.getY() != 0 && type == PExtent::t2D) // get area
		return true;
	else if ( zDim != 0 && base.getZ() != rUNDEF && type == PExtent::t3D)  // get box
		return true;
	else if ( base.isValid() && zDim == 0 && yDim ==0 && xDim == 0) // get all
		return true;

	return false;
}

PExtent::Dimensions PExtent::getDimension() const {
	if ( isValid(PExtent::t1D))
		return PExtent::t1D;
	if ( isValid(PExtent::t2D))
		return PExtent::t2D;
	if ( isValid(PExtent::t3D))
		return PExtent::t3D;
	return tUNKNWON;

}

//---------------------------------------------------------
PGrid::PGrid(const PGrid& grid) {
	lasty = iUNDEF;
	cache = 0;
	values.resize(grid.dimSize(PGrid::aZ));
	for(long z = 0; z < grid.dimSize(PGrid::aZ); ++z) {
		values.at(z).resize(grid.dimSize(PGrid::aY));
		for(long y = 0; y < grid.dimSize(PGrid::aY); ++y) {
			values.at(z).at(y).Size(grid.dimSize(PGrid::aX));
			for(long x = 0; x < grid.dimSize(PGrid::aX); ++x) {
				double d = grid.getd(x,y,z);
				(values.at(z).at(y))[x] = d;
			}
		}

	}
}

PGrid::PGrid(long xsize, long ysize, long zsize) {
	resize(xsize, ysize, zsize);
}

void PGrid::resize(long xsize, long ysize, long zsize) {
	lasty = iUNDEF;
	cache = 0;
	values.clear();
	values.resize(zsize);
	for(long z = 0; z < zsize; ++z) {
		values.at(z).resize(ysize);
		for(long y = 0; y < ysize; ++y) {
			values.at(z).at(y).Size(xsize);
		}

	}
}

long PGrid::dimSize(Axis axis) const {
	if ( axis == PGrid::aZ)
		return values.size();
	else if ( axis == PGrid::aY)
		return values.at(0).size();
	else
		return values.at(0).at(0).iSize();
}
double PGrid::getd(const PPixel& px){

	return getd(px.getX(), px.getY(), px.getZ());

}

double PGrid::getd(long x, long y, long z) const{
	return (values.at(z).at(y))[x];
}

double PGrid::getd(long x, long y, long z) {
	if ( cache != 0 && y == lasty && z == iUNDEF && lasty != iUNDEF)
		(*cache)[x];

	if ( y == iUNDEF) {
		if ( x < values.at(0).at(0).iSize())
			return (values.at(0).at(0))[x];
	} else if ( z == iUNDEF) {
		if ( y < values.at(0).size() && x < values.at(0).at(y).iSize()) {
			lasty = y;
			cache = &values.at(0).at(y);
			return (*cache)[x];
		}
	} else {
		if ( z < values.size() && y < values.at(z).size() && x < values.at(z).at(y).iSize())
			return (values.at(0).at(y))[x];
	}
	return rUNDEF;
}

RealBuf& PGrid::get(long y, long z) {
	if ( z < values.size() && y < values.at(0).size())
		return values.at(z).at(y);
	return dummy;
}

//std::string PGridD::gets(const PPixel& px){
//	if ( px.getX() < values.iSize())
//		return String("%f",values[px.getX()]);
//	return sUNDEF;
//}
//
//std::string PGridD::gets(long x){
//	if ( x < values.iSize())
//		return String("%f",values[x]);
//	return sUNDEF;
//}
//
void PGrid::setd(double v, long x, long y, long z){
	if ( z < values.size() && y < values.at(0).size() && x < values.at(0).at(0).iSize())
		(values.at(z).at(y))[x] = v;
}
//
//void PGrid1D::sets(const PPixel& px, double v){
//}