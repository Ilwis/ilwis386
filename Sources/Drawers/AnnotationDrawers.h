#pragma once

ILWIS::NewDrawer *createAnnotationValueLegendDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationClassLegendDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationBorderDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationDrawers(ILWIS::DrawerParameters *parms);

namespace ILWIS {
	class TextLayerDrawer;
	class BoxDrawer;
	struct LineProperties;
	class TextDrawer;

class _export AnnotationDrawers : public ComplexDrawer {
public:
	AnnotationDrawers(DrawerParameters *parms);
	void prepare(PreparationParameters *pp);
};

class _export AnnotationDrawer : public ComplexDrawer{
public:
	AnnotationDrawer(DrawerParameters *parms, const String& name);
protected:
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
};

class _export AnnotationLegendDrawer : public AnnotationDrawer{
public:
	AnnotationLegendDrawer(DrawerParameters *parms, const String& name);
	void setOrientation(bool yesno);
	bool getOrientation() const;
	void setDrawBorder(bool yesno);
	bool getDrawBorder() const;
	void prepare(PreparationParameters *);
	CoordBounds getBox() const;
	void setBox(const CoordBounds& cb);
	bool getUseBackBackground() const;
	void setUseBackground(bool yesno);
	Color getBackgroundColor() const;
	void setBackgroundColor(const Color& clr);
	Domain getDomain() const { return dm; }
	int noOfColumns() const;
	void setNoOfColumns(int n) ;
	bool getIncludeName() const;
	void setInlcudeName(bool yesno);

protected:
	bool draw( const CoordBounds& cbArea) const;
	void setText(const vector<String>& v, int count, const Coord& c) const;
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);

	CoordBounds cbBox;
	bool drawOutsideBox;
	TextLayerDrawer *texts;
	LineProperties lproperties;
	bool vertical;
	bool useBackground;
	Color bgColor;
	Domain dm;
	int columns;
	bool includeName;
	FileName fnName;

};

struct RawInfo {
	RawInfo(int r, const Color& c) : raw(r), clr(c) {}
	int raw;
	Color clr;
};

class _export AnnotationClassLegendDrawer : public AnnotationLegendDrawer
{
public:
	AnnotationClassLegendDrawer(DrawerParameters *parms);
	void setActiveClasses(const vector<int>& rws);
	void getActiveClasses(vector<int>& rws) const;
protected:
	void prepare(PreparationParameters *pp) ;
	bool draw( const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
	vector<RawInfo> raws;
	double maxw ;
	double cellWidth;

};

class _export AnnotationValueLegendDrawer : public AnnotationLegendDrawer{
public:
	AnnotationValueLegendDrawer(DrawerParameters *parms);
protected:
	bool draw( const CoordBounds& cbArea) const;
	void prepare(PreparationParameters *pp);
	vector<String> makeRange(LayerDrawer *dr) const;
	void drawVertical(CoordBounds& cbInner, const RangeReal& rr, double z, const vector<String>& values) const;
	void drawHorizontal(CoordBounds& cbInner, const RangeReal& rr, double z, const vector<String>& values) const;
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);

	int noTicks;
};

class _export AnnotationBorderDrawer : public AnnotationDrawer {
public:
	enum Side{sLEFT, sRIGHT, sTOP, sBOTTOM};

	AnnotationBorderDrawer(DrawerParameters *parms);
	void prepare(PreparationParameters *pp) ;
	bool hasNeatLine() const;
	int getStep() const;
	void setHasNeatLine(bool yesno);
	void setStep(int st);
private:
	bool draw( const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);
	void calcLocations();
	TextDrawer *getTextDrawer(int index, AnnotationBorderDrawer::Side side);
	void setText(double border, AnnotationBorderDrawer::Side side) const;

	double xborder; // percent;
	double yborder; // percent;
	vector<double> ypos;
	vector<double> xpos;
	vector<bool> hasText;
	BoxDrawer *borderBox;
	TextLayerDrawer *texts;
	bool isLatLon;
	bool neatLine;
	int step;

};
}