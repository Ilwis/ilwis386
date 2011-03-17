#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\AnimationManagement.h"
#include "DrawersUI\AnimationManagementTool.h"
#include "DrawersUI\AnimationTool.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createAnimationManagementTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationManagementTool(zv, view, drw);
}

AnimationManagementTool::AnimationManagementTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnimationManagementTool",zv, view, drw)
{
}

AnimationManagementTool::~AnimationManagementTool() {
}

bool AnimationManagementTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return dynamic_cast<AnimationTool *>(tool) != 0;
}

HTREEITEM AnimationManagementTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item2->setDoubleCickAction(this, (DTDoubleClickActionFunc) &AnimationManagementTool::animationManagement);
	htiNode = insertItem(TR("Run"),"History",item2);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void AnimationManagementTool::animationManagement() {
	AnimationPropertySheet *propSheet = AnimationTool::getManagement();
	if ( propSheet) {
		propSheet->setActiveAnimation((AnimationDrawer *)drawer);
		propSheet->BringWindowToTop();
		propSheet->ShowWindow(SW_SHOW);
	}
}

void AnimationManagementTool::timedEvent(UINT timerid) {
	AnimationPropertySheet *propSheet = AnimationTool::getManagement();
	if ( propSheet) 
		propSheet->PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
}
//---------------------------------------------------

