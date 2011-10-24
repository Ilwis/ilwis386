#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\fldlist.h"
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
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::displayOptionCC);
	htiNode = insertItem(TR("Color Composite"),"ColorComposite",item);
	createBandItems();

	DrawerTool::configure(htiNode);

	return htiNode;
}

void ColorCompositeTool::createBandItems() {
	RasterLayerDrawer *rdrw = (RasterLayerDrawer *)drawer;
	MapList mpl = rdrw->getMapList();
	if ( mpl.fValid()) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, htiNode, drawer);
		Map mp = mpl[rdrw->getColorCompositeBand(0)];
		insertItem(String(TR("Red: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC1);

		item = new DisplayOptionTreeItem(tree, htiNode, drawer);
		mp = mpl[rdrw->getColorCompositeBand(1)];
		insertItem(String(TR("Green: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC2);

		item = new DisplayOptionTreeItem(tree, htiNode, drawer);
		mp = mpl[rdrw->getColorCompositeBand(2)];
		insertItem(String(TR("Blue: %S").c_str(), mp->fnObj.sFile),".mpr", item);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::stretchCC3);
	}
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
	new SetBandsForm(tree,(RasterLayerDrawer *)getDrawer(),this);
}

String ColorCompositeTool::getMenuString() const {
	return TR("Color Composite");
}

//------------------------------------------------
SetBandsForm::SetBandsForm(CWnd *wPar, RasterLayerDrawer *dr, ColorCompositeTool *_ccTool) : 
	DisplayOptionsForm(dr, wPar,TR("Select bands for Color Composite")), ccTool(_ccTool)
{
	exception = false;
	MapList mpl = dr->getMapList();
	v1 = dr->getColorCompositeBand(0);
	v2=  dr->getColorCompositeBand(1);
	v3 = dr->getColorCompositeBand(2);
	string band1 = mpl[v1]->fnObj.sFile + ".mpr";
	string band2 = mpl[v2]->fnObj.sFile + ".mpr";
	string band3 = mpl[v3]->fnObj.sFile + ".mpr";
	names.push_back(band3);
	names.push_back(band2);
	names.push_back(band1);

	fm1 = new FieldOneSelectString(root,TR("Red"),&v1,names);
	fm2 = new FieldOneSelectString(root,TR("Blue"),&v2,names);
	fm3 = new FieldOneSelectString(root,TR("Green"),&v3, names);
	FieldGroup *fg = new FieldGroup(root, true);
	cb = new CheckBox(fg,TR("Exception Color"),&exception);
	cb->SetCallBack((NotifyProc)&SetBandsForm::setExc);
	cb->SetIndependentPos();
	fi1 = new FieldInt(cb,"",&e1);
	fi1->Align(cb, AL_AFTER);
	fi2 = new FieldInt(cb,"",&e2);
	fi2->Align(fi1, AL_AFTER);
	fi3 = new FieldInt(cb,"",&e3);
	fi3->Align(fi2, AL_AFTER);


	create();
}

int SetBandsForm::setExc(Event *ev) {
	cb->StoreData();
	if ( exception) {
		RasterLayerDrawer *rdr = (RasterLayerDrawer *)drw;
		Color clr = rdr->getExceptionColor();
		if ( clr == colorUNDEF)
			clr = Color(0,0,0);
		fi1->SetVal(clr.red());
		fi2->SetVal(clr.green());
		fi3->SetVal(clr.blue());
	}
	return 1;
}

void  SetBandsForm::apply() {
	fm1->StoreData();
	fm2->StoreData();
	fm3->StoreData();
	cb->StoreData();
	fi1->StoreData();
	fi2->StoreData();
	fi3->StoreData();
	RasterLayerDrawer *rdr = (RasterLayerDrawer *)drw;
	rdr->setColorCompositeBand(0,v1);
	rdr->setColorCompositeBand(1,v2);
	rdr->setColorCompositeBand(2,v3);
	if ( exception)
		rdr->setExceptionColor(Color(e1,e2,e3));
	view->DeleteAllItems(ccTool->getTreeItem(),true);
	ccTool->createBandItems();


	PreparationParameters pp(NewDrawer::ptREDRAW, 0);
	
	rdr->prepareChildDrawers(&pp);

	updateMapView();
}

//------------------------------------
SetStretchCCForm::SetStretchCCForm(CWnd *wPar, RasterLayerDrawer *dr,int _index) :
	index(_index),
	DisplayOptionsForm2((ComplexDrawer *)dr,wPar,"Set stretch")
{
	RangeReal rrAllowedRange = dr->getMapList()[dr->getColorCompositeBand(index)]->rrMinMax();
	RangeReal rrCurrentLoHi = dr->getColorCompositeRange(index);
	if (!rrCurrentLoHi.fValid())
		rrCurrentLoHi = rrAllowedRange;

	low = rrCurrentLoHi.rLo();
	high = rrCurrentLoHi.rHi();
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rrAllowedRange),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rrAllowedRange),true);
	sliderHigh->Align(sliderLow, AL_UNDER);
	sliderLow->SetCallBack((NotifyProc)&SetStretchCCForm::check);
	sliderHigh->SetCallBack((NotifyProc)&SetStretchCCForm::check);
	create();
}

FormEntry *SetStretchCCForm::CheckData() {
	sliderLow->StoreData();
	sliderHigh->StoreData();
	RasterLayerDrawer *dr = (RasterLayerDrawer *)drw;
	RangeReal rr = dr->getMapList()[dr->getColorCompositeBand(index)]->rrMinMax();
	if ( low < rr.rLo())
		return sliderLow;
	if ( high > rr.rHi())
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

