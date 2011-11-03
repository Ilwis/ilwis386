///////////////////////////////////////////////////////////
//  PPixel.cpp
//  Implementation of the Class PPixel
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PPixel.h"


PPixel::PPixel() {
 z = iUNDEF;
}

PPixel::PPixel(long x, long y, long zd) {
	rc = RowCol(y,x);
	z = zd;
}

PPixel::PPixel(const RowCol& rcp) {
	rc = rcp;
}

long PPixel::getX() const{

	return rc.Col;
}


long PPixel::getY() const{

	return rc.Row;
}

long PPixel::getZ() const{

	return z;
}

void PPixel::setX(long v){
	rc.Col = v;
}


void PPixel::setXY(long x, long y){
	rc = RowCol(y,x);
}

void PPixel::setXYZ(long x, long y, long zd){
	rc = RowCol(y,x);
	z =zd;
}


void PPixel::setY(long v){
	rc.Row = v;
}

void PPixel::setZ(long v){
	z = v;
}

bool PPixel::isValid() const {
	return !rc.fUndef();
}

bool PPixel::isEqual(const PPixel& pix) const {
	if (!isValid() || !pix.isValid())
		return false;
	return rc == pix.getRC();
}

RowCol PPixel::getRC() const {
	return rc;
}

bool PPixel::isNear2D(const PPixel& pix, long tolerance) const {
	if (!isValid() || !pix.isValid())
		return false;
	return rc.fNear(pix.getRC(), tolerance);

}

string PPixel::toString() const{
	if ( z == iUNDEF)
		return String("%d, %d", rc.Col, rc.Row);
	return String("%d, %d %d", rc.Col, rc.Row, z);
}