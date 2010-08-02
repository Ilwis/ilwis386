#pragma once;

#include "ComplexDrawer.h"

namespace ILWIS {
class RootDrawer : public ComplexDrawer {
public:
	RootDrawer();
	~RootDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	void draw(bool norecursion = false);
private:
	void calcCanvas();
	void addDataSource(void *);
};
}