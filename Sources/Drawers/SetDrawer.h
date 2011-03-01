#pragma once


class FieldColor;
class FieldIntSliderEx;
class FieldRealSliderEx;
class FieldRepresentation;
class DisplayOptionsLegend;
class FieldColumn;

namespace ILWIS{

class DrawingColor;

class _export SetDrawer : public ComplexDrawer {
	friend class ExtrusionOptions;

	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };
		ILWIS::NewDrawer *createSetDrawer(DrawerParameters *parms);

		SetDrawer(DrawerParameters *parms, const String& name);
		virtual ~SetDrawer();
		virtual void prepare(PreparationParameters *pp);
		Representation getRepresentation() const;
		virtual void setRepresentation(const Representation& rp);
		//bool isLegendUsefull() const;
		RangeReal getStretchRangeReal() const;
		void setStretchRangeReal(const RangeReal& rr);
		RangeInt getStretchRangeInt() const;
		void setStretchRangeInt(const RangeInt& rr);
		bool isStretched() const;
		StretchMethod getStretchMethod() const;
		void setStretchMethod(StretchMethod sm);
		ILWIS::DrawingColor *getDrawingColor() const;
		CoordSystem getCoordSystem() const;
		bool useInternalDomain() const;
		String iconName(const String& subtype="?") const ;
		String getInfo(const Coord& crd) const;
		static unsigned long test_count;
		bool draw(bool norecursion, const CoordBounds& cbArea) const;
		//void updateLegendItem();
		Column getAtttributeColumn() const;
		void setAttributeColumn(const Column& col );
		bool useAttributeColumn() const;
		void setUseAttributeColumn(bool yesno);
		double getExtrusionTransparency() const;
		virtual void modifyLineStyleItem(LayerTreeView  *tv, bool remove=false) {}
		void *getDataSource() { return getParentDrawer()->getDataSource(); }
			
	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;

		//BaseMap basemap;
		Representation rpr;
		CoordSystem csy;
		Column attColumn;
		bool useAttColumn;
		DrawingColor *drawColor;
		RangeReal rrStretch;
		RangeInt riStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		double extrTransparency;
	};

	/*class ZDataScaling : public DisplayOptionsForm {
		public:
		ZDataScaling(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		int settransforms(Event *);
		FieldRealSliderEx *sliderScale;
		FieldRealSliderEx *sliderOffset;

		double zoffset;
		double zscale;
	};*/

	/*class DisplayZDataSourceForm : public DisplayOptionsForm {
		public:
		DisplayZDataSourceForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		RadioGroup *rg;
		FieldMap *fmap;

		String colName;
		Table  attTable;
		String mapName;
		int sourceIndex;
		BaseMap bmp;
	};*/

	/*class ExtrusionOptions : public DisplayOptionsForm {
	public:
		ExtrusionOptions(CWnd *p, SetDrawer *fsd);
		void apply();
	private:
		int setTransparency(Event *ev);
		int line;
		int transparency;
		RadioGroup *rg;
		FieldIntSliderEx *slider;

	};*/

	/*class SetStretchForm : public DisplayOptionsForm {
		public:
		SetStretchForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		RangeReal rr;
		FieldRealSliderEx *sliderLow;
		FieldRealSliderEx *sliderHigh;
		double low, high;

		int check(Event *);

	};*/

	//class ChooseAttributeColumnForm : public DisplayOptionsForm {
	//	public:
	//	ChooseAttributeColumnForm(CWnd *wPar, SetDrawer *dr);
	//	void apply(); 
	//private:
	//	Table attTable;
	//	String attColumn;
	//	FieldColumn *fc;
	//};

}