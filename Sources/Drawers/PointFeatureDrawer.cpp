#include "Headers\toolspch.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\featuredatadrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
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
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(parentDrawer);
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
		properties.drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
		extrTransparency = fdr->getExtrusionTransparency();
		for(int j =0 ; j < p->filteredRaws.size(); ++j) {
			int raw = p->filteredRaws[j];
			if ( getFeature()->rValue() == abs(raw)) {
				setActive(raw > 0);
			}
		}
		double v = feature->rValue();
		if ( properties.scaleMode != PointProperties::sNONE && v != rUNDEF ) {
			BaseMapPtr *bmpptr = ((SpatialDataDrawer *)fdr->getParentDrawer())->getBaseMap();
			if ( bmpptr->fTblAtt()) {
				Table tbl = bmpptr->tblAtt();
				if ( properties.stretchColumn != "") {
					Column col = tbl->col(properties.stretchColumn);
					if ( col.fValid() && col->dm()->pdv()) {
						v = col->rValue(v);
						if ( properties.stretchRange.fValid()) {
							setActive(properties.stretchRange.fContains(v));
						} else {
							setActive(true);
						}
						RangeReal rr = properties.stretchRange.fValid() ? properties.stretchRange : col->dvrs().rrMinMax();
						
						if ( properties.scaleMode == PointProperties::sLINEAR) {
							double scale = 1.0 + (v - rr.rLo()) / rr.rWidth();
							properties.stretchScale = scale;
						}
						else if (properties.scaleMode == PointProperties::sLOGARITHMIC) {
							double scale = 1.0 + log(1.0 + (v - rr.rLo()) / rr.rWidth());
							properties.stretchScale = scale;

						}
					}
				}
			}
		}
	}
}

Feature *PointFeatureDrawer::getFeature() const {
	return feature;
}