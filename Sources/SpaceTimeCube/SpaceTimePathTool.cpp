#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "SpaceTimePathTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "SpaceTimePathDrawer.h"


DrawerTool *createSpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SpaceTimePathTool(zv, view, drw);
}

SpaceTimePathTool::SpaceTimePathTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("SpaceTimePathTool",zv, view, drw)
{
}

SpaceTimePathTool::~SpaceTimePathTool() {
}

bool SpaceTimePathTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	SpaceTimePathDrawer *pdrw = dynamic_cast<SpaceTimePathDrawer *>(drawer);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM SpaceTimePathTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimePathTool::displayOptionEdges);
	SpaceTimePathDrawer *pdrw = dynamic_cast<SpaceTimePathDrawer *>(drawer);
	String transp("Edges (%d)", pdrw->iNrSteps());
	htiNode = insertItem(transp,"Edges", item);

	return htiNode;
}

void SpaceTimePathTool::displayOptionEdges() {
	new EdgesForm(tree, (ComplexDrawer *)drawer, htiNode);
}

String SpaceTimePathTool::getMenuString() const {
	return TR("SpaceTimePathTool");
}


//---------------------------------------------------
EdgesForm::EdgesForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti) : 
DisplayOptionsForm(dr,wPar,"Nr Edges"),
nrEdges(((SpaceTimePathDrawer*)dr)->iNrSteps()),
htiNrEdges(hti), fldNrEdges(0)
{
	slider = new FieldIntSliderEx(root,"Edges(1-25)", &nrEdges,ValueRange(1,25),true);
	slider->SetCallBack((NotifyProc)&EdgesForm::setNrEdges);
	slider->setContinuous(true);
	create();
}

int EdgesForm::setNrEdges(Event *ev) {
	apply();
	return 1;
}

void  EdgesForm::apply() {
	if ( initial) return;
	slider->StoreData();
	if ( fldNrEdges) {
		fldNrEdges->StoreData();
	}

	SpaceTimePathDrawer *cdrw = (SpaceTimePathDrawer*)drw;
	cdrw->SetNrSteps(nrEdges);
	String transp("Edges (%d)",nrEdges);
	TreeItem titem;
	view->getItem(htiNrEdges,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

	strcpy(titem.item.pszText,transp.c_str());
	view->GetTreeCtrl().SetItem(&titem.item);

	LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(cdrw);
	PreparationParameters pp(NewDrawer::ptGEOMETRY, 0);
	cdrw->prepare(&pp);

	updateMapView();

}
