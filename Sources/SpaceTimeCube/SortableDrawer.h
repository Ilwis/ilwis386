#pragma once

#include "Engine\Table\Col.h"

namespace ILWIS {

class _export SortableDrawer {
public:
	SortableDrawer();
	virtual ~SortableDrawer();
	void SetSortAttribute(const Column & col);
	void SetNoSort();
	const double getSortValue(Feature * f) const;
	const String getSortString(Feature * f) const;
	const Column & getSortAttribute() const;
	const bool fGetUseSort() const;

protected:
	String storeSortable(const FileName& fnView, const String& parentSection) const;
	void loadSortable(const FileName& fnView, const String& currentSection);
	bool fUseSort;
	bool fSortValues;
	Column colSort;
};

}