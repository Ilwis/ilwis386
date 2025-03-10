#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\ColorTool.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"

//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createColorTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ColorTool(zv, view, drw);
}

ColorTool::ColorTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("ColorTool",zv, view, drw), colorCheck(0)
{
}

ColorTool::~ColorTool() {
}

bool ColorTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;

	parentTool = tool;
	return true;
}

HTREEITEM ColorTool::configure( HTREEITEM parentItem) {
	htiNode = insertItem(parentItem, TR("Colors"), "Colors");
	colorCheck = new SetChecks(tree,this,(DTSetCheckFunc)&ColorTool::setcheckRpr);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void ColorTool::setcheckRpr(void *value) {
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

	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drawer);
	if ( adrw) {
		sdrw = (SetDrawer *)adrw->getDrawer(0);
	}
	Representation rpr = sdrw->getRepresentation();
	if ( method == "Representation")
		sdrw->setDrawMethod(NewDrawer::drmRPR);
	else if ( method == "Single Color")
		sdrw->setDrawMethod(NewDrawer::drmSINGLE);
	else if ( method == "Multiple Colors"){
		sdrw->setDrawMethod(NewDrawer::drmMULTIPLE);
	}
	if ( adrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < adrw->getDrawerCount(); ++i) {
			SetDrawer *sdr = (SetDrawer *)adrw->getDrawer(i);
			sdr->modifyLineStyleItem(tree, (sdrw->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		sdrw->modifyLineStyleItem(tree, (sdrw->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		sdrw->prepareChildDrawers(&pp);
	}
	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

String ColorTool::getMenuString() const {
	return TR("Color");
}

//---------------------------------------------------

