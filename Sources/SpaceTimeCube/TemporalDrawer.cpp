#include "Client\Headers\formelementspch.h"
#include "TemporalDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

TemporalDrawer::TemporalDrawer()
: fTimeAttribute(false)
, fTimeAttribute2(false)
{
}

TemporalDrawer::~TemporalDrawer()
{
}

void TemporalDrawer::SetTimeAttribute(const Column & col)
{
	fTimeAttribute = col.fValid();
	if (fTimeAttribute)
		colTime = col;
}

void TemporalDrawer::SetTimeAttribute2(const Column & col)
{
	fTimeAttribute2 = col.fValid();
	if (fTimeAttribute2)
		colTime2 = col;
}

const Column & TemporalDrawer::getTimeAttribute() const
{
	return colTime;
}

void TemporalDrawer::SetSelfTime()
{
	fTimeAttribute = false;
}

const double TemporalDrawer::getTimeValue(Feature * f) const
{
	if (f) {
		if (fTimeAttribute)
			return colTime->rValue(f->iValue()) - timeBounds->tMin();
		else
			return f->rValue() - timeBounds->tMin();
	} else
		return 0;
}

const double TemporalDrawer::getTimeValue2(Feature * f) const
{
	if (f && fTimeAttribute2)
		return colTime2->rValue(f->iValue()) - timeBounds->tMin();
	else
		return getTimeValue(f);
}

const TimeBounds * TemporalDrawer::getTimeBounds() const
{
	return timeBounds;
}

String TemporalDrawer::storeTemporal(const FileName& fnView, const String& parentSection) const
{
	ObjectInfo::WriteElement(parentSection.c_str(), "UseTime", fnView, fTimeAttribute);
	ObjectInfo::WriteElement(parentSection.c_str(), "UseTime2", fnView, fTimeAttribute2);
	if (colTime.fValid()) {
		ObjectInfo::WriteElement(parentSection.c_str(), "TimeTable", fnView, colTime->fnTbl.sRelativeQuoted());
		ObjectInfo::WriteElement(parentSection.c_str(), "TimeColumn", fnView, colTime);
		if (colTime2.fValid())
			ObjectInfo::WriteElement(parentSection.c_str(), "TimeColumn2", fnView, colTime2);
	}
	return parentSection;
}

void TemporalDrawer::loadTemporal(const FileName& fnView, const String& currentSection)
{
	ObjectInfo::ReadElement(currentSection.c_str(), "UseTime", fnView, fTimeAttribute);
	ObjectInfo::ReadElement(currentSection.c_str(), "UseTime2", fnView, fTimeAttribute2);
	Table tbl;
	ObjectInfo::ReadElement(currentSection.c_str(), "TimeTable", fnView, tbl);
	if (tbl.fValid()) {
		String sCol;
		ObjectInfo::ReadElement(currentSection.c_str(), "TimeColumn", fnView, sCol);
		if (sCol.length() > 0)
			colTime = tbl->col(sCol);
		else
			fTimeAttribute = false;
		sCol = "";
		ObjectInfo::ReadElement(currentSection.c_str(), "TimeColumn2", fnView, sCol);
		if (sCol.length() > 0)
			colTime2 = tbl->col(sCol);
		else
			fTimeAttribute2 = false;
	} else {
		fTimeAttribute = false;
		fTimeAttribute2 = false;
	}
}
