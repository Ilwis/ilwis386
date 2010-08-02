#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
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
	FeatureSetDrawer *fdr = dynamic_cast<FeatureSetDrawer *>(parentDrawer);
	if ( (p->type & ptALL) ||  (p->type & ptGEOMETRY)) {
	    CoordSystem csy = fdr->getCoordSystem();
		ILWIS::Point *point = (ILWIS::Point *)feature;
		FileName fn = drawcontext->getCoordinateSystem()->fnObj;
		if ( drawcontext->getCoordinateSystem()->fnObj == csy->fnObj) {
			cNorm = *(point->getCoordinate());
		}
		else {
			cNorm = csy->cConv(drawcontext->getCoordinateSystem(), Coord(*(point->getCoordinate())));
		}
	}
	if (  p->type == ptALL || p->type & ptRENDER) {
		setColor(fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod()));
	}
}

void PointDrawer::draw(bool norecursion) {
	FeatureDrawer::draw(norecursion);

}

