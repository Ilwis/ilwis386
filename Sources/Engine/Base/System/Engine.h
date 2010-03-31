// main header file for the Engine DLL
//

#ifndef ENGINE_H
#define ENGINE_H

#include "Headers\xercesc\util\Platforms\Win32\resource.h"		// main symbols
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\Version.h"


class COMServerHandler;

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class ForeignFormatMap;
class Logger;

class IMPEXP Engine 
{
public:
	Engine();
	~Engine();
	void Init(const String& prog, const String& sCmdLn);
	void SetCurDir(const String& sDir);
	String sGetCurDir() const;
	void *pGetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType);
	void SetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType, void *var);
	void InitThreadLocalVars();
	void RemoveThreadLocalVars();
	void Execute(String& command);
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

public:
	IlwisAppContext *context;
	COMServerHandler *chCOMServerHandler;
	static ForeignFormatMap formats;
private:
	bool stayResident;
	bool debugMode;
	Array<String> debugClasses;
	Logger *logger;
	ILWIS::Version *version;

};

_export Engine* getEngine();

#endif
