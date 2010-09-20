#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createRasterLayerDrawer(DrawerParameters *parms) {
	return new RasterLayerDrawer(parms);
}

RasterLayerDrawer::RasterLayerDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"RasterLayerDrawer")
{
	setTransparency(rUNDEF);
}

RasterLayerDrawer::~RasterLayerDrawer(){
}

void RasterLayerDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
}

void RasterLayerDrawer::addDataSource(void *bmap, int options){
}

HTREEITEM RasterLayerDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return AbstractMapDrawer::configure(tv,parent);
}

bool RasterLayerDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	return true;
}
