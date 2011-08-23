#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"


using namespace ILWIS;

OWSHandler::OWSHandler(const String& name,struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv) : 
RequestHandler(name, c,ri,kvps, serv)
{
}

void OWSHandler::writeError(const String& err, const String& code) const{


	ILWIS::XMLDocument doc;
	doc.addNodeTo(doc,"ExceptionReport");
	doc.addNameSpace("xmlns:ows","http://www.opengis.net/ows/1.1");
	doc.addNameSpace("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	doc.append_attribute("xsi:schemaLocation") = "http://www.opengis.net/ows/1.1 ../../../ows/1.1.0/owsExceptionReport.xsd";
	doc.append_attribute("xml:lang") = "en-CA";

	pugi::xml_node exc = doc.addNodeTo(doc,"ows:Exception");
	exc.append_attribute("exceptionCode") = code.c_str();
	doc.addNodeTo(exc,"ows:ExceptionText",err);

	String txt = doc.toString();

	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}
