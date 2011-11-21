#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "TransparencyTool.h"
#include "LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "AnimationTool.h"
#include "drawers\linedrawer.h"
#include "drawers\GridDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "GridTool.h"
#include "drawers\CanvasBackgroundDrawer.h"

DrawerTool *createTransparencyTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TransparencyTool(zv, view, drw);
}

TransparencyTool::TransparencyTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("TransparencyTool",zv, view, drw), isDataLayer(false)
{
}

TransparencyTool::~TransparencyTool() {
}

bool TransparencyTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	GridTool *gdrt = dynamic_cast<GridTool *>(tool);
//	CanvasBackgroundDrawer *cbdr = dynamic_cast<CanvasBackgroundDrawer *>(tool);
	if ( !sdrwt && !adrwt && !gdrt)
		return false;
	isDataLayer = sdrwt || adrwt;

	parentTool = tool;
	return true;
}

HTREEITEM TransparencyTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&TransparencyTool::displayOptionTransparency);
	String transp("Transparency (%d)", 100 * drawer->getTransparency());
	htiNode = insertItem(transp,"Transparent", item);

	return htiNode;
}

void TransparencyTool::displayOptionTransparency() {
	new TransparencyForm(tree, (ComplexDrawer *)drawer, htiNode, isDataLayer);
}

String TransparencyTool::getMenuString() const {
	return TR("Transparency");
}

//---------------------------------------------------
TransparencyForm::TransparencyForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr, bool _isDataLayer) : 
DisplayOptionsForm(dr,wPar,"Transparency"),
transparency(100 *(1.0-dr->getTransparency())),
htiTransparent(htiTr), fldTranspValue(0), isDataLayer(_isDataLayer),useTV(false)
{
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&TransparencyForm::setTransparency);
	slider->setContinuous(true);
	if ( isDataLayer) {
		LayerDrawer *ldr = (LayerDrawer *)dr;
		if (ldr->getRepresentation()->prg() ) {
			transpValues = ldr->getTransparentValues();
			if ( transpValues.fValid())
				useTV = true;
			new FieldBlank(root);
			cb = new CheckBox(root,TR("Use transparency range"),&useTV);
			fldTranspValue = new FieldRangeReal(cb, TR(""), &transpValues);
		}
	}
	create();
}

int TransparencyForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  TransparencyForm::apply() {
	if ( initial) return;
	slider->StoreData();
	if ( fldTranspValue) {
		fldTranspValue->StoreData();
		cb->StoreData();

	}

	SetDrawer *animDrw = dynamic_cast<SetDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			LayerDrawer *cdrw = (LayerDrawer *)drw;
			cdrw->setTransparency(1.0 - (double)transparency/100.0);
			if ( !(isDataLayer || useTV == false) ) {
				transpValues = RangeReal();
			}
			cdrw->setTransparentValues(transpValues);
			cdrw->prepareChildDrawers(&pp);
		}
	}
	else {
		ComplexDrawer *cdrw = (ComplexDrawer *)drw;
		cdrw->setTransparency(1.0 - (double)transparency/100.0);
		String transp("Transparency (%d)",transparency);
		TreeItem titem;
		view->getItem(htiTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

		strcpy(titem.item.pszText,transp.c_str());
		view->GetTreeCtrl().SetItem(&titem.item);

		if ( useTV) {
			if ( !isDataLayer)
				transpValues = RangeReal();
		}
		else
			transpValues = RangeReal();

		((LayerDrawer *)cdrw)->setTransparentValues(transpValues);
		if ( oldRange != transpValues) {
			PreparationParameters pp(NewDrawer::ptRENDER, 0);
			cdrw->prepare(&pp);
		}
		oldRange = transpValues;
	}


	updateMapView();

}
