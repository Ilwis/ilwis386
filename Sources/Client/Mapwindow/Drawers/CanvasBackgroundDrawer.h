#pragma once;

#include "Client\MapWindow\Drawers\SimpleDrawer.h"


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {
class CanvasBackgroundDrawer : public SimpleDrawer {
public:
	CanvasBackgroundDrawer(DrawerParameters *parms);
	~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	void draw(bool norecursion = false);
private:
	virtual HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) { return 0;}
	void calcCanvas();
	void addDataSource(void *);
};
}