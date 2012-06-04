#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
//#include "Engine\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createFeatureDataDrawer(DrawerParameters *parms) {
	return new FeatureDataDrawer(parms);
}

FeatureDataDrawer::FeatureDataDrawer(DrawerParameters *parms) : 
	SpatialDataDrawer(parms,"FeatureDataDrawer")
{
	setTransparency(rUNDEF);
}

FeatureDataDrawer::~FeatureDataDrawer() {
}

void FeatureDataDrawer::prepare(PreparationParameters *pp){
	SpatialDataDrawer::prepare(pp);
	BaseMapPtr *bmptr = getBaseMap();
	BaseMap basemap;
	basemap.SetPointer(bmptr);
	if ( pp->type & RootDrawer::ptGEOMETRY) {
		if ( !(pp->type & NewDrawer::ptANIMATION))
			clear();
		FeatureLayerDrawer *fsd;
		ILWIS::DrawerParameters dp(getRootDrawer(), this);
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basemap->fnObj);
		switch ( otype) {
			case IlwisObject::iotPOINTMAP:
				fsd = (FeatureLayerDrawer *)NewDrawer::getDrawer("PointLayerDrawer", pp, &dp); 
				addLayerDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotSEGMENTMAP:
				fsd = (FeatureLayerDrawer *)NewDrawer::getDrawer("LineLayerDrawer", pp, &dp); 
				addLayerDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotPOLYGONMAP:
				fsd = (FeatureLayerDrawer *)NewDrawer::getDrawer("PolygonLayerDrawer", pp, &dp); 
				addLayerDrawer(basemap,pp,fsd, "Areas");
				break;
		}
	}
	if ( pp->type & NewDrawer::ptRENDER || pp->type & NewDrawer::ptRESTORE || pp->type & NewDrawer::pt3D ) {
		for(int i = 0; i < drawers.size(); ++i) {
			FeatureLayerDrawer *fsd = (FeatureLayerDrawer *)drawers.at(i);
			fsd->addDataSource(&basemap);
			PreparationParameters fp((int)pp->type & ~NewDrawer::ptGEOMETRY, 0);
			fp.csy = basemap->cs();
			fsd->prepare(&fp);
		}
	}
}

void FeatureDataDrawer::addLayerDrawer(const BaseMap& basemap,PreparationParameters *pp,LayerDrawer *fsd, const String& name) {
	PreparationParameters fp((int)pp->type, 0);
	fp.csy = basemap->cs();
	if ( getName() == "Unknown")
		fsd->setName(name);
	fsd->setRepresentation(basemap->dm()->rpr()); //  default choice
	fsd->getZMaker()->setSpatialSource(basemap, getRootDrawer()->getMapCoordBounds());
	fsd->getZMaker()->setDataSourceMap(basemap);
	BaseMap bmp(basemap);
	fsd->addDataSource(&bmp);
	fsd->prepare(&fp);
	addDrawer(fsd);
}

void FeatureDataDrawer::getFeatures(vector<Feature *>& features, int index) const{
	BaseMapPtr *basemap = getBaseMap(index);
	features.clear();
	int numberOfFeatures = basemap->iFeatures();
	if ( numberOfFeatures == iUNDEF)
		throw ErrorObject(TR("Corrupt or not supported type of Segment map"));
	features.resize(numberOfFeatures);
	for(int i=0; i < basemap->iFeatures(); ++i) {
		Feature *feature = CFEATURE(basemap->getFeature(i));
		features.at(i) = feature;
	}
}

String FeatureDataDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "FeatureDataDrawer::" + parentSection;
	SpatialDataDrawer::store(fnView, currentSection);

	return currentSection;
}

void FeatureDataDrawer::load(const FileName& fnView, const String& parentSection){
	SpatialDataDrawer::load(fnView, parentSection);
}






