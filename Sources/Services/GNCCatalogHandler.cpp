#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "Services\GNCCatalogHandler.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"


using namespace ILWIS;

RequestHandler *GNCCatalogHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new GNCCatalogHandler(c, request_info,kvps, serv);
}

GNCCatalogHandler::GNCCatalogHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler(c,ri,_kvps, serv)
{
}

#define MAX_IN_BUF  1000000

void GNCCatalogHandler::writeResponse(IlwisServer*server) const{
	FileName fnConfig(getConfigValue("GNC:ServiceContext:GNCConfigFile"));
	if ( fnConfig.fExist()) {

	}

}

bool GNCCatalogHandler::needsResponse() const{
	return true;
}