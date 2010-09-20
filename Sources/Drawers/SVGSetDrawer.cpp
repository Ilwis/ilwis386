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
#include "Drawers\SVGSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSVGSetDrawer(DrawerParameters *parms) {
	return new SVGSetDrawer(parms);
}

SVGSetDrawer::SVGSetDrawer(DrawerParameters *parms) : 
	SetDrawer(parms,"SVGSetDrawer")
{
	setTransparency(1);
}

SVGSetDrawer::~SVGSetDrawer(){
}

void SVGSetDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);
}

void SVGSetDrawer::addDataSource(void *bmap, int options){
}

HTREEITEM SVGSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return SetDrawer::configure(tv,parent);
}

bool SVGSetDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	return true;
}
