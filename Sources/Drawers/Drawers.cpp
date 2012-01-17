#include "Headers\toolspch.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "drawers\PointDrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Engine\Base\Algorithm\TriangulationAlg\gpc.h"
#include "Engine\Base\Algorithm\TriangulationAlg\Triangulator.h"
#include "drawers\polygondrawer.h"
#include "drawers\polygonfeaturedrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\CanvasBackgroundDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\ColorCompositeDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\GridDrawer.h"
#include "Drawers\GraticuleDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\Boxdrawer.h"
#include "Drawers\CollectionDrawer.h"
#include "Drawers\AnnotationDrawers.h"



using namespace ILWIS;


DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("PointFeatureDrawer","ilwis38",createPointFeatureDrawer));
	infos->push_back(new DrawerInfo("PointLayerDrawer","ilwis38",createPointLayerDrawer));
	infos->push_back(new DrawerInfo("LineFeatureDrawer","ilwis38",createLineFeatureDrawer));
	infos->push_back(new DrawerInfo("LineLayerDrawer","ilwis38",createLineLayerDrawer));
	infos->push_back(new DrawerInfo("LineDrawer","ilwis38",createLineDrawer));
	infos->push_back(new DrawerInfo("PointDrawer","ilwis38",createPointDrawer));
	infos->push_back(new DrawerInfo("PolygonFeatureDrawer","ilwis38",createPolygonFeatureDrawer));
	infos->push_back(new DrawerInfo("PolygonLayerDrawer","ilwis38",createPolygonLayerDrawer));
	infos->push_back(new DrawerInfo("GridDrawer","ilwis38",createGridDrawer));
	infos->push_back(new DrawerInfo("GridLine","ilwis38",createGridLine));
	infos->push_back(new DrawerInfo("FeatureDataDrawer","ilwis38", createFeatureDataDrawer));
	infos->push_back(new DrawerInfo("CanvasBackgroundDrawer","ilwis38", createCanvasBackgroundDrawer));
	infos->push_back(new DrawerInfo("RasterLayerDrawer","ilwis38", createRasterLayerDrawer));
	infos->push_back(new DrawerInfo("RasterDataDrawer","ilwis38", createRasterDataDrawer));
	infos->push_back(new DrawerInfo("AnimationDrawer","ilwis38", createAnimationDrawer));
	infos->push_back(new DrawerInfo("BoxDrawer","ilwis38", createBoxDrawer));
	infos->push_back(new DrawerInfo("CollectionDrawer","ilwis38", createCollectionDrawer));
	infos->push_back(new DrawerInfo("GraticuleDrawer","ilwis38", createGraticuleDrawer));
	infos->push_back(new DrawerInfo("AnnotationClassLegendDrawer","ilwis38", createAnnotationClassLegendDrawer));
	infos->push_back(new DrawerInfo("AnnotationValueLegendDrawer","ilwis38", createAnnotationValueLegendDrawer));
	infos->push_back(new DrawerInfo("AnnotationScaleBarDrawer","ilwis38", createAnnotationScaleBarDrawer));
	infos->push_back(new DrawerInfo("AnnotationBorderDrawer","ilwis38", createAnnotationBorderDrawer));
	infos->push_back(new DrawerInfo("ColorCompositeDrawer","ilwis38", createColorCompositeDrawer));
	infos->push_back(new DrawerInfo("AnnotationDrawers","ilwis38", createAnnotationDrawers));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("Drawers", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer);  

	return module;
}