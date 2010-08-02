#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "client\mapwindow\drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\PointSymbolDrawer.h"
#include "Drawers\gpc.h"
#include "drawers\polygondrawer.h"

using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointSymbolDrawer","ilwis38",createPointSymbolDrawer));
	infos->push_back(new DrawerInfo("PointSetDrawer","ilwis38",createPointSetDrawer));
	infos->push_back(new DrawerInfo("LineDrawer","ilwis38",createLineDrawer));
	infos->push_back(new DrawerInfo("LineSetDrawer","ilwis38",createLineSetDrawer));
	infos->push_back(new DrawerInfo("PolygonDrawer","ilwis38",createPolygonDrawer));
	infos->push_back(new DrawerInfo("PolygonSetDrawer","ilwis38",createPolygonSetDrawer));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Drawers", "IlwisDrawers.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}