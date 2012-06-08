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
