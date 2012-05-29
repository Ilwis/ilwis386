#pragma once

#include "Engine\Table\Col.h"

namespace ILWIS {

class GroupableDrawer {
public:
	GroupableDrawer();
	virtual ~GroupableDrawer();
	void SetGroupAttribute(const Column & col);
	void SetNoGroup();
	const String getGroupValue(Feature * f) const;

protected:
	bool fUseGroup;
	Column colGroup;
};

}