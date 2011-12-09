#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\col.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Map\Feature.h"
#include "Drawer_n.h"
#include "Engine\Drawers\ComplexDrawer.h"

using namespace ILWIS;

#define DEFAULT_SCALE 1.0

ZValueMaker::ZValueMaker(NewDrawer *drw)  : scalingType(zvsNONE),threeDPossible(false),offset(0), zscale(DEFAULT_SCALE), zOrder(0), fakeZ(0), isSameCsy(true), associatedDrawer(drw){
	isSetDrawer = false;
	sourceType = styNONE;
	NewDrawer *parentDrw = associatedDrawer->getParentDrawer();
	if (parentDrw && !associatedDrawer->isSimple() && ((ComplexDrawer *)parentDrw)->isSet())
		isSetDrawer = true;
}
void ZValueMaker::setDataSourceMap(const BaseMap& mp){
	threeDPossible =  mp->dm()->dmt() == dmtVALUE || mp->dm()->dmt() == dmtIMAGE;
	datasourcemap = mp;
	addRange(mp);
	table = Table();
	type = IlwisObject::iotObjectType(datasourcemap->fnObj);
	if ( sourceType == styNONE) // first set the source type before being able to make changes here
		sourceType = spatialsourcemap == datasourcemap ? stySELF : styMAP;
	offset = 0;
	isSameCsy = spatialsourcemap->cs() == datasourcemap->cs();
	zscale = DEFAULT_SCALE;
}

void ZValueMaker::addRange(const BaseMap& mp) {
	if ( !mp.fValid())
		return;

	RangeReal tempRange = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);
	if ( !tempRange.fValid() && cbLimits.fValid()) {
		range = RangeReal(0,min(cbLimits.width(), cbLimits.height()));
	} else {
		range += tempRange;
	}
	if ( isSetDrawer ) {
		NewDrawer *parentDrw = associatedDrawer->getParentDrawer();
		if ( parentDrw)
			((ComplexDrawer *)parentDrw)->getZMaker()->addRange(mp);
	}
}

void ZValueMaker::setSpatialSource(const BaseMap& mp, const CoordBounds& cb) {
	spatialsourcemap = mp;
	cbLimits = cb;
}

void ZValueMaker::setTable(const Table& tbl, const String& colName) {
	table = tbl;
	columns.clear();
	Column column = tbl->col(colName);
	columns.push_back(tbl->col(colName));
	threeDPossible = column->dm()->dmt() != dmtVALUE ? false : true;
	datasourcemap = BaseMap();
	range = column->rrMinMax();
	type = IlwisObject::iotObjectType(table->fnObj);
	offset = 0;
	zscale = DEFAULT_SCALE;
	sourceType = styTABLE;
}

void ZValueMaker::store(const FileName& fnView, const String& section) {
	if ( table.fValid())
		ObjectInfo::WriteElement(section.c_str(),"Table",fnView, table);
	if ( columns.size() > 0)
		ObjectInfo::WriteElement(section.c_str(),"Column",fnView, columns.at(0));
	if ( spatialsourcemap.fValid()) 
		ObjectInfo::WriteElement(section.c_str(),"Spatialsourcemap",fnView, spatialsourcemap);
	if ( datasourcemap.fValid())
		ObjectInfo::WriteElement(section.c_str(),"Datasourcemap",fnView, datasourcemap);
	if ( range.fValid())
		ObjectInfo::WriteElement(section.c_str(),"Range",fnView, range);
	ObjectInfo::WriteElement(section.c_str(),"SourceType",fnView, sourceType);
	ObjectInfo::WriteElement(section.c_str(),"Zscale",fnView, zscale);
	ObjectInfo::WriteElement(section.c_str(),"Offset",fnView, offset);
}

void ZValueMaker::load(const FileName& fnView, const String& section) {
	String sCol;
	ObjectInfo::ReadElement(section.c_str(),"Column",fnView, sCol);
	if ( sCol != "") {
		ObjectInfo::ReadElement(section.c_str(),"Table",fnView, table);
		if ( table.fValid()) {
			setTable(table, sCol);
		}
	}
	FileName fn;
	ObjectInfo::ReadElement(section.c_str(),"Spatialsourcemap",fnView, fn);
	if ( fn.fValid()){
		setSpatialSource(BaseMap(fn),associatedDrawer->getRootDrawer()->getMapCoordBounds());
	}
	ObjectInfo::ReadElement(section.c_str(),"Datasourcemap",fnView, fn);
	if ( fn.fValid()) {
		setDataSourceMap(BaseMap(fn));
	}
	ObjectInfo::ReadElement(section.c_str(),"Range",fnView, range);
	long ty = (long)sourceType;
	ObjectInfo::ReadElement(section.c_str(),"SourceType",fnView, ty );
	ObjectInfo::ReadElement(section.c_str(),"Zscale",fnView, zscale);
	ObjectInfo::ReadElement(section.c_str(),"Offset",fnView, offset);
}

void ZValueMaker::setTable(const Table& tbl, const vector<String>& names) {
	table = tbl;
	if ( !tbl.fValid())
		return;

	columns.clear();
	for(int i = 0; i < names.size(); ++i) {
		Column column = tbl->col(names[i]);
		if ( !column.fValid())
			continue;

		columns.push_back(column);
		threeDPossible = column->dm()->dmt() != dmtVALUE ? false : true;
		datasourcemap = BaseMap();
		range += column->rrMinMax();
	}
	type = IlwisObject::iotObjectType(table->fnObj);
	offset = 0;
	zscale = DEFAULT_SCALE;
}

void ZValueMaker::setScaleType(ZValueTypeScaling t){
	scalingType = t;
}

RangeReal ZValueMaker::getRange() const {
	if ( isSetDrawer) {
		return ((ComplexDrawer *)associatedDrawer->getParentDrawer())->getZMaker()->getRange();
	}
	return range;
}

void ZValueMaker::setRange(const RangeReal& rr) {
	range = rr;
}

double ZValueMaker::getValue(const Coord& crd, Feature *f ){
	if ( sourceType == styNONE)
		return offset; 

	if (!threeDPossible && spatialsourcemap.fValid())
		return spatialsourcemap->cb().width() * 0.01;
	double value = 0;
	if (sourceType == stySELF && f && type != IlwisObject::iotRASMAP) {
		//if (!datasourcemap->dvrs().fRawAvailable())
			value =  f->rValue();
		//else
		//	value = datasourcemap->dvrs().rValue(f->rValue());
	}
	if (sourceType == stySELF && type == IlwisObject::iotRASMAP){
		value = spatialsourcemap->rValue(crd);
	}
	if ( type == IlwisObject::iotRASMAP && datasourcemap.fValid()) {
		Coord c = crd;
		if (!isSameCsy) {
			c = datasourcemap->cs()->cConv( spatialsourcemap->cs(), c);
		}
		value = datasourcemap->rValue(c);
	}
	if (table.fValid() && columns[0].fValid()) {
		value =  columns[0]->rValue(f->iValue());
	}
	return scaleValue(value);
}

double ZValueMaker::scaleValue(double value) {
	if ( styNONE)
		return offset;
	if ( value == rUNDEF)
		return 0;
	RangeReal scaleRange = getRange();
	double scale = (value - scaleRange.rLo()) / scaleRange.rWidth();
	double zMaxSizeEstimate = (cbLimits.width() + cbLimits.height())/ 2.0;
	double endvalue = scale * zMaxSizeEstimate * 0.20;
	return endvalue;
}

BaseMapPtr * ZValueMaker::getSourceRasterMap() const { // we return the pointer to avoid copy constructors
	if (type == IlwisObject::iotRASMAP) {
		if (sourceType == stySELF) {
			if (spatialsourcemap.fValid())
				return spatialsourcemap.ptr();
			else
				return 0;
		}
		else if (datasourcemap.fValid())
			return datasourcemap.ptr();
		else
			return 0;
	} else
		return 0;
}
void ZValueMaker::setOffset(double v, bool useTrueCoords){
	if (useTrueCoords) {
		offset = v;
	} else {
		if ( getRange().rLo() == rUNDEF) {
			v = 0;
		}
		double scale = (v - getRange().rLo()) / getRange().rWidth();
		double zMaxSizeEstimate = (cbLimits.width() + cbLimits.height())/ 2.0;
		offset = scale * zMaxSizeEstimate * 0.25;
	}
}

double ZValueMaker::getOffset() const {
	if (offset != rUNDEF)
		return offset;
	return 0;
}

void ZValueMaker::setThreeDPossible(bool v) {
	if ( datasourcemap.fValid()) {
		threeDPossible = v && (datasourcemap->dm()->dmt() == dmtVALUE || datasourcemap->dm()->dmt() == dmtIMAGE);
	}
	else if ( columns.size() > 0) {
		threeDPossible = v && (columns[0]->dm()->dmt() != dmtVALUE ? false : true); 
	}
	else
		threeDPossible = v;
}

bool ZValueMaker::getThreeDPossible() const
{
	return threeDPossible;
}

double ZValueMaker::getZScale() const{
	return zscale;
}
void ZValueMaker::setZScale(double v){
	zscale = v;
}

void ZValueMaker::setZOrder(int index, double base) {
	zOrder = index;
	fakeZ = (zOrder+1.0) * base;
}

int ZValueMaker::getZOrder() const{
	return zOrder;
}

double ZValueMaker::getZ0(bool is3D) const{
	return is3D ? fakeZ : 0;
}

void ZValueMaker::setBounds(const CoordBounds& bnd) {
	cbLimits = bnd;
}

ZValueMaker::SourceType ZValueMaker::getSourceType() const{
	return sourceType;
}

void ZValueMaker::setSourceType(SourceType s){
	sourceType = s;
}

String 	ZValueMaker::getColumnName(int index) {
	if ( index < columns.size())
		return columns[index]->sName();
	return sUNDEF;
}

BaseMap ZValueMaker::getSpatialSourceMap() const{
	return spatialsourcemap;
}