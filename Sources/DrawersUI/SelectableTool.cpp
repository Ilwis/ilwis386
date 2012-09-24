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
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawers\LayerDrawer.h"
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
DrawerTool("SelectableTool",zv, view, drw), fCtrl(false)
{
}

SelectableTool::~SelectableTool() {
}

bool SelectableTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(getDrawer());
	SpatialDataDrawer *datadrw = dynamic_cast<SpatialDataDrawer *>(layerDrawer->getParentDrawer());
	if ( datadrw) {
		bmapptr = datadrw->getBaseMap();
	}
	parentTool = tool;
	stay = true;
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
	if ( use) {
		tree->GetDocument()->mpvGetView()->addTool(this, ID_SELECTFEATURES);
		MapPaneView *view = tree->GetDocument()->mpvGetView();

		view->selectArea(this,
			(NotifyRectProc)&SelectableTool::FeatureAreaSelected,"Edit",Color(0,255,0,0.2)); 
	}
	if (!use) {
		tree->GetDocument()->mpvGetView()->noTool(getId());
		getDrawer()->select(CoordBounds()); // deselect all points
		selectedRaws.clear();
		mpvGetView()->Invalidate();
	}
}


String SelectableTool::getMenuString() const {
	return TR("Features selectable");
}

bool SelectableTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	return fCtrl;
}

bool SelectableTool::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	return fCtrl;
}

void SelectableTool::OnEscape() {
	getDrawer()->select(CoordBounds()); // deselect all points
	selectedRaws.clear();
	IlwWinApp()->SendUpdateTableSelection(selectedRaws, bmapptr->dm()->fnObj, (long)mpvGetView());
	mpvGetView()->Invalidate();
}

void SelectableTool::FeatureAreaSelected(CRect rect)
{
	if ( fCtrl || isActive() == false)
		return;
	else {
		selectedRaws.clear();
		getDrawer()->select(CoordBounds());
	}
	MapCompositionDoc* mcd = dynamic_cast<MapCompositionDoc*>(tree->GetDocument());
	if ( mcd) {
		MapPaneView *view = mcd->mpvGetView();
		if ( rect.Height() == 0 || rect.Width() == 0) {
			selectedRaws.clear();
			IlwWinApp()->SendUpdateTableSelection(selectedRaws, bmapptr->dm()->fnObj, (long)view);
			view->Invalidate();
			return;
		}



		CoordBounds cbZoom = mcd->rootDrawer->getCoordBoundsZoom();
		CRect rectWindow;
		view->GetClientRect(&rectWindow);
		Coord c1,c2;
		if ( rect.Width() == 0 || rect.Height() == 0) { // case of clicking on the map in zoom mode
			double posx = cbZoom.cMin.x + cbZoom.width() * rect.left / (double)rectWindow.Width(); // determine click point
			double posy = cbZoom.cMax.y - cbZoom.height() * rect.top / (double)rectWindow.Height();
			CoordBounds cb = cbZoom; // == cbView ? cbMap : cbZoom;
			double w = cb.width() / (2.0 * 1.41); // determine new window size
			double h = cb.height() / (2.0 * 1.41);
			c1.x = posx - w; // determine new bounds
			c1.y = posy - h;
			c2.x = posx + w;
			c2.y = posy + h;
		} else {
			c1.x = cbZoom.cMin.x + cbZoom.width() * rect.left / (double)rectWindow.Width(); // determine zoom rectangle in GL coordinates
			c1.y = cbZoom.cMax.y - cbZoom.height() * rect.top / (double)rectWindow.Height();
			c2.x = cbZoom.cMin.x + cbZoom.width() * rect.right / (double)rectWindow.Width();
			c2.y = cbZoom.cMax.y - cbZoom.height() * rect.bottom / (double)rectWindow.Height();
		}
		c1.z = c2.z = 0;

		cbZoom = CoordBounds (c1,c2);
		mcd->rootDrawer->select(cbZoom);
		vector<Feature *> features = bmapptr->getFeatures(cbZoom);
		for(int i = 0; i < features.size(); ++i) {
			Feature *f = features[i];
			if (!f || f->fValid() == false)
				continue;
			selectedRaws.push_back(f->iValue());
		}
		IlwWinApp()->SendUpdateTableSelection(selectedRaws, bmapptr->dm()->fnObj, (long)view);
		view->Invalidate();
	}
}
void SelectableTool::OnLButtonUp(UINT nFlags, CPoint point) {
	if ( !fCtrl){
		tree->GetDocument()->mpvGetView()->OnLButtonUp(nFlags, point);
		return;
	}

	if ( !getDrawer()->isSelectable())
		return;
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(getDrawer());
	if (!layerDrawer)
		return;

	Coord crd = layerDrawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	vector<Geometry *> geometries = bmapptr->getFeatures(crd);
	if ( geometries.size() > 0) {
		for(int i = 0; i < geometries.size(); ++i) {
			Feature *f = CFEATURE(geometries[i]);
			if (!f || f->fValid() == false)
				continue;
			selectedRaws.push_back(f->iValue());
			vector<NewDrawer *> drawers;
			layerDrawer->getDrawerFor(f,drawers);
			for(int j = 0; j< drawers.size(); ++j) {
				drawers[j]->setSpecialDrawingOptions(NewDrawer::sdoSELECTED, true);
			}
		}
	} else {
		getDrawer()->select(CoordBounds());
		selectedRaws.clear();
	}
}

