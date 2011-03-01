#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Drawers\ColorTool.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\NonRepresentationTool.h"

DrawerTool *createNonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new NonRepresentationToolTool(zv, view, drw);
}

NonRepresentationToolTool::NonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("NonRepresentationToolTool", zv, view, drw)
{
}

NonRepresentationToolTool::~NonRepresentationToolTool() {
}

bool NonRepresentationToolTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	if ( dynamic_cast<NonRepresentationToolTool *>(tool->getParentTool()) != 0) 
		return false;
	if ( dynamic_cast<ColorTool *>(tool) == 0)
		return false;

	FeatureSetDrawer *sdrw = dynamic_cast<FeatureSetDrawer *>(tool->getDrawer());
	if (!sdrw)
		return false;
	Representation rpr = sdrw->getRepresentation();
	bool isAcceptable = true;
	if ( rpr.fValid()) {
		isAcceptable = !(rpr->prv() || rpr->prg());
	}
	if ( isAcceptable)
		parentTool = tool;
	return isAcceptable;
}

HTREEITEM NonRepresentationToolTool::configure( HTREEITEM parentItem) {
	int iImgLeg = IlwWinApp()->iImage("Picture");
	htiNode = tree->GetTreeCtrl().InsertItem(TR("Fixed Colors").scVal(), iImgLeg, iImgLeg, parentItem);
	FeatureSetDrawer *sdrw = dynamic_cast<FeatureSetDrawer *>(drawer);
	ColorTool *ctool = (ColorTool *)parentTool;
	bool useSingleColor = sdrw->getDrawMethod() == NewDrawer::drmSINGLE;
	bool useRpr = sdrw->getDrawMethod() == NewDrawer::drmRPR;
	DisplayOptionColorItem *colorItem = new DisplayOptionColorItem("Single color", tree,htiNode,drawer);
	colorItem->setDoubleCickAction(this,(DTDoubleClickActionFunc)&NonRepresentationToolTool::displayOptionSingleColor);
	colorItem->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
	colorItem->setColor(sdrw->getSingleColor());
	HTREEITEM singleColorItem = insertItem("Single Color","SingleColor",colorItem, useSingleColor & !useRpr);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,htiNode,drawer);	
	item->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
	HTREEITEM multiColorItem = insertItem("Multiple Colors","MultipleColors",item, !useSingleColor & !useRpr);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void NonRepresentationToolTool::displayOptionSingleColor() {
	new SetSingleColorForm(tree, (FeatureSetDrawer *)drawer);
}

String NonRepresentationToolTool::getMenuString() const {
	return TR("Fixed Color Scheme");
}
//------------------------------------------------
SetSingleColorForm::SetSingleColorForm(CWnd *wPar, FeatureSetDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(dr->getSingleColor())
{
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetSingleColorForm::apply() {
	fc->StoreData();
	((FeatureSetDrawer *)drw)->setSingleColor(c);
	//((FeatureSetDrawer *)drw)->colorItem->setColor(c);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}

