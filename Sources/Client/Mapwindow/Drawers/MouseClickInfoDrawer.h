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
	void draw(bool);
	void addDataSource(void *);
private:
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
	void displayOptionsText(CWnd *parent);
	Coord activePoint;
	vector<BaseMap> maps;
	String sInfo;

	Color clrText;
	String sFaceName;
	short iFHeight;
	short iFWeight;
	short iFStyle;
	short iWidth;
	bool hasText;
};
}

