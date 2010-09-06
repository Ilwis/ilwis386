#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

FeatureSetDrawer::FeatureSetDrawer(DrawerParameters *parms, const String& name) : 
	SetDrawer(parms,name),
	singleColor(Color(0,176,20)),
	useMask(false),
	colorItem(0)
{
	setDrawMethod(drmNOTSET); // default
}

FeatureSetDrawer::~FeatureSetDrawer() {
}

void FeatureSetDrawer::prepare(PreparationParameters *parms){
	SetDrawer::prepare(parms);
	FeatureLayerDrawer *mapDrawer = (FeatureLayerDrawer *)parentDrawer;
	vector<Feature *> features;
	if ( parms->type & RootDrawer::ptGEOMETRY){
		mapDrawer->getFeatures(features);
		clear();
		drawers.resize( features.size());
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
		if ( parms->type & NewDrawer::ptRENDER || parms->type & NewDrawer::pt3D) {
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

//-------------------------------------- UI --------------------------------------------------------
HTREEITEM FeatureSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = SetDrawer::configure(tv,parent);
	threeDItem = 0;
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
		colorItem = new DisplayOptionColorItem("Single color", tv,portrayalItem,this,
						(DisplayOptionItemFunc)&FeatureSetDrawer::displayOptionSingleColor,0,colorCheck);
		colorItem->setColor(singleColor);
		HTREEITEM singleColorItem = InsertItem("Single Color","SingleColor",colorItem, useSingleColor & !useRpr);
		item = new DisplayOptionTreeItem(tv,portrayalItem,this, 0,0,colorCheck);									
		HTREEITEM multiColorItem = InsertItem("Multiple Colors","MultipleColors",item, !useSingleColor & !useRpr);
	}


	return hti;
}

HTREEITEM FeatureSetDrawer::set3D(bool yesno, LayerTreeView  *tv){
	threeD = yesno;
	if ( yesno) {
		HTREEITEM parent = tv->getAncestor(portrayalItem,1);
		if ( parent != 0) {
			DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(SetCheckFunc)&SetDrawer::SetthreeD);
			threeDItem = InsertItem("3D properties","3D",item,threeD);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(DisplayOptionItemFunc)&FeatureSetDrawer::displayZOption3D);
			InsertItem("Data source", ".mpv",item);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(DisplayOptionItemFunc)&FeatureSetDrawer::displayZScaling);
			InsertItem("Scaling", "ScaleBar",item);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(SetCheckFunc)&FeatureSetDrawer::setExtrusion);
			InsertItem("Extrusion","Extrusion",item,getSpecialDrawingOption(sdoExtrusion));
			InsertItem(tv, threeDItem, "Axis", "Axis");
		}
	}
	else {
		if ( threeDItem) {
			tv->DeleteAllItems(threeDItem);
			tv->GetTreeCtrl().DeleteItem(threeDItem);
		}
		threeDItem = 0;
	}
	PreparationParameters pp(NewDrawer::pt3D);
	prepare(&pp);
	return threeDItem;
}

void FeatureSetDrawer::setExtrusion(void *value, LayerTreeView *tree) {
	bool v = *(bool *)value;
	setSpecialDrawingOptions(sdoExtrusion, v);
	tree->GetDocument()->mpvGetView()->Invalidate();

}

void FeatureSetDrawer::displayOptionMask(CWnd *parent) {
	new SetMaskForm(parent, this);
}

void FeatureSetDrawer::displayOptionSingleColor(CWnd *parent) {
	new SetSingleColorForm(parent, this);
}

void FeatureSetDrawer::displayZOption3D(CWnd *parent) {
	new DisplayZDataSourceForm(parent, this);

}

void FeatureSetDrawer::displayZScaling(CWnd *parent) {
	new ZDataScaling(parent, this);

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
	((FeatureSetDrawer *)drw)->colorItem->setColor(c);
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
//--------------------------------
ZDataScaling::ZDataScaling(CWnd *wPar, SetDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Scaling and offset"),
zscale(dr->getZMaker()->getZScale() * 100),
zoffset(dr->getZMaker()->getOffset()),
sliderOffset(0) {
	sliderScale = new FieldRealSliderEx(root,"Z Scaling", &zscale,ValueRange(0,1000),true);
	sliderScale->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
	sliderScale->setContinuous(true);

	if (dr->getZMaker()->getRange().fValid()) { 
		RangeReal rr = dr->getZMaker()->getRange();
		ValueRangeReal vr(- ( rr.rHi() + rr.rLo()), rr.rWidth());
		zoffset -= rr.rLo();
		sliderOffset = new FieldRealSliderEx(root,"Z Offset", &zoffset,vr,true);
		sliderOffset->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
		sliderOffset->setContinuous(true);
	}
	create();
}

int ZDataScaling::settransforms(Event *) {
	apply();
	return 1;
}

void ZDataScaling::apply() {
	sliderScale->StoreData();
	if ( sliderOffset)
		sliderOffset->StoreData();
	drw->getZMaker()->setZScale(zscale/100.0);
	RangeReal rr = drw->getZMaker()->getRange();
	drw->getZMaker()->setOffset(zoffset + rr.rLo());
	updateMapView();
}

//--------------------------------
DisplayZDataSourceForm::DisplayZDataSourceForm(CWnd *wPar, SetDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"3D Options"), sourceIndex(0) 
{
	FeatureLayerDrawer *fdrw = (FeatureLayerDrawer *)dr->getParentDrawer();
	attTable = fdrw->getAtttributeTable();
	bmp = fdrw->getBaseMap();
	rg = new RadioGroup(root,"Data Source",&sourceIndex);
	new RadioButton(rg,"Self");
	RadioButton *rbMap = new RadioButton(rg,"Raster Map");
	fmap = new FieldMap(rbMap,"",&mapName, new MapListerDomainType(dmVALUE|dmIMAGE));

	if ( attTable.fValid()) {
		RadioButton *rbTable = new RadioButton(rg,"Attribute column");
		FieldColumn *fcol = new FieldColumn(rbTable,"",attTable,&colName,dmVALUE&dmIMAGE);
	}

	rg->SetIndependentPos();


	create();
	
}


void DisplayZDataSourceForm::apply() {
	rg->StoreData();
//	fmap->StoreData();
	if ( sourceIndex == 0) {
		drw->getZMaker()->setDataSourceMap(bmp);
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	}
	else if ( mapName != "" && sourceIndex == 1) {
		drw->getZMaker()->setDataSourceMap(BaseMap(FileName(mapName)));
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	} else if ( colName != "" && sourceIndex == 2) {
		drw->getZMaker()->setTable(attTable,colName);
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	}
	updateMapView();
}



