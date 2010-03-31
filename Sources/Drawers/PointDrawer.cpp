#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\pointdrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createPointDrawer(DrawerContext *context) {
	return new PointDrawer(context);
}

PointDrawer::PointDrawer(DrawerContext *context) : FeatureDrawer(context,"PointDrawerSimple") {
}

void PointDrawer::draw(){
	
}

void PointDrawer::prepare(PreparationType t){
	if ( t == ptALL || ptGEOMETRY) {
		ILWIS::Point *point = (ILWIS::Point *)feature;
		if ( drawcontext->getCoordinateSystem() == csy) {
			cNorm = *(point->getCoordinate());
		}
		else {
			cNorm = csy->cConv(drawcontext->getCoordinateSystem(), Coord(*(point->getCoordinate())));
		}
	}
}

