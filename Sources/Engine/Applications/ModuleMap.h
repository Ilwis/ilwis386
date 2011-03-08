#pragma once

struct ApplicationInfo;

typedef vector<ApplicationInfo *> InfoVector;
typedef InfoVector::iterator InfoVIter;
typedef pair<string, ApplicationInfo *> InfoPair;
typedef InfoVector *(*AppInfo)();
//typedef ILWIS::Module *(*ModuleInfo)();
typedef void(*ModuleInit)(ILWIS::Module *module);
typedef void(*Test1)(ApplicationInfo* name);
typedef IlwisObjectPtr *(* CreateFunc)(const FileName& fn, IlwisObjectPtr& p, const String& sExpr, vector<void *> parms);
typedef String (* CommandName)();
typedef String (* WPSMetadataFunc)();

//typedef void (* HandlerFunction)(const String& sCmd);

struct _export ApplicationMetadata {
	ApplicationMetadata() { returnType = IlwisObject::iotANY; } 
	String wpsxml;
	IlwisObject::iotIlwisObjectType returnType;
};

struct _export ApplicationQueryData {
	String queryType;
	String expression;
	FileName fnInput;
};

typedef ApplicationMetadata (* MetaDataFunc)(ApplicationQueryData* query);

struct _export ApplicationInfo 
{
public:
	String name;
	CreateFunc createFunction;
	MetaDataFunc metadata;
	ApplicationInfo() { createFunction = NULL;metadata = NULL;} 
};

class _export ApplicationMap : public map<String, ApplicationInfo *> {
public:
	static ApplicationInfo *newApplicationInfo(CreateFunc appFunc, String appName);
	static ApplicationInfo *newApplicationInfo(CreateFunc appFunc, String appName, MetaDataFunc mdFunc);
	ApplicationInfo * operator[](String name);
	void addApplications(vector<ApplicationInfo *> apps);
	void addExtraFunctions();
	~ApplicationMap();

private:

};

class _export ModuleMap : public map<String, ILWIS::Module*> {
public:
	~ModuleMap();
	void addModule(const FileName& fnModule, bool retry = false);
	void addModules();
	void initModules();
	void getAppInfo(const String& name, vector<ApplicationInfo *>& infos);
private:
	void addFolder(const String& dir);
	void addModule(ILWIS::Module *m);
	map<String, ModuleInit> moduleInits;
	ApplicationMap applications;
	vector<FileName> retryList;

};

typedef ModuleMap::iterator ModuleIter;
typedef ApplicationMap::iterator AppIter;
