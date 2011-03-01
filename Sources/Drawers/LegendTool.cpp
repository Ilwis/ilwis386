#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\RepresentationTool.h"
#include "Drawers\LegendTool.h"
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
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drawer);
	if ( adrw) {
		rr = adrw->getStretchRangeReal();
	} else
		rr = sdrw->getStretchRangeReal();

	if (rStep > 1e-6) {
		int iSteps = 1 + round(rr.rWidth() / rStep);
		if (iSteps < 2)
			iSteps = 2;
		if (iSteps <= 11)
			iItems = iSteps;
	}
	for (int i = 0; i < iItems; ++i) {
		double rMaxItem = iItems - 1;
		double rVal = rr.rLo() + i / rMaxItem * rr.rWidth();
		String sName = dvs.sValue(rVal, 0);
		HTREEITEM hti = tree->GetTreeCtrl().InsertItem(sName.scVal(), htiNode);
		tree->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendValueLayerTreeItem(tree, drawer, dvs, rVal));		
	}
}

void LegendTool::insertLegendItemsClass(const Representation& rpr){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	SetDrawer *setdr = (SetDrawer *)drawer;
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
	AbstractMapDrawer *mapDrawer = dynamic_cast<AbstractMapDrawer *>(drawer); // case animation drawer
	if ( !mapDrawer)
		mapDrawer = dynamic_cast<AbstractMapDrawer *>(drawer->getParentDrawer());

	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();

	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	AnimationDrawer *adrw = dynamic_cast<AnimationDrawer *>(drawer);
	if ( adrw) {
		sdrw = (SetDrawer *)adrw->getDrawer(0);
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
