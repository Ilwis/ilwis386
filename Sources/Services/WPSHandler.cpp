#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"


using namespace ILWIS;

WPSHandler::WPSHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv) : 
OWSHandler(c,ri,kvps, serv)
{
}

void WPSHandler::createHeader(ILWIS::XMLDocument& doc, const String& xsd) const{
	pugi::xml_node first = doc.first_child();
	if ( first == 0)
		return;

	doc.addNameSpace("xlink","http://www.w3.org/1999/xlink");
	doc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");
	doc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
	doc.addNameSpace("xsi","http://www.w3.org/2001/XMLSchema-instance");

	first.append_attribute("service") = "WPS";
	first.append_attribute("version") = "1.0.0";
	first.append_attribute("xml:lang") = "en-CA";
	first.append_attribute("xsi:schemaLocation") = xsd.c_str();
	first.append_attribute("updateSequence") = "1";

}

