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
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

unsigned long SetDrawer::test_count = 0;

SetDrawer::SetDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	stretched(false),
	stretchMethod(smLINEAR),
	drawColor(0),
	colorCheck(0),
	rprItem(0),
	threeDItem(0),
	portrayalItem(0)
{
	setInfo(true);
	setTransparency(1);
}

SetDrawer::~SetDrawer() {
	delete colorCheck;
	delete drawColor;
}

void SetDrawer::prepare(PreparationParameters *parm){
	//delete drawColor; // the drawColor is still in use elsewhere (a local pointer is kept for efficiency). Unless there's a good reason to fully renew it here, I try commenting this out for now.
	//drawColor = 0;
	ComplexDrawer::prepare(parm);
	if ( parm->type & NewDrawer::ptGEOMETRY ||  parm->type & NewDrawer::ptRESTORE) {
		csy = parm->csy;
		AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
		Representation rpr = mapDrawer->getRepresentation();
		//if ( rpr.fValid() && !rpr->prv())
		if ( rpr.fValid() && !rpr->prv())
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		if (!drawColor)
			drawColor = new DrawingColor(this);
	}
	setDrawMethod();
}

bool SetDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*CoordBounds cb = getRootDrawer()->getCoordBoundsZoom();
	RowCol rc1= getRootDrawer()->worldToScreen(cb.cMin);
	RowCol rc2= getRootDrawer()->worldToScreen(cb.cMax);*/
	glEnable(GL_BLEND);
	//glEnable(GL_SCISSOR_TEST);
	//glScissor(rc1.Col, rc2.Row, abs(rc1.Col - rc2.Col), abs(rc2.Col - rc2.Col));
	ComplexDrawer::draw(norecursion, cbArea);
	glDisable(GL_BLEND);
	//glDisable(GL_SCISSOR_TEST);
	return true;
}

void SetDrawer::SetthreeD(void *v, LayerTreeView *tv) {
	bool value = *(bool *)v;
	getZMaker()->setThreeDPossible(value);
	getRootDrawer()->getDrawerContext()->doDraw();
}

String SetDrawer::iconName(const String& subtype) const 
{ 
	return "Set";
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
	getRootDrawer()->getDrawerContext()->doDraw();
}

Representation SetDrawer::getRepresentation() const { // avoiding copy constructotrs
	return rpr;
}

void SetDrawer::setRepresentation( const Representation& rp){
	rpr = rp;
	stretched = false;
	setDrawMethod(NewDrawer::drmRPR);
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	if ( rpr.fValid() && mapDrawer->getBaseMap()->dm()->pdv())
		setStretchRangeReal(mapDrawer->getStretchRangeReal());
}

CoordSystem SetDrawer::getCoordSystem() const {
	return csy;
}

bool SetDrawer::isLegendUsefull() const {
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	BaseMapPtr *bm = mapDrawer->getBaseMap();
	if (bm != 0 && bm->dm()->pdv() && stretched) 
		return true;
	return drm != drmSINGLE;
}

bool SetDrawer::isStretched() const {
	return stretched;
}

RangeReal SetDrawer::getStretchRangeReal() const{
	return rrStretch;
}

String SetDrawer::getInfo(const Coord& c) const {
	if ( !hasInfo() || !isActive())
		return "";
	Coord crd = c;
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	BaseMapPtr *bmptr = mapDrawer->getBaseMap();
	if (bmptr->cs() != rootDrawer->getCoordinateSystem())
	{
		crd = bmptr->cs()->cConv(rootDrawer->getCoordinateSystem(), c);
	}
	vector<String> infos = bmptr->vsValue(crd);
	String info;
	DomainValue* dv = bmptr->dm()->pdv();
	for(int i = 0; i < infos.size(); ++i) {
		String s = infos[i].sTrimSpaces();
		if ( s == "?")
			continue;
		if (0 != dv && dv->fUnit())
			s = String("%S %S", s, dv->sUnit());
		info += i == 0 ? s : ";" + s;
	}
	return info;
}

void SetDrawer::setStretchRangeReal(const RangeReal& rr){
	if ( rr.fValid())
		stretched = true;
	rrStretch = rr;
	riStretch.iLo() = (long)(rounding(rrStretch.rLo()));
	riStretch.iHi() = (long)(rounding(rrStretch.rHi()));
	//if ( drawColor) {
	//	drawColor->
	//}
}

RangeInt SetDrawer::getStretchRangeInt() const{
	return riStretch;
}

void SetDrawer::setStretchRangeInt(const RangeInt& ri){
	if ( ri != riStretch && ri.fValid())
		stretched = true;
	riStretch = ri;
	rrStretch.rLo() = doubleConv(riStretch.iLo());
	rrStretch.rHi() = doubleConv(riStretch.iHi());
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

String SetDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.scVal(),"CoordinateSystem",fnView, csy);
	ObjectInfo::WriteElement(parentSection.scVal(),"Representation",fnView, rpr);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchReal",fnView, rrStretch);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchInt",fnView, riStretch);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsStretched",fnView, stretched);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchMethod",fnView, stretchMethod);

	return parentSection;
}

void SetDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.scVal(),"CoordinateSystem",fnView, csy);
	ObjectInfo::ReadElement(parentSection.scVal(),"Representation",fnView, rpr);
	ObjectInfo::ReadElement(parentSection.scVal(),"StretchReal",fnView, rrStretch);
	ObjectInfo::ReadElement(parentSection.scVal(),"StretchInt",fnView, riStretch);
	ObjectInfo::ReadElement(parentSection.scVal(),"IsStretched",fnView, stretched);
	long method;
	ObjectInfo::ReadElement(parentSection.scVal(),"StretchMethod",fnView, method);
	stretchMethod = (StretchMethod)method;
}
//---------------UI---------------

HTREEITEM SetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = ComplexDrawer::configure(tv,parent);

	NewDrawer::DrawMethod method = getDrawMethod();
	portrayalItem = InsertItem(tv,parent, "Portrayal", "Colors");

	colorCheck = new SetChecks(tv,this,(SetCheckFunc)&SetDrawer::setcheckRpr);
	if ( rpr.fValid() ) {
		bool usesRpr = method == NewDrawer::drmRPR;
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,portrayalItem,this,(DisplayOptionItemFunc)&SetDrawer::displayOptionSubRpr,0,colorCheck);
		rprItem = InsertItem("Representation", ".rpr", item, (int)usesRpr);
		insertLegendItems(tv, rprItem);
		if ( usesRpr)
			InsertItem(tv, rprItem,String("Value : %S",rpr->sName()),".rpr");
		if ( rpr->prg() || rpr->prv()){
			insertStretchItem(tv, portrayalItem);
		
		}

	}
	return hti;
}

void SetDrawer::insertLegendItems(LayerTreeView  *tv, HTREEITEM parent) {
	String sName = SDCRemLegend;
	int iImgLeg = IlwWinApp()->iImage("legend");
	HTREEITEM htiLeg = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImgLeg, iImgLeg, parent);
	if (0 == htiLeg)
		return;

	if ( rpr->prg() || rpr->prv())
		insertLegendItemsValue(tv, htiLeg);
	else if ( rpr->prc()) {
		insertLegendItemsClass(tv, htiLeg);
	}
	tv->GetTreeCtrl().Expand(htiLeg, TVE_EXPAND);

}
void SetDrawer::insertLegendItemsValue(LayerTreeView  *tv, HTREEITEM htiLeg){
	tv->GetTreeCtrl().SetItemData(htiLeg, (DWORD_PTR)new ObjectLayerTreeItem(tv, rpr.pointer()));
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();					
	RangeReal rr = getStretchRangeReal();
	int iItems = 5;
	double rStep = dvs.rStep();
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
		HTREEITEM hti = tv->GetTreeCtrl().InsertItem(sName.scVal(), htiLeg);
		tv->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendValueLayerTreeItem(tv, this, dvs, rVal));		
	}
}

void SetDrawer::insertLegendItemsClass(LayerTreeView  *tv, HTREEITEM htiLeg){
	tv->GetTreeCtrl().SetItemData(htiLeg, (DWORD_PTR)new LegendLayerTreeItem(tv, this));		
	DomainClass* dc = rpr->dm()->pdc();
	int iItems = dc->iNettoSize();
	for (int i = 1; i <= iItems; ++i) {
		int iRaw = dc->iKey(i);
		String sName = dc->sValueByRaw(iRaw, 0);
		HTREEITEM hti = tv->GetTreeCtrl().InsertItem(sName.scVal(), htiLeg);
		tv->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(tv, this, rpr->dm(), iRaw));		
	}
}


void SetDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();
	Color clr;
	if ( dvs.dm()->pdv())
		clr = getDrawingColor()->clrVal(rVal);
	else
		clr = getDrawingColor()->clrRaw((long)rVal, getDrawMethod());
	

	CBrush brushColor(clr);
	CBrush *br = dc->SelectObject(&brushColor);
	dc->Rectangle(rct);
	dc->SelectObject(br);
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

RepresentationForm::RepresentationForm(CWnd *wPar, SetDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Set Representation"),
	rpr(dr->getRepresentation()->sName())
{
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

void  RepresentationForm::apply() {
	fldRpr->StoreData();
	SetDrawer *setDrawer = (SetDrawer *)drw;
	setDrawer->setRepresentation(rpr);
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
	low(dr->getStretchRangeReal().rLo()),
	high(dr->getStretchRangeReal().rHi())

{
	BaseMapPtr *ptr = ((AbstractMapDrawer *)dr->getParentDrawer())->getBaseMap();
	rr = ptr->dvrs().rrMinMax();
	double rStep = ptr->dvrs().rStep();
	sliderLow = new FieldRealSliderEx(root,"Lower", &low,ValueRange(rr,rStep),true);
	sliderHigh = new FieldRealSliderEx(root,"Upper", &high,ValueRange(rr,rStep),true);
	sliderHigh->Align(sliderLow, AL_UNDER);
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
	((SetDrawer *)drw)->setStretchRangeReal(RangeReal(low,high));

	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepare(&pp);
	updateMapView();
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








