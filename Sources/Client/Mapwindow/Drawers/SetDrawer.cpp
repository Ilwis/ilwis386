#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

SetDrawer::SetDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	stretched(false),
	stretchMethod(smLINEAR),
	drawColor(0),
	colorCheck(0),
	rprItem(0),
	itemTransparent(0)
{
	setInfo(true);
}

SetDrawer::~SetDrawer() {
	delete colorCheck;
	delete drawColor;
}

void SetDrawer::prepare(PreparationParameters *parm){
	delete drawColor;
	delete colorCheck;
	drawColor = 0;
	colorCheck = 0;
	ComplexDrawer::prepare(parm);
	csy = parm->csy;
	if ( getUICode() == NewDrawer::ucALL) {
		AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
		rpr = mapDrawer->getRepresentation();
		if ( rpr.fValid() && !rpr->prv())
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
	}
	drawColor = new DrawingColor(this);
	setDrawMethod();
}

HTREEITEM SetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = ComplexDrawer::configure(tv,parent);

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&SetDrawer::displayOptionTransparency);
	String transp("Transparency (%d)", 100 * getTransparency());
	itemTransparent = InsertItem(transp,"Transparent", item, -1);

	NewDrawer::DrawMethod method = getDrawMethod();
	portrayalItem = InsertItem(tv,parent, "Portrayal", "Colors");
	
	colorCheck = new SetChecks(tv,this,(SetCheckFunc)&SetDrawer::setcheckRpr);
	if ( rpr.fValid() ) {
		bool usesRpr = method == NewDrawer::drmRPR;
		item = new DisplayOptionTreeItem(tv,portrayalItem,this,(DisplayOptionItemFunc)&SetDrawer::displayOptionSubRpr,0,colorCheck);
		rprItem = InsertItem("Representation", ".rpr", item, (int)usesRpr);

		if ( usesRpr)
			InsertItem(tv, rprItem,String("Value : %S",rpr->sName()),".rpr");

	}
	return hti;
}

void SetDrawer::insertStretchItem(LayerTreeView  *tv, HTREEITEM parent) {
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	Column attColumn = mapDrawer->getAtttributeColumn();
	if ( rpr->prg()) {
		if ( rpr->dm()->pdv() || (attColumn.fValid() && attColumn->dm()->pdv())) {
			DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&SetDrawer::displayOptionStretch);
			HTREEITEM itemStretch = InsertItem("Stretch","Valuerange", item,-1); 
			RangeReal rr = getStretchRangeReal();
			if ( rr.fValid()) {
				InsertItem(tv,itemStretch, String("Lower : %f",rr.rLo()), "Calculationsingle");
				InsertItem(tv,itemStretch, String("Upper : %f",rr.rHi()), "Calculationsingle");
			} else if ( getStretchRangeInt().fValid()) {
				RangeInt ri = getStretchRangeInt();
				InsertItem(tv,itemStretch, String("Lower : %d",ri.iLo()), "Calculationsingle");
				InsertItem(tv,itemStretch, String("Upper : %d",ri.iHi()), "Calculationsingle");
			}
		}
	}
}

bool SetDrawer::useInternalDomain() const {
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	if ( !mapDrawer)
		throw ErrorObject("Parent drawer not set");
	return mapDrawer->useInternalDomain();
}

void SetDrawer::setcheckRpr(void *value, LayerTreeView *tree) {
	if ( value == 0)
		return;
	HTREEITEM hItem = *((HTREEITEM *)value);
	String name = tree->getItemName(hItem);
	if ( name == sUNDEF)
		return;
	int index = name.find_last_of("|");

	if ( index == string::npos)
		return;

	String method = name.substr(index + 1);
	if ( method == "Representation")
		setDrawMethod(NewDrawer::drmRPR);
	else if ( method == "Single Color")
		setDrawMethod(NewDrawer::drmSINGLE);
	else if ( method == "Multiple Colors"){
		setDrawMethod(NewDrawer::drmMULTIPLE);
	}
	modifyLineStyleItem(tree, (getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	prepareChildDrawers(&pp);
	MapCompositionDoc* doc = tree->GetDocument();
	doc->mpvGetView()->Invalidate();
}

void SetDrawer::setActiveMode(void *v,LayerTreeView *tv) {
	bool value = *(bool *)v;
	setActive(value);
	MapCompositionDoc* doc = tv->GetDocument();
	doc->mpvGetView()->Invalidate();

}
Representation SetDrawer::getRepresentation() const {
	return rpr;
}

void SetDrawer::setRepresentation(const Representation& rp){
	rpr = rp;
	stretched = false;
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	if ( rpr.fValid() && !rpr->prv())
		setStretchRangeReal(mapDrawer->getStretchRangeReal());
}

CoordSystem SetDrawer::getCoordSystem() const {
	return csy;
}

bool SetDrawer::isLegendUsefull() const {
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	BaseMap bm = mapDrawer->getBaseMap();
	if (bm.fValid() && bm->dm()->pdv() && stretched) 
		return true;
	return drm != drmSINGLE;
}

bool SetDrawer::isStretched() const {
	return stretched;
}

RangeReal SetDrawer::getStretchRangeReal() const{
	return rrStretch;
}

void SetDrawer::setStretchRangeReal(const RangeReal& rr){
	if ( rr.fValid())
		stretched = true;
	rrStretch = rr;
}

RangeInt SetDrawer::getStretchRangeInt() const{
	return riStretch;
}

void SetDrawer::setStretchRangeInt(const RangeInt& ri){
	if ( ri != riStretch && ri.fValid())
		stretched = true;
	riStretch = ri;
}

RangeReal SetDrawer::getLegendRange() const{
	return rrLegendRange;
}

void SetDrawer::setLegendRange(const RangeReal& rr){
	rrLegendRange = rr;
}

SetDrawer::StretchMethod SetDrawer::getStretchMethod() const{
	return stretchMethod;
}
void SetDrawer::setStretchMethod(StretchMethod sm){
	stretchMethod = sm;
}

ILWIS::DrawingColor * SetDrawer::getDrawingColor() const {
	return drawColor;
}

void SetDrawer::displayOptionSubRpr(CWnd *parent) {
	new RepresentationForm(parent, this);
}

void SetDrawer::displayOptionStretch(CWnd *parent) {
	new SetStretchForm(parent, this);
}

void SetDrawer::displayOptionTransparency(CWnd *parent) {
	new TransparencyForm(parent, this);
}

//--------------------------------
RepresentationForm::RepresentationForm(CWnd *wPar, SetDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Set Representation"),
	rpr(dr->getRepresentation()->sName())
{
	//fldRpr = new FieldRepresentation(root, "Representation", &rpr, dr->getRepresentation()->dm());
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

void  RepresentationForm::apply() {
	fldRpr->StoreData();
	SetDrawer *setDrawer = (SetDrawer *)drw;
	setDrawer->setRepresentation(Representation(FileName(rpr)));
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);
	updateMapView();

	HTREEITEM child = view->GetTreeCtrl().GetNextItem(setDrawer->rprItem, TVGN_CHILD);
	if ( child) {
		FileName fn(rpr);
		String name("Value : %S",fn.sFile);
		TreeItem titem;
		view->getItem(child,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);
	
		strcpy(titem.item.pszText,name.scVal());
		view->GetTreeCtrl().SetItem(&titem.item);
	}
	Representation setRpr = Representation(FileName(rpr));
	HTREEITEM parent = view->getAncestor(setDrawer->rprItem, 2);
	if ( parent) {
		HTREEITEM stretchItem = setDrawer->findTreeItemByName(view,parent,"Stretch");
		if ( !stretchItem && setRpr->prg()) {
			setDrawer->insertStretchItem(view,parent);
		}
		if ( stretchItem && setRpr->prv()) {
			view->GetTreeCtrl().DeleteItem(stretchItem);
		}
		view->collectStructure();
	}

}
//------------------------------------
SetStretchForm::SetStretchForm(CWnd *wPar, SetDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Set stretch"),
	rr(dr->getStretchRangeReal()),
	low(dr->getStretchRangeReal().rLo()),
	high(dr->getStretchRangeReal().rHi())

{
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rr),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rr),true);
	sliderHigh->Align(sliderLow, AL_UNDER, -25);
	sliderLow->SetCallBack((NotifyProc)&SetStretchForm::check);
	sliderHigh->SetCallBack((NotifyProc)&SetStretchForm::check);
	create();
}

int  SetStretchForm::check(Event *) {
	sliderLow->StoreData();
	sliderHigh->StoreData();

	if ( low > high){
		low = high;
		sliderLow->SetVal(low);
	}
	if ( high < low){
		high = low;
		sliderHigh->SetVal(high);
	}
	return 1;
}

void  SetStretchForm::apply() {
	sliderLow->StoreData();
	sliderHigh->StoreData();

	((SetDrawer *)drw)->setStretchRangeReal(RangeReal(low,high));

	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);
	updateMapView();
}

//--------------------------------
TransparencyForm::TransparencyForm(CWnd *wPar, SetDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Transparency"),
	transparency(100 *(1.0-dr->getTransparency()))
{
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	create();
}

void  TransparencyForm::apply() {
	slider->StoreData();
	((SetDrawer *)drw)->setTransparency(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);
	String transp("Transparency (%d)",transparency);
	TreeItem titem;
	view->getItem(((SetDrawer *)drw)->itemTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);
	
	strcpy(titem.item.pszText,transp.scVal());
	view->GetTreeCtrl().SetItem(&titem.item);
	updateMapView();

}






