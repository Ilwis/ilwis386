#include "Headers\toolspch.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\featurelayerdrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\PointFeatureDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

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

bool PointFeatureDrawer::draw( const CoordBounds& cbArea) const{

	return PointDrawer::draw( cbArea);
}

void PointFeatureDrawer::prepare(PreparationParameters *p){
	PointDrawer::prepare(p);
	FeatureSetDrawer *fdr = dynamic_cast<FeatureSetDrawer *>(parentDrawer);
	if ( p->type & ptGEOMETRY | p->type & ptRESTORE) {
	    CoordSystem csy = fdr->getCoordSystem();
		ILWIS::Point *point = (ILWIS::Point *)feature;
		Coord c = *(point->getCoordinate());
		FileName fn = getRootDrawer()->getCoordinateSystem()->fnObj;
		if ( getRootDrawer()->getCoordinateSystem()->fnObj == csy->fnObj) {
			cNorm = c;
		}
		else {
			cNorm = csy->cConv(getRootDrawer()->getCoordinateSystem(), c);
		}
		cb += cNorm;
	}
	if ( fdr->getRootDrawer()->is3D() && p->type & NewDrawer::pt3D) {
		ZValueMaker *zmaker = ((ComplexDrawer *)parentDrawer)->getZMaker();
		double zv = zmaker->getValue(cNorm,feature);
		cNorm.z = zv;
	}
	if (  p->type & ptRENDER || p->type & ptRESTORE) {
		drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
		extrTransparency = fdr->getExtrusionTransparency();
		for(int j =0 ; j < p->filteredRaws.size(); ++j) {
			int raw = p->filteredRaws[j];
			if ( getFeature()->rValue() == abs(raw)) {
				setActive(raw > 0);
			}
		}
	}
}

Feature *PointFeatureDrawer::getFeature() const {
	return feature;
}