#pragma once

#include "HttpServer\mongoose.h"
#include "HttpServer/ServiceConfiguration.h"

class IlwisServer;

namespace ILWIS {
class _export RequestHandler {
public:
	static void parseQuery(const String& query, map<String, String>& kvps);

	struct mg_connection *getConnection() const;
	String getValue(const String& key) const;
	virtual void writeResponse() const;
	virtual bool needsResponse() const;
	virtual bool doCommand();
	virtual void writeError(const String& err, const String& code="none") const;
	String getId() const { return id; }
protected:
	RequestHandler(const String& name, struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

	String getConfigValue(const String& key) const;
	struct mg_connection *connection;
	const struct mg_request_info *request_info;
	map<String, String> kvps;
	static map<String, bool> activeServices;
	IlwisServer *ilwisServer;
	ILWIS::ServiceConfiguration config;
	String id;
};
}