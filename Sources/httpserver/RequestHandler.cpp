#include "headers/toolspch.h"
#include "HttpServer\command.h"
#include "httpserver\RequestHandler.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\OWSHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSGetCapabilities.h"
#include "httpserver\WPSDescribeProcess.h"
#include "httpserver\WPSExecute.h"
#include "httpserver\WMSGetCapabilities.h"
#include "httpserver\SharedDataHandler.h"
#include "Engine\Base\System\Engine.h"


using namespace ILWIS;

void RequestHandler::parseQuery(const String& query, map<String, String>& kvps) {
	Array<String> queryParts;
	Split(query, queryParts,"&");
	for(int i=0; i < queryParts.size(); ++i) {
		String queryElement = queryParts[i];
		String key = queryElement.sHead("=");
		String value = queryElement.sTail("=");
		kvps[key.toLower()] = value;
	}
} 
RequestHandler *RequestHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info) {
	if ( request_info->query_string == 0)
		return 0;

	String	query(request_info->query_string);
	getEngine()->InitThreadLocalVars();
	getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvSERVERMODE, new bool(true));
	map<String, String> kvps;
	RequestHandler::parseQuery(query, kvps);
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

	if ( serviceValue == "wps" && requestValue == "getcapabilities")
		return new WPSGetCapabilities(c, request_info, kvps);
	if ( serviceValue == "wps" && requestValue == "describeprocess")
		return new WPSDescribeProcess(c, request_info, kvps);
	if ( serviceValue == "wps" && requestValue == "execute")
		return new WPSExecute(c, request_info, kvps);
	if ( serviceValue == "wms" && requestValue == "getcapabilities")
		return new WMSGetCapabilities(c, request_info, kvps);

	getEngine()->RemoveThreadLocalVars();
	return 0;
}

RequestHandler::RequestHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps) : 
	connection(c), request_info(ri)
{
	for(map<String, String>::const_iterator iter = _kvps.begin(); iter != _kvps.end(); ++iter)
		kvps[(*iter).first] = (*iter).second;
}

void RequestHandler::setConfig(map<String, String>* _config) {
	config = _config;
}

void RequestHandler::writeError(const String& err, const String& code) const{
}

void RequestHandler::writeResponse(IlwisServer *server) const {
}

bool RequestHandler::needsResponse() const{
	return false;
}

struct mg_connection *RequestHandler::getConnection() const{
	return connection;
}

String RequestHandler::getValue(const String& key) const{
	map<String, String>::const_iterator iter;
	if ( (iter = kvps.find(key)) != kvps.end()) {
		return (*iter).second;
	}
	return sUNDEF;
}

String RequestHandler::getConfigValue(const String& key) const {
	map<String, String>::const_iterator iter;
	if ( (iter = (*config).find(key)) != (*config).end()) {
		return (*iter).second;
	}
	return sUNDEF;
}



bool RequestHandler::doCommand() {
	return false;

}

