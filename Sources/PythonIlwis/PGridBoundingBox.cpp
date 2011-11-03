///////////////////////////////////////////////////////////
//  PGridBoundingBox.cpp
//  Implementation of the Class PGridBoundingBox
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PGridBoundingBox.h"


PGridBoundingBox::PGridBoundingBox(){
}

PGridBoundingBox::PGridBoundingBox(RowCol rc) {
	setMinX(0);
	setMaxX(rc.Col);
	setMinY(0);
	setMaxY(rc.Row);
}


PGridBoundingBox::~PGridBoundingBox(){

}

void PGridBoundingBox::add(const PPixel& pix){
	PBoundingBox::extend(pix.getX(), pix.getY());
}

bool PGridBoundingBox::isInside(const PPixel& pix){
	return false;
}

PGridBoundingBox& PGridBoundingBox::operator+(const PGridBoundingBox& box){
	return *this;
}