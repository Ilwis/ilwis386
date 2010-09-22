#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\col.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

#define DEFAULT_SCALE 3.0

ZValueMaker::ZValueMaker()  : scalingType(zvsNONE), self(true),threeDPossible(false),offset(0), zscale(DEFAULT_SCALE){
}
void ZValueMaker::setDataSourceMap(const BaseMap& mp){
	threeDPossible =  mp->dm()->dmt() != dmtVALUE ? false : true;
	datasourcemap = mp;
	table = Table();
	range = mp->dvrs().rrMinMax();
	type = IlwisObject::iotObjectType(datasourcemap->fnObj);
	self =  spatialsourcemap == datasourcemap;
	offset = 0;
	zscale = DEFAULT_SCALE;
}

void ZValueMaker::setSpatialSourceMap(const BaseMap& mp) {
	spatialsourcemap = mp;
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
}

void ZValueMaker::store(const FileName& fnView, const String& section) {
	if ( table.fValid())
		ObjectInfo::WriteElement(section.scVal(),"Table",fnView, table);
	if ( spatialsourcemap.fValid()) 
		ObjectInfo::WriteElement(section.scVal(),"Spatialsourcemap",fnView, spatialsourcemap);
	if ( datasourcemap.fValid())
		ObjectInfo::WriteElement(section.scVal(),"Datasourcemap",fnView, datasourcemap);
	if ( range.fValid())
		ObjectInfo::WriteElement(section.scVal(),"Range",fnView, range);
	ObjectInfo::WriteElement(section.scVal(),"Self",fnView, self);
	ObjectInfo::WriteElement(section.scVal(),"Zscale",fnView, zscale);
	ObjectInfo::WriteElement(section.scVal(),"Range",fnView, offset);
}

void ZValueMaker::load(const FileName& fnView, const String& section) {
	ObjectInfo::ReadElement(section.scVal(),"Table",fnView, table);
	FileName fn;
	ObjectInfo::ReadElement(section.scVal(),"Spatialsourcemap",fnView, fn);
	if ( fn.fValid())
		spatialsourcemap = BaseMap(fn);
	ObjectInfo::ReadElement(section.scVal(),"Datasourcemap",fnView, fn);
	if ( fn.fValid())
		datasourcemap = BaseMap(fn);
	ObjectInfo::ReadElement(section.scVal(),"Range",fnView, range);
	ObjectInfo::ReadElement(section.scVal(),"Self",fnView, self);
	ObjectInfo::ReadElement(section.scVal(),"Zscale",fnView, zscale);
	ObjectInfo::ReadElement(section.scVal(),"Range",fnView, offset);
}

void ZValueMaker::setTable(const Table& tbl, const vector<String>& names) {
	table = tbl;
	columns.clear();
	for(int i = 0; i < names.size(); ++i) {
		Column column = tbl->col(names[i]);
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
	return range;
}

double ZValueMaker::getValue(const Coord& crd, Feature *f ){

	if (!threeDPossible)
		return spatialsourcemap->cb().width() * 0.01;
	double value = 0;
	if (self && f && type != IlwisObject::iotRASMAP)
		value =  f->rValue();
	if (self && type == IlwisObject::iotRASMAP){
		value = spatialsourcemap->rValue(crd);
	}
	if ( type == IlwisObject::iotRASMAP)
		value = datasourcemap->rValue(crd);
	if (table.fValid()) {
		value =  columns[0]->rValue(f->iValue());
	}
	if ( value == rUNDEF)
		return 0;
	double scale = (value - range.rLo()) / range.rWidth();
	double zMaxSizeEstimate = (spatialsourcemap->cb().width() + spatialsourcemap->cb().height())/ 2.0;
	double endvalue = scale * zMaxSizeEstimate * 0.25;
	if ( endvalue <= 0) {
		endvalue = zMaxSizeEstimate * 0.01;
	}
	return endvalue;
}

void ZValueMaker::setOffset(double v){
	if ( range.rLo() == rUNDEF) {
		offset = 0;
	}
	offset = v;
}

double ZValueMaker::getOffset() const {
	if (offset != rUNDEF)
		return offset;
	return 0;
}

void ZValueMaker::setThreeDPossible(bool v) {
	if ( datasourcemap.fValid()) {
		threeDPossible = v && (datasourcemap->dm()->dmt() != dmtVALUE ? false : true);
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

