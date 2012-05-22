#include "Client\Headers\formelementspch.h"
#include "TemporalDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

TemporalDrawer::TemporalDrawer()
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
