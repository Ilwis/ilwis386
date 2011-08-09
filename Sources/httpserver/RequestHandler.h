#pragma once

#include "HttpServer\mongoose.h"

	class IlwisServer;

namespace ILWIS {
class RequestHandler {
public:
	static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info);
	static void parseQuery(const String& query, map<String, String>& kvps);

	struct mg_connection *getConnection() const;
	String getValue(const String& key) const;
	virtual void writeResponse(IlwisServer*server=0) const;
	virtual bool needsResponse() const;
	virtual bool doCommand();
	virtual void setConfig(map<String, String>* _config);
	virtual void writeError(const String& err, const String& code="none") const;
protected:
	RequestHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);
	String getConfigValue(const String& key) const;
	struct mg_connection *connection;
	const struct mg_request_info *request_info;
	map<String, String> kvps;
	map<String, String> *config;
	static map<String, bool> activeServices;
};
}