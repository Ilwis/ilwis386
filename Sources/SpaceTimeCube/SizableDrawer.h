#pragma once

#include "Engine\Table\Col.h"

namespace ILWIS {

class SizableDrawer {
public:
	SizableDrawer();
	virtual ~SizableDrawer();
	void SetSizeStretch(RangeReal * _sizeStretch) {sizeStretch = _sizeStretch;};
	void SetSizeAttribute(const Column & col);
	void SetNoSize();
	const double getSizeValue(Feature * f) const;

protected:
	RangeReal * sizeStretch;
	bool fUseSize;
	Column colSize;
};

}