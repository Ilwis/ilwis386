#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\PointFeatureDrawer.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointFeatureDrawer(ILWIS::DrawerParameters *parms) {
	return new PointFeatureDrawer(parms);
}

PointFeatureDrawer::PointFeatureDrawer(DrawerParameters *parms) : PointDrawer(parms,"PointFeatureDrawer") {
}

PointFeatureDrawer::PointFeatureDrawer(DrawerParameters *parms, const String& name) : PointDrawer(parms,name) {
}

void PointFeatureDrawer::addDataSource(void *f, int options) {
	 feature = (Feature *)f;
}

bool PointFeatureDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	return PointDrawer::draw(norecursion, cbArea);
}

void PointFeatureDrawer::prepare(PreparationParameters *p){
	PointDrawer::prepare(p);
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
		cb += cNorm;
	}
	if (  p->type == ptALL || p->type & ptRENDER) {
		drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
	}
}