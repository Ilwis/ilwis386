#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\SharedDataHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"


using namespace ILWIS;

SharedDataHandler::SharedDataHandler(struct mg_connection *c, const struct mg_request_info *request, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("SharedDataHandler", c,request,_kvps, serv)
{
	map<String, String>::const_iterator iter = kvps.find("context");
	String context = (*iter).second;
	config.add(ilwisServer->getConfiguration(context));
	String uri(request->uri);
	isOutput = uri.find("output_data/") != string::npos;
}

#define MAX_IN_BUF  1000000

void SharedDataHandler::writeResponse() const{

	String path = isOutput ? getConfigValue("WPS:ServiceContext:LocalRoot") : getConfigValue("WPS:ServiceContext:SharedData");
	String uri(request_info->uri);
	int index = uri.find("/process_");
	String name;
	if ( index != string::npos) {
		int index2 = uri.find("?");
		name = uri.substr(index+1, uri.size() - index2);
		name = String("%S\\%S", path, name);
	} else {
		index = uri.find_last_of("/");
		name = name = uri.substr(index+1);
		name = String("%S\\%S", path, name);
	}
#undef close // huh? wie definieert nou een macro close, das vragen om problemen
#undef read

	File file(name);
	long iFileLength = file.iSize();
	fstream binfile(name.c_str(), ios::in | ios::binary);
	if ( binfile.is_open()) {
		writeHeaders("application/x-zip-compressed", iFileLength);
		char buffer[MAX_IN_BUF];
		bool notFinished = true;
		while(notFinished) {
			binfile.read(buffer,MAX_IN_BUF);
			int len = binfile.gcount();
			int w = mg_write(connection,buffer,len);
			if ( len != MAX_IN_BUF)
				notFinished = false;
			ilwisServer->updateTimeOutLocation(name);
		}

		binfile.close();
	}
}

bool SharedDataHandler::needsResponse() const{
	return true;
}