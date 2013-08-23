#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Drawers\CanvasBackgroundDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\BackgroundTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createBackgroundTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new BackgroundTool(zv, view, drw);
}

BackgroundTool::BackgroundTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("BackgroundTool",zv, view, drw)
{
	if ( drw)
		drawer = ((RootDrawer *)drw->getRootDrawer())->getBackgroundDrawer();
}

BackgroundTool::~BackgroundTool() {
}

bool BackgroundTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return false; 
}

HTREEITEM BackgroundTool::configure( HTREEITEM parentItem) {

	//htiNode = insertItem(TVI_ROOT,"","MapPane");
	DisplayOptionColorItem *item = new DisplayOptionColorItem(TR("Background"), tree,TVI_ROOT,drawer);
	htiNode = insertItem(TR("Background Area"),"MapPane",item,1);
	item->setCheckAction(this,0,(DTSetCheckFunc )&BackgroundTool::makeActive);
	bool is3D = drawer->getRootDrawer()->is3D();
	CanvasBackgroundDrawer *cbdr = (CanvasBackgroundDrawer *)drawer;

	item = new DisplayOptionColorItem("Outside", tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&BackgroundTool::displayOptionOutsideColor);
	item->setColor(is3D ? cbdr->getColor(CanvasBackgroundDrawer::clOUTSIDE3D) :  cbdr->getColor(CanvasBackgroundDrawer::clOUTSIDE2D));
	insertItem("Outside map","SingleColor",item);

	item = new DisplayOptionColorItem("Inside", tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&BackgroundTool::displayOptionInsideColor);
	insertItem("Inside map","SingleColor",item);
	item->setColor(is3D ? cbdr->getColor(CanvasBackgroundDrawer::clINSIDE3D) :  cbdr->getColor(CanvasBackgroundDrawer::clINSIDE2D));

	item = new DisplayOptionColorItem("3D Sky", tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&BackgroundTool::displayOptionSkyColor);
	insertItem("3D Sky","SingleColor",item);
	item->setColor(cbdr->getColor(CanvasBackgroundDrawer::clSKY3D));

	DrawerTool *dt = DrawerTool::createTool("TransparencyTool", getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
	}

	DrawerTool::configure(htiNode);
	isConfigured = true;

	return htiNode;
}
void BackgroundTool::makeActive(void *v, HTREEITEM) {
	bool yesno = *(bool*)v;
	drawer->setActive(yesno);
	mpvGetView()->Invalidate();
	
}

void BackgroundTool::displayOptionOutsideColor() {
	CanvasBackgroundDrawer *cbdr = (CanvasBackgroundDrawer *)drawer;
	Color &clr = cbdr->getRootDrawer()->is3D() ? cbdr->getColor(CanvasBackgroundDrawer::clOUTSIDE3D) :  
		                                  cbdr->getColor(CanvasBackgroundDrawer::clOUTSIDE2D);
	new SetColorForm("Outside map", tree, (CanvasBackgroundDrawer *)drawer,  clr);
}

void BackgroundTool::displayOptionInsideColor() {
	CanvasBackgroundDrawer *cbdr = (CanvasBackgroundDrawer *)drawer;
	Color& clr = cbdr->getRootDrawer()->is3D() ? cbdr->getColor(CanvasBackgroundDrawer::clINSIDE3D) :  
		                                  cbdr->getColor(CanvasBackgroundDrawer::clINSIDE2D);
	new SetColorForm("Inside map", tree, (CanvasBackgroundDrawer *)drawer, clr);
}

void BackgroundTool::displayOptionSkyColor() {
	CanvasBackgroundDrawer *cbdr = (CanvasBackgroundDrawer *)drawer;
	Color& clr = cbdr->getColor(CanvasBackgroundDrawer::clSKY3D);
	new SetColorForm("3D Sky color", tree, (CanvasBackgroundDrawer *)drawer, clr);
}

String BackgroundTool::getMenuString() const {
	return TR("Background");
}

//------------------------------------------------
SetColorForm::SetColorForm(const String& title, CWnd *wPar, CanvasBackgroundDrawer *dr, Color& color) : 
	DisplayOptionsForm(dr, wPar,title),clr(color), c(color)
{
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetColorForm::apply() {
	fc->StoreData();
	clr = c;
	drw->getRootDrawer()->SetSkyColor(c);
	updateMapView();
}

