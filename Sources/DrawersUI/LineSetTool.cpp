#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldclass.cpp"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
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
#include "DrawersUI\LineSetTool.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineLayerDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createLineSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LineSetTool(zv, view, drw);
}

LineSetTool::LineSetTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("LineSetTool",zv, view, drw)
{
}

LineSetTool::~LineSetTool() {
}

bool LineSetTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setdrawertool = dynamic_cast<SetDrawerTool *>(tool);
	if ( !ldrwt && !setdrawertool)
		return false;

	NewDrawer *ndrw = drawer;;
	if ( setdrawertool) {
		SetDrawer *drw = (SetDrawer *)(setdrawertool->getDrawer());
		ndrw = drw->getDrawer(0);
	}

	LineLayerDrawer *ldrw = dynamic_cast<LineLayerDrawer *>(ndrw);
	if ( !ldrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM LineSetTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&LineSetTool::displayOptionRprLine);
	htiNode = insertItem(TR("Segment Representation"),".mps", item); 	

	return htiNode;
}

void LineSetTool::displayOptionRprLine() {

}

String LineSetTool::getMenuString() const {
	return TR("Segment tool");
}


