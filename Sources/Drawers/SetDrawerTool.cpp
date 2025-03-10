#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\SetDrawerTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createSetDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SetDrawerTool(zv, view, drw);
}

SetDrawerTool::SetDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("SetDrawerTool",zv, view, drw)
{
}

SetDrawerTool::~SetDrawerTool() {
}

bool SetDrawerTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<SetDrawer *>(drw) != 0;
}

HTREEITEM SetDrawerTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);
	DrawerTool::configure(htiNode);

	return htiNode;
}

//---------------------------------------------------

