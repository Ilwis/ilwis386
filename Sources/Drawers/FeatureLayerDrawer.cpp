#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
//#include "Engine\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms) {
	return new FeatureLayerDrawer(parms);
}

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"FeatureLayerDrawer")
{
	setTransparency(rUNDEF);
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	BaseMapPtr *bmptr = getBaseMap();
	BaseMap basemap;
	basemap.SetPointer(bmptr);
	if ( pp->type & RootDrawer::ptGEOMETRY) {
		if ( !(pp->type & NewDrawer::ptANIMATION))
			clear();
		FeatureSetDrawer *fsd;
		ILWIS::DrawerParameters dp(getRootDrawer(), this);
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basemap->fnObj);
		switch ( otype) {
			case IlwisObject::iotPOINTMAP:
				fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("PointSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotSEGMENTMAP:
				fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("LineSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotPOLYGONMAP:
				fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("PolygonSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd, "Areas");
				break;
		}
	} else {
		if ( pp->type & NewDrawer::ptRENDER | pp->type & NewDrawer::ptRESTORE) {
			for(int i = 0; i < drawers.size(); ++i) {
				FeatureSetDrawer *fsd = (FeatureSetDrawer *)drawers.at(i);
				PreparationParameters fp((int)pp->type, 0);
				fp.csy = basemap->cs();
				fsd->prepare(&fp);
			}
		}
	}

}

void FeatureLayerDrawer::addSetDrawer(const BaseMap& basemap,PreparationParameters *pp,SetDrawer *fsd, const String& name) {
	PreparationParameters fp((int)pp->type, 0);
	fp.csy = basemap->cs();
	if ( getName() == "Unknown")
		fsd->setName(name);
	fsd->setRepresentation(basemap->dm()->rpr()); //  default choice
	fsd->getZMaker()->setSpatialSource(basemap, getRootDrawer()->getMapCoordBounds());
	fsd->getZMaker()->setDataSourceMap(basemap);
	BaseMap bmp(basemap);
	fsd->addDataSource(bmp.ptr());
	fsd->prepare(&fp);
	addDrawer(fsd);
}

void FeatureLayerDrawer::getFeatures(vector<Feature *>& features) const{
	BaseMapPtr *basemap = getBaseMap();
	features.clear();
	int numberOfFeatures = basemap->iFeatures();
	features.resize(numberOfFeatures);
	for(int i=0; i < basemap->iFeatures(); ++i) {
		Feature *feature = CFEATURE(basemap->getFeature(i));
		features.at(i) = feature;
	}
}

String FeatureLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "FeatureLayerDrawer::" + parentSection;
	AbstractMapDrawer::store(fnView, currentSection);

	return currentSection;
}

void FeatureLayerDrawer::load(const FileName& fnView, const String& parentSection){
	AbstractMapDrawer::load(fnView, parentSection);
}






