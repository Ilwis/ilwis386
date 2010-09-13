#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\linedrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms) {
	return new FeatureLayerDrawer(parms);
}

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"FeatureLayerDrawer"),
	singleColor(Color(0,176,20))
{
	setTransparency(rUNDEF);
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	if ( pp->type == ptALL || pp->type & RootDrawer::ptGEOMETRY) {
		if ( !(pp->type & NewDrawer::ptANIMATION))
			clear();
		BaseMap basemap = getBaseMap();
		FeatureSetDrawer *fsd;
		ILWIS::DrawerParameters dp(drawcontext, this);
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basemap->fnObj);
		switch ( otype) {
			case IlwisObject::iotPOINTMAP:
				fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("PointSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotSEGMENTMAP:
				fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("LineSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd);
				break;
			case IlwisObject::iotPOLYGONMAP:
				fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("PolygonSetDrawer", pp, &dp); 
				addSetDrawer(basemap,pp,fsd, "Areas");
				break;
		}
	} else {
		if ( pp->type & RootDrawer::ptRENDER) {
			for(int i = 0; i < drawers.size(); ++i) {
				FeatureSetDrawer *fsd = (FeatureSetDrawer *)drawers.at(i);
				PreparationParameters fp((int)pp->type, 0);
				fsd->prepare(&fp);
			}
		}
	}
}

void FeatureLayerDrawer::addSetDrawer(const BaseMap& basemap,PreparationParameters *pp,SetDrawer *fsd, const String& name) {
	PreparationParameters fp((int)pp->type, 0);
	fp.csy = basemap->cs();
	fsd->setName(name);
	fsd->getZMaker()->setSpatialSourceMap(basemap);
	fsd->getZMaker()->setDataSourceMap(basemap);
	fsd->prepare(&fp);
	addDrawer(fsd);
}

void FeatureLayerDrawer::getFeatures(vector<Feature *>& features) const{
	BaseMap basemap = getBaseMap();
	features.clear();
	int numberOfFeatures = basemap->iFeatures();
	features.resize(numberOfFeatures);
	for(int i=0; i < basemap->iFeatures(); ++i) {
		Feature *feature = CFEATURE(basemap->getFeature(i));
		features.at(i) = feature;
	}
}


String FeatureLayerDrawer::getMask() const{
	return mask;
}

void FeatureLayerDrawer::setMask(const String& sm){
	mask = sm;
}

void FeatureLayerDrawer:: setSingleColor(const Color& c){
	singleColor = c;
}

Color FeatureLayerDrawer::getSingleColor() const {
	return singleColor;
}

String FeatureLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "FeatureLayerDrawer::" + parentSection;
	AbstractMapDrawer::store(fnView, currentSection);

	return currentSection;
}

void FeatureLayerDrawer::load(const FileName& fnView, const String& parentSection){
}

//-------------------------------------- UI ----------------------------------------------
HTREEITEM FeatureLayerDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = AbstractMapDrawer::configure(tv,parent);
	bool singleSet = (drawers.size() + preDrawers.size() + postDrawers.size())  == 1;
	for(map<String, NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		NewDrawer *draw = (*cur).second;
		if ( !singleSet)
			
			hti = InsertItem(draw->getName(),draw->iconName(),
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&SetDrawer::setActiveMode,0,draw),
							 draw->isActive());
		draw->configure(tv,hti);
	}
	for(int i = 0; i < drawers.size(); ++i) {
		FeatureSetDrawer *fsd = (FeatureSetDrawer *)drawers.at(i);
		if ( !singleSet)
			
			hti = InsertItem(fsd->getName(),fsd->iconName(), 
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&SetDrawer::setActiveMode, 0, fsd)
							,fsd->isActive());
			                
		fsd->configure(tv,hti);
	}
	for(map<String, NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		NewDrawer *draw = (*cur).second;
		if ( !singleSet)
			hti = InsertItem(draw->getName(),draw->iconName(),
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&SetDrawer::setActiveMode,0,draw),
							 draw->isActive());
		draw->configure(tv,hti);
	}
	return hti;
}




