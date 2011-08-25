#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms, const String& name) : 
	LayerDrawer(parms,name),
	singleColor(Color(0,176,20)),
	useMask(false)
{
	setDrawMethod(drmNOTSET); // default
	setInfo(false);
}

FeatureLayerDrawer::~FeatureLayerDrawer() {

}

void FeatureLayerDrawer::addDataSource(void *bmap,int options) {
	fbasemap = BaseMap(((BaseMapPtr *)bmap)->fnObj);
}

void *FeatureLayerDrawer::getDataSource() const {
	return (void *)&fbasemap;
}

void FeatureLayerDrawer::getFeatures(vector<Feature *>& features) const {
	features.clear();
	int numberOfFeatures = fbasemap->iFeatures();
	features.resize(numberOfFeatures);
	for(int i=0; i < numberOfFeatures; ++i) {
		Feature *feature = CFEATURE(fbasemap->getFeature(i));
		features.at(i) = feature;
	}
}

void FeatureLayerDrawer::prepare(PreparationParameters *parms){
	/*if ( !isActive())
		return;*/

	clock_t start = clock();
	LayerDrawer::prepare(parms);
	FeatureDataDrawer *mapDrawer = (FeatureDataDrawer *)parentDrawer;
	if ( getName() == "Unknown")
		setName(mapDrawer->getBaseMap()->sName());
	vector<Feature *> features;
	if ( parms->type & RootDrawer::ptGEOMETRY || parms->type & NewDrawer::ptRESTORE){
		bool isAnimation = mapDrawer->getType() == "AnimationDrawer";
		if ( isAnimation ) {
			getFeatures(features);
		} else {
			mapDrawer->getFeatures(features, parms->index);
		}
		clear();
		drawers.resize( features.size());
		for(int i=0; i<drawers.size(); ++i)
			drawers.at(i) = 0;
		int count = 0;
		Tranquilizer trq(TR("preparing data"));
		for(int i=0; i < features.size(); ++i) {
			Feature *feature = features.at(i);
			NewDrawer *pdrw;
			if ( feature && feature->fValid() ){
				ILWIS::DrawerParameters dp(getRootDrawer(), this);
				pdrw = createElementDrawer(parms, &dp);
				pdrw->addDataSource(feature);
				PreparationParameters fp((int)parms->type, mapDrawer->getBaseMap()->cs());
				pdrw->prepare(&fp);
				if ( feature->rValue() == rUNDEF)
					pdrw->setActive(false);
				setDrawer(i, pdrw);
				++count;
				if ( i % 100 == 0) {
					trq.fUpdate(i,features.size()); 
				}
			}
		}

	} if ( parms->type & NewDrawer::ptRENDER || parms->type & NewDrawer::pt3D || parms->type & NewDrawer::ptRESTORE) {
			PreparationParameters pp(parms);
			pp.type = pp.type & ~NewDrawer::ptGEOMETRY;
			prepareChildDrawers(&pp);
	}
	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Prepared in %2.2f seconds;\n", duration/1000);
}

String FeatureLayerDrawer::getMask() const{
	return mask;
}

void FeatureLayerDrawer::setMask(const String& sm){
	mask = sm;
}

void FeatureLayerDrawer:: setSingleColor(const Color& c){
	singleColor = c;
	setDrawMethod(drmSINGLE);
}

Color FeatureLayerDrawer::getSingleColor() const {
	return singleColor;
}

String FeatureLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	LayerDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.c_str(),"SingleColor",fnView, singleColor);
	return parentSection;
}

void FeatureLayerDrawer::load(const FileName& fnView, const String& parentSection){
	LayerDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.c_str(),"SingleColor",fnView, singleColor);

}

