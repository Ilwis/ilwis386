#include "headers\toolspch.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "client\mapwindow\drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\pointdrawer.h"
#include "Drawers\FeatureLayerDrawer.h"

using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointDrawerSimple",createPointDrawer));
	infos->push_back(new DrawerInfo("FeatureLayerDrawer",createFeatureLayerDrawer));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Drawers", "IlwisDrawers.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}