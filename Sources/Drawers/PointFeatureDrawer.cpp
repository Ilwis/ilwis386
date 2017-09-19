#include "Headers\toolspch.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmsort.h"
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
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointFeatureDrawer(ILWIS::DrawerParameters *parms) {
	if ( parms->managed) {
		if ( parms->parent && parms->parent->getType() == "PointLayerDrawer") {
			PointFeatureDrawer *pfdr =  ((FeatureLayerDrawer *)parms->parent)->allocator<PointFeatureDrawer>()->allocate();
			pfdr->setDrawerParameters(parms);
			return pfdr;
		}
	}
	return new PointFeatureDrawer(parms);
}

PointFeatureDrawer::PointFeatureDrawer()
: PointDrawer(0,"PointFeatureDrawer")
, fRawEnabled(true)
, fColorVisible(true)
, fContainedInStretchRange(true)
{
}

PointFeatureDrawer::PointFeatureDrawer(DrawerParameters *parms)
: PointDrawer(parms,"PointFeatureDrawer")
, fRawEnabled(true)
, fColorVisible(true)
, fContainedInStretchRange(true)
{

}

PointFeatureDrawer::PointFeatureDrawer(DrawerParameters *parms, const String& name)
: PointDrawer(parms,name)
, fRawEnabled(true)
, fColorVisible(true)
, fContainedInStretchRange(true)
{
}

void PointFeatureDrawer::addDataSource(void *f, int options) {
	 feature = (Feature *)f;
}

bool PointFeatureDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{

		if ( label) {
			Coord crdLabel = cNorm;
			const CoordBounds& cbZoom = getRootDrawer()->getCoordBoundsZoom();
			crdLabel.x += cbZoom.width() / 200.0;
			//crdLabel.y += cbZoom.height() / 100.0;
			label->setCoord(crdLabel);
		}

	return PointDrawer::draw(drawLoop, cbArea);
}

long PointFeatureDrawer::getRaw() const{
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(parentDrawer);
	long raw = feature->iValue();
	if ( fdr->useAttributeColumn()) {
		Column col = fdr->getAtttributeColumn();
		if ( col->dm()->pdv())
			return col->rValue(raw);
		else if ( col->dm()->pdsrt()) {
			String sV = col->sValue(raw);
			long r = col->dm()->pdsrt()->iRaw(sV);
			return r;
		}
	}
	BaseMapPtr *bmpptr = ((SpatialDataDrawer *)fdr->getParentDrawer())->getBaseMap();
	if ( bmpptr->dm()->pdsrt())
		return raw;

	return bmpptr->dvrs().rValue(raw);
}
void PointFeatureDrawer::prepare(PreparationParameters *p){

	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(parentDrawer);
	BaseMapPtr *bmpptr = ((BaseMap*)fdr->getDataSource())->ptr();
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
		if ( fdr->useRaw()){
			Color clr = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
			fColorVisible = clr != colorUNDEF;
			properties.drawColor = clr;
		} else {
			fColorVisible = true;
			properties.drawColor = (fdr->getDrawingColor()->clrVal(feature->rValue()));
		}
		extrAlpha = fdr->getExtrusionAlpha();
		if (p->filteredRaws.size() == 1 && p->filteredRaws[0] == iUNDEF) {
			fRawEnabled = true;
		} else {
			for(int j =0 ; j < p->filteredRaws.size(); ++j) {
				int raw = p->filteredRaws[j];
				if ( getFeature()->rValue() == abs(raw)) {
					fRawEnabled = raw > 0;
				}
			}
		}
		double v = feature->rValue();
		setSpecialDrawingOptions(NewDrawer::sdoSELECTED,false);
		if ( bmpptr->fTblAtt()) {
			if ( bmpptr->tblAtt().fValid()) // incase of missing/ corrput data
				setTableSelection(bmpptr->tblAtt()->dm()->fnObj,v, p);
		}
		Representation rpr = fdr->getRepresentation();
		if (rpr.fValid() && rpr->prc()) {
			if ( fdr->useRepresentation()) {
				properties.scale = rpr->prc()->iSymbolSize(getRaw());
				properties.symbol = rpr->prc()->sSymbolType(getRaw());
				properties.drawColor = rpr->prc()->clrSymbol(getRaw());
			}
		}
		properties.stretchScale = 1.0 + properties.exaggeration;
		if ( properties.scaleMode != PointProperties::sNONE && v != rUNDEF ) {
			if ( bmpptr->fTblAtt() || bmpptr->dm()->pdv()) {
				properties.exaggeration = properties.exaggeration * p->props.symbolSize / 100.0;
				RangeReal rr = properties.stretchRange;
				if ( properties.stretchColumn != "") {
					Table tbl = bmpptr->tblAtt();
					Column col = tbl->col(properties.stretchColumn);
					if ( col.fValid() && col->dm()->pdv()) {
						v = col->rValue(v);
					}
					if ( !rr.fValid())
						rr = col->dvrs().rrMinMax();
				} else {
					v = bmpptr->dvrs().rValue(feature->rValue());
				}
				if ( properties.stretchRange.fValid()) {
					fContainedInStretchRange = properties.stretchRange.fContains(v);
				} else {
					fContainedInStretchRange = true;
				}

				if (properties.scaleMode == PointProperties::sLOGARITHMIC) {
					rr.rLo() = (rr.rLo() < 1e-10) ? 0 : log(rr.rLo());
					rr.rHi() = (rr.rHi() < 1e-10) ? 0 : log(rr.rHi());
				}
				if (rr.rHi() <= rr.rLo())
					rr.rHi() = rr.rLo() + 1;
				v = (v - rr.rLo()) / rr.rWidth();
				if (v < 0)
					v = 0;
				else if (v > 1)
					v = 1;
				if (properties.radiusArea == PointProperties::sAREA)
					v = sqrt(v);
				double scale = max(1.0, 1.0 + properties.exaggeration * v);
				properties.stretchScale = scale;
			}
		} else
			fContainedInStretchRange = true;
		setActive(fRawEnabled && fColorVisible && fContainedInStretchRange);
		if ( label) {
			const CoordBounds& cbZoom = getRootDrawer()->getCoordBoundsZoom();
			Coord crdLabel = cNorm;
			//crdLabel.x += cbZoom.width() / 800.0;
			//crdLabel.y += cbZoom.height() / 800.0;
			label->setCoord(crdLabel);
		}
	}
	p->props.symbolType = properties.symbol;
	p->props.symbolSize = properties.scale;
	PointDrawer::prepare(p);
}

Feature *PointFeatureDrawer::getFeature() const {
	return feature;
}

void* PointFeatureDrawer::getDataSource() const{
	return (void *) feature;
}