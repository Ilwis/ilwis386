#pragma once

#include "TimeBounds.h"
#include "Engine\Table\Col.h"

namespace ILWIS {

class TemporalDrawer {
public:
	TemporalDrawer();
	virtual ~TemporalDrawer();
	void SetTimeBounds(TimeBounds * _timeBounds) {timeBounds = _timeBounds;};
	void SetTimeAttribute(const Column & col);
	void SetTimeAttribute2(const Column & col);
	void SetSelfTime();
	const double getTimeValue(Feature * f) const;
	const double getTimeValue2(Feature * f) const;

protected:
	TimeBounds * timeBounds;
	bool fTimeAttribute;
	bool fTimeAttribute2;
	Column colTime;
	Column colTime2;
};

}