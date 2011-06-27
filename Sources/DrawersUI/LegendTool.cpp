#include "Client\Headers\formelementspch.h"
#include "Engine\Base\Round.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "DrawersUI\RepresentationTool.h"
#include "DrawersUI\LegendTool.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"


DrawerTool *createLegendTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LegendTool(zv, view, drw);
}

LegendTool::LegendTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("LegendTool", zv, view, drw)
{
}

LegendTool::~LegendTool(){
}

HTREEITEM LegendTool::configure( HTREEITEM parentItem){

	String sName = SDCRemLegend;
	int iImgLeg = IlwWinApp()->iImage("legend");
	htiNode = tree->GetTreeCtrl().InsertItem(sName.scVal(), iImgLeg, iImgLeg, parentItem);

	update();
	tree->GetTreeCtrl().Expand(htiNode, TVE_EXPAND);
	
	return htiNode;
}

void LegendTool::insertLegendItemsValue(const Representation& rpr, const DomainValueRangeStruct& dvs){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	tree->GetTreeCtrl().SetItemData(htiNode, (DWORD_PTR)new ObjectLayerTreeItem(tree, rpr.pointer()));
	int iItems = 5;
	double rStep = dvs.rStep();
	RangeReal rr;
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	if ( sdrw) {
		rr = sdrw->getStretchRangeReal();
	} else {
		rr = ldrw->getStretchRangeReal(true);
	}

	if (rStep > 1e-6) {
		int iSteps = 1 + round(rr.rWidth() / rStep);
		if (iSteps < 2)
			iSteps = 2;
		if (iSteps <= 11)
			iItems = iSteps;
	}
	RangeReal rmd = roundRange(rr.rLo(), rr.rHi());
	double rVal = rRound(rmd.rWidth()/ iItems);
	double rStart = rRound(rmd.rLo());
	double rHi = rmd.rHi() + rVal;
	if (dvs.rValue(rHi) == rUNDEF)
		rHi = rmd.rHi();
	bool fImage = dvs.dm()->pdi();

	for (double v = rStart; v <= rHi; v += rVal) {
		String sName = dvs.sValue(v);
		HTREEITEM hti = tree->GetTreeCtrl().InsertItem(sName.scVal(), htiNode);
		if ( fImage && v + rVal > 255) {
			v = 255;
		}
		tree->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendValueLayerTreeItem(tree, drawer, dvs, v));		
	}
}

void LegendTool::insertLegendItemsClass(const Representation& rpr){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	LayerDrawer *setdr = (LayerDrawer *)drawer;
	tree->GetTreeCtrl().SetItemData(htiNode, (DWORD_PTR)new LegendLayerTreeItem(tree, setdr));		
	DomainClass* dc = rpr->dm()->pdc();
	if (!dc) // huh, seen it happen though, rprclass without a domain class
		return;
	int iItems = dc->iNettoSize();
	for (int i = 1; i <= iItems; ++i) {
		int iRaw = dc->iKey(i);
		String sName = dc->sValueByRaw(iRaw, 0);
		HTREEITEM hti = tree->GetTreeCtrl().InsertItem(sName.scVal(), htiNode);
		tree->GetTreeCtrl().SetCheck(hti);
		tree->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(tree, drawer, rpr->dm(), iRaw));		
	}
}

void LegendTool::update() {
	SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer); // case animation drawer
	if ( !mapDrawer)
		mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());

	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();

	LayerDrawer *sdrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	if ( adrw) {
		sdrw = (LayerDrawer *)adrw->getDrawer(0);
	}

	if ( sdrw->useAttributeColumn() && sdrw->getAtttributeColumn().fValid()) {
		dvs = sdrw->getAtttributeColumn()->dvrs();
	}
	Representation rpr = sdrw->getRepresentation();
	if ( rpr->prg() || rpr->prv())
		insertLegendItemsValue(rpr, dvs);
	else if ( rpr->prc()) {
		insertLegendItemsClass(rpr);
	}
}

bool LegendTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
  bool isAcceptable =  dynamic_cast<RepresentationTool *>(tool) != 0;
  if ( isAcceptable)
	  parentTool = tool;
  return isAcceptable;
}

String LegendTool::getMenuString() const {
	return TR("Legend");
}
