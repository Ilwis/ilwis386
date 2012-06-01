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
#include "drawers\CanvasBackgroundDrawer.h"

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
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeAxis);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setAxisVisibility);
	insertItem(TR("Axis"),"Axis",item,1);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeCoordinates);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setCoordVisibility);
	insertItem(TR("Coordinates"),".csy",item,1);

	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CubeElementsTool::changeLabels);
	item->setCheckAction(this,0,(DTSetCheckFunc)&CubeElementsTool::setLabelVisibility);
	insertItem(TR("Labels"),"Annotation",item,1);



	return htiNode;
}

void CubeElementsTool::setVisibility(const String& element, bool value) {
	CubeProperties *prop = (CubeProperties *)((CubeDrawer *)(drawer->getRootDrawer()->getDrawer("CubeDrawer")))->getProperties();
	if (prop) {
		(*prop)[element].visible = value;
		
	}
	mpvGetView()->Invalidate();
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


void CubeElementsTool::setAxisVisibility(void *value, HTREEITEM) {
	if (!value)
		return;

	bool v = *(bool *)value;
	setVisibility("axis",v);
}

void CubeElementsTool::changeAxis() {
	elementForm("axis");
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
	if ( element.color != colorUNDEF) {
		fc = new FieldColor(root,TR("Color"),&(element.color));
	}
	if ( element.transparency != rUNDEF) {
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
		element.transparency = (double)transparency/100.0;
	}
	if ( fc)
		fc->StoreData();


	updateMapView();

}
