#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Engine/base/system/engine.h"
#include "HttpServer\IlwisServer.h"
#include "WPSGetCapabilities.h"
#include "WPSExecute.h"
#include "WPSDescribeProcess.h"
#include "UpdateService.h"
#include "WMSGetCapabilities.h"
#include "WMSGetMap.h"

ServiceInfoVec* serviceInfo() {

	ServiceInfoVec *infos = new ServiceInfoVec();

	infos->push_back( new ServiceInfo("wps","getcapabilities", ILWIS::WPSGetCapabilities::createHandler));
	infos->push_back( new ServiceInfo("wps","execute", ILWIS::WPSExecute::createHandler));
	infos->push_back( new ServiceInfo("wps","describeprocess", ILWIS::WPSDescribeProcess::createHandler));
	infos->push_back( new ServiceInfo("ilwisupdate","", ILWIS::UpdateService::createHandler));
	infos->push_back( new ServiceInfo("wms","getcapabilities", ILWIS::WMSGetCapabilities::createHandler));
	infos->push_back( new ServiceInfo("wms","getmap", ILWIS::WMSGetMap::createHandler));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Services for ILWIS server", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifService, (void *)serviceInfo);  

	return module;
}