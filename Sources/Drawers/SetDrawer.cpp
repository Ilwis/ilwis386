#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Representation\Rpr.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

unsigned long SetDrawer::test_count = 0;

SetDrawer::SetDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	stretched(false),
	stretchMethod(smLINEAR),
	drawColor(0),
	extrTransparency(0.2),
	useAttColumn(false)
{
	setInfo(true);
	setTransparency(1);
}

SetDrawer::~SetDrawer() {
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

bool SetDrawer::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	getZMaker()->setZOrder(getRootDrawer()->getZIndex(),getRootDrawer()->getFakeZ());
	ComplexDrawer::draw( cbArea);
	glDisable(GL_BLEND);
	return true;
}

//void SetDrawer::SetthreeD(void *v, LayerTreeView *tv) {
//	bool value = *(bool *)v;
//	getZMaker()->setThreeDPossible(value);
//	getRootDrawer()->getDrawerContext()->doDraw();
//}

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


Representation SetDrawer::getRepresentation() const { // avoiding copy constructotrs
	return rpr;
}

double SetDrawer::getExtrusionTransparency() const {
	return extrTransparency;
}

void SetDrawer::setExtrustionTransparency(double v) {
	extrTransparency = v;
	for(int i = 0; i < drawers.size(); ++i) {
		if ( drawers[i]->isSimple())
			((SimpleDrawer *)drawers[i])->setExtrustionTransparency(v);
	}
}

void SetDrawer::setRepresentation( const Representation& rp){
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
	if ( !rp.fValid()) { 
		// domain id has no rpr
		BaseMap bmp;
		bmp.SetPointer(mapDrawer->getBaseMap());
		Domain dm = bmp->dm();
		if ( dm->pdid()) {
			setDrawMethod(NewDrawer::drmMULTIPLE);
		}
		return;
	}

	bool isValue = false;
	if ( useAttributeColumn() && getAtttributeColumn().fValid() ) {
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

Column SetDrawer::getAtttributeColumn() const{
	return attColumn;
}

void SetDrawer::setAttributeColumn(const Column& c){
	attColumn = c;
}

void SetDrawer::setUseAttributeColumn(bool yesno) {
	if ( attColumn.fValid() ) { // dont mess around with stretch if there was never a valid column, leave it as is
		if ( yesno) {
			if (attColumn->dm()->pdv())
				setStretchRangeReal(attColumn->rrMinMax());
		} else {
			AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)parentDrawer;
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		}
	}
	useAttColumn = yesno;
}

bool SetDrawer::useAttributeColumn() const{
	return useAttColumn ;
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

void SetDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();

	if ( useAttributeColumn() && getAtttributeColumn().fValid()) {
		dvs = getAtttributeColumn()->dvrs();
	}
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










