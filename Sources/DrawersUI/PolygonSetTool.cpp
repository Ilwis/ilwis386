#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Editors\Utils\line.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "DrawersUI\PolygonSetTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"

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
	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setdrw = dynamic_cast<SetDrawerTool *>(tool);
	if ( !ldrwt && !setdrw)
		return false;

	NewDrawer *ndrw = drawer;;
	if ( setdrw) {
		SetDrawer *drw = (SetDrawer *)(setdrw->getDrawer());
		for(int i=0; i < drw->getDrawerCount(); ++i) {
			if ( drw->getDrawer(i)->getType() == "PolygonLayerDrawer") {
				ndrw = drawer = drw->getDrawer(i);
				break;
			}
		}
	}
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(ndrw);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM PolygonSetTool::configure( HTREEITEM parentItem) {
	if ( isConfigured)
		return parentItem;
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(drawer);
	if ( !pdrw) {
		SetDrawer *drw = (SetDrawer *)(drawer);
		for(int i=0; i < drw->getDrawerCount() && pdrw == 0; ++i) {
			pdrw = (PolygonLayerDrawer *)drw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
		}
	}
	htiNode = parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&PolygonSetTool::setActiveBoundaries);
	HTREEITEM itemBoundaries = insertItem("Boundaries",".mps",item,(int)pdrw->getShowBoundaries());

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(itemBoundaries);
	}

	item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&PolygonSetTool::setActiveAreas);
	HTREEITEM itemAreas = insertItem("Areas",".mpa",item,(int)pdrw->getShowAreas());


	item = new DisplayOptionTreeItem(tree,itemAreas,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&PolygonSetTool::displayOptionTransparencyP);
	String transp("Transparency (%d)", 100 - 100 * pdrw->getAreaAlpha());
	htiTransparency = insertItem(transp,"Transparent", item);

	item = new DisplayOptionTreeItem(tree,itemAreas,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&PolygonSetTool::displayOptionSimplify);
	insertItem("Simplification",".mpa", item);

	DrawerTool::configure(parentItem);
	isConfigured = true;

	return parentItem;
}

void PolygonSetTool::setActiveAreas(void *v, HTREEITEM hti) {
	bool value = *(bool *)v;
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(drawer);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if ( pdrw) {
		pdrw->setShowAreas(value);
		drawer->prepare(&pp);
	} else {
		SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drawer);
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PolygonLayerDrawer *sdr = (PolygonLayerDrawer *)setDrw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
			if ( sdr) {
				sdr->setShowAreas(value);
				sdr->prepareChildDrawers(&pp);
			}
		}
	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetTool::setActiveBoundaries(void *v, HTREEITEM hti) {
	bool value = *(bool *)v;
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(drawer);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	if ( pdrw) {
		PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(drawer);
		pdrw->setShowBoundaries(value);
		drawer->prepare(&pp);
	} else {
		SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drawer);
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PolygonLayerDrawer *sdr = (PolygonLayerDrawer *)setDrw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
			if ( sdr) {
				sdr->setShowBoundaries(value);
				sdr->prepareChildDrawers(&pp);
			}
		}

	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetTool::displayOptionTransparencyP() {
	new TransparencyFormP(tree, (ComplexDrawer *)drawer, htiTransparency);
}

void PolygonSetTool::displayOptionSimplify() {
	new PolygonSimplificationForm(tree, (ComplexDrawer *)drawer);
}

String PolygonSetTool::getMenuString() const {
	return TR("Polygon drawer");
}
//------------------------------------------------
TransparencyFormP::TransparencyFormP(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr) : 
DisplayOptionsForm(dr,wPar,"Transparency"),
htiTransparent(htiTr)
{
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(dr);
	if ( !pdrw) {
		SetDrawer *drw = (SetDrawer *)(dr);
		for(int i=0; i < drw->getDrawerCount() && pdrw == 0; ++i) {
			pdrw = (PolygonLayerDrawer *)drw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
		}
	}
	transparency = 100 *(1.0-pdrw->getAreaAlpha());
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
	if ( initial) return;
	slider->StoreData();

	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
	if ( setDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PolygonLayerDrawer *pdrw = (PolygonLayerDrawer *)drw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
			if ( pdrw) {
				pdrw->setAreaAlpha(1.0 - (double)transparency/100.0);
				pdrw->prepareChildDrawers(&pp);
			}
		}
	}
	else {
		PolygonLayerDrawer *pdrw = (PolygonLayerDrawer *)drw;
		pdrw->setAreaAlpha(1.0 - (double)transparency/100.0);
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		pdrw->prepare(&pp);
		String transp("Transparency (%d)",transparency);
		TreeItem titem;
		view->getItem(htiTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

		strcpy(titem.item.pszText,transp.c_str());
		view->GetTreeCtrl().SetItem(&titem.item);
	}

	updateMapView();
}

//------------------------------------------------
PolygonSimplificationForm::PolygonSimplificationForm(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Simplification")
{
	PolygonLayerDrawer *pdrw = dynamic_cast<PolygonLayerDrawer *>(dr);
	if ( !pdrw) {
		SetDrawer *drw = (SetDrawer *)(dr);
		for(int i=0; i < drw->getDrawerCount() && pdrw == 0; ++i) {
			pdrw = (PolygonLayerDrawer *)drw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
		}
	}
	simplify = pdrw->asQuads();
	boundaries = pdrw->asQuadsBoundaries();
	CheckBox * cbSimplify = new CheckBox(root, TR("Simplify polygons that are too small"),&simplify);
	CheckBox * cbBoundaries = new CheckBox(cbSimplify, TR("Draw boundaries of simplified polygons"),&boundaries);
	cbBoundaries->Align(cbSimplify, AL_UNDER);
	create();
}

void PolygonSimplificationForm::apply() {
	if ( initial) return;
	root->StoreData();
	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
	if ( setDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PolygonLayerDrawer *pdrw = (PolygonLayerDrawer *)drw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER);
			if ( pdrw) {
				pdrw->setAsQuads(simplify);
				pdrw->setAsQuadsBoundaries(boundaries);
				pdrw->prepareChildDrawers(&pp);
			}
		}
	} else {
		PolygonLayerDrawer *pdrw = (PolygonLayerDrawer *)drw;
		pdrw->setAsQuads(simplify);
		pdrw->setAsQuadsBoundaries(boundaries);
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		pdrw->prepare(&pp);
	}

	updateMapView();
}

