#include "Headers\toolspch.h"
#include "Engine\Base\System\Engine.h"
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
#include "Engine\Representation\Rprclass.h"
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
		cNorm = getRootDrawer()->glConv(csy, c);
		cb += cNorm;
	}
	if ( fdr->getRootDrawer()->is3D() && p->type & NewDrawer::pt3D) {
		ZValueMaker *zmaker = ((ComplexDrawer *)parentDrawer)->getZMaker();
		double zv = zmaker->getValue(cNorm,feature);
		cNorm.z = zv;
	}
	if (  p->type & ptRENDER || p->type & ptRESTORE) {
		PointProperties *prop = (PointProperties *)fdr->getProperties();
		if ( prop) {
			properties.set(prop);
		}
		properties.drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
		extrTransparency = fdr->getExtrusionTransparency();
		for(int j =0 ; j < p->filteredRaws.size(); ++j) {
			int raw = p->filteredRaws[j];
			if ( getFeature()->rValue() == abs(raw)) {
				setActive(raw > 0);
			}
		}
		double v = feature->rValue();
		BaseMapPtr *bmpptr = ((SpatialDataDrawer *)fdr->getParentDrawer())->getBaseMap();
		setSpecialDrawingOptions(NewDrawer::sdoSELECTED,false);
		if ( p->rowSelect.raws.size() > 0) {
			if ( bmpptr->fTblAtt()) {

				if ( bmpptr->tblAtt()->fnObj == p->rowSelect.fn) {
					if ( find(p->rowSelect.raws.begin(), p->rowSelect.raws.end(), (long)v) != p->rowSelect.raws.end()) {
						setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true);
					}
				}
			}
		}
		Representation rpr = fdr->getRepresentation();
		if ( rpr->prc()) {
			properties.scale = rpr->prc()->iSymbolSize(feature->iValue()) / 100;
		}
		if ( properties.scaleMode != PointProperties::sNONE && v != rUNDEF ) {
			if ( bmpptr->fTblAtt()) {
				Table tbl = bmpptr->tblAtt();
				properties.exaggeration = properties.exaggeration * p->props->symbolSize / 100.0;
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
							double scale = max(1.0, 1.0 + properties.exaggeration * (v - rr.rLo()) / rr.rWidth());
							properties.stretchScale = scale;
						}
						else if (properties.scaleMode == PointProperties::sLOGARITHMIC) {
							double scale = max(1.0,1.0 + properties.exaggeration * log(1.0 + (v - rr.rLo()) / rr.rWidth()));
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