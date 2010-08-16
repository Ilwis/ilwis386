#pragma once;

#include "Client\Mapwindow\Drawers\Drawer_n.h"

ILWIS::NewDrawer *createMouseClickInfoDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class MouseClickInfoDrawer : public TextSetDrawer {
public:
	MouseClickInfoDrawer(DrawerParameters *parms);
	~MouseClickInfoDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setActivePoint(const Coord& c);
	bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
	void addDataSource(void *);
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	Coord activePoint;
	vector<BaseMap> maps;
	String sInfo;

	bool hasText;
};
}

