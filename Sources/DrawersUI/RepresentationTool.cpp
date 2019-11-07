#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\LayerTreeView.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "DrawersUI\RepresentationTool.h"
#include "DrawersUI\ColorTool.h"
#include "DrawersUI\LegendTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\StretchTool.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createRepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new RepresentationTool(zv, view, drw);
}

RepresentationTool::RepresentationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("RepresentationTool", zv,view,drw)
{
	//LayerDrawer *setdrw = (LayerDrawer *)drawer;
	//Representation rpr = setdrw->getRepresentation();
	//DrawerTool *legendTool = new LegendTool(zv,tree, drw);
	//legendTool->setVisible(true);
	//addTool(legendTool);
}

RepresentationTool::~RepresentationTool() {
}


HTREEITEM RepresentationTool::configure( HTREEITEM parentItem){
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	if ( ldrw == 0) {
		SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
		if (sdrw)
			ldrw = (LayerDrawer *)sdrw->getDrawer(0);
		else
			throw ErrorObject(TR("Inconsistent drawer configuration"));
	}
	rpr = ldrw->getRepresentation();
	if ( !rpr.fValid())
		return parentItem;
	DisplayOptionItemFunc func = (DisplayOptionItemFunc)&RepresentationTool::displayOptionSubRpr; 
	bool usesRpr = drawer->getDrawMethod() == NewDrawer::drmRPR;
	DisplayOptionRadioButtonItem *item = new DisplayOptionRadioButtonItem(TR("Representation"), tree,parentItem,drawer);
	item->setState(true);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&RepresentationTool::displayOptionSubRpr);
	ColorTool *ctool = (ColorTool *)parentTool;
	item->setCheckAction(ctool,ctool->getColorCheck(), (DTSetCheckFunc)&ColorTool::setcheckRpr);
	htiNode = insertItem("Representation", ".rpr", item, -1);
	DrawerTool::configure(htiNode);
	
	return htiNode;
}

bool RepresentationTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	if ( dynamic_cast<RepresentationTool *>(tool->getParentTool()) != 0) 
		return false;
	if ( dynamic_cast<ColorTool *>(tool) == 0)
		return false;

	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(tool->getDrawer());
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(tool->getDrawer());
	if ( adrw) {
		ldrw = (LayerDrawer *)adrw->getDrawer(0);
	}
	if ( !ldrw)
		return false;
	if (ldrw->getDrawMethod() == NewDrawer::drmCOLOR)
		return false; // RepresentationTool is not usable for color composites

	Representation rpr = ldrw->getRepresentation();

	bool isValid = rpr.fValid();
	if (!isValid)
		return false;
	bool isAcceptable = rpr->prg() || rpr->prc();
	if ( isAcceptable)
		parentTool = tool;
	return isAcceptable;
}

void RepresentationTool::displayOptionSubRpr() {
	SetDrawer *animDrw = dynamic_cast<SetDrawer *>(drawer);
	DrawerTool *legendTool = getTool("LegendTool");
	new RepresentationToolForm(tree, (LayerDrawer *)drawer,animDrw, legendTool);
}

//---------------------------------------------------
RepresentationToolForm::RepresentationToolForm(CWnd *wPar, ILWIS::LayerDrawer *dr,SetDrawer *adr, DrawerTool* t) : 
	DisplayOptionsForm(dr,wPar,"Set Representation"),
	layerDrawer(dr),
	animDrw(adr),
	tool(t)
{
	if ( animDrw) {
		rpr = ((ILWIS::LayerDrawer *)animDrw->getDrawer(0))->getRepresentation()->sName();
	}
	else {
		rpr = layerDrawer->getRepresentation()->sName();
	}
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

void  RepresentationToolForm::apply() {
	fldRpr->StoreData();
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			LayerDrawer *sdr = (LayerDrawer *)animDrw->getDrawer(i);
			sdr->setRepresentation(rpr);
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		layerDrawer->setRepresentation(rpr);
		layerDrawer->prepareChildDrawers(&pp);
	}
	tool->update();
	
	updateMapView();
}
