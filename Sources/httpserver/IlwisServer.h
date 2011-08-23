#pragma once

#include "HttpServer\mongoose.h"

class IlwisServer;

namespace ILWIS {
	class RequestHandler;
}
typedef ILWIS::RequestHandler *(* CreateRequestHandler)(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer*serv);

struct ServiceInfo {
	ServiceInfo(const String& serv, const String req, CreateRequestHandler handl) : service(serv), request(req), createHandler(handl) {}
	String id() { return service + "::" + request; }
	String service;
	String request;
	CreateRequestHandler createHandler;
};

typedef vector<ServiceInfo *> ServiceInfoVec;
typedef ServiceInfoVec *(*ServiceInfoFunc)();

class _export IlwisServer {
public:
	IlwisServer();
	~IlwisServer();
	bool start(String* cmd);
	static map<String,String> config;
	static UINT executeInThread(LPVOID lp);
	void checkTimeOutLocations();
	void addTimeOutLocation(const String& folder, time_t t);
	void updateTimeOutLocation(const String& folder);
	bool fValid() { return isValid;}
	void loadServices();

private:
    struct mg_context *ctx;
	void parseQuery(const String& query, map<String, String>& kvps);
	void addOptions(int& index, char *coptions[40], const String& option, const String& value);
	static void *IlwisServer::event_handler(enum mg_event event, struct mg_connection *conn,  const struct mg_request_info *request_info);
	ILWIS::RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info) ;
	void addFolder(const String& dir, int depth);
	void addServices(const FileName& fnModule);

	map<String,time_t> timeOuts;
	CCriticalSection csAccess;
	CWinThread *watcherThread;
	static UINT timeOutChecker(LPVOID data);
	map<String, ServiceInfo *> handlers;
	bool isValid;
};

