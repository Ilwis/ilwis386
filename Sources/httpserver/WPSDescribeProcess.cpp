#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSDescribeProcess.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
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
	XMLDocument doc;
	doc.set_name("wps:DescribeProcess");
	doc.addNodeTo(doc,"wps:DescribeProcess");
	createHeader(doc, "wpsDescribeProcess_response.xsd");
	pugi::xml_node descriptions = doc.addNodeTo(doc.first_child(),"wps:ProcessDescriptions");

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
				XMLDocument xmldoc(amd.wpsxml);;
				doc.addNodeTo(descriptions,xmldoc);
			}
		}

	}
	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;

}

bool WPSDescribeProcess::needsResponse() const{
	return true;
}

	
