///////////////////////////////////////////////////////////
//  PBoundingBox.cpp
//  Implementation of the Class PBoundingBox
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PBoundingBox.h"


PBoundingBox::PBoundingBox(){
	minX = minY =  0;
	maxX = maxY = -1;
}

PBoundingBox::PBoundingBox(double _minX, double _minY, double _maxX, double _maxY) {
	minX = _minX;
	maxX = _maxX;
	minY = _minY;
	maxY = _maxY;
}

PBoundingBox::~PBoundingBox(){

}

bool PBoundingBox::isValid() const{
	return minX <= maxX && minY <= maxY;
}

double PBoundingBox::getWidth() const{
	if (!isValid())
		return iUNDEF;
	return maxX - minX;
}

double PBoundingBox::getHeight() const{
	if (!isValid())
		return iUNDEF;
	return maxY - minY;
}

void PBoundingBox::add(const PBoundingBox& box) {
	if ( !box.isValid())
		return;

	if ( !isValid()) {
		minX = box.getMinX();
		maxX = box.getMaxX();
		minY = box.getMinY();
		maxY = box.getMaxY();
	}

	if ( box.getMinX() < minX)
		minX = box.getMinX();
	if ( box.getMaxX() > maxX)
		maxX = box.getMaxX();
	if ( box.getMinY() < minY)
		minY = box.getMinY();
	if ( box.getMaxY() > maxY)
		maxY = box.getMaxY();

}

double PBoundingBox::getMaxX() const{

	return maxX;
}


double PBoundingBox::getMaxY() const{

	return maxY;
}

double PBoundingBox::getMinX() const{

	return minX;
}

double PBoundingBox::getMinY() const{

	return minY;
}

void PBoundingBox::setMaxX(double v) {
	maxX = v;
}

void PBoundingBox::setMaxY(double v) {
	maxY = v;
}

void PBoundingBox::setMinX(double v) {
	minX = v;
}

void PBoundingBox::setMinY(double v) {
	minY = v;
}

void PBoundingBox::extend(double x, double y){
	if ( x > maxX) maxX = x;
	if ( y > maxY) maxY = y;
	if ( y < minY) minY = y;
	if ( x < minX) minX = x;
}

bool PBoundingBox::isInside(double x, double y) const {
	return x > minX && x < maxX && y > minY && y < maxY;
}