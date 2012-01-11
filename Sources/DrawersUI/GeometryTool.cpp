#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "DrawersUI\GeometryTool.h"
#include "DrawersUI\GlobalTool.h"

DrawerTool *createGeometryTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GeometryTool(zv, view, drw);
}

GeometryTool::GeometryTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("GeometryTool", zv, view, drw)
{
	ComplexDrawer *annotations = (ComplexDrawer *)(drw->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if ( annotations)
		drawer = (ComplexDrawer *)annotations->getDrawer(200, ComplexDrawer::dtPOST);
}

GeometryTool::~GeometryTool() {
}

bool GeometryTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	return dynamic_cast<GlobalTool *>(drw) != 0;
}

HTREEITEM GeometryTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	htiNode = insertItem(TR("Geometry"),".csy",item);

	DrawerTool::configure(htiNode);
	tree->GetTreeCtrl().Expand(htiNode,TVE_COLLAPSE);

	return htiNode;
}

String GeometryTool::getMenuString() const {
	return TR("Geometry");
}
