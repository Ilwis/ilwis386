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
#include "Drawers\AnimationDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\HistogramRasterTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Engine\Table\TBLHIST.H"
#include "SetDrawerTool.h"
#include "AnimationTool.h"
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

	//SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)drawer->getParentDrawer();
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&HistogramRasterTool::displayOptionHisto);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&HistogramRasterTool::setHisto);
	htiNode = insertItem("Histogram","histo16", item,0); 
	DrawerTool::configure(htiNode);
	isConfigured = true;
	setActive(false);
	return htiNode;
}


BaseMapPtr *HistogramRasterTool::get(int i) const{
	MapCompositionDoc *mdoc = tree->GetDocument();
	RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(getDrawer());
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(getDrawer());
	BaseMapPtr *bm = 0;
	if ( rdrw) {
		RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
		bm = datadrw->getBaseMap();
	} else {
		IlwisObjectPtr *ptr = adrw->getObject();
		MapListPtr *mptr = (MapListPtr *)ptr;
		bm = mptr->map(i).ptr();
	}
	return bm;
}
void HistogramRasterTool::displayOptionHisto() {
	MapCompositionDoc *mdoc = tree->GetDocument();
	BaseMapPtr *bm = get();
	if (!bm)
		return;
	if (!mdoc->getHistoView(bm->fnObj))
		return;

	new HistogramRasterToolForm(tree, drawer);
}

void HistogramRasterTool::update() {
	BOOL status = tree->GetTreeCtrl().GetCheck(htiNode);
	tree->GetTreeCtrl().SetCheck(htiNode,!status); 
}

void HistogramRasterTool::setHisto(void *v, HTREEITEM) {
	bool use = *(bool *)v;
	MapCompositionDoc *mdoc = tree->GetDocument();
	BaseMapPtr *bm = get();
	if ( !bm)
		return ;
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);
	
	if ( use) {
		if ( !hview){
			AnimationTool *animationTool = dynamic_cast<AnimationTool *>(parentTool);
			if ( animationTool) {
				AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(animationTool->getDrawer());
				if ( adrw){
					IlwisObjectPtr *ptr = adrw->getObject();
					MapListPtr *mptr = (MapListPtr *)ptr;
					for(int i=0; i < mptr->iSize(); ++i){
						TableHistogramInfo hinf(mptr->map(i));
						Column col = hinf.colNPix();
						if ( col.fValid()) {
							RangeReal rr = hinf.colNPix()->rrMinMax();
							if ( rr.fValid())
								rangeUsedY += rr;
						}
					}
				}
			}
		}
		mdoc->ShowHistogram(bm->fnObj,true, rangeUsedX, rangeUsedY);
	} else {
		AnimationTool *animationTool = dynamic_cast<AnimationTool *>(parentTool);
		if ( animationTool) {
			AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(animationTool->getDrawer());
			if ( adrw){
				bm = get(adrw->getCurrentIndex());
			}
		}
		mdoc->ShowHistogram(bm->fnObj, false);
	}
}

bool HistogramRasterTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	AnimationTool *animationTool = dynamic_cast<AnimationTool *>(tool);
	if ( !layerDrawerTool && !animationTool)
		return false;
	parentTool = tool;
	if ( layerDrawerTool) {
		RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(tool->getDrawer());
		if (!rdrw)
			return false;
		if (rdrw->getDrawMethod() == NewDrawer::drmCOLOR)
			return false; // HistogramRasterTool is not usable for color composites
		rangeUsedX = rdrw->getStretchRangeReal();
		return rangeUsedX.fValid();
	} else {
		AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(animationTool->getDrawer());
		rangeUsedX = adrw->getStretchRangeReal();
		return rangeUsedX.fValid();
	}
	return false;

}

String HistogramRasterTool::getMenuString() const {
	return TR("Raster Histogram");
}

//------------------------------------
HistogramRasterToolForm::HistogramRasterToolForm(CWnd *wPar, NewDrawer *dr) : 
	DisplayOptionsForm((ComplexDrawer *)dr,wPar,"Select Histogram Options")
{
	BaseMapPtr *bm;

	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( adrw) {
		bm = adrw->getBaseMap(adrw->getCurrentIndex());
	}else {
		RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(drw);
		RasterDataDrawer *datadrw = dynamic_cast<RasterDataDrawer *>(rdrw->getParentDrawer());
		bm = datadrw->getBaseMap();
	}
	MapCompositionDoc *mdoc = view->GetDocument();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);
	color = hview->getTresholdColor();
	color.alpha() = 255 - color.alpha();
	spread = hview->getSpread() * 100;


	fcolor = new FieldColor(root,TR("Color"), &color);
	fspread = new FieldInt(root,TR("Spread"), &spread,ValueRange(1,100));

	create();
}


void  HistogramRasterToolForm::apply() {
	fcolor->StoreData();
	fspread->StoreData();
	BaseMapPtr *bm;
	SpatialDataDrawer *datadrw;
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( adrw) {
		datadrw = adrw;
		bm = adrw->getBaseMap(adrw->getCurrentIndex());
	} else {
		RasterLayerDrawer *rdrw = dynamic_cast<RasterLayerDrawer *>(drw);
		datadrw = dynamic_cast<SpatialDataDrawer *>(rdrw->getParentDrawer());
		bm = datadrw->getBaseMap();
	}

	MapCompositionDoc *mdoc = view->GetDocument();
	HistogramGraphView *hview = mdoc->getHistoView(bm->fnObj);
	Color clr (color);
	clr.alpha() = 255 - clr.alpha();
	hview->setTresholdColor(clr);
	hview->setSpread(spread / 100.0);
	datadrw->setTresholdColor(clr);
	hview->setThresholdRange();
	
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	datadrw->prepareChildDrawers(&pp);
	updateMapView();

}

