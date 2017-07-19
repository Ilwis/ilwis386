#pragma once;


ILWIS::NewDrawer *createCanvasBackgroundDrawer(ILWIS::DrawerParameters *parms);
class FieldColor;

namespace ILWIS {
class _export CanvasBackgroundDrawer : public ComplexDrawer {
public:
	enum ColorLocation{clINSIDE2D, clOUTSIDE2D,clINSIDE3D, clOUTSIDE3D, clSKY3D};
	CanvasBackgroundDrawer(DrawerParameters *parms);
	virtual ~CanvasBackgroundDrawer();
	virtual void prepare(PreparationParameters *pp);
	bool draw(const DrawLoop drawLoop, const CoordBounds& cb=CoordBounds()) const;
	Color& getColor(ColorLocation cl);
private:
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
	Color inside2D;
	Color outside2D;
	Color inside3D;
	Color outside3D;
	Color sky3D;
};


}