#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\GraticuleDrawer.h"
#include "DrawersUI\GraticuleTool.h"
#include "DrawersUI\GlobalTool.h"

using namespace std;

DrawerTool *createGraticuleTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new GraticuleTool(zv, view, drw);
}

GraticuleTool::GraticuleTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("GraticuleTool", zv, view, drw), hasGraticule(false)
{
	drawer = ((ComplexDrawer *)(drw->getRootDrawer()))->getDrawer(700, ComplexDrawer::dtPOST);
}

GraticuleTool::~GraticuleTool() {
}

bool GraticuleTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	return dynamic_cast<GlobalTool *>(drw) != 0;
}

HTREEITEM GraticuleTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&GraticuleTool::GraticuleOptions);
	item->setCheckAction(this,0,(DTSetCheckFunc)&GraticuleTool::graticuleActive);
	htiNode = insertItem("Graticule",".grat",item, drawer->isActive(),TVI_ROOT);

	DrawerTool *dt = DrawerTool::createTool("LineStyleTool", getDocument()->mpvGetView(),tree,drawer);
	if ( dt) {
		addTool(dt);
		dt->configure(htiNode);
	}

	
	DrawerTool::configure(htiNode);
	tree->GetTreeCtrl().Expand(htiNode,TVE_COLLAPSE);

	return htiNode;
}

void GraticuleTool::graticuleActive(void *v, HTREEITEM) {
	bool value = *(bool *)v;
	NewDrawer *drw = drawer->getRootDrawer()->getDrawer("GraticuleDrawer");
	if ( drw) {
		if ( value) {
			PreparationParameters pp(NewDrawer::ptGEOMETRY);
			drw->prepare(&pp);
		}
		drw->setActive(value);
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
}

void GraticuleTool::GraticuleOptions() {
	new GraticuleForm(tree, (GraticuleDrawer *)drawer);
}

String GraticuleTool::getMenuString() const {
	return TR("Graticule");
}

//---------------------------------------------------
GraticuleForm::GraticuleForm(CWnd *par, GraticuleDrawer *gdr) 
	: DisplayOptionsForm(gdr, par, TR("Graticule Properties")), rDist(gdr->getGridSpacing())
	
{
	fr = new FieldReal(root, TR("Graticule spacing"), &rDist, ValueRange(0.0,1e10,0.001));


  create();
}

void  GraticuleForm::apply() {
	fr->StoreData();
	GraticuleDrawer *grd = (GraticuleDrawer *)drw;
	grd->setGridSpacing(rDist);
	PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();
}
