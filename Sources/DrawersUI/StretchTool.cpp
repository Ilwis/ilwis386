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

	RangeReal rr = getBaseRange();
	if(!rr.fValid())
		return parentItem;
	//RangeReal rr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();

	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)drawer->getParentDrawer();
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

RangeReal StretchTool::getBaseRange() const {
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	RangeReal rr;
	if ( ldrw ) {
		BaseMapPtr *bmp = ((SpatialDataDrawer *)ldrw->getParentDrawer())->getBaseMap();
		if ( ldrw->useAttributeColumn()) {
			rr = ldrw->getAtttributeColumn()->rrMinMax();
		} else
			rr = bmp->rrMinMax();
	} else if ( sdrw) {
		IlwisObject *obj = (IlwisObject *)sdrw->getDataSource();
		if ( IOTYPE((*obj)->fnObj) == IlwisObject::iotMAPLIST) {
			MapList *mpl = (MapList *)obj;
			rr = (*mpl)->getRange();
		}else if ( IOTYPE((*obj)->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
			ObjectCollection *oc = (ObjectCollection *)obj;
			rr = (*oc)->getRange();
		}
	}
	return rr;
}

void StretchTool::displayOptionStretch() {
	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	RangeReal rr = getBaseRange();
	RangeReal currentrr = adrw ? adrw->getStretchRangeReal() : sdrw->getStretchRangeReal();
	double rStep;
	if ( sdrw) {
		BaseMapPtr *bmp = ((SpatialDataDrawer *)(sdrw->getParentDrawer()))->getBaseMap();
		rStep = bmp->dvrs().rStep();
	} else {
		BaseMapPtr *bmp = adrw->getBaseMap();
		rStep = bmp->dvrs().rStep();
	}

	new SetStretchValueForm(tree, drawer, rr,currentrr, rStep);
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
SetStretchValueForm::SetStretchValueForm(CWnd *wPar, NewDrawer *dr, const RangeReal& _baserr, const RangeReal& _currentrr, double rStep) : 
	DisplayOptionsForm2((ComplexDrawer *)dr,wPar,"Set stretch"),
	rr(_baserr),
	low(_currentrr.rLo()),
	high(_currentrr.rHi()),
	inRace(false)
{
	LayerDrawer *ldrw = (LayerDrawer *)dr; // needs not be a valid cast
	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	if ( setdrw) {
		ldrw = (LayerDrawer *)setdrw->getDrawer(0);
	} 
	LayerDrawer::StretchMethod method = ldrw->getStretchMethod();
	logStretch = method ==LayerDrawer::smLINEAR ? false : true;
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rr,rStep),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rr,rStep),true);
	sliderHigh->Align(sliderLow, AL_UNDER);
	cb  = new CheckBox(root,TR("Logarithmic stretching"),&logStretch);
	cb->SetCallBack((NotifyProc)&SetStretchValueForm::logStretching);

	sliderLow->SetCallBack((NotifyProc)&SetStretchValueForm::check);
	sliderHigh->SetCallBack((NotifyProc)&SetStretchValueForm::check);
	create();
}
int SetStretchValueForm::logStretching(Event *) {
	cb->StoreData();
	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	LayerDrawer::StretchMethod method = logStretch ? LayerDrawer::smLOGARITHMIC : LayerDrawer::smLINEAR;
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if ( setdrw) {
		for(int i=0; i < setdrw->getDrawerCount(); ++i) {
			LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(setdrw->getDrawer(i));
			if ( ldrw) {
				ldrw->setStretchMethod(method);
			}
		}
		setdrw->prepareChildDrawers(&pp);
	}
	else {
		LayerDrawer *ldrw = (LayerDrawer *)drw;
		ldrw->setStretchMethod(method);
		ldrw->prepare(&pp);
	}
	updateMapView();

	return 1;
}
int  SetStretchValueForm::check(Event *) {
	sliderLow->StoreData();
	sliderHigh->StoreData();
	cb->StoreData();

	if ( low == rUNDEF || high == rUNDEF)
		return 1;

	if ( low > high && !inRace){
		low = high;
		inRace = true;
		sliderLow->SetVal(low);
	}
	if ( high < low && !inRace){
		high = low;
		inRace = true;
		sliderHigh->SetVal(high);
	}
	
	SetDrawer *setdrw = dynamic_cast<SetDrawer *>(drw);
	if ( setdrw) {
		setdrw->setStretchRangeReal(RangeReal(low,high));
	}
	else {
		LayerDrawer *setdr = (LayerDrawer *)drw;
		setdr->setStretchRangeReal(RangeReal(low,high));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdr->prepareChildDrawers(&pp);
	}
	view->Invalidate();
	updateMapView();
	inRace = false;
	return 1;
}

void  SetStretchValueForm::apply() {
	check(0);

}

