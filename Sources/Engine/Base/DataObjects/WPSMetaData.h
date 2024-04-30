#pragma once

#include "Engine\Applications\ModuleMap.h"

#define TEXT_SIZE 1000
class WPSMetaData;

class _export ProcessMetaData : public map<String, WPSMetaData *> {
public:
	void addDefinitionFile(const FileName& fn);
	WPSMetaData * operator[](const String& name);
};


class _export WPSMetaData {
public:
	WPSMetaData(const String& appName);
	~WPSMetaData();
	String toString();
private:
	String id;
};