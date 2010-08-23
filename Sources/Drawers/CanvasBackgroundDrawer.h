#pragma once;


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);
class FieldColor;

namespace ILWIS {
class CanvasBackgroundDrawer : public ComplexDrawer {
public:
	CanvasBackgroundDrawer(DrawerParameters *parms);
	~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	void displayOptionOutsideColor(CWnd *parent);
	void displayOptionInsideColor(CWnd *parent);
	void calcCanvas();
	void addDataSource(void *);
	Color inside;
	Color outside;
};

class SetColorForm : public DisplayOptionsForm {
public:
	SetColorForm(const String& title, CWnd *wPar, CanvasBackgroundDrawer *dr, Color* color);
	void apply();
private:
	FieldColor *fc;
	Color* c;

};
}