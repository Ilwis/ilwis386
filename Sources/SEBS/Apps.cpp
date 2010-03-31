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

InfoVector* getApplicationInfo() {

	InfoVector *infos = new InfoVector();
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapAlbedo,"MapAlbedo"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapLandSurfaceTemperature,"MapLandSurfaceTemperature"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapEmissivity,"MapEmissivity"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapSEBS,"MapSEBS"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapSI2Radiance,"MapSI2Radiance"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapSMAC,"MapSMAC"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapETo,"MapETo"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapRadiance2Reflectance,"MapRadiance2Reflectance"));
	(*infos).push_back(ApplicationMap::newApplicationInfo(createMapBrightnessTemperature,"MapBrightnessTemperature"));

	return infos;

}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("SEBS", "IlwisSEBS.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifGetAppInfo, (void *)getApplicationInfo);  

	return module;
}



