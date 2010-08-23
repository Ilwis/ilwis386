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
	range = mp->rrMinMax();
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
	column  = tbl->col(colName);
	threeDPossible = column->dm()->dmt() != dmtVALUE ? false : true;
	datasourcemap = BaseMap();
	range = column->rrMinMax();
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
		value =  column->rValue(f->iValue());
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
	else if ( column.fValid()) {
		threeDPossible = v && (column->dm()->dmt() != dmtVALUE ? false : true); 
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