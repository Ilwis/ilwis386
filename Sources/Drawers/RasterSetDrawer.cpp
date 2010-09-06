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
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\RasterSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createRasterSetDrawer(DrawerParameters *parms) {
	return new RasterSetDrawer(parms);
}

RasterSetDrawer::RasterSetDrawer(DrawerParameters *parms) : 
	SetDrawer(parms,"RasterSetDrawer")
{
	setTransparency(1);
}

RasterSetDrawer::~RasterSetDrawer(){
}

void RasterSetDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);
}

void RasterSetDrawer::addDataSource(void *bmap, int options){
}

HTREEITEM RasterSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return SetDrawer::configure(tv,parent);
}

bool RasterSetDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	return true;
}
