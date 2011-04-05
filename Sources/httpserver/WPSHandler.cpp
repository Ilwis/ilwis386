#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WPSHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "httpserver\XMLDocument.h"
#include "Headers\XQuila\xqilla\utils\XStr.hpp"
#include <xercesc\dom\DOMLSSerializer.hpp>

using namespace ILWIS;

WPSHandler::WPSHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps) : 
RequestHandler(c,ri,kvps)
{
}

void WPSHandler::createHeader(XERCES_CPP_NAMESPACE::DOMDocument *doc, const String& xsd) const{
	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();
	root->setAttribute(L"xmlns:wps",L"http://www.opengis.net/wps/1.0.0");
	root->setAttribute(L"xmlns:ows",L"http://www.opengis.net/ows/1.1");
	root->setAttribute(L"xmlns:xlink",L"http://www.w3.org/1999/xlink");
	root->setAttribute(L"xmlns:xsi",L"http://www.w3.org/2001/XMLSchema-instance");
	String schema("http://www.opengis.net/wps/1.0.0 ../%S", xsd);
	root->setAttribute(L"xsi:schemaLocation",X(schema.scVal()));
	root->setAttribute(L"service",L"WPS");
	root->setAttribute(L"version",L"1.0");
	root->setAttribute(L"xml:lang",L"en-CA");

}