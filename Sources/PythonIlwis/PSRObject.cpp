///////////////////////////////////////////////////////////
//  PSRObject.cpp
//  Implementation of the Class PSRObject
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PSRObject.h"


PSRObject::PSRObject() {

}



PSRObject::~PSRObject(){

}

int PSRObject::getEPSG() const{

	return iUNDEF;
}


string PSRObject::getName() const{

	return  "?";
}