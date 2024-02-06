#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Map\Segment\seg.h"
#include "Engine\Drawers\drawer_n.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmsort.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\featureDatadrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\linefeaturedrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineFeatureDrawer(DrawerParameters *parms) {
	if ( parms->managed) {
		if ( parms->parent && parms->parent->getType() == "LineLayerDrawer") {
			LineFeatureDrawer *pfdr =  ((FeatureLayerDrawer *)parms->parent)->allocator<LineFeatureDrawer>()->allocate();
			pfdr->setDrawerParameters(parms);
			return pfdr;
		}
	}
	return new LineFeatureDrawer(parms);
}

LineFeatureDrawer::LineFeatureDrawer()
: LineDrawer(0,"LineFeatureDrawer")
, feature(0)
, fColorVisible(true)
, fRawEnabled(true)
{
	lproperties.drawColor = Color(0,167,18);
}

LineFeatureDrawer::LineFeatureDrawer(DrawerParameters *parms)
: LineDrawer(parms,"LineFeatureDrawer")
, feature(0)
, fColorVisible(true)
, fRawEnabled(true)
{
	lproperties.drawColor = Color(0,167,18);

}

LineFeatureDrawer::LineFeatureDrawer(DrawerParameters *parms, const String& name)
: LineDrawer(parms,name),feature(0)
, fColorVisible(true)
, fRawEnabled(true)
{
	lproperties.drawColor = Color(0,167,18);
}

LineFeatureDrawer::~LineFeatureDrawer() {
}

bool LineFeatureDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	return LineDrawer::draw(drawLoop, cbArea);
}

void LineFeatureDrawer::addDataSource(void *f, int options) {
	feature = (Feature *)f;
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(parentDrawer);
	if ( fdr) {
		dmt = ((BaseMap*)fdr->getDataSource())->ptr()->dm()->dmt();
	}
}

long LineFeatureDrawer::getRaw() const{
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
	BaseMapPtr *bmpptr = ((BaseMap*)fdr->getDataSource())->ptr();
	if ( bmpptr->dm()->pdsrt())
		return raw;

	return bmpptr->dvrs().rValue(raw);
}

void LineFeatureDrawer::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(parentDrawer);
	if ( p->type & ptGEOMETRY | p->type & ptRESTORE) {
		CoordSystem csy = fdr->getCoordSystem();
		clear();
		cb = feature->cbBounds();
		feature->getBoundaries(lines);
		if (getRootDrawer()->fConvNeeded(csy)) {
			cb = CoordBounds();
			for(int j = 0; j < lines.size(); ++j) {
				CoordinateSequence *seq = lines.at(j);	
				for(int  i = 0; i < seq->size(); ++i) {
					Coord c = getRootDrawer()->glConv( csy, seq->getAt(i));
					cb += c;
					seq->setAt(c,i);
				}
			}
		}
	}
	if ( p->type & NewDrawer::pt3D || p->type & ptRESTORE) {
		ZValueMaker *zmaker = ((ComplexDrawer *)parentDrawer)->getZMaker();
		for(int j = 0; j < lines.size(); ++j) {
			CoordinateSequence *seq = lines.at(j);
			for(int  i = 0; i < seq->size(); ++i) {
				Coord c = seq->getAt(i);
				double zv = zmaker->getValue(c,feature);
				c.z = zv;
				seq->setAt(c,i);
			}
		}
	}
	if (  p->type & RootDrawer::ptRENDER || p->type & ptRESTORE) {
		LineProperties *lparent = (LineProperties *)fdr->getProperties();
		lproperties.linestyle = lparent->linestyle;
		bool isDmSort = dmt == dmtCLASS || dmt == dmtID || dmt == dmtUNIQUEID;
		lproperties.thickness = lparent->thickness;
		if ( p->props.linewidth != rUNDEF)
			lproperties.thickness = max(lproperties.thickness, p->props.linewidth);
		lproperties.linestyle = LineDrawer::openGLLineStyle(p->props.lineType);
		if (( isDmSort && feature->rValue() == 0) || feature->rValue() == rUNDEF) // 0 == undef for classes
			lproperties.linestyle = 0xF0F0;

		if ( fdr->useRaw()){
			Color clr = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
			fColorVisible = clr != colorUNDEF;
			lproperties.drawColor = clr;
		} else {
			fColorVisible = true;
			lproperties.drawColor = fdr->getDrawingColor()->clrVal(feature->rValue());
		}

		Representation rpr = fdr->getRepresentation();
		if (rpr.fValid() && rpr->prc()) {
			if ( fdr->useRepresentation()) {
				lproperties.thickness = rpr->prc()->rLineWidth(getRaw());
				lproperties.linestyle = LineDrawer::openGLLineStyle(rpr->prc()->iLine(feature->iValue()));
				lproperties.drawColor = rpr->prc()->clrRaw(getRaw());
			}
		}

		BaseMapPtr *bmpptr = ((BaseMap*)fdr->getDataSource())->ptr();
		if ( bmpptr->fTblAtt()) {
			if ( bmpptr->tblAtt().fValid()) // incase of missing/ corrput data
				setTableSelection(bmpptr->tblAtt()->dm()->fnObj,feature->iValue(), p);
		}
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
		if ( specialOptions == 0)
			specialOptions = fdr->getSpecialDrawingOption();
		double alpha = fdr->getAlpha();
		setAlpha(alpha);
		extrAlpha = fdr->getExtrusionAlpha();
		setActive(fColorVisible && fRawEnabled);
	}
}

Feature *LineFeatureDrawer::getFeature() const {
	return feature;
}

void* LineFeatureDrawer::getDataSource() const{
	return (void *) feature;
}