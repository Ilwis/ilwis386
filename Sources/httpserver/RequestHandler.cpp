#include "headers/toolspch.h"
#include "HttpServer\command.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSGetCapabilities.h"
#include "httpserver\WPSDescribeProcess.h"
#include "httpserver\WPSExecute.h"
#include "httpserver\WMSGetCapabilities.h"
#include "httpserver\SharedDataHandler.h"
#include "XQuila\xqilla\xqilla-dom3.hpp"
#include "Engine\Base\System\Engine.h"
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include "xqilla\exceptions\XMLParseException.hpp"
#include <xercesc/framework/MemBufFormatTarget.hpp>

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

XercesDOMParser *RequestHandler::initXerces() const{
	XercesDOMParser *parser = new XERCES_CPP_NAMESPACE::XercesDOMParser;
	parser->setValidationScheme(XERCES_CPP_NAMESPACE::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setValidationSchemaFullChecking(false);
	parser->setCreateEntityReferenceNodes(false);
	return parser;

}

XERCES_CPP_NAMESPACE::DOMElement *RequestHandler::createTextNode(XERCES_CPP_NAMESPACE::DOMDocument *doc,const String& nodeName, const String& value) const {
   wchar_t result[1000];
   XERCES_CPP_NAMESPACE::DOMElement *ele = doc->createElement(nodeName.toWChar(result));
   DOMText* text = doc->createTextNode(value.toWChar(result));
   ele->appendChild(text);
   return ele;

}

String RequestHandler::createOutput(XERCES_CPP_NAMESPACE::DOMDocument *doc) const{
	DOMImplementationLS *domOut = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(L"LS");
	DOMLSSerializer*  writer = domOut->createLSSerializer();

	DOMConfiguration* dc = writer->getDomConfig(); 
	//dc->setParameter(XMLUni::fgDOMErrorHandler,errorHandler); 
	dc->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent,true); 
	dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	CString txt(writer->writeToString(doc));


	doc->release();
	XMLPlatformUtils::Terminate();
	String endResult;
	String temp(txt);
	for(int i=0; i < temp.size(); ++i) {
		if ( temp[i] == '\n') {
			endResult += '\r';
		}
		endResult += temp[i];
	}
	int index = endResult.find("UTF-16");
	if ( index != -1) { // hack, setting other encodings in xerces is a bit awkward
		endResult.replace(index,6,"UTF-8");
	}
	return endResult;

}

void RequestHandler::doCommand() {

}

