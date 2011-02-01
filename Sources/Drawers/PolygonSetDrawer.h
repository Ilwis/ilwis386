#pragma once


ILWIS::NewDrawer *createPolygonSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class PolygonSetDrawer : public FeatureSetDrawer {
	friend class TransparencyFormP;
	friend class BoundaryLineStyleForm;
	public:
		ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms);
		NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;

		PolygonSetDrawer(DrawerParameters *parms);
		virtual ~PolygonSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void addDataSource(void *bmap,int options);
		double getTransparencyArea() const;
		void setTransparencyArea(double v);

	protected:
		void displayOptionTransparencyP(CWnd *);
		void setDrawMethod(DrawMethod method=drmINIT);
		void setActiveAreas(void *w, LayerTreeView *view);
		void setActiveBoundaries(void *w, LayerTreeView *view);
		void displayOptionSetLineStyle(CWnd *parent);
		bool showAreas;
		bool showBoundaries;
		double areaTransparency;
		HTREEITEM itemTransparentP;
		Color           linecolor;
		LineDspType		linestyle;
		double			linethickness;
	};

	class TransparencyFormP : public DisplayOptionsForm {
		public:
		TransparencyFormP(CWnd *wPar, PolygonSetDrawer *dr);
		void apply(); 
	private:
		int setTransparency(Event *ev);

		int transparency;
		FieldIntSliderEx *slider;
	};

	class BoundaryLineStyleForm: public DisplayOptionsForm
	{
	public:
		BoundaryLineStyleForm(CWnd *par, PolygonSetDrawer *gdr);
		void apply();
	private:
		FieldReal *fi;
		FieldLineType *flt;
		FieldColor *fc;
	};
}