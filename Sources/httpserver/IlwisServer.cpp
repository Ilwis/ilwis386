#include "headers/toolspch.h"
#include  <Winsock2.h>
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine/base/system/engine.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\SharedDataHandler.h"
#include "HttpServer\IlwisServer.h"
#include "ServiceConfiguration.h"

using namespace ILWIS;

IlwisServer *server = 0;
map<String,String> IlwisServer::config;

BOOL WINAPI DllMain(
  __in  HINSTANCE hinstDLL,
  __in  DWORD fdwReason,
  __in  LPVOID lpvReserved
  ) 
{
	if ( fdwReason == DLL_PROCESS_DETACH) {
		delete server;
		server = 0;
	}
	return TRUE;
}

void executehttpcommand(const String& cmd) {
	String *_cmd = new String(cmd);
	if ( server == 0) {
		AfxBeginThread(IlwisServer::executeInThread, _cmd);	
	}
	while(server==0 || !server->fValid());
	server->start(_cmd);
}

extern "C" _export void moduleInit(ILWIS::Module *module) {
	getEngine()->getContext()->ComHandler()->AddCommand("startserver",executehttpcommand);

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("HttpServer", "IlwisHttpServer.dll",ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifInit, (void *)moduleInit);

	return module;
}

//-------------------------------------
IlwisServer::IlwisServer() : ctx(0),isValid(false){

}

IlwisServer::~IlwisServer() {
	if ( ctx)
		mg_stop(ctx);
	ctx = 0;
	::WaitForSingleObject(watcherThread->m_hThread, 4000);
	WSACleanup();
}

UINT IlwisServer::executeInThread(LPVOID lp) {
	String *cmd = (String *)lp;
	String ilwDir = getEngine()->getContext()->sIlwDir() + "\\Services\\";
	if ( server == 0) {
		IlwisServer *serverTemp = new IlwisServer();
		serverTemp->loadServices();
		server = serverTemp;
	}
	return 1;
}

void IlwisServer::loadServices() {
	int depth = 0;
	String path = getEngine()->getContext()->sIlwDir() + "Services";
	addFolder(path, depth + 1);
	int index = 0;
	Module *mod = getEngine()->getModule(index);
	while(mod) {
		ServiceInfoFunc handlerFunc = (ServiceInfoFunc)(mod->getMethod(ILWIS::Module::ifService));
		if ( handlerFunc) {
			ServiceInfoVec *infos = (*handlerFunc)();
			if ( infos->size() > 0) {
				for(int i = 0; i < infos->size(); ++i) {
					handlers[infos->at(i)->id()] =  infos->at(i);
				}
			}
			delete infos;
		}
		mod = getEngine()->getModule(++index);
	}
	isValid = true;
}

void IlwisServer::addFolder(const String& dir, int depth) {
	if ( depth > 2)
		return;
	CFileFind finder;
	String pattern = dir + "\\*.*";
	BOOL fFound = finder.FindFile(pattern.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sExt == ".dll" || fnNew.sExt == ".DLL")
				addServices(fnNew);
		} else {
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sFile != "." && fnNew.sFile != ".." && fnNew.sFile != "")
				addFolder(String(fnNew.sFullPath()), depth + 1);
		}
	}
}

void IlwisServer::addServices(const FileName& fnModule) {
	try{
		String sName = fnModule.sFile + fnModule.sExt;
		sName = sName.toLower();
		if ( sName == "cygpng12.dll" || sName == "cygz.dll") // these two are problematic and will be skipped, not ILWIS anyway
			return;
		HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
		if ( hm != NULL ) {
			//AppInfo f = (AppInfo)GetProcAddress(hm, "getCommandInfo");
			ModuleInfo m = (ModuleInfo)GetProcAddress(hm, "getModuleInfo");
			if ( m != NULL) {
				ILWIS::Module *mod = (*m)(fnModule);
				ILWIS::Module::ModuleInterface type = mod->getInterfaceVersion();
				getEngine()->getVersion()->fSupportsModuleInterfaceVersion(type, mod->getName());
				getEngine()->addModule(mod);
			}
		} 
	}catch(ErrorObject& err){
		err.Show();
	}
}
bool IlwisServer::start(String* cmd) {
	ParmList pl(*cmd);
	delete cmd;
	String options;
	char *coptions[40];
	memset(coptions,0, 40);
	//FileName fnConfig(String("%Sservices.ini",getEngine()->getContext()->sIlwDir()));
	ServiceConfiguration config;

	int index = 0;
	addOptions(index,coptions, "document_root", pl.fExist("data_folder")? String("-r %S", pl.sGet("data_root")) : getEngine()->sGetCurDir());
	if ( pl.fExist("port")) {
		addOptions(index, coptions,"listening_ports",pl.sGet("port"));
	} else {
		String port = config.get("Server:Context");
		if ( port != "?")
			addOptions(index, coptions,"listening_ports",port);
	}
	if ( pl.fExist("threads")) {
		addOptions(index, coptions,"num_threads",pl.sGet("threads"));
	}


	ctx = mg_start(&event_handler, 0, (const char **)coptions);
	watcherThread = AfxBeginThread(IlwisServer::timeOutChecker, (void*)this);

	return true;
}

void IlwisServer::addOptions(int& index, char *coptions[40], const String& option, const String& value) {
	coptions[index] = new char(option.size() + 1);
	strcpy_s(coptions[index++],option.size() + 1,  option.c_str());
	coptions[index] = new char(value.size() + 1);
	strcpy_s(coptions[index++], value.size() + 1, value.c_str());
}



void *IlwisServer::event_handler(enum mg_event ev, struct mg_connection *conn,  const struct mg_request_info *request_info) {
	void *processed = "yes"; 
	if ( ev == MG_NEW_REQUEST) {
		String uri(request_info->uri);

		if (  uri.find("/ilwis_server") != string::npos) {

			RequestHandler *rh = server->createHandler(conn, request_info);
			if ( rh) {
				bool res = rh->doCommand();
				if ( res || rh->needsResponse())
					rh->writeResponse();
				delete rh;
			}
		} else {
			ServiceConfiguration configf;
			String name = configf.get("Server:Context:Name");
			mg_printf(conn,"%s  %s\n", name.c_str(), ILWIS::Time::now().toString().c_str());
		}

	}
//	mg_printf(conn,"code %d\n", request_info->remote_port);
	return processed;
}

UINT IlwisServer::timeOutChecker(void *p) {
	IlwisServer *serv = (IlwisServer *)p;
	while(serv->ctx != 0) {
		Sleep(3000);
		serv->checkTimeOutLocations();
	}
	return 1;
}

void IlwisServer::addTimeOutLocation(const String& folder, time_t t) {
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	timeOuts[folder] = t;
}

#define REMOVE_TIME_OUT 600

void IlwisServer::checkTimeOutLocations() {
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	for(map<String, time_t>::iterator cur = timeOuts.begin(); cur != timeOuts.end(); ++cur) {
		time_t end = time(0);
		double diff = difftime(end, (*cur).second);
		if ( diff > REMOVE_TIME_OUT) { // if time out passed, removed dir
			system(String("rmdir %S /s /q", (*cur).first).c_str());
			timeOuts.erase(cur);
			break; // any other timouts will be handled in the next call; it isnt very critical anyway.
		}
	}
}

void IlwisServer::updateTimeOutLocation(const String& folder) {
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	map<String, time_t>::iterator cur = timeOuts.find(folder);
	if ( cur != timeOuts.end()) {
		timeOuts[folder] = time(0) + REMOVE_TIME_OUT; // extend the timeout.
	}
}

RequestHandler *IlwisServer::createHandler(struct mg_connection *c, const struct mg_request_info *request_info) {
	//if ( request_info->query_string == 0)
	//	return 0;

	/*if ( RequestHandler::activeServices.size() == 0) {
		int noS = getConfigValue("ActiveServices:NumberOfServices").iVal();
		if ( nos != iUNDEF) {
			for(int j = 0; j < nos; ++j) {
				String serv = getConfigValue(String("ActiveServices:Service%d", j));
				activeServices[serv.sHead(",")] = serv.sTail(",") == "true";
			}
		}
	}*/

	String uri(request_info->uri);
	getEngine()->InitThreadLocalVars();
	getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvSERVERMODE, new bool(true));
	map<String, String> kvps;
	if ( request_info->query_string)  {
	    String query(request_info->query_string);
		RequestHandler::parseQuery(query, kvps);
	}

	int index;
	if ( (index = uri.find("shared_data/")) != string::npos) {
		String context;
		index = index - 2;
		char ch = uri[index];
		do {
			context = ch + context;
			ch = uri[--index];
		} while ( ch != '/');
		kvps["context"] = context;
		SharedDataHandler *sdh = new SharedDataHandler(c, request_info, kvps, server);
		return sdh;
	}

	String serviceValue, requestValue;
	for(map<String,String>::const_iterator iter = kvps.begin(); iter != kvps.end(); ++iter) {
		String key = (*iter).first;
		key.toLower();
		if ( key == "service")
			serviceValue = (*iter).second;
		if ( key == "request")
			requestValue = (*iter).second;
	}
	serviceValue.toLower();
	requestValue.toLower();
	map<String, ServiceInfo *>::const_iterator iter = handlers.find(serviceValue + "::" + requestValue);
	if ( iter != handlers.end()) {
		CreateRequestHandler func = (*iter).second->createHandler;
		return func(c,request_info,kvps,this);
	}
	return 0;
}



