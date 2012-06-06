#pragma once

#include "Engine\Table\Col.h"

namespace ILWIS {

struct PointProperties;

class SizableDrawer {
public:
	SizableDrawer();
	virtual ~SizableDrawer();
	void SetSizeStretch(RangeReal * _sizeStretch);
	void SetSizeAttribute(const Column & col);
	void SetNoSize();
	const double getSizeValue(Feature * f) const;

protected:
	PointProperties *properties;
	RangeReal * sizeStretch;
	Column colSize;
};

}