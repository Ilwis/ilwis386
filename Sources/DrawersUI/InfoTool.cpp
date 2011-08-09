#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "InfoTool.h"
#include "LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "AnimationTool.h"
#include "drawers\linedrawer.h"
#include "drawers\GridDrawer.h"


DrawerTool *createInfoTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new InfoTool(zv, view, drw);
}

InfoTool::InfoTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("InfoTool",zv, view, drw)
{
}

InfoTool::~InfoTool() {
}

bool InfoTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
//	CanvasBackgroundDrawer *cbdr = dynamic_cast<CanvasBackgroundDrawer *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM InfoTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&InfoTool::setInfo);
	htiNode = insertItem(TR("Map Information"),"info", item, getDrawer()->hasInfo());

	return htiNode;
}

void InfoTool::setInfo(void *v, HTREEITEM) {
	bool use = *(bool *)v;
	getDrawer()->setInfo(use);
}


String InfoTool::getMenuString() const {
	return TR("Map Information");
}

