#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "drawers\pointdrawer.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : FeatureDrawer(parms,"PointDrawer") {
}

PointDrawer::PointDrawer(DrawerParameters *parms, const String& name) : FeatureDrawer(parms,name) {
}

void PointDrawer::prepare(PreparationParameters *p){
	FeatureDrawer::prepare(p);
	if ( (p->type & ptALL) ||  (p->type & ptGEOMETRY)) {
		AbstractMapDrawer *drw = dynamic_cast<AbstractMapDrawer *>(p->parentDrawer);
	    CoordSystem csy = drw->getBaseMap()->cs();
		ILWIS::Point *point = (ILWIS::Point *)feature;
		FileName fn = drawcontext->getCoordinateSystem()->fnObj;
		if ( drawcontext->getCoordinateSystem()->fnObj == csy->fnObj) {
			cNorm = *(point->getCoordinate());
		}
		else {
			cNorm = csy->cConv(drawcontext->getCoordinateSystem(), Coord(*(point->getCoordinate())));
		}
	}
}

