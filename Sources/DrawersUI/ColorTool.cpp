#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\ColorTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\AnnotationDrawers.h"

DrawerTool *createColorTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ColorTool(zv, view, drw);
}

ColorTool::ColorTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("ColorTool",zv, view, drw), colorCheck(0)
{
}

ColorTool::~ColorTool() {
}

void ColorTool::clear() {
	colorCheck->clear();
}

bool ColorTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *adrwt = dynamic_cast<SetDrawerTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM ColorTool::configure( HTREEITEM parentItem) {
	htiNode = insertItem(parentItem, TR("Portrayal"), "Colors");
	colorCheck = new SetChecks(tree,this,(DTSetCheckFunc)&ColorTool::setcheckRpr);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void ColorTool::setcheckRpr(void *value, HTREEITEM item) {
	if ( value == 0)
		return;
	HTREEITEM hItem = *((HTREEITEM *)value);
	String name = tree->getItemName(hItem);
	if ( name == sUNDEF)
		return;
	int index = name.find_last_of("|");

	if ( index == string::npos)
		return;

	String method = name.substr(index + 1);

	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	if ( adrw) {
		sdrw = (LayerDrawer *)adrw->getDrawer(0);
	}
	Representation rpr = sdrw->getRepresentation();
	if ( method == "Representation") {
		sdrw->setDrawMethod(NewDrawer::drmRPR);
		sdrw->setUseRpr(true);
	}
	else if ( method == "Single Color"){
		sdrw->setDrawMethod(NewDrawer::drmSINGLE);
		sdrw->setUseRpr(false);
	}
	else if ( method == "Multiple Colors"){
		sdrw->setDrawMethod(NewDrawer::drmMULTIPLE);
		sdrw->setUseRpr(false);
	}
	if ( adrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < adrw->getDrawerCount(); ++i) {
			LayerDrawer *sdr = (LayerDrawer *)adrw->getDrawer(i);
			sdr->modifyLineStyleItem(tree, (sdrw->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		sdrw->modifyLineStyleItem(tree, (sdrw->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		sdrw->prepareChildDrawers(&pp);
	}
	AnnotationDrawers *annotations = (AnnotationDrawers *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if (annotations)
		annotations->refreshClassLegends();

	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String ColorTool::getMenuString() const {
	return TR("Portrayal");
}

//---------------------------------------------------

