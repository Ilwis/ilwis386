#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\LineSetTool.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"
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

	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	LineSetDrawer *ldrw = dynamic_cast<LineSetDrawer *>(drawer);
	if ( !ldrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM LineSetTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", drawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		htiNode = dt->configure(parentItem);
	}
	DrawerTool::configure(htiNode);

	return htiNode;
}

String LineSetTool::getMenuString() const {
	return TR("Segment Drawer");
}

//---------------------------------------------------

