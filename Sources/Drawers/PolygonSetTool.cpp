#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Editors\Utils\line.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\PolygonSetTool.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"

DrawerTool *createPolygonSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PolygonSetTool(zv, view, drw);
}

PolygonSetTool::PolygonSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("PolygonSetTool", zv, view, drw)
{
}

PolygonSetTool::~PolygonSetTool() {
}

bool PolygonSetTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	NewDrawer *ndrw = drawer;;
	if ( adrwt) {
		ndrw = ((AnimationDrawer *)(adrwt->getDrawer()))->getDrawer(0);
	}
	PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(ndrw);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM PolygonSetTool::configure( HTREEITEM parentItem) {
	if ( isConfigured)
		return parentItem;
	PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(drawer);
	if ( !pdrw) {
		pdrw = dynamic_cast<PolygonSetDrawer *>(((AnimationDrawer *)drawer)->getDrawer(0));
	}
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&PolygonSetTool::setActiveBoundaries);
	HTREEITEM itemBoundaries = insertItem("Boundaries",".mps",item,(int)pdrw->getShowBoundaries());

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", pdrw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(itemBoundaries);
	}

	item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&PolygonSetTool::setActiveAreas);
	HTREEITEM itemAreas = insertItem("Areas",".mpa",item,(int)pdrw->getShowAreas());


	item = new DisplayOptionTreeItem(tree,itemAreas,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&PolygonSetTool::displayOptionTransparencyP);
	String transp("Transparency (%d)", 100 * pdrw->getTransparencyArea());
	htiTransparency = insertItem(transp,"Transparent", item);

	DrawerTool::configure(parentItem);
	isConfigured = true;

	return parentItem;
}

void PolygonSetTool::setActiveAreas(void *v) {
	bool value = *(bool *)v;
	PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(drawer);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if ( pdrw) {
		pdrw->setShowAreas(value);
		drawer->prepare(&pp);
	} else {
		AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drawer);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			PolygonSetDrawer *sdr = (PolygonSetDrawer *)animDrw->getDrawer(i);
			sdr->setShowAreas(value);
			sdr->prepareChildDrawers(&pp);
		}
	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetTool::setActiveBoundaries(void *v) {
	bool value = *(bool *)v;
	PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(drawer);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if ( pdrw) {
		PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(drawer);
		pdrw->setShowBoundaries(value);
		drawer->prepare(&pp);
	} else {
		AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drawer);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			PolygonSetDrawer *sdr = (PolygonSetDrawer *)animDrw->getDrawer(i);
			sdr->setShowBoundaries(value);
			sdr->prepareChildDrawers(&pp);
		}

	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetTool::displayOptionTransparencyP() {
	new TransparencyFormP(tree, (ComplexDrawer *)drawer, htiTransparency);
}

String PolygonSetTool::getMenuString() const {
	return TR("Polygon drawer");
}
//------------------------------------------------
TransparencyFormP::TransparencyFormP(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr) : 
DisplayOptionsForm(dr,wPar,"Transparency"),
htiTransparent(htiTr)
{
	PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(dr);
	if ( !pdrw) {
		PolygonSetDrawer *pdrw = dynamic_cast<PolygonSetDrawer *>(((AnimationDrawer *)dr)->getDrawer(0));
	}
	transparency = 100 *(1.0-pdrw->getTransparencyArea());
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&TransparencyFormP::setTransparency);
	slider->setContinuous(true);
	create();
}

int TransparencyFormP::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  TransparencyFormP::apply() {
	slider->StoreData();
	//PreparationParameters parm(NewDrawer::ptRENDER, 0);
	//pdrw->prepare(&parm);

	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			PolygonSetDrawer *pdrw = (PolygonSetDrawer *)drw;
			pdrw->setTransparencyArea(1.0 - (double)transparency/100.0);
			pdrw->prepareChildDrawers(&pp);
		}
	}
	else {
		PolygonSetDrawer *pdrw = (PolygonSetDrawer *)drw;
		pdrw->setTransparencyArea(1.0 - (double)transparency/100.0);
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		pdrw->prepare(&pp);
		String transp("Transparency (%d)",transparency);
		TreeItem titem;
		view->getItem(htiTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

		strcpy(titem.item.pszText,transp.scVal());
		view->GetTreeCtrl().SetItem(&titem.item);
	}


	updateMapView();

}

