#pragma once

#include "Engine\Table\Col.h"

namespace ILWIS {

struct PointProperties;

class _export SizableDrawer {
public:
	SizableDrawer();
	virtual ~SizableDrawer();
	void SetSizeStretch(RangeReal * _sizeStretch);
	void SetSizeAttribute(const Column & col);
	void SetSize2Attribute(const Column & col);
	void SetNoSize();
	const double getSizeValue(Feature * f) const;
	const double getSize2Value(Feature * f) const;
	const Column & getSizeAttribute() const;
	const Column & getSize2Attribute() const;
	const bool fGetUseSize() const;

protected:
	String storeSizable(const FileName& fnView, const String& parentSection) const;
	void loadSizable(const FileName& fnView, const String& currentSection);
	PointProperties *properties;
	RangeReal * sizeStretch;
	Column colSize;
	Column colSize2;
};

}