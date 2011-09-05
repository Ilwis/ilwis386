#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "GNCThreddsHandler.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "EOToolbox\EOtoolbox.h"
#include "netcdf.h"
#include "IlwisNetCdf.h"
#include <regex> 

long ILWIS::GNCThreddsHandler::threddsFolderCount = 0;

using namespace ILWIS;
using namespace std::tr1;

RequestHandler *GNCThreddsHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new GNCThreddsHandler(c, request_info,kvps, serv);
}

GNCThreddsHandler::GNCThreddsHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("GNCThreddsHandler", c,ri,_kvps, serv)
{
	config.add("Extensions\\EO-toolbox", "EORequestHandler");
}

bool GNCThreddsHandler::doCommand() {
	createExecutionEnviroment();
	String product = kvps["product"];
	String time = kvps["time"];
	executeGNCCommand(product + " " + time);
	FilenameIter itr("*.mpr");
	while(itr.fValid()) {
		FileName fn((*itr));
		FileName fnOut(fn, ".nc");
		IlwisNetCdf ncd;
		ncd.addMap(Map(fn));
		ncd.saveToNetCdf(fnOut);
		++itr;
	}
	return true;

}

void GNCThreddsHandler::writeResponse() const{
	ILWIS::XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"GNC_Catalog");


	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

bool GNCThreddsHandler::needsResponse() const {
	return true;
}

void GNCThreddsHandler::createExecutionEnviroment() {
	String rootDir = getConfigValue("GNC:ServiceContext:LocalRoot");
	if ( rootDir == sUNDEF)
		throw ErrorObject(TR("Missing entry LocalRoot in server configuration"));

	executionDir = String("%S\\process_%d", rootDir, threddsFolderCount++);
	_mkdir(executionDir.c_str());
	//setConfigValue("GNC:Service:Context",String("%d",threddsFolderCount);
	getEngine()->SetCurDir(executionDir);
}