#pragma once;


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);
class FieldColor;

namespace ILWIS {
class CanvasBackgroundDrawer : public ComplexDrawer {
public:
	CanvasBackgroundDrawer(DrawerParameters *parms);
	virtual ~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	void displayOptionOutsideColor(CWnd *parent);
	void displayOptionInsideColor(CWnd *parent);
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
	void calcCanvas();
	void addDataSource(void *);
	Color inside2D;
	Color outside2D;
	Color inside3D;
	Color outside3D;
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