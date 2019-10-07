#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "SpaceTimeCube\CubeDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "SpaceTimeCubeTool.h"
#include "CubeElementsTool.h"

DrawerTool *createCubeElementsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CubeElementsTool(zv, view, drw);
}

CubeElementsTool::CubeElementsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("CubeElementsTool",zv, view, drw), isDataLayer(false)
{
}

CubeElementsTool::~CubeElementsTool() {
}

bool CubeElementsTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return false;
}

HTREEITEM CubeElementsTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	htiNode = insertItem(parentItem, TR("Elements"),"Transparent");

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeCube);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setCubeVisibility);
	insertItem(TR("Cube"),"SpaceTimeCube",item,getVisibility("cube"));

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeTicks);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setTicksVisibility);
	insertItem(TR("Tic Marks"),"Axis",item,getVisibility("ticks"));

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeCoordinates);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setCoordVisibility);
	insertItem(TR("Coordinates"),".csy",item,getVisibility("coordinates"));

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeLabels);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setLabelVisibility);
	insertItem(TR("Labels"),"Annotation",item,getVisibility("labels"));

	return htiNode;
}

void CubeElementsTool::setVisibility(const String& element, bool value) {
	CubeProperties *prop = (CubeProperties *)((CubeDrawer *)(drawer->getRootDrawer()->getDrawer("CubeDrawer")))->getProperties();
	if (prop) {
		(*prop)[element].visible = value;
	}
	mpvGetView()->Invalidate();
}

int CubeElementsTool::getVisibility(const String& element) {
	CubeProperties *prop = (CubeProperties *)((CubeDrawer *)(drawer->getRootDrawer()->getDrawer("CubeDrawer")))->getProperties();
	if (prop) {
		return (*prop)[element].visible ? 1 : 0;
	}
	return 1;
}

void CubeElementsTool::elementForm(const String& element) {
	CubeProperties *prop = (CubeProperties *)((CubeDrawer *)(drawer->getRootDrawer()->getDrawer("CubeDrawer")))->getProperties();
	if ( prop)
		new CubeElementsForm(tree,(ComplexDrawer *)drawer, htiNode,prop->elements[element]);
}

void CubeElementsTool::setCoordVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("coordinates",v);
}

void CubeElementsTool::setLabelVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("labels",v);
}


void CubeElementsTool::setCubeVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("cube",v);
}

void CubeElementsTool::setTicksVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("ticks",v);
}

void CubeElementsTool::changeCube() {
	elementForm("cube");
}

void CubeElementsTool::changeTicks() {
	elementForm("ticks");
}

void CubeElementsTool::changeCoordinates() {
	elementForm("coordinates");
}

void CubeElementsTool::changeLabels() {
	elementForm("labels");
}

String CubeElementsTool::getMenuString() const {
	return TR("Cube Elements");
}

//---------------------------------------------------

CubeElementsForm::CubeElementsForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM htiTr, CubeElement& elem) : 
DisplayOptionsForm(dr,wPar,elem.label), slider(0), fc(0), element(elem)
{
	if ( !element.color.fEqual(colorUNDEF)) {
		fc = new FieldColor(root,TR("Color"),&(element.color));
	}
	if ( element.alpha != rUNDEF) {
		transparency = round(100.0 * (1.0 - element.alpha));
		slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
		slider->SetCallBack((NotifyProc)&CubeElementsForm::setTransparency);
		slider->setContinuous(true);
	}
	create();
}

int CubeElementsForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  CubeElementsForm::apply() {
	if ( slider) {
		slider->StoreData();
		element.alpha = 1.0 - (double)transparency/100.0;
	}
	if ( fc)
		fc->StoreData();


	updateMapView();

}
