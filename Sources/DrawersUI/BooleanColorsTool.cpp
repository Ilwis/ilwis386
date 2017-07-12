#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "DrawersUI\ColorTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\BooleanColorsTool.h"

DrawerTool *createBooleanColorsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new BooleanColorsTool(zv, view, drw);
}

BooleanColorsTool::BooleanColorsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("BooleanColorsTool", zv, view, drw)
{
}

BooleanColorsTool::~BooleanColorsTool() {
}

bool BooleanColorsTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	if ( dynamic_cast<BooleanColorsTool *>(tool->getParentTool()) != 0) 
		return false;
	if ( dynamic_cast<ColorTool *>(tool) == 0)
		return false;

	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(tool->getDrawer());
	if (!ldrw)
		return false;
	bool isAcceptable = ldrw->useAttributeColumn() ? ldrw->getAtttributeColumn()->dm()->pdbool() : (*(BaseMap*)ldrw->getDataSource())->dm()->pdbool();

	return isAcceptable;
}

HTREEITEM BooleanColorsTool::configure( HTREEITEM parentItem) {
	int iImgLeg = IlwWinApp()->iImage(".bool");
	htiNode = tree->GetTreeCtrl().InsertItem(TR("Boolean Colors").c_str(), iImgLeg, iImgLeg, parentItem);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&BooleanColorsTool::displayOptionColorTrue); 
	insertItem(TR("True Color"),"true",item);

	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item2->setDoubleCickAction(this,(DTDoubleClickActionFunc)&BooleanColorsTool::displayOptionColorFalse); 
	insertItem(TR("False Color"),"false",item2);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void BooleanColorsTool::displayOptionColorTrue() {
	new SetColorFormTrue(tree, (LayerDrawer *)drawer);
}

void BooleanColorsTool::displayOptionColorFalse() {
	new SetColorFormFalse(tree, (LayerDrawer *)drawer);
}

String BooleanColorsTool::getMenuString() const {
	return TR("Boolean Color Scheme");
}
//------------------------------------------------
SetColorFormTrue::SetColorFormTrue(CWnd *wPar, LayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("draw color for %S",dr->getName())),
		c(dr->getDrawingColor()->getColor2())
{
	c.alpha() = 255 - c.alpha(); // inverse the alpha, for FieldColor
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetColorFormTrue::apply() {
	fc->StoreData();
	Color clr (c);
	clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
	((LayerDrawer *)drw)->getDrawingColor()->color2(clr);
	LayerDrawer *lyerdrw = dynamic_cast<LayerDrawer *>(drw);
	if (lyerdrw) {
		lyerdrw->setDrawMethod(NewDrawer::drmBOOL);
		lyerdrw->setUseRpr(false);
	}
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}
//----------------------------------------------------------
SetColorFormFalse::SetColorFormFalse(CWnd *wPar, LayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("draw color for %S",dr->getName())),
		c(dr->getDrawingColor()->getColor1())
{
	c.alpha() = 255 - c.alpha(); // inverse the alpha, for FieldColor
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetColorFormFalse::apply() {
	fc->StoreData();
	Color clr (c);
	clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
	((LayerDrawer *)drw)->getDrawingColor()->color1(clr);
	LayerDrawer *lyerdrw = dynamic_cast<LayerDrawer *>(drw);
	if (lyerdrw) {
		lyerdrw->setDrawMethod(NewDrawer::drmBOOL);
		lyerdrw->setUseRpr(false);
	}
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}
