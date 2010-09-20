#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "drawers\svgdrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;


ILWIS::NewDrawer *createSVGDrawer(DrawerParameters *parms) {
	return new SVGDrawer(parms);
}

SVGDrawer::SVGDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"SVGDrawer") {
}

SVGDrawer::SVGDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name) {
}

void SVGDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

bool SVGDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	return true;
}

HTREEITEM SVGDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}







