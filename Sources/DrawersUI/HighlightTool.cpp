#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\HighLightTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"

DrawerTool *createHighLightTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new HighLightTool(zv, view, drw);
}

HighLightTool::HighLightTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("HighLightTool", zv, view, drw)
{
	active = false;
}

HighLightTool::~HighLightTool() {
}

bool HighLightTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	bool ok =  dynamic_cast<AnimationTool *>(tool) != 0;
	if (ok)
		parentTool = tool;
	return ok;
}

HTREEITEM HighLightTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem * itemAOI = new DisplayOptionTreeItem(tree, parentItem,drawer);
	itemAOI->setDoubleCickAction(this, (DTDoubleClickActionFunc)&HighLightTool::areaOfInterest);
	htiNode = insertItem(TR("Area of Interest"),"SelectAoi",itemAOI);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void HighLightTool::areaOfInterest() {
	new AnimationAreaOfInterest(tree,(AnimationDrawer *)drawer);
}

String HighLightTool::getMenuString() const {
	return TR("Area of Interest");
}

//----------------------------------------------------------
AnimationAreaOfInterest::AnimationAreaOfInterest(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm(adr, par, TR("Area of Interest")), boxdrw(0)
{
	clr = Color(60,60,60, 135);
	fb = new FlatIconButton(root, "SelectAoi", TR("Select Area"),(NotifyProc)&AnimationAreaOfInterest::createROIDrawer,FileName());
	fb->SetIndependentPos();
	fc = new FieldColor(root, TR("Out area color"), &clr, true);
	fb->SetWidth(80);
	create();
}

AnimationAreaOfInterest::~AnimationAreaOfInterest() {
	if ( boxId != "") {
		drw->getRootDrawer()->removeDrawer(boxId);
		drw->getRootDrawer()->getDrawerContext()->doDraw();
		boxdrw = 0;
	}
	::SetCursor(zCursor(Arrow));
}

void AnimationAreaOfInterest::areaOfInterest(CRect rect) {
	if ( boxId != "") {
		drw->getRootDrawer()->removeDrawer(boxId);
		boxId = "";
		boxdrw = 0;
	}
	if ( rect.Width() != 0 && rect.Height() != 0) { // case of clicking on the map in zoom mode
		Coord c1 = drw->getRootDrawer()->screenToWorld(RowCol(rect.top, rect.left));
		Coord c2 = drw->getRootDrawer()->screenToWorld(RowCol(rect.bottom, rect.right));
		CoordBounds cbInner(c1,c2);
		CoordBounds cbOuter = drw->getRootDrawer()->getCoordBoundsZoom();
		ILWIS::DrawerParameters sp(drw->getRootDrawer(), drw->getRootDrawer());
		boxdrw = (ILWIS::BoxDrawer *)NewDrawer::getDrawer("BoxDrawer", "Ilwis38", &sp);
		boxdrw->setBox(cbOuter, cbInner);
		boxId = String("%d", BOX_DRAWER_ID);
		drw->getRootDrawer()->addPostDrawer(BOX_DRAWER_ID,boxdrw); 
		::SetCursor(zCursor(Arrow));
	} 
}

void AnimationAreaOfInterest::apply() {
	fc->StoreData();
	if ( boxdrw) {
		boxdrw->setDrawColor(clr);
		updateMapView();
	}
	

}

int AnimationAreaOfInterest::createROIDrawer(Event*) {

	if ( boxId == "") {
		fb->SetText(TR("Unselect Area"));
		view->GetDocument()->mpvGetView()->selectArea(this,
			(NotifyRectProc)&AnimationAreaOfInterest::areaOfInterest,"DRAGOK",Color(255,0,0,200)); 
	} else {
		drw->getRootDrawer()->removeDrawer(boxId);
		fb->SetText(TR("Select Area"));
		boxId= "";
		boxdrw = 0;
		::SetCursor(zCursor(Arrow));
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;

}
