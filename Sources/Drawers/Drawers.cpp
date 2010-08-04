#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\Drawers\FeaturePortrayal.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "drawers\PointDrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Drawers\gpc.h"
#include "drawers\polygondrawer.h"
#include "drawers\polygonfeaturedrawer.h"
#include "Drawers\GridDrawer.h"

using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointFeatureDrawer","ilwis38",createPointFeatureDrawer));
	infos->push_back(new DrawerInfo("PointSetDrawer","ilwis38",createPointSetDrawer));
	infos->push_back(new DrawerInfo("LineFeatureDrawer","ilwis38",createLineFeatureDrawer));
	infos->push_back(new DrawerInfo("LineSetDrawer","ilwis38",createLineSetDrawer));
	infos->push_back(new DrawerInfo("PolygonFeatureDrawer","ilwis38",createPolygonFeatureDrawer));
	infos->push_back(new DrawerInfo("PolygonSetDrawer","ilwis38",createPolygonSetDrawer));
	infos->push_back(new DrawerInfo("GridDrawer","ilwis38",createGridDrawer));
	infos->push_back(new DrawerInfo("GridLine","ilwis38",createGridLine));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Drawers", "IlwisDrawers.dll",ILWIS::Module::mi37,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}