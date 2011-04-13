#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSDescribeProcess.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "httpserver\XMLDocument.h"
#include <xercesc\dom\DOMLSSerializer.hpp>


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

WPSDescribeProcess::WPSDescribeProcess(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps)
: WPSHandler(c,ri,kvps)
{
	map<String,String>::const_iterator cur = kvps.find("identifier");
	if ( cur != kvps.end()) {
		String ids = (*cur).second;
		Split(ids,processIDs,",");
	}
}

void WPSDescribeProcess::writeResponse(IlwisServer *server) const{
	XMLPlatformUtils::Initialize();
	DOMImplementation* dom = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dom->createDocument(0, L"ProcessDescriptions", 0);
	createHeader(doc, "wpsDescribeProcess_response.xsd");
	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();

	//XERCES_CPP_NAMESPACE::DOMElement *node1,*node2,*node3, *node4, *node5;
	String txt;
	for(int i =0; i < processIDs.size(); ++i) {
		vector<CommandInfo *> infos;
		Engine::modules.getCommandInfo(processIDs[i], infos);
		if ( infos.size() == 0)
			continue;
		CommandInfo *info = infos[0];
		if ( info->metadata != NULL) {
			ApplicationQueryData query;
			query.queryType = "WPSMETADATA";
			ApplicationMetadata amd = (info->metadata)(&query);
			if ( amd.wpsxml != "") {
				String xml = amd.wpsxml;
				vector<String> results;
				ILWIS::XMLDocument xmldoc(xml);
				xmldoc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
				xmldoc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");
				xmldoc.executeXPathExpression("//ProcessDescription",results);
				if ( results.size() > 0) {
					txt += results[0];
				}
			}
		}

	}
	String header = createOutput(doc);
	int index = header.find("/>");
	header.replace(index,2,">");
	//index = header.find_last_of("/>");
	//header.replace(index,2,">");
	txt = header + txt + "\r\n</ProcessDescriptions>";
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;

}

bool WPSDescribeProcess::needsResponse() const{
	return true;
}

	
