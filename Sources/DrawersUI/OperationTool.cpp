#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\MainWindow\ACTION.H"
#include "OperationTool.h"
#include "LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "AnimationTool.h"
#include "drawers\linedrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "drawers\CanvasBackgroundDrawer.h"

DrawerTool *createOperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new OperationTool(zv, view, drw);
}

OperationTool::OperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("OperationTool",zv, view, drw)
{
}

OperationTool::~OperationTool() {
}

bool OperationTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM OperationTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	itemActions.clear();
	actions.clear();
	HTREEITEM  hti = tree->GetTreeCtrl().GetParentItem(parentItem);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,hti,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&OperationTool::doOperation);

	htiNode = insertItem(TR("Operations"),"ExeIcoL", item);
	ActionList *actList = IlwWinApp()->acl();

	BaseMapPtr *bmptr = (((SpatialDataDrawer  *)(drawer->getParentDrawer()))->getBaseMap());
	addOperationItems(bmptr->fnObj.sExt);
	String sTop, sMid, sAction, sLastTop="-", sLastMid;
	HTREEITEM htiTop = htiNode, htiMid, htiOpt;

	for(int i = 0; i < actions.size(); ++i) {
		Action *act = actions[i];
		sAction = act->sName();
		String sIcon;
		if ("" ==	 sAction) {
			sIcon = act->sIcon();
			continue;
		}
		sTop = act->sMenOpt();
		if (sLastTop != sTop) {
				htiTop = htiNode;
			CString str(sTop.c_str());
			str.Remove('&');
			sIcon = actList->getTopIcon(sTop);
			htiTop = insertItem(htiTop, String(str), sIcon);
			sLastTop = sTop;
			sLastMid = "-";
			htiMid = htiTop;
		}
		sIcon = act->sIcon();
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiMid,drawer);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&OperationTool::doOperation);
		itemActions[(long)item] = act;
		htiOpt = insertItem(sAction.c_str(), sIcon, item);

	}



	return htiNode;
}

void OperationTool::doOperation() {
	//new OperationForm(tree, (ComplexDrawer *)drawer, htiNode);
	DisplayOptionTreeItem *item = (DisplayOptionTreeItem *)tree->getCurrent();
	if ( item) {
		Action *act = itemActions[(long)item];
		BaseMapPtr *bmptr = (((SpatialDataDrawer  *)(drawer->getParentDrawer()))->getBaseMap());
		long *handle = new long((long)(tree->GetDocument()->mpvGetView()->m_hWnd));
		getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP, handle);
		IlwWinApp()->ExecuteUI(act->sAction() + " " + bmptr->fnObj.sFile + bmptr->fnObj.sExt, tree);
	}

}

String OperationTool::getMenuString() const {
	return TR("Operations");
}

void OperationTool::addOperationItems(const String& sExt)
{
	ActionList *actList = IlwWinApp()->acl();
	for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter) {
		Action* act = iter();
		if (!act->fExtension(sExt))
			continue;
		actions.push_back(act);
	}


}



