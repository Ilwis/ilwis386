#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Base\Round.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
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
#include "Drawers\LineDrawer.h"
#include "Drawers\PolygonDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PointLayerDrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "DrawersUI\RepresentationTool.h"
#include "DrawersUI\LegendTool.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Base\System\Engine.h"
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
	//htiNode = tree->GetTreeCtrl().InsertItem(sName.c_str(), iImgLeg, iImgLeg, parentItem);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&LegendTool::displayOptionLegend);
	htiNode = insertItem("legend","Legend", item); 
	update();
	DrawerTool::configure(htiNode);
	tree->GetTreeCtrl().Expand(htiNode, TVE_EXPAND);
		
	return htiNode;
}

void LegendTool::displayOptionLegend() {
	new LegendValueForm(tree, (LayerDrawer *)drawer, this, vrr,step );
}

void LegendTool::insertLegendItemsValue(const Representation& rpr){
	if ( htiNode) {
		tree->DeleteAllItems(htiNode, true);
	}
	//tree->GetTreeCtrl().SetItemData(htiNode, (DWORD_PTR)new ObjectLayerTreeItem(tree, rpr.pointer()));
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(drawer);
	if ( sdrw) {
		vrr = sdrw->getStretchRangeReal();
	} else {
		vrr = ldrw->getStretchRangeReal(true);
	}

	step = dvrs.rStep();

	addValueItems();

}

void LegendTool::addValueItems(bool force) {
	bool fImage = false;
	if (!force) {
		bool fminLimit = vrr.rLo() == 1 || vrr.rLo() == 0;
		bool fhilimit = vrr.rHi() == 255;
		fImage = dvrs.dm()->pdi() || (  fminLimit && fhilimit );
		if ( fImage) {
			vrr = RangeReal(0,255);
			step = 30;
		} else
			vrr = roundRange(vrr.rLo(), vrr.rHi(), step);
	} 

	double domstep = dvrs.rStep();
	bool isInteger = abs(domstep - (int)domstep) < 0.0000001;
	int count = vrr.rWidth() / step;
	vector<double> values;
	for (double v = vrr.rLo(); v <= vrr.rHi(); v += step) {
		if ( fImage && v + step > 255) {
			values.push_back(255);
		} else {
			values.push_back(v);
		}
	}

	for (double v = vrr.rHi(); v >= vrr.rLo(); v -= step, --count) {
		String sName = dvrs.sValue(v);

		double value = values[count];
		if ( count == 0) {
			int w = isInteger ? 0: -1;
			sName = dvrs.sValue(vrr.rLo(),w,w);
			value = vrr.rLo();
		}
		LegendValueLayerTreeItem *it = new LegendValueLayerTreeItem(tree, htiNode, drawer, dvrs, value);
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
	if ( drawerType == dtSEGMENT)
		new LineRprForm(tree, ldr,ldr->getRepresentation()->prc(),it->raw());
	else if ( drawerType == dtPOLYGON) {
		new PolRprForm(tree,ldr,ldr->getRepresentation()->prc(),it->raw());
	} else if ( drawerType == dtPOINT) {
		new PointRprForm(tree,ldr,ldr->getRepresentation()->prc(),it->raw());
	}
}

void LegendTool::update() {
	SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer); // case animation drawer
	if ( !mapDrawer)
		mapDrawer = dynamic_cast<SpatialDataDrawer *>(drawer->getParentDrawer());

	dvrs = mapDrawer->getBaseMap()->dvrs();
	LayerDrawer *layerDrawer = dynamic_cast<LayerDrawer *>(drawer);
	SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	if ( adrw) {
		layerDrawer = (LayerDrawer *)adrw->getDrawer(0);
	}
	if ( dynamic_cast<PointLayerDrawer *>(layerDrawer)) {
		drawerType = dtPOINT;
	} else if (dynamic_cast<LineLayerDrawer *>(layerDrawer)){
		drawerType = dtSEGMENT;
	} else if ( dynamic_cast<PolygonLayerDrawer *>(layerDrawer)) {
		drawerType = dtPOLYGON;
	}

	if ( layerDrawer->useAttributeColumn() && layerDrawer->getAtttributeColumn().fValid()) {
		dvrs = layerDrawer->getAtttributeColumn()->dvrs();
	}
	Representation rpr = layerDrawer->getRepresentation();
	if ( rpr->prg() || rpr->prv())
		insertLegendItemsValue(rpr);
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
    ((LayerDrawer *)drw)->setUseRpr(true);
  PreparationParameters pp(NewDrawer::ptRENDER, 0);
  drw->prepare(&pp);

  updateMapView();
}

//--------------------------------------------------
LegendValueForm::LegendValueForm(CWnd *wPar, LayerDrawer *dr, LegendTool *tl, RangeReal& rnge, double& step) : 
DisplayOptionsForm(dr,wPar,TR("Value Representation")), range(rnge),rstep(step), tool(tl)
{
	rmin = range.rLo();
	rmax = range.rHi();
	fmin = new FieldReal(root,"Min",&rmin);
	fmax = new FieldReal(root,"Max", &rmax);
	fmax->Align(fmin, AL_UNDER);
	fstep = new FieldReal(root,"Step",&rstep);
  create();
}

void LegendValueForm::apply() {
	fmin->StoreData();
	fmax->StoreData();
	fstep->StoreData();
	range.rHi() = rmax;
	range.rLo() = rmin;
	view->DeleteAllItems(tool->getTreeItem(), true);
	tool->addValueItems(true);
	
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
  col2 = rcl->clrSecondRaw(iRaw);

  String base = getEngine()->getContext()->sIlwDir();
  base += "Resources\\Symbols\\";
  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  new FieldColor(root,"Area Color",&col);
  FieldIntSliderEx *slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
  slider->SetCallBack((NotifyProc)&PolRprForm::setTransparency);
  slider->setContinuous(true);
  hatching = rcl->sHatch(raw);
  if ( hatching == sUNDEF)
	  hatching = "none";
  new FieldDataType(root,TR("Hatching"),&hatching,".ivh",false,0,FileName(base),false);
  FieldColor *fc = new FieldColor(root,"Hatching Background Color",&col2, true);
  fc->SetBevelStyle(FormEntry::bsLOWERED);

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
  rcl->PutSecondColor(iRaw, col2);
  rcl->PutTransparency(iRaw,transparency/ 100.0);
  if ( hatching != ""){
	  hatching = hatching.sTrimSpaces();
	  FileName fn(hatching);
	rcl->PutHatchingName(iRaw, fn.sFile);
	rcl->Updated();
  }
    ((LayerDrawer *)drw)->setUseRpr(true);
  PreparationParameters pp(NewDrawer::ptRENDER, 0);
  RepresentationProperties props;
  pp.props = props;
  drw->prepare(&pp);
  view->Invalidate();
  updateMapView();
}

//---------------------------------------------------
PointRprForm::PointRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw) : 
DisplayOptionsForm(dr,wPar,TR("Point Representation")),rcl(rc), iRaw(raw)
{
   String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);

    symbol = rc->sSymbolType(raw);
    scale = rc->iSymbolSize(raw) / 100.0;
	col = rc->clrSymbol(raw);
    StaticText* st = new StaticText(root, sText);
    st->SetIndependentPos();
	String base = getEngine()->getContext()->sIlwDir();
	base += "Resources\\Symbols\\";
	new FieldDataType(root,TR("Symbols"),&symbol,".ivg",false,0,FileName(base),false);
	new FieldColor(root,TR("Symbol Color"),&col);
	new FieldReal(root,TR("Symbol scale"),&scale,ValueRange(RangeReal(0.1,100.0),0.1));

  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_Pointygon.htm");
  create();
}

void  PointRprForm::apply() {
  root->StoreData();
  rcl->PutColor(iRaw, col);
  rcl->PutSymbolColor(iRaw, col);
  rcl->PutSymbolSize(iRaw,scale * 100);
  FileName fn(symbol);
  rcl->PutSymbolType(iRaw, fn.sFile);
  PreparationParameters pp(NewDrawer::ptRENDER, 0);
  ((LayerDrawer *)drw)->setUseRpr(true);
 // pp.props.symbolType = fn.sFile;
  //pp.props.symbolSize = scale * 100.0;
  drw->prepare(&pp);
  view->Invalidate();
  updateMapView();
}