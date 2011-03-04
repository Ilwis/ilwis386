#pragma once;

#include "Engine\Drawers\Drawer_n.h"

ILWIS::NewDrawer *createMouseClickInfoDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class MouseClickInfoDrawer : public TextSetDrawer {
public:
	MouseClickInfoDrawer(DrawerParameters *parms);
	~MouseClickInfoDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setActivePoint(const Coord& c);
	bool draw( const CoordBounds& cbArea=CoordBounds()) const;
	void addDataSource(void *);
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);

	Coord activePoint;
	vector<NewDrawer *> drawers;
	String sInfo;

	bool hasText;
};
}

