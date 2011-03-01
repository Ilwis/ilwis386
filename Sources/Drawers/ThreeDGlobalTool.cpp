#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\ThreeDGlobalTool.h"
//#include "Drawers\RepresentationTool.h"
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

bool ThreeDGlobalTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<RootDrawer *>(drw) != 0;
}

HTREEITEM ThreeDGlobalTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&ThreeDGlobalTool::setthreeD);
	HTREEITEM hti =  insertItem("3D","3D",item,((RootDrawer *)drawer)->is3D(),TVI_FIRST);

	DrawerTool::configure(hti);

	return hti;
}


void ThreeDGlobalTool::setthreeD(void *v) {
	bool value = *(bool *)(v);

	((RootDrawer *)drawer)->set3D(value);

	//make3D(value,tv);
	((RootDrawer *)drawer)->getDrawerContext()->getDocument()->mpvGetView()->Invalidate();
}

//---------------------------------------------------

