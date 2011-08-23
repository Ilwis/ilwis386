#include "headers/toolspch.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Engine/base/system/engine.h"
#include "HttpServer\IlwisServer.h"
#include "GNCCatalogHandler.h"
#include "GNCThreddsHandler.h"

ServiceInfoVec* serviceInfo() {

	ServiceInfoVec *infos = new ServiceInfoVec();

	infos->push_back( new ServiceInfo("gnc","thredds", ILWIS::GNCThreddsHandler::createHandler));
	infos->push_back( new ServiceInfo("gnc","catalog", ILWIS::GNCCatalogHandler::createHandler));



	return infos;

}