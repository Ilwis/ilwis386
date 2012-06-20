// main header file for the Engine DLL
//

#ifndef ENGINE_H
#define ENGINE_H

//#include "Headers\xercesc\util\Platforms\Win32\resource.h"		// main symbols
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Engine\Base\System\Database.h"


class COMServerHandler;

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class ForeignFormatMap;
class Logger;
class GdalProxy;

struct RowSelectInfo {
	FileName fn; // table;
	vector<long> raws; // selected rows;
};

namespace ILWIS {
	class Database;
}

class IMPEXP Engine 
{
public:
	friend BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);
	friend _export Engine* getEngine();
	Engine();
	~Engine();
	void Init();
	void SetCurDir(const String& sDir);
	String sGetCurDir() const;
	void *pGetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType);
	void SetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType, void *var);
	void InitThreadLocalVars();
	void RemoveThreadLocalVars();
	void Execute(const String& command);
	FileName fnGetSearchPath(int iIndex);
	void AddSearchPath(const FileName& fn);
	void RemoveSearchPath(const FileName& fn);
	IlwisAppContext *getContext();
	COMServerHandler *GetCOMServerHandler() { return chCOMServerHandler; }
	LRESULT SendMessage(UINT msg, WPARAM p1=0, LPARAM p2=0) ;
	void PostMessage(UINT msg, WPARAM p1=0, LPARAM p2=0); 
	int Message(LPCSTR lpText,LPCSTR lpCaption,UINT uType) { return AfxGetMainWnd()->MessageBox(lpText, lpCaption, uType);};
	bool hasClient();
	bool fStayResident();
	void setStayResident(bool stay);
	bool getDebugMode(char *name=NULL);
	void setDebugMode(bool debug, const String& names="");
	Logger* getLogger();
	ILWIS::Module * getModule(const String& name);
	ILWIS::Module * getModule(int index);
	void addModule(ILWIS::Module *m);
	ILWIS::Version *getVersion();
	static ModuleMap modules;
	bool fServerMode() const;
	static HMODULE getModuleHandle() { return engineHandle; }
	void getServicesFor(const String& serviceType, map<String,String>& services, bool getUrl) const;
	ILWIS::Database *pdb();
	GdalProxy *gdal;

public:
	IlwisAppContext *context;
	COMServerHandler *chCOMServerHandler;
	static ForeignFormatMap formats;
private:
	void loadSystemTables(const String& ilwDir);
	void loadServiceLocations(const String& dir);
	bool stayResident;
	bool debugMode;
	Array<String> debugClasses;
	map<String, String> serviceLocations;
	Logger *logger;
	ILWIS::Version *version;
	static HMODULE engineHandle;
	static Engine *engine;
	ILWIS::Database *db;
};

_export Engine* getEngine();

#endif
