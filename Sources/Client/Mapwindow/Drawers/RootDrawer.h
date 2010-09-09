#pragma once;

#include "ComplexDrawer.h"

class MapCompositionDoc;

namespace ILWIS {
class RootDrawer : public ComplexDrawer {
public:
	RootDrawer(MapCompositionDoc *doc);
	~RootDrawer();
	virtual void prepare(PreparationParameters *pp);
	String addDrawer(NewDrawer *drw);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
	void timedEvent(UINT timerID);
	void store(const FileName& fnView, const String parenSection, SubType subtype) const;
	void load(const FileName& fnView, const String parenSection);
private:
	void calcCanvas();
	void addDataSource(void *);
	void SetthreeD(void *v, LayerTreeView *tv); 
};
}