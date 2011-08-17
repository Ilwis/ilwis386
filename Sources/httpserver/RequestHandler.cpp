#include "headers/toolspch.h"
#include "HttpServer\IlwisServer.h"
#include "Engine\Map\basemap.h"
#include "httpserver\RequestHandler.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\OWSHandler.h"
//#include "httpserver\WPSHandler.h"
//#include "httpserver\WPSGetCapabilities.h"
//#include "httpserver\WPSDescribeProcess.h"
//#include "httpserver\WPSExecute.h"
//#include "httpserver\WMSGetCapabilities.h"
//#include "httpserver\WMSGetMap.h"
//#include "httpserver\GNCCatalogHandler.h"
#include "httpserver\SharedDataHandler.h"
//#include "httpserver\UpdateService.h"
#include "Engine\Base\System\Engine.h"


using namespace ILWIS;

map<String, bool> RequestHandler::activeServices;

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


RequestHandler::RequestHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv) : 
	connection(c), request_info(ri)
{
	for(map<String, String>::const_iterator iter = _kvps.begin(); iter != _kvps.end(); ++iter)
		kvps[(*iter).first] = (*iter).second;
	ilwisServer = serv;
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
	String tempKey = key;
	tempKey.toLower();
	map<String, String>::const_iterator iter;
	if ( (iter = (*config).find(tempKey)) != (*config).end()) {
		return (*iter).second;
	}
	return sUNDEF;
}



bool RequestHandler::doCommand() {
	return false;

}

