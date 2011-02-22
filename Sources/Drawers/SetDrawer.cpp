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
#include "Drawers\DisplayOptionsLegend.h"
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
	portrayalItem(0),
	doLegend(0),
	extrTransparency(0.2),
	useAttColumn(false)
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
	if ( doLegend)
		doLegend->setcheckRpr(value, tree);
}

Representation SetDrawer::getRepresentation() const { // avoiding copy constructotrs
	return rpr;
}

double SetDrawer::getExtrusionTransparency() const {
	return extrTransparency;
}

void SetDrawer::setRepresentation( const Representation& rp){
	if ( !rp.fValid())
		return;

	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	bool isValue = false;
	if ( useAttributeColumn()) {
		isValue = getAtttributeColumn()->dm()->pdv() != 0;
	} else {
		isValue = mapDrawer->getBaseMap()->dm()->pdv() !=0;
	}

	rpr = rp;
	stretched = false;
	setDrawMethod(NewDrawer::drmRPR);
	if ( isValue) {
		if ( useAttributeColumn()) {
			attColumn->CalcMinMax();
			setStretchRangeReal(attColumn->rrMinMax());
		}
		else{
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		}
	}
}

CoordSystem SetDrawer::getCoordSystem() const {
	return csy;
}

//bool SetDrawer::isLegendUsefull() const {
//	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
//	BaseMapPtr *bm = mapDrawer->getBaseMap();
//	if (bm != 0 && bm->dm()->pdv() && stretched) 
//		return true;
//	return drm != drmSINGLE;
//}

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

void SetDrawer::displayOptionStretch(CWnd *parent) {
	new SetStretchForm(parent, this);
}

Column SetDrawer::getAtttributeColumn() const{
	return attColumn;
}

void SetDrawer::setAttributeColumn(const Column& c){
	attColumn = c;
}

void SetDrawer::setUseAttributeColumn(bool yesno) {
	useAttColumn = yesno;
}

bool SetDrawer::useAttributeColumn() const{
	return attColumn.fValid() && useAttColumn ;
}

String SetDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.scVal(),"CoordinateSystem",fnView, csy);
	ObjectInfo::WriteElement(parentSection.scVal(),"Representation",fnView, rpr);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchReal",fnView, rrStretch);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchInt",fnView, riStretch);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsStretched",fnView, stretched);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchMethod",fnView, stretchMethod);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.scVal(),"UseAttributes",fnView, useAttColumn);

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
	String colname;
	ObjectInfo::ReadElement(parentSection.scVal(),"AttributeColumn",fnView, colname);
	attColumn = ((AbstractMapDrawer *)getParentDrawer())->getBaseMap()->tblAtt()->col(colname);
	ObjectInfo::ReadElement(parentSection.scVal(),"UseAttributes",fnView, useAttColumn);
}
//---------------UI---------------

HTREEITEM SetDrawer::make3D(bool yesno, LayerTreeView  *tv){
	threeD = yesno;
	if ( yesno) {
		if ( portrayalItem != 0) {
			DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,portrayalItem,this,(SetCheckFunc)&SetDrawer::SetthreeD);
			threeDItem = InsertItem("3D properties","3D",item,threeD);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(DisplayOptionItemFunc)&SetDrawer::displayZOption3D);
			InsertItem("Data source", ".mpv",item);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(DisplayOptionItemFunc)&SetDrawer::displayZScaling);
			InsertItem("Scaling", "ScaleBar",item);
			item = new DisplayOptionTreeItem(tv,threeDItem,this,(SetCheckFunc)&SetDrawer::setExtrusion,(DisplayOptionItemFunc)&SetDrawer::extrusionOptions);
			InsertItem("Extrusion","Extrusion",item,getSpecialDrawingOption(sdoExtrusion));
			InsertItem(tv, threeDItem, "Axis", "Axis");
		}
	}
	else {
		if ( threeDItem) {
			tv->DeleteAllItems(threeDItem);
			tv->GetTreeCtrl().DeleteItem(threeDItem);
		}
		threeDItem = 0;
	}
	PreparationParameters pp(NewDrawer::pt3D);
	prepare(&pp);
	return threeDItem;
}

HTREEITEM SetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = ComplexDrawer::configure(tv,parent);
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	if ( mapDrawer->getBaseMap()->dm()->pdsrt()) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,
				(SetCheckFunc)&SetDrawer::setColumnCheckumn,
				(DisplayOptionItemFunc)&SetDrawer::displayOptionAttColumn);

		HTREEITEM itemColumn = InsertItem("Attribute table",".tbt",item,(int)useAttColumn);
		if ( useAttributeColumn() && attColumn.fValid())
			InsertItem(tv,itemColumn,String("Column : %S",attColumn->sName()),"column");
	}
	NewDrawer::DrawMethod method = getDrawMethod();
	portrayalItem = InsertItem(tv,parent, "Portrayal", "Colors");
	doLegend = new DisplayOptionsLegend(tv,portrayalItem);
	doLegend->createForSet(this);

	colorCheck = new SetChecks(tv,this,(SetCheckFunc)&SetDrawer::setcheckRpr);
	if ( rpr.fValid() && (rpr->prg() || rpr->prv())){
		insertStretchItem(tv, portrayalItem);
	
	}
	if ( getRootDrawer()->is3D()) {
		make3D(true, tv);
	}

	//}
	return hti;
}

void SetDrawer::setColumnCheckumn(void *w, LayerTreeView *view) {
	bool yesno = *(bool *)w;
	setUseAttributeColumn(yesno);
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	if ( yesno && attColumn.fValid())
		setRepresentation(attColumn->dm()->rpr());
	else
		setRepresentation(mapDrawer->getBaseMap()->dm()->rpr());
	if ( !yesno)
		getDrawingColor()->setDataColumn(Column());

	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	prepareChildDrawers(&pp);
	getRootDrawer()->getDrawerContext()->doDraw();
}

void SetDrawer::displayOptionAttColumn(CWnd *w) {
	if ( useAttColumn)
		new ChooseAttributeColumnForm(w, this);
}

void SetDrawer::extrusionOptions(CWnd *p) {
	new ExtrusionOptions(p, this);
}

void SetDrawer::setExtrusion(void *value, LayerTreeView *tree) {
	bool v = *(bool *)value;
	setSpecialDrawingOptions(sdoExtrusion | sdoTOCHILDEREN, v);
	//PreparationParameters parm(NewDrawer::ptRENDER, 0);
	//prepareChildDrawers(&parm);
	getRootDrawer()->getDrawerContext()->doDraw();

}

void SetDrawer::displayZOption3D(CWnd *parent) {
	new DisplayZDataSourceForm(parent, this);

}

void SetDrawer::displayZScaling(CWnd *parent) {
	new ZDataScaling(parent, this);

}

//--------------------------------
ZDataScaling::ZDataScaling(CWnd *wPar, SetDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Scaling and offset"),
zscale(dr->getZMaker()->getZScale() * 100),
zoffset(dr->getZMaker()->getOffset()),
sliderOffset(0) {
	sliderScale = new FieldRealSliderEx(root,"Z Scaling", &zscale,ValueRange(0,1000),true);
	sliderScale->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
	sliderScale->setContinuous(true);

	if (dr->getZMaker()->getRange().fValid()) { 
		RangeReal rr = dr->getZMaker()->getRange();
		ValueRangeReal vr(- ( rr.rHi() + rr.rLo()), rr.rWidth());
		zoffset -= rr.rLo();
		sliderOffset = new FieldRealSliderEx(root,"Z Offset", &zoffset,vr,true);
		sliderOffset->SetCallBack((NotifyProc)&ZDataScaling::settransforms);
		sliderOffset->setContinuous(true);
	}
	create();
}

int ZDataScaling::settransforms(Event *) {
	apply();
	return 1;
}

void ZDataScaling::apply() {
	sliderScale->StoreData();
	if ( sliderOffset)
		sliderOffset->StoreData();
	drw->getZMaker()->setZScale(zscale/100.0);
	RangeReal rr = drw->getZMaker()->getRange();
	drw->getZMaker()->setOffset(zoffset + rr.rLo());
	updateMapView();
}

//--------------------------------
DisplayZDataSourceForm::DisplayZDataSourceForm(CWnd *wPar, SetDrawer *dr) : 
DisplayOptionsForm(dr,wPar,TR("3D Options")), sourceIndex(0) 
{
	AbstractMapDrawer *fdrw = (AbstractMapDrawer *)dr->getParentDrawer();
	bmp.SetPointer(fdrw->getBaseMap());
	attTable = bmp->tblAtt();
	rg = new RadioGroup(root,TR("Data Source"),&sourceIndex);
	new RadioButton(rg,"Self");
	RadioButton *rbMap = new RadioButton(rg,TR("Raster Map"));
	fmap = new FieldMap(rbMap,"",&mapName, new MapListerDomainType(dmVALUE|dmIMAGE));

	if ( attTable.fValid()) {
		RadioButton *rbTable = new RadioButton(rg,TR("Attribute column"));
		FieldColumn *fcol = new FieldColumn(rbTable,"",attTable,&colName,dmVALUE&dmIMAGE);
	}

	rg->SetIndependentPos();


	create();
	
}


void DisplayZDataSourceForm::apply() {
	rg->StoreData();
//	fmap->StoreData();
	if ( sourceIndex == 0) {
		drw->getZMaker()->setDataSourceMap(bmp);
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	}
	else if ( mapName != "" && sourceIndex == 1) {
		drw->getZMaker()->setDataSourceMap(BaseMap(FileName(mapName)));
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	} else if ( colName != "" && sourceIndex == 2) {
		drw->getZMaker()->setTable(attTable,colName);
		PreparationParameters pp(NewDrawer::pt3D);
		drw->prepare(&pp);
	}
	updateMapView();
}

//-----------------------------------------
ExtrusionOptions::ExtrusionOptions(CWnd *p, SetDrawer *fsd) :
DisplayOptionsForm(fsd, p, TR("Extrusion options") )
{
	transparency = 100 *(1.0-fsd->extrTransparency);
	line = fsd->specialOptions & ( NewDrawer::sdoFilled| NewDrawer::sdoExtrusion) ? 0 : 1;
	rg = new RadioGroup(root, TR("Appearence"),&line);
	new RadioButton(rg, TR("Line"));
	new RadioButton(rg,TR("Filled"));
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&ExtrusionOptions::setTransparency);
	slider->setContinuous(true);

	create();

}

int ExtrusionOptions::setTransparency(Event *ev) {
	slider->StoreData();
	((SetDrawer *)drw)->extrTransparency = 1.0 - (double)transparency/100.0;
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
	return 1;
}
void ExtrusionOptions::apply() {
	rg->StoreData();
	slider->StoreData();
	if ( line == 1)
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled | NewDrawer::sdoTOCHILDEREN, true );
		//specialOptions |= NewDrawer::sdoFilled;
	else
		((SetDrawer *)drw)->setSpecialDrawingOptions(NewDrawer::sdoFilled | NewDrawer::sdoTOCHILDEREN, false);
	((SetDrawer *)drw)->extrTransparency = 1.0 - (double)transparency/100.0;
	updateMapView();

}

//--------------------------------------
void SetDrawer::updateLegendItem() {
	if ( doLegend)
		doLegend->updateLegendItem();
}

void SetDrawer::displayOptionSubRpr(CWnd *parent) {
	new RepresentationFormL(parent, this,0);
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
	Column attColumn = getAtttributeColumn();
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
	apply();
	return 1;
}

void  SetStretchForm::apply() {
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
	drw->prepareChildDrawers(&pp);
	updateMapView();
}

//-------------------------------------
ChooseAttributeColumnForm::ChooseAttributeColumnForm(CWnd *wPar, SetDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Choose attribute column")
{
	attTable = ((AbstractMapDrawer *)dr->getParentDrawer())->getBaseMap()->tblAtt();
	attColumn = dr->getAtttributeColumn().fValid() ? dr->getAtttributeColumn()->sName() : "";
	fc = new FieldColumn(root, "Column", attTable, &attColumn);
	create();
}


void  ChooseAttributeColumnForm::apply() {
	fc->StoreData();
	if ( attColumn != "") {
		Column col = attTable->col(attColumn);
		((SetDrawer *)drw)->setUseAttributeColumn(true);
		((SetDrawer *)drw)->setAttributeColumn(col);
		((SetDrawer *)drw)->setRepresentation(col->dm()->rpr());
		((SetDrawer *)drw)->getDrawingColor()->setDataColumn(col);
	}
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);
	updateMapView();

}








