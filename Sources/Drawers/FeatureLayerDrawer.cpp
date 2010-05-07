#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\featurelayerdrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms) {
	return new FeatureLayerDrawer(parms);
}

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms) : AbstractDrawer(parms,"FeatureLayerDrawer"){
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::prepare(PreparationType t,CDC *dc){
	clear();
	for(int i=0; i < basemap->iFeatures(); ++i) {
		Feature *p = CFEATURE(basemap->getFeature(i));
		if ( p && p->fValid()){
			ILWIS::DrawerParameters *dp = new ILWIS::DrawerParameters;
			dp->context = drawcontext;
			FeatureDrawer *pdrw = (FeatureDrawer *)IlwWinApp()->getDrawer("PointDrawerSimple",dp);
			pdrw->setCoordinateSystem(basemap->cs());
			pdrw->setDomain(basemap->dm());
			pdrw->setDataSource(p);
			pdrw->prepare(t);
			drawers.push_back(pdrw);
		}
	}

}

void FeatureLayerDrawer::setDataSource(void *bmap) {
	basemap = *((BaseMap *)bmap);
}