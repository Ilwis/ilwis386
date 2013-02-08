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
	const String getGroupValue(long iRaw) const;
	const bool fGetUseGroup() const;
	const Column & getGroupAttribute() const;

protected:
	String storeGroupable(const FileName& fnView, const String& parentSection) const;
	void loadGroupable(const FileName& fnView, const String& currentSection);
	bool fUseGroup;
	Column colGroup;
};

}