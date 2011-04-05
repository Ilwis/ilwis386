#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\SharedDataHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "HttpServer\Downloader.h"


using namespace ILWIS;

SharedDataHandler::SharedDataHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, const String& ctx)
: RequestHandler(c,ri,_kvps), context(ctx)
{
}

#define MAX_IN_BUF  1000000

void SharedDataHandler::writeResponse(IlwisServer*server) const{
	String uri(request_info->uri);
	int index = uri.find_last_of("/");
	String name;
	if ( index != string::npos) {
		name = uri.substr(index+1);
		name = String("%S\\%S", getConfigValue(context), name);
	}
#undef close // huh? wie definieert nou een macro close, das vragen om problemen
#undef read

	fstream binfile(name.scVal(), ios::in | ios::binary);
	if ( binfile.is_open()) {
		char buffer[MAX_IN_BUF];
		bool notFinished = true;
		while(notFinished) {
			binfile.read(buffer,MAX_IN_BUF);
			int len = binfile.gcount();
			int w = mg_write(connection,buffer,len);
			if ( len != MAX_IN_BUF)
				notFinished = false;
			server->updateTimeOutLocation(name);
		}

		binfile.close();
	}
}

bool SharedDataHandler::needsResponse() const{
	return true;
}