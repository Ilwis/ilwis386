#include "Headers\toolspch.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "drawers\PointDrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Drawers\gpc.h"
#include "drawers\polygondrawer.h"
#include "drawers\polygonfeaturedrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\CanvasBackgroundDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\GridDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\Boxdrawer.h"


using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointFeatureDrawer","ilwis38",createPointFeatureDrawer));
	infos->push_back(new DrawerInfo("PointSetDrawer","ilwis38",createPointSetDrawer));
	infos->push_back(new DrawerInfo("LineFeatureDrawer","ilwis38",createLineFeatureDrawer));
	infos->push_back(new DrawerInfo("LineSetDrawer","ilwis38",createLineSetDrawer));
	infos->push_back(new DrawerInfo("LineDrawer","ilwis38",createLineDrawer));
	infos->push_back(new DrawerInfo("PointDrawer","ilwis38",createPointDrawer));
	infos->push_back(new DrawerInfo("PolygonFeatureDrawer","ilwis38",createPolygonFeatureDrawer));
	infos->push_back(new DrawerInfo("PolygonSetDrawer","ilwis38",createPolygonSetDrawer));
	infos->push_back(new DrawerInfo("GridDrawer","ilwis38",createGridDrawer));
	infos->push_back(new DrawerInfo("GridLine","ilwis38",createGridLine));
	infos->push_back(new DrawerInfo("FeatureLayerDrawer","ilwis38", createFeatureLayerDrawer));
	infos->push_back(new DrawerInfo("CanvasBackgroundDrawer","ilwis38", createCanvasBackgroundDrawer));
	infos->push_back(new DrawerInfo("RasterLayerDrawer","ilwis38", createRasterLayerDrawer));
	infos->push_back(new DrawerInfo("RasterSetDrawer","ilwis38", createRasterSetDrawer));
	infos->push_back(new DrawerInfo("AnimationDrawer","ilwis38", createAnimationDrawer));
	infos->push_back(new DrawerInfo("BoxDrawer","ilwis38", createBoxDrawer));


	//addSVGDrawers(infos);
	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Drawers", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}