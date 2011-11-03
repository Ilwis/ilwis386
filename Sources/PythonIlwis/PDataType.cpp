///////////////////////////////////////////////////////////
//  PDataType.cpp
//  Implementation of the Class PDataType
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PDataType.h"


PDataType::PDataType(){
}

PDataType::PDataType(const std::string& n){
	name = n;
}

PDataType::~PDataType(){
}


std::string PDataType::getName() const{
	return name;
}

