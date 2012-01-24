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
#include "GlobalOperationTool.h"
#include "LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "drawers\CanvasBackgroundDrawer.h"
#include "DrawersUI\GlobalTool.h"

DrawerTool *createGlobalOperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GlobalOperationTool(zv, view, drw);
}

GlobalOperationTool::GlobalOperationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("GlobalOperationTool",zv, view, drw), first(true)
{
}

GlobalOperationTool::~GlobalOperationTool() {
}

bool GlobalOperationTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return dynamic_cast<GlobalTool *>(tool) != 0;
}

HTREEITEM GlobalOperationTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	itemActions.clear();
	actions.clear();
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GlobalOperationTool::doOperation);
	item->setTool(this);

	htiNode = insertItem(TR("Operations"),"ExeIcoL", item);
	htiDummy = insertItem(htiNode, "dummy", ".exe");



	return htiNode;
}

void GlobalOperationTool::doAction(int option) {

	if ( !first)
		return;

	ActionList *actList = IlwWinApp()->acl();

	tree->GetTreeCtrl().DeleteItem(htiDummy);

	addGlobalOperationItems();
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
			//DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiTop,drawer);
			//item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GlobalOperationTool::doOperation);
			htiTop = insertItem(htiTop, String(str), sIcon);
			sLastTop = sTop;
			sLastMid = "-";
			htiMid = htiTop;
		}
		sIcon = act->sIcon();
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiMid,drawer);
		item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GlobalOperationTool::doOperation);
		itemActions[(long)item] = act;
		htiOpt = insertItem(sAction.c_str(), sIcon, item);

	}
	first = false;

}

void GlobalOperationTool::doOperation() {
	DisplayOptionTreeItem *item = (DisplayOptionTreeItem *)tree->getCurrent();
	if ( item) {
		Action *act = itemActions[(long)item];
		long *handle = new long((long)(tree->GetDocument()->mpvGetView()->m_hWnd));
		getEngine()->getContext()->SetThreadLocalVar(IlwisAppContext::tlvMAPWINDOWAPP, handle);
		IlwWinApp()->ExecuteUI(act->sAction(), tree);
	}

}

String GlobalOperationTool::getMenuString() const {
	return TR("Operations");
}

void GlobalOperationTool::addGlobalOperationItems()
{
	ActionList *actList = IlwWinApp()->acl();
	for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter) {
		Action* act = iter();
		actions.push_back(act);
	}


}



