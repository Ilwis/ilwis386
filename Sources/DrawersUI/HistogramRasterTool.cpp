#include "Client\Headers\formelementspch.h"
#include "Client\Formelements\fldcolor.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\TableWindow\HistogramDoc.h"
#include "Client\TableWindow\HistogramGraphView.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\HistogramRasterTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createHistogramRasterTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new HistogramRasterTool(zv, view, drw);
}

HistogramRasterTool::HistogramRasterTool(ZoomableView* zv, LayerTreeView *tree, NewDrawer *drw) : DrawerTool("HistogramRasterTool", zv, tree, drw){
}

HTREEITEM HistogramRasterTool::configure( HTREEITEM parentItem){
	if ( !active)
		return parentItem;

	if ( isConfigured)
		return htiNode;

	MapCompositionDoc *mdoc = tree->GetDocument();
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(getDrawer());
	RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
	BaseMapPtr *bm = datadrw->getBaseMap();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);


	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)drawer->getParentDrawer();
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&HistogramRasterTool::displayOptionHisto);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&HistogramRasterTool::setHisto);
	htiNode = insertItem("Histogram","histo16", item,0); 
	DrawerTool::configure(htiNode);
	isConfigured = true;
	return htiNode;
}



void HistogramRasterTool::displayOptionHisto() {
	new HistogramRasterToolForm(tree, drawer);
}

void HistogramRasterTool::update() {
	BOOL status = tree->GetTreeCtrl().GetCheck(htiNode);
	tree->GetTreeCtrl().SetCheck(htiNode,!status); 
}

void HistogramRasterTool::setHisto(void *v, HTREEITEM) {
	bool use = *(bool *)v;
	MapCompositionDoc *mdoc = tree->GetDocument();
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(getDrawer());
	RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
	BaseMapPtr *bm = datadrw->getBaseMap();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);
	if ( use) {
		if ( !hview){
			mdoc->ShowHistogram(bm->fnObj);
		}
	} else {
		mdoc->ShowHistogram(bm->fnObj, false);
	}
}

bool HistogramRasterTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !layerDrawerTool )
		return false;
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(tool->getDrawer());
	if (!rdrw)
		return false;
	RangeReal rr = rdrw->getStretchRangeReal();
	if ( rr.fValid())
		parentTool = tool;
	return rr.fValid();
}

String HistogramRasterTool::getMenuString() const {
	return TR("Raster Histogram");
}

//------------------------------------
HistogramRasterToolForm::HistogramRasterToolForm(CWnd *wPar, NewDrawer *dr) : 
	DisplayOptionsForm((ComplexDrawer *)dr,wPar,"Select Histogram Options")
{
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(drw);
	RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
	BaseMapPtr *bm = datadrw->getBaseMap();
	MapCompositionDoc *mdoc = view->GetDocument();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);
	color = hview->getTresholdColor();
	spread = hview->getSpread() * 100;


	fcolor = new FieldColor(root,TR("Color"), &color);
	fspread = new FieldInt(root,TR("Spread"), &spread,ValueRange(1,100));

	create();
}


void  HistogramRasterToolForm::apply() {
	fcolor->StoreData();
	fspread->StoreData();
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(drw);
	RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
	BaseMapPtr *bm = datadrw->getBaseMap();

	MapCompositionDoc *mdoc = view->GetDocument();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);

	datadrw->setTresholdColor(color);
	hview->setTresholdColor(color);
	hview->setSpread(spread / 100.0);
	
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	datadrw->prepareChildDrawers(&pp);
	updateMapView();

}

