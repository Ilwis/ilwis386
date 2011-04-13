#pragma once

struct CommandInfo;

typedef vector<CommandInfo *> InfoVector;
typedef InfoVector::iterator InfoVIter;
typedef pair<string, CommandInfo *> InfoPair;
typedef InfoVector *(*AppInfo)();
//typedef ILWIS::Module *(*ModuleInfo)();
typedef void(*ModuleInit)(ILWIS::Module *module);
typedef void(*Test1)(CommandInfo* name);
typedef IlwisObjectPtr *(* CreateFunc)(const FileName& fn, IlwisObjectPtr& p, const String& sExpr, vector<void *> parms);
typedef void (*CommandFunc)(const String& expression);
typedef String (* CommandName)();
typedef String (* WPSMetadataFunc)();

//typedef void (* HandlerFunction)(const String& sCmd);

struct _export ApplicationMetadata {
	ApplicationMetadata() { returnType = IlwisObject::iotANY; } 
	String wpsxml;
	IlwisObject::iotIlwisObjectType returnType;
	String skeletonExpression;
};

struct _export ApplicationQueryData {
	String queryType;
	String expression;
	FileName fnInput;
};

typedef ApplicationMetadata (* MetaDataFunc)(ApplicationQueryData* query);

struct _export CommandInfo 
{
public:
	String name;
	CreateFunc createFunction;
	CommandFunc commandFunction;
	MetaDataFunc metadata;
	CommandInfo() { createFunction = NULL;metadata = NULL;commandFunction=NULL;} 
	CommandInfo(const String& sName, CommandFunc cf,MetaDataFunc mf=0) : name(sName), commandFunction(cf), metadata(mf) {}
};

class _export CommandMap : public map<String, CommandInfo *> {
public:
	static CommandInfo *newCommandInfo(CreateFunc appFunc, String appName);
	static CommandInfo *newCommandInfo(CreateFunc appFunc, String appName, MetaDataFunc mdFunc);
	CommandInfo * operator[](String name);
	void addApplications(vector<CommandInfo *> apps);
	void addExtraFunctions();
	void addCommand(const String& sName, CommandFunc cf,MetaDataFunc mf =0); 
	~CommandMap();

private:

};

class _export ModuleMap : public map<String, ILWIS::Module*> {
public:
	~ModuleMap();
	void addModule(const FileName& fnModule, bool retry = false);
	void addModules();
	void initModules();
	void getCommandInfo(const String& name, vector<CommandInfo *>& infos);
	void addCommand(const String& sName, CommandFunc cf,MetaDataFunc mf =0); 
private:
	void addFolder(const String& dir);
	void addModule(ILWIS::Module *m);
	map<String, ModuleInit> moduleInits;
	CommandMap applications;
	vector<FileName> retryList;

};

typedef ModuleMap::iterator ModuleIter;
typedef CommandMap::iterator AppIter;
