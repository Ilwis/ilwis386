#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\GlobalAnnotationTool.h"
#include "DrawersUI\GlobalTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createGlobalAnnotationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GlobalAnnotationTool(zv, view, drw);
}

GlobalAnnotationTool::GlobalAnnotationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("GlobalAnnotationTool",zv, view, drw)
{
}

GlobalAnnotationTool::~GlobalAnnotationTool() {
}

bool GlobalAnnotationTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	bool ok = dynamic_cast<GlobalTool *>(drw) != 0;
	if (ok)
		return true;
	return false;
}

HTREEITEM GlobalAnnotationTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	htiNode =  insertItem("Annotations","Annotation",item,-1, TVI_FIRST);

	DrawerTool::configure(htiNode);

	return htiNode;
}




//---------------------------------------------------

