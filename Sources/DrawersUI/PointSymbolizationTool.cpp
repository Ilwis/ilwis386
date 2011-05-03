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
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "DrawersUI\PointSymbolizationTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Drawers\SetDrawer.h"


DrawerTool *createPointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new PointSymbolizationTool(zv, view, drw);
}

PointSymbolizationTool::PointSymbolizationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("PointSymbolizationTool", zv, view, drw)
{
}

PointSymbolizationTool::~PointSymbolizationTool() {
}

bool PointSymbolizationTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if ( !ldrwt && !setDrawerTool)
		return false;
	PointLayerDrawer *pdrw = dynamic_cast<PointLayerDrawer *>(drawer);
	if ( setDrawerTool) {
		NewDrawer *drw = setDrawerTool->getDrawer();
		pdrw = dynamic_cast<PointLayerDrawer *>(((SetDrawer *)(drw))->getDrawer(0));
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
	new PointSymbolizationForm(tree, (PointLayerDrawer *)drawer);
}

String PointSymbolizationTool::getMenuString() const {
	return TR("Point Symbolization");
}

//---------------------------------------------------
PointSymbolizationForm::PointSymbolizationForm(CWnd *wPar, PointLayerDrawer *dr):
DisplayOptionsForm(dr,wPar,TR("Symbolization")), selection(0), thick(1), scale(1)
{
	ILWIS::SVGLoader *loader = NewDrawer::getSvgLoader();
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

	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
	String symbol = names[selection];
	if ( setDrw) {
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PointLayerDrawer *psdrw = (PointLayerDrawer *) (setDrw->getDrawer(i));
			psdrw->setSymbolProperties(symbol,scale);
		}
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setDrw->prepareChildDrawers(&pp);
	} else {
		PointLayerDrawer *psdrw = (PointLayerDrawer *)drw;
		psdrw->setSymbolProperties(symbol,scale);
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		psdrw->prepare(&pp);
	}


	updateMapView();
}

