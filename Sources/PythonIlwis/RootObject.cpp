///////////////////////////////////////////////////////////
//  RootObject.cpp
//  Implementation of the Class RootObject
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "RootObject.h"
#include "Engine\Base\System\Engine.h"


RootObject::RootObject(){

}

RootObject::RootObject(const string& _name, bool setWorkingDir) : name(_name) {
	FileName fn(name);
	if ( setWorkingDir)
		changeWorkingDir(fn);

}

RootObject::~RootObject(){

}


string RootObject::getId(){

	return  string();
}


double RootObject::getModifiedTime(){

	return 0;
}


string RootObject::getName(){

	return  string();
}

void RootObject::changeWorkingDir(const FileName& fn){
	getEngine()->getContext()->SetCurDir(fn.sDrive + fn.sDir);
}