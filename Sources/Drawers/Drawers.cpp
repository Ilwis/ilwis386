#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "client\mapwindow\drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\linedrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerForms.h"
#include "drawers\PointSymbolDrawer.h"
#include "drawers\polygondrawer.h"

using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointDrawer",createPointDrawer));
	infos->push_back(new DrawerInfo("LineDrawer",createLineDrawer));
	infos->push_back(new DrawerInfo("PointSymbolDrawer",createPointSymbolDrawer));
	infos->push_back(new DrawerInfo("PolygonDrawer",createPolygonDrawer));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Drawers", "IlwisDrawers.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}