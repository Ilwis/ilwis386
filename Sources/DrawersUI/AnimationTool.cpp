#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "AnimationManagement.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"

using namespace ILWIS;

AnimationPropertySheet *AnimationTool::animManagement=0;

DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationTool(zv, view, drw);
}

AnimationTool::AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	SetDrawerTool("AnimationTool",zv, view, drw)
{
}

AnimationTool::~AnimationTool() {
}

void AnimationTool::clear() {
	animManagement->removeAnimation((AnimationDrawer *)drawer);
	SendMessage(animManagement->m_hWnd,ILWM_UPDATE_ANIM,0,0);
}

bool AnimationTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<AnimationDrawer *>(drw) != 0;
}

HTREEITEM AnimationTool::configure( HTREEITEM parentItem) {
	AnimationProperties *candidate = animManagement->findAnimationProps((AnimationDrawer *)drawer);
	if ( candidate == 0) {
		AnimationProperties props;
		props.drawer = (AnimationDrawer *)drawer;
		props.mdoc = tree->GetDocument();
		if ( props.animBar == 0) {
			props.animBar = new AnimationBar();
			MapWindow *parent = (MapWindow *)props.mdoc->mpvGetView()->GetParent();
			props.animBar->Create(parent,props);
			CRect rect;
			parent->bbDataWindow.GetWindowRect(&rect);
			rect.OffsetRect(1,0);
			parent->DockControlBar(props.animBar,AFX_IDW_DOCKBAR_TOP, rect);
		}
		animManagement->addAnimation(props);
	}

	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);

	//htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv");
	//DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,htiNode,drawer);
	//item2->setDoubleCickAction(this, (DTDoubleClickActionFunc) &AnimationTool::animationDefaultView);
	//HTREEITEM hti = insertItem(TR("Restore default view"),".isl",item2);


	DrawerTool::configure(htiNode);

	return htiNode;
}

AnimationPropertySheet *AnimationTool::getManagement()  {
	return animManagement;
}

String AnimationTool::getMenuString() const {
	return TR("Animation");
}

void AnimationTool::animationDefaultView() {
	((AnimationDrawer *)drawer)->animationDefaultView();
}

