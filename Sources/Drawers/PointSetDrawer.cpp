#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPointSetDrawer(DrawerParameters *parms) {
	return new PointSetDrawer(parms);
}

PointSetDrawer::PointSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"PointSetDrawer")
{
}

PointSetDrawer::~PointSetDrawer() {
}

NewDrawer *PointSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);

}

void PointSetDrawer::prepare(PreparationParameters *parms){
	FeatureSetDrawer::prepare(parms);
	if ( parms->type & NewDrawer::ptRENDER) {
		for(int i = 0; i < getDrawerCount(); ++i) {
			PointDrawer *pdrw = (PointDrawer *)getDrawer(i);
			if ( pdrw != 0){
				pdrw->prepare(parms);
			}
		}
	}
}

String PointSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = getType() + "::" + parentSection;
	FeatureSetDrawer::store(fnView, currentSection);

	return currentSection;
}

void PointSetDrawer::load(const FileName& fnView, const String& parenSection){
	FeatureSetDrawer::load(fnView, parenSection);
}

void PointSetDrawer::setDrawMethod(DrawMethod method) {
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

void PointSetDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	for(int i=0; i< getDrawerCount(); ++i) {
		PointFeatureDrawer *pfdrw = dynamic_cast<PointFeatureDrawer *>(getDrawer(i));
		if ( pfdrw) {
			if ( feature->getGuid() == pfdrw->getFeature()->getGuid())
				featureDrawers.push_back(pfdrw);
		}
	}
}

void PointSetDrawer::setSymbolProperties(const String& symbol, double scale) {
	for(int i=0; i< getDrawerCount(); ++i) {
		PointDrawer *pdrw = dynamic_cast<PointDrawer *>(getDrawer(i));
		if ( pdrw) {
			pdrw->setSymbol(symbol);
			pdrw->setScale(scale);
		}
	}
}

//-----------------------------------------------------------------





