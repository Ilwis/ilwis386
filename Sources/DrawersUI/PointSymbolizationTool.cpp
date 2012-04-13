#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\engine.h"
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
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\pointdrawer.h"
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
DisplayOptionsForm(dr,wPar,TR("Symbolization")), selection(0)
{
	ILWIS::SVGLoader *loader = NewDrawer::getSvgLoader();
	for(map<String, SVGElement *>::iterator cur = loader->begin(); cur != loader->end(); ++cur) {
		String name = (*cur).first;
		name = name.sHead("|");
		names.push_back(name);
	}
	String base = getEngine()->getContext()->sIlwDir();
	base += "Resources\\Symbols\\";
	props = (PointProperties *)dr->getProperties();
	name = props->symbol == "" ? "open-rectangle" : props->symbol;
	fdSelect = new FieldDataType(root,TR("Symbols"),&name,".ivg",false,0,FileName(base),false);
	// fselect = new FieldOneSelectString(root,TR("Symbols"),&selection, names);
	fiThick = new FieldReal(root,TR("Line thickness"),&(props->thickness));
	frScale = new FieldReal(root,TR("Symbol scale"),&(props->scale),ValueRange(RangeReal(0.1,100.0),0.1));
	frRot = new FieldReal(root,TR("Symbol rotation"),&(props->angle),ValueRange(RangeReal(0.0,360.0),0.1));
	t3dOr = props->threeDOrientation ? 1 : 0;
	f3d = new CheckBox(root,TR("3D orientation"),&t3dOr);
	create();
}

void PointSymbolizationForm::apply(){
	fdSelect->StoreData();
	fiThick->StoreData();
	frScale->StoreData();
	f3d->StoreData();
	frRot->StoreData();

	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
	//String symbol = names[selection];
	SVGLoader *loader = NewDrawer::getSvgLoader();
	SVGLoader::const_iterator cur = loader->find(name);
	if ( cur == loader->end()) {
		loader->getSVGSymbol(name);

	}

	if ( setDrw) {
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			PointLayerDrawer *psdrw = (PointLayerDrawer *) (setDrw->getDrawer(i));
			PointProperties *oldprops = (PointProperties *)psdrw->getProperties();
			props->threeDOrientation = t3dOr != 0;
			oldprops->set(props);
			PreparationParameters pp(NewDrawer::ptRENDER);
			psdrw->prepareChildDrawers(&pp);
		}
	} else {
		FileName fn(name);
		props->symbol = fn.sFile;
		props->threeDOrientation = t3dOr != 0;
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		RepresentationProperties rprop;
		rprop.symbolSize = props->scale * 100;
		rprop.symbolType = props->symbol;
		pp.props = &rprop;
		drw->prepare(&pp);
	}


	updateMapView();
}

