#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ApplicationMap.h"
#include "Engine\Map\Raster\Map.h"


extern "C" _export InfoVector* getApplicationInfo() {
	InfoVector *infos = new InfoVector();

	(*infos).push_back(ApplicationMap::newApplicationInfo(NULL,"YourAppName"));

	return infos;

}



