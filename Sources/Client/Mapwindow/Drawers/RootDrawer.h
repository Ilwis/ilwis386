#pragma once;

#include "drawer_n.h"

namespace ILWIS {
class RootDrawer : public AbstractDrawer {
public:
	RootDrawer();
	~RootDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	void draw(bool norecursion = false);
private:
	void calcCanvas();
	void setDataSource(void *);
};
}