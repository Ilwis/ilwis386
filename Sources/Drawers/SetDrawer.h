#pragma once


class FieldColor;
class FieldIntSliderEx;
class FieldRealSliderEx;
class FieldRepresentation;
class DisplayOptionsLegend;
class FieldColumn;

namespace ILWIS{

class _export SetDrawer : public ComplexDrawer {
	friend class ExtrusionOptions;

	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };
		ILWIS::NewDrawer *createSetDrawer(DrawerParameters *parms);

		SetDrawer(DrawerParameters *parms, const String& name);
		virtual ~SetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
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
		void SetthreeD(void *v, LayerTreeView *tv);
		String iconName(const String& subtype="?") const ;
		String getInfo(const Coord& crd) const;
		static unsigned long test_count;
		bool draw(bool norecursion, const CoordBounds& cbArea) const;
		virtual void modifyLineStyleItem(LayerTreeView  *tv, bool remove=false) {}	
		HTREEITEM getRprItem() { return rprItem; }
		void insertStretchItem(LayerTreeView  *tv, HTREEITEM parent);
		void displayOptionSubRpr(CWnd *parent);
		void setcheckRpr(void *value, LayerTreeView *);
		void updateLegendItem();
		Column getAtttributeColumn() const;
		void setAttributeColumn(const Column& col );
		bool useAttributeColumn() const;
		void setUseAttributeColumn(bool yesno);
		double getExtrusionTransparency() const;
			
	protected:
		HTREEITEM make3D(bool yeno, LayerTreeView  *tvm);
		void displayZOption3D(CWnd *parent);
		void displayZScaling(CWnd *parent);
		void extrusionOptions(CWnd *p) ;
		void setExtrusion(void *value, LayerTreeView *tree);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void displayOptionStretch(CWnd *parent);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;
		void setColumnCheckumn(void *w, LayerTreeView *view);
		void displayOptionAttColumn(CWnd *w);

		//BaseMap basemap;
		Representation rpr;
		CoordSystem csy;
		Column attColumn;
		bool useAttColumn;
		DrawingColor *drawColor;
		SetChecks *colorCheck;
		RangeReal rrStretch;
		RangeInt riStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		DisplayOptionsLegend *doLegend;
		double extrTransparency;

		HTREEITEM rprItem;
		HTREEITEM portrayalItem;
		HTREEITEM threeDItem;
	};

	class ZDataScaling : public DisplayOptionsForm {
		public:
		ZDataScaling(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		int settransforms(Event *);
		FieldRealSliderEx *sliderScale;
		FieldRealSliderEx *sliderOffset;

		double zoffset;
		double zscale;
	};

	class DisplayZDataSourceForm : public DisplayOptionsForm {
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
	};

	class ExtrusionOptions : public DisplayOptionsForm {
	public:
		ExtrusionOptions(CWnd *p, SetDrawer *fsd);
		void apply();
	private:
		int setTransparency(Event *ev);
		int line;
		int transparency;
		RadioGroup *rg;
		FieldIntSliderEx *slider;

	};

	class SetStretchForm : public DisplayOptionsForm {
		public:
		SetStretchForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		RangeReal rr;
		FieldRealSliderEx *sliderLow;
		FieldRealSliderEx *sliderHigh;
		double low, high;

		int check(Event *);

	};

	class ChooseAttributeColumnForm : public DisplayOptionsForm {
		public:
		ChooseAttributeColumnForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		Table attTable;
		String attColumn;
		FieldColumn *fc;
	};

}