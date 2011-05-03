#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPointLayerDrawer(DrawerParameters *parms) {
	return new PointLayerDrawer(parms);
}

PointLayerDrawer::PointLayerDrawer(DrawerParameters *parms) : 
	FeatureLayerDrawer(parms,"PointLayerDrawer")
{
}

PointLayerDrawer::~PointLayerDrawer() {
}

NewDrawer *PointLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);

}

void PointLayerDrawer::prepare(PreparationParameters *parms){
	FeatureLayerDrawer::prepare(parms);
	//if ( parms->type & NewDrawer::ptRENDER) {
	//	for(int i = 0; i < getDrawerCount(); ++i) {
	//		PointDrawer *pdrw = (PointDrawer *)getDrawer(i);
	//		if ( pdrw != 0){
	//			pdrw->prepare(parms);
	//		}
	//	}
	//}
}

String PointLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = getType() + "::" + parentSection;
	FeatureLayerDrawer::store(fnView, currentSection);

	return currentSection;
}

void PointLayerDrawer::load(const FileName& fnView, const String& parenSection){
	FeatureLayerDrawer::load(fnView, parenSection);
}

void PointLayerDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) { 
		if ( useInternalDomain() || !rpr.fValid()) {
			setDrawMethod(drmSINGLE);
		}
		else if ( rpr.fValid()) {
			setDrawMethod(drmRPR);
		}
	}
	else
		drm = method; 
}

void PointLayerDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	for(int i=0; i< getDrawerCount(); ++i) {
		PointFeatureDrawer *pfdrw = dynamic_cast<PointFeatureDrawer *>(getDrawer(i));
		if ( pfdrw) {
			if ( feature->getGuid() == pfdrw->getFeature()->getGuid())
				featureDrawers.push_back(pfdrw);
		}
	}
}

void PointLayerDrawer::setSymbolProperties(const String& symbol, double scale) {
	for(int i=0; i< getDrawerCount(); ++i) {
		PointDrawer *pdrw = dynamic_cast<PointDrawer *>(getDrawer(i));
		if ( pdrw) {
			pdrw->setSymbol(symbol);
			pdrw->setScale(scale);
		}
	}
}

//-----------------------------------------------------------------





