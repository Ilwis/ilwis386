#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\LayerDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createLayerDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LayerDrawerTool(zv, view, drw);
}

LayerDrawerTool::LayerDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("LayerDrawerTool",zv, view, drw)
{
}

LayerDrawerTool::~LayerDrawerTool() {
}

bool LayerDrawerTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<LayerDrawer *>(drw) != 0;
}

HTREEITEM LayerDrawerTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);
	DrawerTool::configure(htiNode);

	return htiNode;
}

//---------------------------------------------------

