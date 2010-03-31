#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\featurelayerdrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerContext *context) {
	return new FeatureLayerDrawer(context);
}

FeatureLayerDrawer::FeatureLayerDrawer(DrawerContext *context) : AbstractDrawer(context,"FeatureLayerDrawer"){
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::draw(){
		for(int i=0; i < drawers.size(); ++i)
			drawers[i]->draw();
}

void FeatureLayerDrawer::prepare(PreparationType t){
	for(int i=0; i < basemap->iFeatures(); ++i) {
		Feature *p = CFEATURE(basemap->getFeature(i));
		if ( p && p->fValid()){
			FeatureDrawer *pdrw = (FeatureDrawer *)IlwWinApp()->getDrawer("PointDrawerSimple",drawcontext);
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