#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
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
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

void setcheckSingleColor(NewDrawer *drw, void *value) {
	drw->setDrawMethod(NewDrawer::drmSINGLE);
}

void displayOptionSingleColor(NewDrawer *drw, CWnd *parent) {
	new SetSingleColorForm(parent, (FeatureLayerDrawer *)drw);
}

ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms) {
	return new FeatureLayerDrawer(parms);
}

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"FeatureLayerDrawer"),
	singleColor(Color(0,176,20))
{
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	if ( pp->type == ptALL || pp->type & RootDrawer::ptGEOMETRY) {
		clear();
		BaseMap basemap = getBaseMap();
		for(int i=0; i < basemap->iFeatures(); ++i) {
			Feature *p = CFEATURE(basemap->getFeature(i));
			if ( p && p->fValid()){
				ILWIS::DrawerParameters dp(drawcontext);
				FeatureDrawer *pdrw;
				switch ( p->getType()) {
				case Feature::ftPOINT:
					pdrw = (FeatureDrawer *)IlwWinApp()->getDrawer("PointSymbolDrawer",&dp); break;
				case Feature::ftSEGMENT:
					pdrw = (FeatureDrawer *)IlwWinApp()->getDrawer("LineDrawer",&dp); break;
				case Feature::ftPOLYGON:
					pdrw = (FeatureDrawer *)IlwWinApp()->getDrawer("PolygonDrawer",&dp); break;

				}
				pdrw->setDataSource(p);
				PreparationParameters fp((int)pp->type, 0, this);
				pdrw->prepare(&fp);
				drawers.push_back(pdrw);
			}
		}
	} 
	if (  pp->type == ptALL || pp->type & RootDrawer::ptRENDER) {
		for(int i = 0; i < drawers.size(); ++i) {
			FeatureDrawer *pdrw = (FeatureDrawer *)drawers.at(i);
			PreparationParameters fp((int)pp->type, 0, this);
			pdrw->prepare(&fp);
		}

	}

}

void FeatureLayerDrawer::setDataSource(void *bmap,int options) {
	AbstractMapDrawer::setDataSource(bmap, options);
	if ( !rpr.fValid()) {
		BaseMap basemap = getBaseMap();
		if ( basemap->fnObj == basemap->dm()->fnObj) // internal domain
			setDrawMethod(drmMULTIPLE);
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
	setDrawMethod(drmSINGLE);
}

Color FeatureLayerDrawer::getSingleColor() const {
	return singleColor;
}

HTREEITEM FeatureLayerDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = AbstractMapDrawer::configure(tv,parent);
	return hti;
}

HTREEITEM FeatureLayerDrawer::SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm) {
	HTREEITEM colorItem = AbstractMapDrawer::SetColors(tv, parent, bm);
	NewDrawer::DrawMethod method = getDrawMethod();
	IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(bm->fnObj);
	if (  otype != IlwisObject::iotRASMAP) {
		String sName = String("Single color");
		int iImg = IlwWinApp()->iImage("SingleColor");
		bool v = method == NewDrawer::drmSINGLE;
		HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, colorItem);
		tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTreeItem(tv, this, displayOptionSingleColor,htiDisplayOptions,colorCheck,setcheckSingleColor));
		tv->GetTreeCtrl().SetCheck(htiDisplayOptions, v );
		//iImg = IlwWinApp()->iImage("CalculationMultiple");
		tv->GetTreeCtrl().InsertItem(String("Color : ").scVal(), iImg, iImg, htiDisplayOptions);
	}
	return colorItem;
}

//------------------------------------------------
SetSingleColorForm::SetSingleColorForm(CWnd *wPar, FeatureLayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(((FeatureLayerDrawer *)dr)->getSingleColor())
{
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetSingleColorForm::apply() {
	fc->StoreData();
	((FeatureLayerDrawer *)drw)->setSingleColor(c);
	PreparationParameters pp(NewDrawer::ptRENDER, 0, drw);
	drw->prepare(&pp);
	updateViews();
}



