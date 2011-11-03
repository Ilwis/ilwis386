///////////////////////////////////////////////////////////
//  RootObject.h
//  Implementation of the Class RootObject
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once


using namespace std;
class FileName;

class RootObject
{

public:
	RootObject();
	RootObject(const string& name, bool setWorkingDir);
	virtual ~RootObject();

	string getId();
	double getModifiedTime();
	string getName();
	virtual bool isValid() const = 0;
	virtual bool isReadOnly() const =0;
	
protected:
	void changeWorkingDir(const FileName& fn);
	string name;

};
