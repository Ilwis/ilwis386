#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\ColorCompositeTool.h"
#include "DrawersUI\LayerDrawerTool.h"

using namespace ILWIS;

DrawerTool *createColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ColorCompositeTool(zv, view, drw);
}

ColorCompositeTool::ColorCompositeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("ColorCompositeTool",zv, view, drw)
{
}

ColorCompositeTool::~ColorCompositeTool() {
}

void ColorCompositeTool::clear() {
}

bool ColorCompositeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *sdrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !sdrwt)
		return false;
	RasterLayerDrawer *rdrw = 0;
	if ( (rdrw = dynamic_cast<RasterLayerDrawer *>(tool->getDrawer())) == 0)
		return false;
	if ( !rdrw->isColorComposite())
		return false;
	parentTool = tool;

	return true;
}

HTREEITEM ColorCompositeTool::configure( HTREEITEM parentItem) {
	RasterLayerDrawer *rdrw = (RasterLayerDrawer *)drawer;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)(DisplayOptionItemFunc)&ColorCompositeTool::displayOptionCC);
	htiNode = insertItem(TR("Color Composite"),"ColorComposite",item);
	MapList mpl = rdrw->getMapList();
	if ( mpl.fValid()) {
		for(int i=0 ; i < 3; ++i) {
			item = new DisplayOptionTreeItem(tree, htiNode, drawer);
			Map mp = mpl[rdrw->getColorCompositeBand(i)];
			insertItem(String(TR("Map %d: %S").c_str(),i, mp->fnObj.sFile),".mpr", item);

		}
	}

	DrawerTool::configure(htiNode);

	return htiNode;
}

void ColorCompositeTool::displayOptionCC() {
	new SetBandsForm(tree,(RasterLayerDrawer *)getDrawer());
}

String ColorCompositeTool::getMenuString() const {
	return TR("Color Composite");
}

//------------------------------------------------
SetBandsForm::SetBandsForm(CWnd *wPar, RasterLayerDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,TR("Select bands for Color Composite"))
{
	MapList mpl = dr->getMapList();
	band1 = mpl[dr->getColorCompositeBand(0)]->fnObj.sFile;
	band2 = mpl[dr->getColorCompositeBand(1)]->fnObj.sFile;
	band3 = mpl[dr->getColorCompositeBand(2)]->fnObj.sFile;

	fm1 = new FieldMap(root,TR("Band 1"),&band1);
	fm2 = new FieldMap(root,TR("Band 2"),&band2);
	fm3 = new FieldMap(root,TR("Band 3"),&band3);

	create();
}

void  SetBandsForm::apply() {
	fm1->StoreData();
	fm2->StoreData();
	fm3->StoreData();
	RasterLayerDrawer *rdr = (RasterLayerDrawer *)drw;
	MapList mpl = rdr->getMapList();
	for(int i = 0; i < mpl->iSize(); ++i) {
		if ( mpl[i]->fnObj.sFile == band1) {
			rdr->setColorCompositeBand(0,i);

		}
		if ( mpl[i]->fnObj.sFile == band2) {
			rdr->setColorCompositeBand(1,i);

		}
		if ( mpl[i]->fnObj.sFile == band3) {
			rdr->setColorCompositeBand(2,i);

		}

	}
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	rdr->prepareChildDrawers(&pp);

	updateMapView();
}

