#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\SetDrawerTool.h"

using namespace ILWIS;



SetDrawerTool::SetDrawerTool(const String& name, ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(name,zv, view, drw)
{
}

SetDrawerTool::~SetDrawerTool() {
}

void SetDrawerTool::clear() {
}

bool SetDrawerTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<SetDrawer *>(drw) != 0;
}

HTREEITEM SetDrawerTool::configure( HTREEITEM parentItem) {
	DrawerTool::configure(htiNode);

	return htiNode;
}

String SetDrawerTool::getMenuString() const {
	return "";
}

