#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\GlobalTool.h"

using namespace ILWIS;


DrawerTool *createGlobalTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GlobalTool("GlobalTool", zv, view, drw);
}

GlobalTool::GlobalTool(const String& name, ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(name,zv, view, drw)
{
}

GlobalTool::~GlobalTool() {
}

void GlobalTool::clear() {
}

bool GlobalTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<RootDrawer *>(drw) != 0;
}

HTREEITEM GlobalTool::configure( HTREEITEM parentItem) {
	htiNode = insertItem(parentItem,"Global tools","AllLayers");

	DrawerTool::configure(htiNode);
	return htiNode;
}

String GlobalTool::getMenuString() const {
	return "";
}

