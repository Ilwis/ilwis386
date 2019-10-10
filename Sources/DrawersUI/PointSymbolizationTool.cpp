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
	SpatialDataDrawer *sdr = dynamic_cast<SpatialDataDrawer *>(dr->getParentDrawer());
	if ( sdr) {
		BaseMapPtr *bmp = sdr->getBaseMap();
		if ( bmp->dm()->pdc())
			names.push_back("default");
	}
	for(map<String, IVGElement *>::iterator cur = loader->begin(); cur != loader->end(); ++cur) {
		if ( (*cur).second->getType() == IVGElement::ivgPOINT) {
			String name = (*cur).first;
			name = name.sHead("|");
			names.push_back(name);
		}
	}
	String base = getEngine()->getContext()->sIlwDir();
	base += "Resources\\Symbols\\";
	if (dr->isSet())
		props = (PointProperties *)dr->getDrawer(0)->getProperties();
	else
		props = (PointProperties *)dr->getProperties();
	name = props->symbol == "" ? DEFAULT_POINT_SYMBOL_TYPE : props->symbol;
	scale = props->scale / 100.0;
	//fdSelect = new FieldDataType(root,TR("Symbols"),&name,".ivg",false,0,FileName(base),false);
	fselect = new FieldOneSelectString(root,TR("Symbols"),&selection, names);
	fiThick = new FieldReal(root,TR("Line thickness"),&(props->thickness));
	frScale = new FieldReal(root,TR("Symbol scale"),&scale,ValueRange(RangeReal(0.0,100.0),0.0));
	frRot = new FieldReal(root,TR("Symbol rotation"),&(props->angle),ValueRange(RangeReal(0.0,360.0),0.1));
	t3dOr = props->threeDOrientation ? 1 : 0;
	f3d = new CheckBox(root,TR("3D orientation"),&t3dOr);
	create();
	fselect->SetVal(name);
}

void PointSymbolizationForm::apply(){
	fselect->StoreData();
	fiThick->StoreData();
	frScale->StoreData();
	f3d->StoreData();
	frRot->StoreData();

	SVGLoader *loader = NewDrawer::getSvgLoader();
	name = names[selection];
	SVGLoader::const_iterator cur = loader->find(name);
	if ( cur == loader->end()) {
		loader->getSVGSymbol(name);
	}

	FileName fn(name);
	props->symbol = fn.sFile;
	props->threeDOrientation = t3dOr != 0;
	PreparationParameters pp(NewDrawer::ptRENDER);
	RepresentationProperties rprop;
	rprop.symbolSize = props->scale = scale * 100;
	rprop.symbolType = props->symbol;
	pp.props = rprop;
	if (drw->isSet()) {
		for(int i = 0; i < drw->getDrawerCount(); ++i) {
			PointLayerDrawer *psdrw = (PointLayerDrawer *) (drw->getDrawer(i));
			psdrw->setUseRpr(props->symbol == "default");
			PointProperties *oldprops = (PointProperties *)psdrw->getProperties();
			oldprops->set(props);
			psdrw->prepareChildDrawers(&pp);
		}
	} else {
		((PointLayerDrawer *)drw)->setUseRpr(props->symbol == "default");
		drw->prepare(&pp);
	}

	updateMapView();
}

