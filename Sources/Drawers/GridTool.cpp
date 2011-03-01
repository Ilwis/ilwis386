#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\GridDrawer.h"
#include "Drawers\GridTool.h"

using namespace std;

DrawerTool *createGridTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GridTool(zv, view, drw);
}

GridTool::GridTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("GridTool", zv, view, drw)
{
	drawer = ((RootDrawer *)drw)->getDrawer(800, ComplexDrawer::dtPOST);
}

GridTool::~GridTool() {
}

bool GridTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<RootDrawer *>(drw) != 0;
}

HTREEITEM GridTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GridTool::gridOptions);
	item->setCheckAction(this,0,(DTSetCheckFunc)&GridTool::gridActive);
	htiNode = insertItem("Grid",".grid",item, drawer->isActive(),TVI_ROOT);

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", drawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(htiNode);
	}
	
	DrawerTool::configure(htiNode);
	tree->GetTreeCtrl().Expand(htiNode,TVE_COLLAPSE);

	return htiNode;
}

void GridTool::gridActive(void *v) {
	bool value = *(bool *)v;
	drawer->setActive(value);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drawer->prepare(&pp);
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void GridTool::gridOptions() {
	new GridForm(tree, (GridDrawer *)drawer);
}

String GridTool::getMenuString() const {
	return TR("Grid");
}

//---------------------------------------------------
GridForm::GridForm(CWnd *par, GridDrawer *gdr) 
	: DisplayOptionsForm(gdr, par, TR("Grid Properties")), rDist(gdr->getGridSpacing())
	
{
	fr = new FieldReal(root, TR("Grid spacing"), &rDist, ValueRange(0.0,1e10,0.001));

  create();
}

void  GridForm::apply() {
	fr->StoreData();
	GridDrawer *grd = (GridDrawer *)drw;
	grd->setGridSpacing(rDist);
	PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();
}
