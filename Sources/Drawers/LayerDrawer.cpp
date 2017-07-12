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
	extrAlpha(0.2),
	useAttColumn(false),
	useRpr(true)
	
{
	setInfo(true);
	setAlpha(1);
}

LayerDrawer::~LayerDrawer() {
	delete drawColor;
}

void LayerDrawer::prepare(PreparationParameters *parm){
	ComplexDrawer::prepare(parm);
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
	if ( parm->type & NewDrawer::ptGEOMETRY ||  parm->type & NewDrawer::ptRESTORE) {
		if ( parm->csy.fValid() && parm->csy->fUnknown() == false)
			csy = parm->csy;
		Representation rpr = mapDrawer->getRepresentation();
		//if ( rpr.fValid() && !rpr->prv())
		if ( rpr.fValid() && !rpr->prv())
			setStretchRangeReal(mapDrawer->getStretchRangeReal());
		if (!drawColor)
			drawColor = new DrawingColor(this, parm->index);
	}
	if ( parm->type & NewDrawer::ptRENDER) {
		if ( parm->displayOptions != "") {
			Array<String> parts;
			Split( parm->displayOptions,parts,":");
			map<String, String> options;
			for(int i=0; i <parts.size(); ++i) {
				options[parts[i].sHead("=")] = parts[i].sTail("=");
			}
			map<String,String>::const_iterator cur = options.find("tr");
			if ( cur != options.end() && drawColor) {
				String values = (*cur).second;
				String sv1 = values.sHead(",");
				String sv2 = values.sTail(",");
				if ( sv2 == "")
					sv2 = sv1;
				RangeReal rr(sv1.rVal(), sv2.rVal());
				drawColor->setTransparentValues(rr);
			}
			cur = options.find("rpr");
			if ( cur != options.end()) {
				String repr = (*cur).second;
				FileName fnrpr(repr,".rpr");
				setRepresentation(Representation(fnrpr));
			}
		}
	}
	setDrawMethod(drm);
}

bool LayerDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//if ( getRootDrawer()->is3D() ) {
	//	getRootDrawer()->init3D();
	//}
	getZMaker()->setZOrder(getRootDrawer()->getZIndex());
	ComplexDrawer::draw(drawLoop, cbArea);
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

double LayerDrawer::getExtrusionAlpha() const {
	return extrAlpha;
}

void LayerDrawer::setExtrusionAlpha(double v) {
	extrAlpha = v;
	for(int i = 0; i < drawers.size(); ++i) {
		if ( drawers[i]->isSimple())
			((SimpleDrawer *)drawers[i])->setExtrusionAlpha(v);
	}
}

void LayerDrawer::setRepresentation( const Representation& rp){
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
	if ( !rp.fValid()) { 
		// domains id and bool have no rpr
		Domain dm;
		if (useAttributeColumn() && getAtttributeColumn().fValid()) {
			dm.SetPointer(getAtttributeColumn()->dm().ptr());
		} else {
			dm.SetPointer(mapDrawer->getBaseMap()->dm().ptr());
		}
		if ( dm->pdid()) {
			bool fUseMultiple = (getType() == "PolygonLayerDrawer" || getType() == "RasterLayerDrawer");
			setDrawMethod(fUseMultiple ? NewDrawer::drmMULTIPLE : NewDrawer::drmSINGLE);
		} else if ( dm->pdbool()) {
			setDrawMethod(NewDrawer::drmBOOL);
		}
		rpr = rp; // set the representation to the "invalid" one to prevent the wrong tools from appearing (based on the old representation)
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
	BaseMapPtr *bmptr = mapDrawer->getBaseMap(mapDrawer->getCurrentIndex());
	if (bmptr->cs() != rootDrawer->getCoordinateSystem())
	{
		crd = bmptr->cs()->cConv(rootDrawer->getCoordinateSystem(), c);
	}
	vector<String> infos = bmptr->vsValue(crd,0);
	String info;
	DomainValue* dv = bmptr->dm()->pdv();
	int count = 0;
	for(int i = 0; i < infos.size(); ++i) {
		String s = infos[i].sTrimSpaces();
		if ( s == "?")
			continue;
		if (0 != dv && dv->fUnit())
			s = String("%S %S", s, dv->sUnit());
		info += count == 0 ? s : ";" + s;
		++count;
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
	ObjectInfo::WriteElement(parentSection.c_str(),"CoordinateSystem",fnView, csy);
	ObjectInfo::WriteElement(parentSection.c_str(),"Representation",fnView, rpr);
	ObjectInfo::WriteElement(parentSection.c_str(),"StretchReal",fnView, rrStretch);
	ObjectInfo::WriteElement(parentSection.c_str(),"IsStretched",fnView, stretched);
	ObjectInfo::WriteElement(parentSection.c_str(),"StretchMethod",fnView, stretchMethod);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.c_str(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.c_str(),"UseAttributes",fnView, useAttColumn);
	ObjectInfo::WriteElement(parentSection.c_str(),"ExtrusionAlpha",fnView, extrAlpha);
	ObjectInfo::WriteElement(parentSection.c_str(),"UseRpr",fnView, useRpr);
	if ( drawColor) {
		drawColor->store(fnView, parentSection);
	}
	return parentSection;
}

void LayerDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.c_str(),"CoordinateSystem",fnView, csy);
	ObjectInfo::ReadElement(parentSection.c_str(),"Representation",fnView, rpr);
	ObjectInfo::ReadElement(parentSection.c_str(),"StretchReal",fnView, rrStretch);
	ObjectInfo::ReadElement(parentSection.c_str(),"IsStretched",fnView, stretched);
	long method;
	ObjectInfo::ReadElement(parentSection.c_str(),"StretchMethod",fnView, method);
	stretchMethod = (StretchMethod)method;
	String colname;
	ObjectInfo::ReadElement(parentSection.c_str(),"AttributeColumn",fnView, colname);
	ObjectInfo::ReadElement(parentSection.c_str(),"UseRpr",fnView, useRpr);
	if ( colname != "") {
		attColumn = ((SpatialDataDrawer *)getParentDrawer())->getBaseMap()->tblAtt()->col(colname);
	}
	ObjectInfo::ReadElement(parentSection.c_str(),"UseAttributes",fnView, useAttColumn);
	drawColor = new DrawingColor(this, 0);
	if ( drawColor) {
		drawColor->load(fnView, parentSection);
	}
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

RangeReal LayerDrawer::getTransparentValues() const{
	if ( drawColor) {
		return drawColor->getTransparentValues();
	}
	return RangeReal();
}

void LayerDrawer::setTransparentValues(const RangeReal& rr){
	if ( drawColor) {
		drawColor->setTransparentValues(rr);
	}
}

bool LayerDrawer::isSelectable() const { 
	return selectable;
}
void LayerDrawer::setSelectable(bool yesno ){
	selectable = yesno;
}

vector<long> * LayerDrawer::getSelectedRaws()
{
	return &selectedRaws;
}

