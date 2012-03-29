#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
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
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "DrawersUI\RepresentationTool.h"
#include "DrawersUI\LegendTool.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Representation\Rprclass.h"
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

	String sName = TR("Legend");
	int iImgLeg = IlwWinApp()->iImage("legend");
	htiNode = tree->GetTreeCtrl().InsertItem(sName.c_str(), iImgLeg, iImgLeg, parentItem);

	update();
	tree->GetTreeCtrl().Expand(htiNode, TVE_EXPAND);
	
	return htiNode;
}

void LegendTool::insertLegendItemsValue(const Representation& rpr, const DomainValueRangeStruct& dvs){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	tree->GetTreeCtrl().SetItemData(htiNode, (DWORD_PTR)new ObjectLayerTreeItem(tree, rpr.pointer()));
	RangeReal rr;
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	if ( sdrw) {
		rr = sdrw->getStretchRangeReal();
	} else {
		rr = ldrw->getStretchRangeReal(true);
	}

	double rStep = 1.0;
	RangeReal rmd;
	bool fImage = dvs.dm()->pdi();
	if ( fImage) {
		rmd = RangeReal(0,255);
		rStep = 30;
	} else
		rmd = roundRange(rr.rLo(), rr.rHi(), rStep);

	for (double v = rmd.rLo(); v <= rmd.rHi(); v += rStep) {
		String sName = dvs.sValue(v);
		//HTREEITEM hti = tree->GetTreeCtrl().InsertItem(sName.c_str(), htiNode);
		if ( fImage && v + rStep > 255) {
			v = 255;
		}
		LegendValueLayerTreeItem *it = new LegendValueLayerTreeItem(tree, htiNode, drawer, dvs, v);
		insertItem(sName,"",it);
		//tree->GetTreeCtrl().SetItemData(hti, (DWORD_PTR));		
	}
}

void LegendTool::insertLegendItemsClass(const Representation& rpr){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	LayerDrawer *layerDrawer = (LayerDrawer *)drawer;
	tree->GetTreeCtrl().SetItemData(htiNode, (DWORD_PTR)new LegendLayerTreeItem(tree, layerDrawer));		
	DomainClass* dc = rpr->dm()->pdc();
	if (!dc) // huh, seen it happen though, rprclass without a domain class
		return;
	int iItems = dc->iNettoSize();
	for (int i = 1; i <= iItems; ++i) {
		int iRaw = dc->iKey(i);
		String sName = dc->sValueByRaw(iRaw, 0);
		Column col;
		if ( layerDrawer->useAttributeColumn() && layerDrawer->getAtttributeColumn().fValid())
			col = layerDrawer->getAtttributeColumn();
		LegendClassLayerTreeItem *it = new LegendClassLayerTreeItem(tree, htiNode, drawer, rpr->dm(), iRaw, col);
		it->setDoubleCickAction(this,(DTDoubleClickActionFunc)&LegendTool::displayOptionRprClass);
		insertItem(sName,"",it,1);
	}
}

void LegendTool::displayOptionRprClass() {
	LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(drawer);
	LegendClassLayerTreeItem *it = (LegendClassLayerTreeItem *)tree->getCurrent();
	if ( mapType == IlwisObject::iotSEGMENTMAP)
		new LineRprForm(tree, ldr,ldr->getRepresentation()->prc(),it->raw());
	else if ( mapType == IlwisObject::iotPOLYGONMAP) {
		new PolRprForm(tree,ldr,ldr->getRepresentation()->prc(),it->raw());
	} else if ( mapType == IlwisObject::iotPOINTMAP) {
	}
}

void LegendTool::update() {
	SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer); // case animation drawer
	if ( !mapDrawer)
		mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());

	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();
	mapType = IOTYPE(mapDrawer->getBaseMap()->fnObj);
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	if ( adrw) {
		layerDrawer = (LayerDrawer *)adrw->getDrawer(0);
	}

	if ( layerDrawer->useAttributeColumn() && layerDrawer->getAtttributeColumn().fValid()) {
		dvs = layerDrawer->getAtttributeColumn()->dvrs();
	}
	Representation rpr = layerDrawer->getRepresentation();
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

//---------------------------------------------------
LineRprForm::LineRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw) : 
DisplayOptionsForm(dr,wPar,TR("Line Representation")),rcl(rc), iRaw(raw), line(rc, raw)
{
   String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);

  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  new FieldColor(root,"Line Color",&col);
  new FieldBlank(root, 0.3);
  line.ResizeSymbol(1.0/3.0);
  new FieldLine(root, &line);

  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_segment.htm");
  create();
}

int LineRprForm::CustomColor(Event*)
{
  //cs->CustomColor(0);
  return 0;
}

void  LineRprForm::apply() {
  root->StoreData();
  line.ResizeSymbol(3.0);
  line.clrLine() = col;
  line.Store(rcl, iRaw);
  PreparationParameters pp(NewDrawer::ptRENDER, 0);
  drw->prepare(&pp);

  updateMapView();
}


//---------------------------------------------------
PolRprForm::PolRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw) : 
DisplayOptionsForm(dr,wPar,TR("Polygon Representation")),rcl(rc), iRaw(raw)
{
   String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);

  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  new FieldColor(root,"Area Color",&col);

  FieldIntSliderEx *slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
  slider->SetCallBack((NotifyProc)&PolRprForm::setTransparency);
  slider->setContinuous(true);

  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_polygon.htm");
  create();
}

int PolRprForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  PolRprForm::apply() {
  root->StoreData();
  rcl->PutColor(iRaw, col);
  rcl->PutTransparency(iRaw,1.0 - transparency/ 100.0);
  PreparationParameters pp(NewDrawer::ptRENDER, 0);
  drw->prepare(&pp);
  view->Invalidate();
  updateMapView();
}