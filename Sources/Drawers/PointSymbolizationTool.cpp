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
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "Client\Mapwindow\Drawers\SVGLoader.h"
#include "Drawers\PointSymbolizationTool.h"
#include "Drawers\SetDrawerTool.h"
#include "Drawers\AnimationTool.h"
#include "Drawers\AnimationDrawer.h"


DrawerTool *createPointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PointSymbolizationTool(zv, view, drw);
}

PointSymbolizationTool::PointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("NonRepresentationToolTool", zv, view, drw)
{
}

PointSymbolizationTool::~PointSymbolizationTool() {
}

bool PointSymbolizationTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	AnimationTool *adrwt = dynamic_cast<AnimationTool *>(tool);
	if ( !sdrwt && !adrwt)
		return false;
	PointSetDrawer *pdrw = dynamic_cast<PointSetDrawer *>(drawer);
	if ( adrwt) {
		NewDrawer *drw = adrwt->getDrawer();
		pdrw = dynamic_cast<PointSetDrawer *>(((AnimationDrawer *)(drw))->getDrawer(0));
	}
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM PointSymbolizationTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&PointSymbolizationTool::setSymbolization); 
	htiNode = insertItem(TR("Symbolization"),"Set",item);
	DrawerTool::configure(htiNode);

	return htiNode;
}

void PointSymbolizationTool::setSymbolization() {
	new PointSymbolizationForm(tree, (PointSetDrawer *)drawer);
}

String PointSymbolizationTool::getMenuString() const {
	return TR("Point Symbolization");
}

//---------------------------------------------------
PointSymbolizationForm::PointSymbolizationForm(CWnd *wPar, PointSetDrawer *dr):
DisplayOptionsForm(dr,wPar,TR("Symbolization")), selection(0), thick(1), scale(1)
{
	ILWIS::SVGLoader *loader = IlwWinApp()->getSVGContainer();
	for(map<String, SVGElement *>::iterator cur = loader->begin(); cur != loader->end(); ++cur) {
		String name = (*cur).first;
		name = name.sHead("|");
		names.push_back(name);
	}
	fselect = new FieldOneSelectString(root,TR("Symbols"),&selection, names);
	fiThick = new FieldInt(root,TR("Line thickness"),&thick);
	frScale = new FieldReal(root,TR("Symbol scale"),&scale,ValueRange(RangeReal(0.1,100.0),0.1));
	create();
}

void PointSymbolizationForm::apply(){
	fselect->StoreData();
	fselect->StoreData();
	frScale->StoreData();

	String symbol = names[selection];
	PointSetDrawer *psdrw = (PointSetDrawer *)drw;
	psdrw->setSymbolProperties(symbol,scale);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	psdrw->prepare(&pp);
	updateMapView();
}

