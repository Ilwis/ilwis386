#pragma once;

#include "drawer_n.h"


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {
class CanvasBackgroundDrawer : public ComplexDrawer {
public:
	CanvasBackgroundDrawer(DrawerParameters *parms);
	~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	void draw(bool norecursion = false);
private:
	void calcCanvas();
	void addDataSource(void *);
};
}