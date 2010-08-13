#pragma once


class FieldColor;
class FieldIntSliderEx;
class FieldRealSliderEx;

namespace ILWIS{

class _export SetDrawer : public ComplexDrawer {
	friend class RepresentationForm;
	friend class TransparencyForm;

	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };
		ILWIS::NewDrawer *createSetDrawer(DrawerParameters *parms);

		SetDrawer(DrawerParameters *parms, const String& name);
		virtual ~SetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		Representation getRepresentation() const;
		void setRepresentation(const Representation& rp);
		bool isLegendUsefull() const;
		RangeReal getStretchRangeReal() const;
		void setStretchRangeReal(const RangeReal& rr);
		RangeInt getStretchRangeInt() const;
		void setStretchRangeInt(const RangeInt& rr);
		bool isStretched() const;
		RangeReal getLegendRange() const;
		void setLegendRange(const RangeReal& rr);
		StretchMethod getStretchMethod() const;
		void setStretchMethod(StretchMethod sm);
		ILWIS::DrawingColor *getDrawingColor() const;
		CoordSystem getCoordSystem() const;
		bool useInternalDomain() const;
		void setActiveMode(void *v,LayerTreeView *tv);
	
	protected:
		void setcheckRpr(void *value, LayerTreeView *);
		void displayOptionSubRpr(CWnd *parent);
		void displayOptionStretch(CWnd *parent);
		void displayOptionTransparency(CWnd *parent);
		void insertStretchItem(LayerTreeView  *tv, HTREEITEM parent);
		virtual void modifyLineStyleItem(LayerTreeView  *tv, bool remove=false) {}

		Representation rpr;
		CoordSystem csy;
		DrawingColor *drawColor;
		SetChecks *colorCheck;
		RangeReal rrStretch;
		RangeInt riStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		HTREEITEM rprItem;
		HTREEITEM itemTransparent ;
		HTREEITEM portrayalItem;
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

	class TransparencyForm : public DisplayOptionsForm {
		public:
		TransparencyForm(CWnd *wPar, SetDrawer *dr);
		void apply(); 
	private:
		int transparency;
		FieldIntSliderEx *slider;
	};
}