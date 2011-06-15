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
	properties = new PointProperties();
}

PointLayerDrawer::~PointLayerDrawer() {
	delete properties;
}

NewDrawer *PointLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);

}

void PointLayerDrawer::prepare(PreparationParameters *parm){
	FeatureLayerDrawer::prepare(parm);
	if ( (parm->type & NewDrawer::ptRENDER) != 0) {
		for(int i=0; i < drawers.size(); ++i) {
			PointDrawer *ld = (PointDrawer *)drawers.at(i);
			if ( !ld) 
				continue;
			PointProperties *props = (PointProperties *)ld->getProperties();
			props->set(properties);
			if (!properties->ignoreColor)
				props->drawColor = properties->drawColor;
			ld->prepare(parm);
		}
	} else if ( (parm->type & NewDrawer::pt3D) != 0) {
		prepareChildDrawers(parm);
	}
}

String PointLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = getType() + "::" + parentSection;
	FeatureLayerDrawer::store(fnView, currentSection);
	properties->store(fnView, currentSection);



	return currentSection;
}

void PointLayerDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = getType() + "::" + parentSection;
	FeatureLayerDrawer::load(fnView, currentSection);
	properties->load(fnView, currentSection);

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

GeneralDrawerProperties *PointLayerDrawer::getProperties() {
	return properties;
}

//-----------------------------------------------------------------





