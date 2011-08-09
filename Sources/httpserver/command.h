#pragma once

#include "HttpServer\mongoose.h"

class IlwisServer {
public:
	IlwisServer();
	~IlwisServer();
	void ReadConfigFile(FileName fnConfig);
	bool start(String* cmd);
	static map<String,String> config;
	static UINT executeInThread(LPVOID lp);
	void checkTimeOutLocations();
	void addTimeOutLocation(const String& folder, time_t t);
	void updateTimeOutLocation(const String& folder);
	bool fValid() { return isValid;}

private:
    struct mg_context *ctx;
	void parseQuery(const String& query, map<String, String>& kvps);
	void addOptions(int& index, char *coptions[40], const String& option, const String& value);
	static void *IlwisServer::event_handler(enum mg_event event, struct mg_connection *conn,  const struct mg_request_info *request_info);
	map<String,time_t> timeOuts;
	CCriticalSection csAccess;
	CWinThread *watcherThread;
	static UINT timeOutChecker(LPVOID data);
	bool isValid;
};

