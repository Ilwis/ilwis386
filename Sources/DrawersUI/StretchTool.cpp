#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\StretchTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createStretchTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new StretchTool(zv, view, drw);
}

StretchTool::StretchTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("StretchTool", zv, view, drw){
}

HTREEITEM StretchTool::configure( HTREEITEM parentItem){
	if ( !active)
		return parentItem;

	if ( isConfigured)
		return htiNode;

	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();

	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)drawer->getParentDrawer();
	LayerDrawer *setdrw = (LayerDrawer *)drawer;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&StretchTool::displayOptionStretch);
	htiNode = insertItem("Stretch","Valuerange", item,-1); 
	if ( rr.fValid()) {
			insertItem(htiNode, String("Lower : %f",rr.rLo()), "Calculationsingle");
			insertItem(htiNode, String("Upper : %f",rr.rHi()), "Calculationsingle");
	}
	DrawerTool::configure(htiNode);
	isConfigured = true;
	return htiNode;
}

void StretchTool::displayOptionStretch() {
	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();
	double rStep;
	if ( sdrw) {
		BaseMapPtr *bmp = ((SpatialDataDrawer *)(sdrw->getParentDrawer()))->getBaseMap();
		rStep = bmp->dvrs().rStep();
	} else {
		BaseMapPtr *bmp = adrw->getBaseMap();
		rStep = bmp->dvrs().rStep();
	}

	new SetStretchValueForm(tree, drawer, rr, rStep);
}

bool StretchTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if ( !layerDrawerTool && !setDrawerTool)
		return false;
	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(tool->getDrawer());
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();
	if ( rr.fValid())
		parentTool = tool;
	return rr.fValid();
}

String StretchTool::getMenuString() const {
	return TR("Interactive Stretching");
}

//------------------------------------
SetStretchValueForm::SetStretchValueForm(CWnd *wPar, NewDrawer *dr, const RangeReal& _rr, double rStep) : 
	DisplayOptionsForm((ComplexDrawer *)dr,wPar,"Set stretch"),
	rr(_rr),
	low(rr.rLo()),
	high(rr.rHi())
{
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rr,rStep),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rr,rStep),true);
	sliderHigh->Align(sliderLow, AL_UNDER);
	sliderLow->SetCallBack((NotifyProc)&SetStretchValueForm::check);
	sliderHigh->SetCallBack((NotifyProc)&SetStretchValueForm::check);
	create();
}

int  SetStretchValueForm::check(Event *) {
	apply();
	return 1;
}

void  SetStretchValueForm::apply() {
	sliderLow->StoreData();
	sliderHigh->StoreData();

	if ( low > high){
		low = high;
		sliderLow->SetVal(low);
	}
	if ( high < low){
		high = low;
		sliderHigh->SetVal(high);
	}
	
	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	if ( setdrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < setdrw->getDrawerCount(); ++i) {
			LayerDrawer *ldr = (LayerDrawer *)setdrw->getDrawer(i);
			ldr->setStretchRangeReal(RangeReal(low,high));
			ldr->prepareChildDrawers(&pp);
		}
	}
	else {
		LayerDrawer *setdr = (LayerDrawer *)drw;
		setdr->setStretchRangeReal(RangeReal(low,high));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdr->prepareChildDrawers(&pp);
	}

	updateMapView();
}

