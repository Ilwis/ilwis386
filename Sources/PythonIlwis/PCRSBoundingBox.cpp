///////////////////////////////////////////////////////////
//  PCRSBoundingBox.cpp
//  Implementation of the Class PCRSBoundingBox
//  Created on:      19-Sep-2011 1:29:12 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PCRSBoundingBox.h"


PCRSBoundingBox::PCRSBoundingBox() : PBoundingBox(){
	maxZ = minZ = iUNDEF; // on purpose set to be equal. Z is not mandatory, so a undefined Z should not cause trouble
}

PCRSBoundingBox::PCRSBoundingBox(const CoordBounds& bb) : 
	PBoundingBox(bb.cMin.x, bb.cMin.y, bb.cMax.x, bb.cMax.y), 
	maxZ(bb.cMax.z),
	minZ(bb.cMin.z)
{
}

PCRSBoundingBox::~PCRSBoundingBox(){

}

void PCRSBoundingBox::add(const PCoordinate& coord) {
	PBoundingBox::extend(coord.getX(), coord.getY());
	if ( coord.getZ() != rUNDEF && coord.getZ() < minZ) minZ = coord.getZ();
	if ( coord.getZ() != rUNDEF && coord.getZ() > maxZ) maxZ = coord.getZ();
}

bool PCRSBoundingBox::isInside(const PCoordinate& coord){
	bool inside = PBoundingBox::isInside(coord.getX(), coord.getY());
	if ( minZ == iUNDEF || maxZ == iUNDEF) // undefined Z will not be taken into account
		return inside;
	return inside && coord.getZ() > minZ && coord.getZ() < maxZ;
}

CoordBounds PCRSBoundingBox::toCoordBounds() const{
	return CoordBounds(Coord(getMinX(), getMinY()), Coord(getMaxX(), getMaxY()));
}


double PCRSBoundingBox::getMaxZ() const {
	return maxZ;
}

double PCRSBoundingBox::getMinZ() const{
	return minZ;
}

bool PCRSBoundingBox::isValid() const{
	return PBoundingBox::isValid() && maxZ >= minZ;
}

void  PCRSBoundingBox::setMaxZ(double v){
	maxZ = v;
}

void  PCRSBoundingBox::setMinZ(double v){
	minZ = v;
}

PCRSBoundingBox& PCRSBoundingBox::operator+(const PCRSBoundingBox& box) {
	add(PCoordinate(box.getMinX(), box.getMinY(), box.getMinZ()));
	add(PCoordinate(box.getMaxX(), box.getMaxY(), box.getMaxZ()));
	return *this;
}
