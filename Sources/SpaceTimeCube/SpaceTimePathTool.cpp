#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "SpaceTimeElementsDrawer.h"
#include "Client\FormElements\fldcolor.h"
#include "SpaceTimePathTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "SpaceTimeDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"

DrawerTool *createSpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SpaceTimePathTool(zv, view, drw);
}

SpaceTimePathTool::SpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("SpaceTimePathTool",zv, view, drw)
{
}

SpaceTimePathTool::~SpaceTimePathTool() {
}

bool SpaceTimePathTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	SpaceTimeDrawer *pdrw = dynamic_cast<SpaceTimeDrawer *>(drawer);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM SpaceTimePathTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;

	htiNode = insertItem(parentItem, TR("Elements"),"Transparent");

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::displayOptionEdges);
	SpaceTimeDrawer *pdrw = dynamic_cast<SpaceTimeDrawer *>(drawer);
	String transp("Edges (%d)", pdrw->iNrSteps());
	insertItem(transp,"Circle", item);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	//item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::changeSpaceTimePath);
	item->setCheckAction(this,0,(DTSetCheckFunc)&SpaceTimePathTool::setSpaceTimePathVisibility);
	insertItem(TR("Space Time Path"),"Axis",item,1);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::changeFootprint);
	item->setCheckAction(this,0,(DTSetCheckFunc)&SpaceTimePathTool::setFootprintVisibility);
	insertItem(TR("Footprint"),"Axis",item,0);

	// implement XT, XY, YT lines first, then uncomment the following
	/*
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::changeXT);
	item->setCheckAction(this,0,(DTSetCheckFunc)&SpaceTimePathTool::setXTVisibility);
	insertItem(TR("XT Reference Lines"),"Axis",item,0);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::changeXY);
	item->setCheckAction(this,0,(DTSetCheckFunc)&SpaceTimePathTool::setXYVisibility);
	insertItem(TR("XY Reference Lines"),"Axis",item,0);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::changeYT);
	item->setCheckAction(this,0,(DTSetCheckFunc)&SpaceTimePathTool::setYTVisibility);
	insertItem(TR("YT Reference Lines"),"Axis",item,0);
	*/

	return htiNode;
}

void SpaceTimePathTool::displayOptionEdges() {
	new EdgesForm(tree, (ComplexDrawer *)drawer, tree->GetTreeCtrl().GetNextItem(htiNode, TVGN_CHILD));
}

String SpaceTimePathTool::getMenuString() const {
	return TR("SpaceTimePathTool");
}

void SpaceTimePathTool::setVisibility(const String& element, bool value) {
	SpaceTimeDrawer * stdrw = (SpaceTimeDrawer*)drawer;
	SpaceTimeElementsDrawer * eldrw = stdrw->getAdditionalElementsDrawer();
	PathElementProperties *prop = (PathElementProperties *)(eldrw->getProperties());
	if (prop) {
		(*prop)[element].visible = value;
	}
	mpvGetView()->Invalidate();
}

void SpaceTimePathTool::elementForm(const String& element) {
	SpaceTimeDrawer * stdrw = (SpaceTimeDrawer*)drawer;
	SpaceTimeElementsDrawer * eldrw = stdrw->getAdditionalElementsDrawer();
	PathElementProperties *prop = (PathElementProperties *)(eldrw->getProperties());
	if ( prop)
		new SpaceTimeElementsForm(tree,(ComplexDrawer *)drawer, htiNode,prop->elements[element]);
}

void SpaceTimePathTool::setSpaceTimePathVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("spacetimepath",v);
}

void SpaceTimePathTool::setFootprintVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("footprint",v);
}

void SpaceTimePathTool::setXTVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("xt",v);
}


void SpaceTimePathTool::setXYVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("xy",v);
}

void SpaceTimePathTool::setYTVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("yt",v);
}

void SpaceTimePathTool::changeFootprint() {
	elementForm("footprint");
}

void SpaceTimePathTool::changeXT() {
	elementForm("xt");
}

void SpaceTimePathTool::changeXY() {
	elementForm("xy");
}

void SpaceTimePathTool::changeYT() {
	elementForm("yt");
}

//---------------------------------------------------
EdgesForm::EdgesForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti) : 
DisplayOptionsForm(dr,wPar,"Space Time Path Properties"),
nrEdges(((SpaceTimeDrawer*)dr)->iNrSteps()),
htiNrEdges(hti), fldNrEdges(0)
{
	slider = new FieldIntSliderEx(root,"Edges(1-25)", &nrEdges,ValueRange(1,25),true);
	slider->SetCallBack((NotifyProc)&EdgesForm::setNrEdges);
	slider->setContinuous(true);
	create();
}

int EdgesForm::setNrEdges(Event *ev) {
	apply();
	return 1;
}

void  EdgesForm::apply() {
	if ( initial) return;
	slider->StoreData();
	if ( fldNrEdges) {
		fldNrEdges->StoreData();
	}

	SpaceTimeDrawer *cdrw = (SpaceTimeDrawer*)drw;
	cdrw->SetNrSteps(nrEdges);
	String transp("Edges (%d)",nrEdges);
	TreeItem titem;
	view->getItem(htiNrEdges,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

	strcpy(titem.item.pszText,transp.c_str());
	view->GetTreeCtrl().SetItem(&titem.item);

	LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(cdrw);
	PreparationParameters pp(NewDrawer::ptGEOMETRY, 0);
	cdrw->prepare(&pp);

	updateMapView();
}

//---------------------------------------------------
SpaceTimeElementsForm::SpaceTimeElementsForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr, PathElement& elem) : 
DisplayOptionsForm(dr,wPar,elem.label), slider(0), fc(0), element(elem)
{
	if ( element.color != colorUNDEF) {
		fc = new FieldColor(root,TR("Color"),&(element.color));
	}
	if ( element.transparency != rUNDEF) {
		slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
		slider->SetCallBack((NotifyProc)&SpaceTimeElementsForm::setTransparency);
		slider->setContinuous(true);
	}
	create();
}

int SpaceTimeElementsForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  SpaceTimeElementsForm::apply() {
	if ( slider) {
		slider->StoreData();
		element.transparency = (double)transparency/100.0;
	}
	if ( fc)
		fc->StoreData();

	updateMapView();
}

