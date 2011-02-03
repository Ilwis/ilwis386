#pragma once


class FieldColor;
class FieldIntSliderEx;
class FieldRealSliderEx;
class FieldRepresentation;
class DisplayOptionsLegend;

namespace ILWIS{

class _export SetDrawer : public ComplexDrawer {

	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };
		ILWIS::NewDrawer *createSetDrawer(DrawerParameters *parms);

		SetDrawer(DrawerParameters *parms, const String& name);
		virtual ~SetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		Representation getRepresentation() const;
		virtual void setRepresentation(const Representation& rp);
		bool isLegendUsefull() const;
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
			
	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void displayOptionStretch(CWnd *parent);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;

		//BaseMap basemap;
		Representation rpr;
		CoordSystem csy;
		DrawingColor *drawColor;
		SetChecks *colorCheck;
		RangeReal rrStretch;
		RangeInt riStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		DisplayOptionsLegend *doLegend;

		HTREEITEM rprItem;
		HTREEITEM portrayalItem;
		HTREEITEM threeDItem;
	};

	class RepresentationForm : public DisplayOptionsForm {
		public:
		RepresentationForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		String rpr;
		FieldRepresentation *fldRpr;

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

}