#include "Client\Headers\formelementspch.h"
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
#include "Client\Mapwindow\Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

FeatureSetDrawer::FeatureSetDrawer(DrawerParameters *parms, const String& name) : 
	SetDrawer(parms,name),
	singleColor(Color(0,176,20)),
	useMask(false)
{
	setDrawMethod(drmNOTSET); // default
}

FeatureSetDrawer::~FeatureSetDrawer() {
}

void FeatureSetDrawer::prepare(PreparationParameters *parms){
	SetDrawer::prepare(parms);
	FeatureLayerDrawer *mapDrawer = (FeatureLayerDrawer *)parentDrawer;
	vector<Feature *> features;
	if ( parms->type == ptALL || parms->type & RootDrawer::ptGEOMETRY){
		mapDrawer->getFeatures(features);
		clear();
		drawers.resize(features.size());
		for(int i=0; i < features.size(); ++i) {
			Feature *feature = features.at(i);
			NewDrawer *pdrw;
			if ( feature && feature->fValid()){
				ILWIS::DrawerParameters dp(drawcontext, this);
				pdrw = createElementDrawer(parms, &dp);
				pdrw->addDataSource(feature);
				PreparationParameters fp((int)parms->type, 0);
				pdrw->prepare(&fp);
				drawers.at(i) = pdrw;
			}
		}
	} else {
		if ( parms->type & RootDrawer::ptRENDER) {
			prepareChildDrawers(parms);
		}
	}
}

String FeatureSetDrawer::getMask() const{
	return mask;
}

void FeatureSetDrawer::setMask(const String& sm){
	mask = sm;
}

void FeatureSetDrawer:: setSingleColor(const Color& c){
	singleColor = c;
	setDrawMethod(drmSINGLE);
}

Color FeatureSetDrawer::getSingleColor() const {
	return singleColor;
}

HTREEITEM FeatureSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = SetDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item;
	if ( getUICode() == NewDrawer::ucALL) {
		item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&FeatureSetDrawer::displayOptionMask);
		HTREEITEM maskItem = InsertItem("Mask", "Mask", item, (int)useMask);
		InsertItem(tv, maskItem,String("Value : %S",mask),"Mask");
	}
	HTREEITEM portrayalItem = findTreeItemByName(tv,parent,"Portrayal");
	if ( portrayalItem) {
		bool useSingleColor = getDrawMethod() == NewDrawer::drmSINGLE;
		bool useRpr = getDrawMethod() == NewDrawer::drmRPR;
		item = new DisplayOptionTreeItem(tv,portrayalItem,this,
						(DisplayOptionItemFunc)&FeatureSetDrawer::displayOptionSingleColor,0,colorCheck);
		HTREEITEM singleColorItem = InsertItem("Single Color","SingleColor",item, useSingleColor & !useRpr);
		item = new DisplayOptionTreeItem(tv,portrayalItem,this, 0,0,colorCheck);									
		HTREEITEM multiColorItem = InsertItem("Multiple Colors","MultipleColors",item, !useSingleColor & !useRpr);
		InsertItem(tv,singleColorItem,"Color:","SingleColor");
	}


	return hti;
}


void FeatureSetDrawer::displayOptionMask(CWnd *parent) {
	new SetMaskForm(parent, this);
}

void FeatureSetDrawer::displayOptionSingleColor(CWnd *parent) {
	new SetSingleColorForm(parent, this);
}

//------------------------------------------------
SetSingleColorForm::SetSingleColorForm(CWnd *wPar, FeatureSetDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(((FeatureSetDrawer *)dr)->getSingleColor())
{
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetSingleColorForm::apply() {
	fc->StoreData();
	((FeatureSetDrawer *)drw)->setSingleColor(c);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}

//------------------------------------------------
SetMaskForm::SetMaskForm(CWnd *wPar, FeatureSetDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Mask for %S",dr->getName())),
	mask(dr->getMask())
{
	fs = new FieldString(root, "Mask", &mask);
	create();
}

void SetMaskForm::apply() {
	fs->StoreData();
	((FeatureSetDrawer *)drw)->setMask(mask);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepare(&parm);
	updateMapView();
}



