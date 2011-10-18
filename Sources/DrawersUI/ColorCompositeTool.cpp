#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\ColorCompositeTool.h"
#include "DrawersUI\LayerDrawerTool.h"

using namespace ILWIS;

DrawerTool *createColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ColorCompositeTool(zv, view, drw);
}

ColorCompositeTool::ColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("ColorCompositeTool",zv, view, drw)
{
}

ColorCompositeTool::~ColorCompositeTool() {
}

void ColorCompositeTool::clear() {
}

bool ColorCompositeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !sdrwt)
		return false;
	RasterLayerDrawer *rdrw = 0;
	if ( (rdrw = dynamic_cast<RasterLayerDrawer *>(tool->getDrawer())) == 0)
		return false;
	if ( !rdrw->isColorComposite())
		return false;
	parentTool = tool;

	return true;
}

HTREEITEM ColorCompositeTool::configure( HTREEITEM parentItem) {
	RasterLayerDrawer *rdrw = (RasterLayerDrawer *)drawer;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::displayOptionCC);
	htiNode = insertItem(TR("Color Composite"),"ColorComposite",item);
	MapList mpl = rdrw->getMapList();
	if ( mpl.fValid()) {
		item = new DisplayOptionTreeItem(tree, htiNode, drawer);
		Map mp = mpl[rdrw->getColorCompositeBand(0)];
		insertItem(String(TR("Red: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC1);

		mp = mpl[rdrw->getColorCompositeBand(1)];
		insertItem(String(TR("Green: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC2);

		mp = mpl[rdrw->getColorCompositeBand(2)];
		insertItem(String(TR("Blue: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC2);

	}

	DrawerTool::configure(htiNode);

	return htiNode;
}
void ColorCompositeTool::stretchCC1() {
	new SetStretchCCForm(tree,(RasterLayerDrawer *)getDrawer(),0); 
}

void ColorCompositeTool::stretchCC2() {
	new SetStretchCCForm(tree,(RasterLayerDrawer *)getDrawer(),1); 
}

void ColorCompositeTool::stretchCC3() {
	new SetStretchCCForm(tree,(RasterLayerDrawer *)getDrawer(),2); 
}

void ColorCompositeTool::displayOptionCC() {
	new SetBandsForm(tree,(RasterLayerDrawer *)getDrawer());
}

String ColorCompositeTool::getMenuString() const {
	return TR("Color Composite");
}

//------------------------------------------------
SetBandsForm::SetBandsForm(CWnd *wPar, RasterLayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,TR("Select bands for Color Composite"))
{
	MapList mpl = dr->getMapList();
	band1 = mpl[dr->getColorCompositeBand(0)]->fnObj.sFile;
	band2 = mpl[dr->getColorCompositeBand(1)]->fnObj.sFile;
	band3 = mpl[dr->getColorCompositeBand(2)]->fnObj.sFile;

//	MapListerDomainAndGeoRef *lister = ;
	fm1 = new FieldMap(root,TR("Red"),&band1,new MapListerDomainAndGeoRef(mpl->gr()->fnObj,mpl->rcSize(),mpl[0]->dm()->fnObj,false));
	fm2 = new FieldMap(root,TR("Green"),&band2,new MapListerDomainAndGeoRef(mpl->gr()->fnObj,mpl->rcSize(),mpl[0]->dm()->fnObj,false));
	fm3 = new FieldMap(root,TR("Blue"),&band3,new MapListerDomainAndGeoRef(mpl->gr()->fnObj,mpl->rcSize(),mpl[0]->dm()->fnObj,false));

	create();
}

void  SetBandsForm::apply() {
	fm1->StoreData();
	fm2->StoreData();
	fm3->StoreData();
	RasterLayerDrawer *rdr = (RasterLayerDrawer *)drw;
	MapList mpl = rdr->getMapList();
	for(int i = 0; i < mpl->iSize(); ++i) {
		String oldBand = mpl[i]->fnObj.sFullPath();
		if ( oldBand == band1) {
			rdr->setColorCompositeBand(0,i);

		}
		if ( oldBand == band2) {
			rdr->setColorCompositeBand(1,i);

		}
		if ( oldBand == band3) {
			rdr->setColorCompositeBand(2,i);

		}

	}
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	rdr->prepareChildDrawers(&pp);

	updateMapView();
}

//------------------------------------
SetStretchCCForm::SetStretchCCForm(CWnd *wPar, RasterLayerDrawer *dr,int _index) :
	index(_index),
	DisplayOptionsForm2((ComplexDrawer *)dr,wPar,"Set stretch"),
	rr(dr->getColorCompositeRange(_index))
{
	if ( !rr.fValid()) {
		//rr = RangeReal(0,255);
		rr = dr->getMapList()[index]->rrMinMax();

	}
	low = rr.rLo();
	high = rr.rHi();
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rr),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rr),true);
	sliderHigh->Align(sliderLow, AL_UNDER);
	sliderLow->SetCallBack((NotifyProc)&SetStretchCCForm::check);
	sliderHigh->SetCallBack((NotifyProc)&SetStretchCCForm::check);
	create();
}

FormEntry *SetStretchCCForm::CheckData() {
	sliderLow->StoreData();
	sliderHigh->StoreData();
	if ( low < 0)
		return sliderLow;
	if ( high > 255)
		return sliderHigh;
	if ( high < low)
		return sliderHigh;
	return 0;
}
int  SetStretchCCForm::check(Event *) {
	sliderLow->StoreData();
	sliderHigh->StoreData();

	if ( low == rUNDEF || high == rUNDEF)
		return 1;

	if ( low > high){
		low = high;
		sliderLow->SetVal(low);
	}
	if ( high < low){
		high = low;
		sliderHigh->SetVal(high);
	}
	
	RasterLayerDrawer *setdr = (RasterLayerDrawer *)drw;
	if ( setdr->isColorComposite()) {
		setdr->setColorCompositeRange(index, RangeReal(low, high));
		setdr->setStretchRangeReal(RangeReal(low,high));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdr->prepareChildDrawers(&pp);
		updateMapView();
	}
	return 1;
}

void  SetStretchCCForm::apply() {
	check(0);

}

