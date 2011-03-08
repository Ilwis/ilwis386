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
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\StretchTool.h"
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

	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drawer);
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();

	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)drawer->getParentDrawer();
	SetDrawer *setdrw = (SetDrawer *)drawer;
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
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drawer);
	RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();
	double rStep;
	if ( sdrw) {
		BaseMapPtr *bmp = ((AbstractMapDrawer *)(sdrw->getParentDrawer()))->getBaseMap();
		rStep = bmp->dvrs().rStep();
	} else {
		BaseMapPtr *bmp = adrw->getBaseMap();
		rStep = bmp->dvrs().rStep();
	}

	new SetStretchValueForm(tree, drawer, rr, rStep);
}

bool StretchTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(tool->getDrawer());
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
	
	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			SetDrawer *sdr = (SetDrawer *)animDrw->getDrawer(i);
			sdr->setStretchRangeReal(RangeReal(low,high));
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		SetDrawer *setdr = (SetDrawer *)drw;
		setdr->setStretchRangeReal(RangeReal(low,high));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdr->prepareChildDrawers(&pp);
	}

	updateMapView();
}

