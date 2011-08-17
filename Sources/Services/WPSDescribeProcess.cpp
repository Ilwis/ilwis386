#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Services\WPSDescribeProcess.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"


using namespace ILWIS;


RequestHandler *WPSDescribeProcess::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new WPSDescribeProcess(c, request_info,kvps, serv);
}


WPSDescribeProcess::WPSDescribeProcess(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv)
: WPSHandler(c,ri,kvps, serv)
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
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;

}

bool WPSDescribeProcess::needsResponse() const{
	return true;
}

	
