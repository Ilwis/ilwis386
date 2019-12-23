#pragma once

ILWIS::NewDrawer *createAnnotationValueLegendDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationClassLegendDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationBorderDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationDrawers(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationScaleBarDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createAnnotationNorthArrowDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {
	class TextLayerDrawer;
	class BoxDrawer;
	struct LineProperties;
	class TextDrawer;
	class PointDrawer;

class _export AnnotationDrawers : public ComplexDrawer {
public:
	AnnotationDrawers(DrawerParameters *parms);
	void prepare(PreparationParameters *pp);
	void refreshClassLegends();
	void removeDrawersUsing(NewDrawer *drw);
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);

protected:
};

class _export AnnotationDrawer : public ComplexDrawer{
public:
	AnnotationDrawer(DrawerParameters *parms, const String& name);
	double getScale() const;
	virtual void setScale(double s);
	void setTitle(const String& t);
	String getTitle() const;
	FileName associatedFile() const;
	const bool fUsesDrawer(NewDrawer *drw) const;
protected:
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);
	void setParent(NewDrawer *drw);
	double scale;
	String title;
	ComplexDrawer *dataDrawer;
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
	double getFontScale() const;
	void setFontScale(double v);

protected:
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	void setText(const vector<String>& v, int count, const Coord& c) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);

	CoordBounds cbBox;
	bool drawOutsideBox;
	TextLayerDrawer *texts;
	LineProperties lproperties;
	bool vertical;
	bool useBackground;
	Color bgColor;
	Domain dm;
	IlwisObject::iotIlwisObjectType objType;
	int columns;
	bool includeName;
	FileName fnName;
	double fontScale;
};

struct RawInfo {
	RawInfo() : raw(iUNDEF){}
	RawInfo(int r, const Color& c) : raw(r), clr(c) {}
	int raw;
	Color clr;
};

class AnnotationClassAttributes {
public:
	double alpha;
	const byte * hatch;
	const byte * hatchInverse;
	Color backgroundColor;
};

class _export AnnotationClassLegendDrawer : public AnnotationLegendDrawer
{
public:
	AnnotationClassLegendDrawer(DrawerParameters *parms);
	void setActiveClasses(const vector<int>& rws);
	void getActiveClasses(vector<int>& rws) const;
protected:
	void prepare(PreparationParameters *pp) ;
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);
	vector<RawInfo> raws;
	std::map<int, AnnotationClassAttributes> hatches;
};

class _export AnnotationValueLegendDrawer : public AnnotationLegendDrawer{
public:
	AnnotationValueLegendDrawer(DrawerParameters *parms);
	RangeReal getRange() const;
	void setRange(RangeReal& range);
	double getStep() const;
	void setStep(double s);
protected:
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	void prepare(PreparationParameters *pp);
	vector<String> makeRange(const DomainValueRangeStruct & dvs) const;
	void drawVertical(CoordBounds& cbInner, const RangeReal& rr, const double z, const double rAlpha, const vector<String>& values, const DomainValueRangeStruct & dvs) const;
	void drawHorizontal(CoordBounds& cbInner, const CoordBounds & cbBoxRender, const RangeReal& rr, const double z, const double rAlpha, const vector<String>& values, const DomainValueRangeStruct & dvs) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);

	double rstep;
	RangeReal vrr;
};

class _export AnnotationBorderDrawer : public AnnotationDrawer {
public:
	enum Side{sLEFT, sRIGHT, sTOP, sBOTTOM};

	AnnotationBorderDrawer(DrawerParameters *parms);
	~AnnotationBorderDrawer();
	void prepare(PreparationParameters *pp) ;
	bool hasNeatLine() const;
	bool hasTicks() const;
	int getStep() const;
	void setHasNeatLine(bool yesno);
	void setHasTicks(bool yesno);
	void setStep(int st);
	int getNumberOfDigits() const;
	void setNumberOfDigits(int num);
private:
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);
	void calcLocations();
	TextDrawer *getTextDrawer(int index, AnnotationBorderDrawer::Side side);
	void setText(const CoordBounds & cb, const CoordBounds & cbArea, AnnotationBorderDrawer::Side side, double z) const;
	void drawTicks(const CoordBounds & cb, const CoordBounds & cbArea, AnnotationBorderDrawer::Side side, double z) const;

	vector<double> ypos;
	vector<double> xpos;
	vector<bool> hasText;
	BoxDrawer *borderBox;
	TextLayerDrawer *texts;
	bool isLatLon;
	bool neatLine;
	bool ticks;
	int step;
	int numDigits;
};

class _export AnnotationScaleBarDrawer : public AnnotationDrawer {
public:
	AnnotationScaleBarDrawer(DrawerParameters *parms);
	void prepare(PreparationParameters *pp) ;
	Coord getBegin();
	void setBegin(const Coord& begin);
	double getSize() const;
	void setSize(double w);
	String getUnit() const;
	void setUnit(const String& unit);
	int getTicks() const;
	void setTicks(int t);
	bool getKm() const;
	void setKm(bool k);
	bool getLine() const;
	void setLine(bool l);
	bool getDivideFirstInterval() const;
	void setDivideFirstInterval(bool d);
	bool getMultiLabels() const;
	void setMultiLabels(bool l);
	int getNumberOfDigits() const;
	void setNumberOfDigits(int num);

private:
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);

	double size;
	double height;
	Coord begin;
	int ticks;
	TextLayerDrawer *texts;
	String unit;
	bool isLatLon;
	bool km;
	bool line;
	bool divideFirstInterval;
	bool multiLabels;
	int numDigits;
};

class _export AnnotationNorthArrowDrawer : public AnnotationDrawer {
public:
	AnnotationNorthArrowDrawer(DrawerParameters *parms);
	~AnnotationNorthArrowDrawer();
	void prepare(PreparationParameters *pp) ;
	Coord getBegin();
	void setBegin(const Coord& begin);
	void setArrowType(const String& type);
	String getArrowType() const;
	void setScale(double s);
	void setFollowGraticule(const bool followGraticule);
	bool getFollowGraticule() const;
	
private:
	bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);
	Coord begin;
	String northArrowType;
	bool fFollowGraticule;
	PointDrawer *arrow;
};
}