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
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

unsigned long LayerDrawer::test_count = 0;

LayerDrawer::LayerDrawer(DrawerParameters *parms, const String& name) : 
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

LayerDrawer::~LayerDrawer() {
	delete drawColor;
}

void LayerDrawer::prepare(PreparationParameters *parm){
	//delete drawColor; // the drawColor is still in use elsewhere (a local pointer is kept for efficiency). Unless there's a good reason to fully renew it here, I try commenting this out for now.
	//drawColor = 0;
	ComplexDrawer::prepare(parm);
	if ( parm->type & NewDrawer::ptGEOMETRY ||  parm->type & NewDrawer::ptRESTORE) {
		if ( parm->csy.fValid() && parm->csy->fUnknown() == false)
			csy = parm->csy;
		SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
		Representation rpr = mapDrawer->getRepresentation();
		//if ( rpr.fValid() && !rpr->prv())
		if ( rpr.fValid() && !rpr->prv())
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		if (!drawColor)
			drawColor = new DrawingColor(this, parm->index);
	}
	setDrawMethod();
}

bool LayerDrawer::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//if ( getRootDrawer()->is3D() ) {
	//	getRootDrawer()->init3D();
	//}
	getZMaker()->setZOrder(getRootDrawer()->getZIndex(),getRootDrawer()->getFakeZ());
	ComplexDrawer::draw( cbArea);
	glDisable(GL_BLEND);
	return true;
}

//void LayerDrawer::SetthreeD(void *v, LayerTreeView *tv) {
//	bool value = *(bool *)v;
//	getZMaker()->setThreeDPossible(value);
//	getRootDrawer()->getDrawerContext()->doDraw();
//}

String LayerDrawer::iconName(const String& subtype) const 
{ 
	return "Set";
}

bool LayerDrawer::useInternalDomain() const {
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)getParentDrawer();
	if ( !mapDrawer)
		throw ErrorObject("Parent drawer not set");
	return mapDrawer->useInternalDomain();
}


Representation LayerDrawer::getRepresentation() const { // avoiding copy constructotrs
	return rpr;
}

double LayerDrawer::getExtrusionTransparency() const {
	return extrTransparency;
}

void LayerDrawer::setExtrustionTransparency(double v) {
	extrTransparency = v;
	for(int i = 0; i < drawers.size(); ++i) {
		if ( drawers[i]->isSimple())
			((SimpleDrawer *)drawers[i])->setExtrustionTransparency(v);
	}
}

void LayerDrawer::setRepresentation( const Representation& rp){
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
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
		//isValue = mapDrawer->getBaseMap()->dm()->pdv() !=0;
		isValue = rp->dm()->pdv() !=0;
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

CoordSystem LayerDrawer::getCoordSystem() const {
	return csy;
}

bool LayerDrawer::isStretched() const {
	return stretched;
}

RangeReal LayerDrawer::getStretchRangeReal(bool useAttrib) const{
	if ( useAttrib && useAttributeColumn())
			return getAtttributeColumn()->rrMinMax();
    return rrStretch;
}

String LayerDrawer::getInfo(const Coord& c) const {
	if ( !hasInfo() || !isActive())
		return "";
	Coord crd = c;
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
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

void LayerDrawer::setStretchRangeReal(const RangeReal& rr){
	if ( rr.fValid())
		stretched = true;
	rrStretch = rr;
}

LayerDrawer::StretchMethod LayerDrawer::getStretchMethod() const{
	return stretchMethod;
}
void LayerDrawer::setStretchMethod(StretchMethod sm){
	stretchMethod = sm;
}

ILWIS::DrawingColor * LayerDrawer::getDrawingColor() const {
	return drawColor;
}

Column LayerDrawer::getAtttributeColumn() const{
	return attColumn;
}

void LayerDrawer::setAttributeColumn(const Column& c){
	attColumn = c;
}

void LayerDrawer::setUseAttributeColumn(bool yesno) {
	if ( attColumn.fValid() ) { // dont mess around with stretch if there was never a valid column, leave it as is
		if ( yesno) {
			if (attColumn->dm()->pdv())
				setStretchRangeReal(attColumn->rrMinMax());
		} else {
			SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		}
	}
	useAttColumn = yesno;
}

bool LayerDrawer::useAttributeColumn() const{
	return useAttColumn ;
}

String LayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.scVal(),"CoordinateSystem",fnView, csy);
	ObjectInfo::WriteElement(parentSection.scVal(),"Representation",fnView, rpr);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchReal",fnView, rrStretch);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsStretched",fnView, stretched);
	ObjectInfo::WriteElement(parentSection.scVal(),"StretchMethod",fnView, stretchMethod);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.scVal(),"UseAttributes",fnView, useAttColumn);
	ObjectInfo::WriteElement(parentSection.scVal(),"ExtrusionTransparency",fnView, extrTransparency);

	return parentSection;
}

void LayerDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.scVal(),"CoordinateSystem",fnView, csy);
	ObjectInfo::ReadElement(parentSection.scVal(),"Representation",fnView, rpr);
	ObjectInfo::ReadElement(parentSection.scVal(),"StretchReal",fnView, rrStretch);
	ObjectInfo::ReadElement(parentSection.scVal(),"IsStretched",fnView, stretched);
	long method;
	ObjectInfo::ReadElement(parentSection.scVal(),"StretchMethod",fnView, method);
	stretchMethod = (StretchMethod)method;
	String colname;
	ObjectInfo::ReadElement(parentSection.scVal(),"AttributeColumn",fnView, colname);
	if ( colname != "") {
		attColumn = ((SpatialDataDrawer *)getParentDrawer())->getBaseMap()->tblAtt()->col(colname);
	}
	ObjectInfo::ReadElement(parentSection.scVal(),"UseAttributes",fnView, useAttColumn);
	ObjectInfo::ReadElement(parentSection.scVal(),"ExtrusionTransparency",fnView, extrTransparency);
}

void LayerDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();

	if ( useAttributeColumn() && getAtttributeColumn().fValid()) {
		dvs = getAtttributeColumn()->dvrs();
	}
	Color clr;
	if ( dvs.dm()->pdv()) {
	//	if ( dvs.fUseReals())
			clr = getDrawingColor()->clrVal(rVal);
	//	else
	//		clr = getDrawingColor()->clrRaw(dvs.iRaw(rVal), getDrawMethod());
	}
	else {
		if ( useAttributeColumn() && getAtttributeColumn().fValid()) {
			clr = getRepresentation()->clrRaw((long)rVal);
		} else {
			clr = getDrawingColor()->clrRaw((long)rVal, getDrawMethod());
		}
	}
	

	CBrush brushColor(clr);
	CBrush *br = dc->SelectObject(&brushColor);
	dc->Rectangle(rct);
	dc->SelectObject(br);
}










