#pragma once;


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);
class FieldColor;

namespace ILWIS {
class CanvasBackgroundDrawer : public ComplexDrawer {
public:
	enum ColorLocation{clINSIDE2D, clOUTSIDE2D,clINSIDE3D, clOUTSIDE3D};
	CanvasBackgroundDrawer(DrawerParameters *parms);
	virtual ~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	void setCoordSystem(const CoordSystem& cs, bool overrule=false);
	void addCoordBounds(const CoordBounds& cb, bool overrule=true);
	bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
	Color& getColor(ColorLocation cl);
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


}