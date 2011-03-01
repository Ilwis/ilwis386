#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\LayerTreeView.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\RepresentationTool.h"
#include "Drawers\ColorTool.h"
#include "Drawers\LegendTool.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\StretchTool.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createRepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new RepresentationTool(zv, view, drw);
}

RepresentationTool::RepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("RepresentationTool", zv,view,drw)
{
	//SetDrawer *setdrw = (SetDrawer *)drawer;
	//Representation rpr = setdrw->getRepresentation();
	//DrawerTool *legendTool = new LegendTool(zv,tree, drw);
	//legendTool->setVisible(true);
	//addTool(legendTool);
}

RepresentationTool::~RepresentationTool() {
}


HTREEITEM RepresentationTool::configure( HTREEITEM parentItem){
	SetDrawer *setdrw = (SetDrawer *)drawer;
	rpr = setdrw->getRepresentation();
	DisplayOptionItemFunc func = (DisplayOptionItemFunc)&RepresentationTool::displayOptionSubRpr; 
	bool usesRpr = drawer->getDrawMethod() == NewDrawer::drmRPR;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&RepresentationTool::displayOptionSubRpr);
	ColorTool *ctool = (ColorTool *)parentTool;
	item->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
	HTREEITEM rprItem = insertItem("Representation", ".rpr", item, (int)usesRpr);
	DrawerTool::configure(rprItem);
	
	return rprItem;
}

bool RepresentationTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	if ( dynamic_cast<RepresentationTool *>(tool->getParentTool()) != 0) 
		return false;
	if ( dynamic_cast<ColorTool *>(tool) == 0)
		return false;

	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(tool->getDrawer());
	if ( adrw) {
		sdrw = (SetDrawer *)adrw->getDrawer(0);
	}

	Representation rpr = sdrw->getRepresentation();
	bool isValid = rpr.fValid();
	if (!isValid)
		return false;
	bool isAcceptable = rpr->prv() || rpr->prc() || rpr->prg();
	if ( isAcceptable)
		parentTool = tool;
	return isAcceptable;
}

void RepresentationTool::displayOptionSubRpr() {
	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drawer);
	DrawerTool *legendTool = getTool("LegendTool");
	new RepresentationToolForm(tree, (SetDrawer *)drawer,animDrw, legendTool);
}

//---------------------------------------------------
RepresentationToolForm::RepresentationToolForm(CWnd *wPar, SetDrawer *dr,AnimationDrawer *adr, DrawerTool* t) : 
	DisplayOptionsForm(dr,wPar,"Set Representation"),
	setDrawer(dr),
	animDrw(adr),
	tool(t)
{
	if ( animDrw) {
		rpr = ((SetDrawer *)animDrw->getDrawer(0))->getRepresentation()->sName();
	}
	else {
		rpr = setDrawer->getRepresentation()->sName();
	}
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

void  RepresentationToolForm::apply() {
	fldRpr->StoreData();
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			SetDrawer *sdr = (SetDrawer *)animDrw->getDrawer(i);
			sdr->setRepresentation(rpr);
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setDrawer->setRepresentation(rpr);
		setDrawer->prepareChildDrawers(&pp);
	}
	tool->update();
	
	updateMapView();
}
