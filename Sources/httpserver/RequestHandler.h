#pragma once

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc\dom\DOMImplementationLS.hpp>
#include <xercesc\Sax\InputSource.hpp>
#include <xercesc\framework\MemBufInputSource.hpp>
#include "HttpServer\mongoose.h"

using namespace XERCES_CPP_NAMESPACE;
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
	virtual void doCommand();
	void setConfig(map<String, String>* _config);
protected:
	RequestHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);
	XercesDOMParser *initXerces() const;
	//void createWPSHeader(XERCES_CPP_NAMESPACE::DOMDocument *doc, const String& xsd) const;
	XERCES_CPP_NAMESPACE::DOMElement  *createTextNode(XERCES_CPP_NAMESPACE::DOMDocument *doc,const String& nodeName, const String& value) const;
	String createOutput(XERCES_CPP_NAMESPACE::DOMDocument *doc) const;
	String getConfigValue(const String& key) const;
	struct mg_connection *connection;
	const struct mg_request_info *request_info;
	map<String, String> kvps;
	map<String, String> *config;
};
}