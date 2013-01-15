#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
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
#include "Client\Mapwindow\MapPaneView.h" 
#include "Client\Mapwindow\AreaSelector.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "SelectableTool.h"
#include "LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "AnimationTool.h"
#include "drawers\linedrawer.h"
#include "Engine\Map\Feature.h"
#include "drawers\GridDrawer.h"


DrawerTool *createSelectableTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SelectableTool(zv, view, drw);
}

SelectableTool::SelectableTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
DrawerTool("SelectableTool",zv, view, drw), fCtrl(false), fShift(false), as(0), selectedRaws(0)
{
}

SelectableTool::~SelectableTool() {
	MapPaneView *view = tree->GetDocument()->mpvGetView();
	view->changeStateTool(ID_SELECTFEATURES, false);
}

bool SelectableTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(getDrawer());
	RasterLayerDrawer *rldr = dynamic_cast<RasterLayerDrawer *>(layerDrawer);
	if ( rldr) // raster layers dont have selectable features
		return false;
	SpatialDataDrawer *datadrw = dynamic_cast<SpatialDataDrawer *>(layerDrawer->getParentDrawer());
	if ( datadrw) {
		bmapptr = datadrw->getBaseMap();
	}
	parentTool = tool;
	stay = true;
	selectedRaws = layerDrawer->getSelectedRaws();
	return true;
}

HTREEITEM SelectableTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&SelectableTool::setSelectable);
	htiNode = insertItem(TR("Selectable Features"),"SelectArea", item, getDrawer()->isSelectable());

	return htiNode;
}

void SelectableTool::setSelectable(void *v, HTREEITEM) {
	bool use = *(bool *)v;
	getDrawer()->setSelectable(use);
	MapPaneView *view = tree->GetDocument()->mpvGetView();
	if ( use) {
		view->noTool();
		if (!view->addTool(this, ID_SELECTFEATURES)) // addTool(this, getId());
			view->changeStateTool(ID_SELECTFEATURES, true);		

		if (view->fAdjustSize)
			as = new AreaSelector(view, this, (NotifyRectProc)&SelectableTool::FeatureAreaSelected, Color(0,255,0,0.2));
		else 
			as = new AreaSelector(view, this, (NotifyRectProc)&SelectableTool::FeatureAreaSelected, view->dim, Color(0,255,0,0.2));
		as->SetCursor(zCursor("Edit"));
		as->setActive(true);
	} else {
		view->changeStateTool(ID_SELECTFEATURES, false);
		if (as) {
			as->Stop();
			if ( as->stayResident() == false) {
				delete as;
				as = 0;
			}
		}

		getDrawer()->select(false); // deselect all points
		selectedRaws->clear();
		mpvGetView()->Invalidate();
	}
}

void SelectableTool::Stop()
{
	getDrawer()->setSelectable(false);
	if (as) {
		as->Stop();
		if (!as->stayResident()) {
			delete as;
			as = 0;
		}
	}
	if (tree->m_hWnd)
		tree->GetTreeCtrl().SetCheck(htiNode, false);
}

String SelectableTool::getMenuString() const {
	return TR("Features selectable");
}


void SelectableTool::OnMouseMove(UINT nFlags, CPoint point)
{
	if (as)
		as->OnMouseMove(nFlags, point);
}

void SelectableTool::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (as)
		as->OnLButtonDown(nFlags, point);
}

void SelectableTool::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (as)
		as->OnLButtonUp(nFlags, point);
}

bool SelectableTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	return fCtrl || fShift;
}

bool SelectableTool::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	return fCtrl || fShift;
}

void SelectableTool::OnEscape() {
	getDrawer()->select(false); // deselect all points
	selectedRaws->clear();
	IlwWinApp()->SendUpdateTableSelection(*selectedRaws, bmapptr->dm()->fnObj, (long)mpvGetView());
	mpvGetView()->Invalidate();
}

void SelectableTool::FeatureAreaSelected(CRect rect)
{
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(getDrawer());
	if (!layerDrawer)
		return;
	if (!isActive())
		return;
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(tree->GetDocument());
	if ( mcd) {
		MapPaneView *view = mcd->mpvGetView();
		layerDrawer->select(rect, *selectedRaws, fCtrl ? LayerDrawer::SELECTION_ADD : (fShift ? LayerDrawer::SELECTION_REMOVE : LayerDrawer::SELECTION_NEW));
		IlwWinApp()->SendUpdateTableSelection(*selectedRaws, bmapptr->dm()->fnObj, (long)view);
		view->Invalidate();
	}
}
