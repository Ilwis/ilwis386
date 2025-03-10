#include "headers/toolspch.h"
#include  <Winsock2.h>
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine/base/system/engine.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\SharedDataHandler.h"
#include "HttpServer\command.h"

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
	String ilwDir = getEngine()->getContext()->sIlwDir();
	if ( server == 0) {
		IlwisServer *serverTemp = new IlwisServer();
		String name("%Sconfig.ini",ilwDir);
		serverTemp->ReadConfigFile(name);
		server = serverTemp;
	}
	return 1;
}

void IlwisServer::ReadConfigFile(FileName fnConfig) {
	if ( fnConfig.fExist() == false)
		return;
	ifstream configfile(fnConfig.sFullPath().c_str());
	String line;
	if ( configfile.is_open()) {
		String prefix;
		while(configfile.good()) {
			string l;
			getline(configfile,l);
			line = l;
			line = line.sTrimSpaces();
			if (line[0] == '[') {
				prefix = line.sSub(1, line.size() - 2);
			} else {
				if ( line != "") {
					String key = line.sHead("=");
					key = key.sTrimSpaces();
					String value = line.sTail("=");
					value = value.sTrimSpaces();
					IlwisServer::config[prefix + ":" + key] = value;
				}
			}

		}
	}
	isValid = true;
}


bool IlwisServer::start(String* cmd) {
	ParmList pl(*cmd);
	delete cmd;
	String options;
	char *coptions[40];
	memset(coptions,0, 40);
	int index = 0;
	addOptions(index,coptions, "document_root", pl.fExist("data_folder")? String("-r %S", pl.sGet("data_root")) : getEngine()->sGetCurDir());
	if ( pl.fExist("port")) {
		addOptions(index, coptions,"listening_ports",pl.sGet("port"));
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

		if (  uri == "/ilwis_server") {

			RequestHandler *rh = RequestHandler::createHandler(conn, request_info);
			if ( rh) {
				rh->setConfig(&IlwisServer::config);
				bool res = rh->doCommand();
				if ( res || rh->needsResponse())
					rh->writeResponse(server);
				delete rh;
			}
		}
		else if ( uri.find("/shared_data/") != string::npos) {
			map<String, String> dummy;
			SharedDataHandler *sdh = new SharedDataHandler(conn, request_info, dummy, "WPS:ServiceContext:SharedData");
			sdh->setConfig(&IlwisServer::config);
			sdh->writeResponse(server);
			delete sdh;
		}
		else if ( uri.find("/result_data/") != string::npos) {
			map<String, String> dummy;
			SharedDataHandler *sdh = new SharedDataHandler(conn, request_info, dummy, "WPS:ExecutionContext:Root");
			sdh->setConfig(&IlwisServer::config);
			sdh->writeResponse(server);
			delete sdh;
		} else {
			mg_printf(conn,"Ilwis Server  %s\n", ILWIS::Time::now().toString().c_str());
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



