#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\AnimationTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationTool(zv, view, drw);
}

AnimationTool::AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnimationTool",zv, view, drw)
{
}

AnimationTool::~AnimationTool() {
}

bool AnimationTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<AnimationDrawer *>(drw) != 0;
}

HTREEITEM AnimationTool::configure( HTREEITEM parentItem) {
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);

	//htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv");
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item2->setDoubleCickAction(this, (DTDoubleClickActionFunc) &AnimationTool::animationDefaultView);
	HTREEITEM hti = insertItem(TR("Restore default view"),".isl",item2);


	DrawerTool::configure(htiNode);

	return htiNode;
}

String AnimationTool::getMenuString() const {
	return TR("Animation");
}

void AnimationTool::animationDefaultView() {
	((AnimationDrawer *)drawer)->animationDefaultView();
}

