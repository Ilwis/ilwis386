#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "TransparencyTool.h"
#include "SetDrawerTool.h"
#include "AnimationTool.h"
#include "drawers\linedrawer.h"
#include "drawers\GridDrawer.h"
#include "GridTool.h"

DrawerTool *createTransparencyTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TransparencyTool(zv, view, drw);
}

TransparencyTool::TransparencyTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("TransparencyTool",zv, view, drw)
{
}

TransparencyTool::~TransparencyTool() {
}

bool TransparencyTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	GridTool *gdrt = dynamic_cast<GridTool *>(tool);
	if ( !sdrwt && !adrwt && !gdrt)
		return false;

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
	new TransparencyForm(tree, (ComplexDrawer *)drawer, htiNode);
}

String TransparencyTool::getMenuString() const {
	return TR("Transparency");
}

//---------------------------------------------------
TransparencyForm::TransparencyForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr) : 
DisplayOptionsForm(dr,wPar,"Transparency"),
transparency(100 *(1.0-dr->getTransparency())),
htiTransparent(htiTr)
{
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&TransparencyForm::setTransparency);
	slider->setContinuous(true);
	create();
}

int TransparencyForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  TransparencyForm::apply() {
	if ( initial) return;
	slider->StoreData();
	//PreparationParameters parm(NewDrawer::ptRENDER, 0);
	//pdrw->prepare(&parm);

	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			ComplexDrawer *cdrw = (ComplexDrawer *)drw;
			cdrw->setTransparency(1.0 - (double)transparency/100.0);
			cdrw->prepareChildDrawers(&pp);
		}
	}
	else {
		ComplexDrawer *cdrw = (ComplexDrawer *)drw;
		cdrw->setTransparency(1.0 - (double)transparency/100.0);
	/*	PreparationParameters pp(NewDrawer::ptRENDER, 0);
		cdrw->prepare(&pp);*/
		String transp("Transparency (%d)",transparency);
		TreeItem titem;
		view->getItem(htiTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

		strcpy(titem.item.pszText,transp.scVal());
		view->GetTreeCtrl().SetItem(&titem.item);
	}


	updateMapView();

}
