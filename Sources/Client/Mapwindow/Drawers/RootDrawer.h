#pragma once;

#include "ComplexDrawer.h"

namespace ILWIS {
class RootDrawer : public ComplexDrawer {
public:
	RootDrawer();
	~RootDrawer();
	virtual void prepare(PreparationParameters *pp);
	String addDrawer(NewDrawer *drw);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
private:
	void calcCanvas();
	void addDataSource(void *);
};
}