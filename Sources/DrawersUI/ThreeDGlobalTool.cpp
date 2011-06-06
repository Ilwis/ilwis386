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
#include "DrawersUI\ThreeDGlobalTool.h"
#include "DrawersUI\GlobalTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createThreeDGlobalTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ThreeDGlobalTool(zv, view, drw);
}

ThreeDGlobalTool::ThreeDGlobalTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("ThreeDGlobalTool",zv, view, drw)
{
}

ThreeDGlobalTool::~ThreeDGlobalTool() {
}

bool ThreeDGlobalTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	bool ok = dynamic_cast<GlobalTool *>(drw) != 0;
	if (ok)
		return true;
	return false;
}

HTREEITEM ThreeDGlobalTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDGlobalTool::setthreeD);
	htiNode =  insertItem("3D","3D",item,((RootDrawer *)drawer)->is3D(),TVI_FIRST);

	DrawerTool::configure(htiNode);

	return htiNode;
}


void ThreeDGlobalTool::setthreeD(void *v, HTREEITEM) {
	bool value = *(bool *)(v);

	((RootDrawer *)drawer)->set3D(value);
	PreparationParameters pp(NewDrawer::pt3D);
	((RootDrawer *)drawer)->prepare(&pp);

	//make3D(value,tv);
	getDocument()->mpvGetView()->Invalidate();
}

//---------------------------------------------------

