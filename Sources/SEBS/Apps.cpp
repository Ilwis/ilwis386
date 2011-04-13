#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Map\Raster\Map.h"
#include "SEBS\MapAlbedo.h"
#include "SEBS\MapLandSurfaceTemperature.h"
#include "SEBS\MapNDVI.h"
#include "SEBS\MapSEBS.h"
#include "SEBS\MapSI2Radiance.h"
#include "SEBS\MapSMAC.h"
#include "SEBS\MapRadiance2Reflectance.h"
#include "SEBS\MapBrightnessTemperature.h"
#include "SEBS\MapETo.h"
#include "Client\ilwis.h"

InfoVector* getCommandInfo() {

	InfoVector *infos = new InfoVector();
	(*infos).push_back(CommandMap::newCommandInfo(createMapAlbedo,"MapAlbedo"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapLandSurfaceTemperature,"MapLandSurfaceTemperature"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapEmissivity,"MapEmissivity"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapSEBS,"MapSEBS"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapSI2Radiance,"MapSI2Radiance"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapSMAC,"MapSMAC"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapETo,"MapETo"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapRadiance2Reflectance,"MapRadiance2Reflectance"));
	(*infos).push_back(CommandMap::newCommandInfo(createMapBrightnessTemperature,"MapBrightnessTemperature"));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("SEBS", fnModule,ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getCommandInfo);  

	return module;
}



