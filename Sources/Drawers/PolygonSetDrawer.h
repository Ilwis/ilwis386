#pragma once


ILWIS::NewDrawer *createPolygonSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;

namespace ILWIS{

class PolygonSetDrawer : public FeatureSetDrawer {
	friend class TransparencyFormP;
	public:
		ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms);

		PolygonSetDrawer(DrawerParameters *parms);
		virtual ~PolygonSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		void addDataSource(void *bmap,int options);
		double getTransparencyArea() const;
		void setTransparencyArea(double v);

	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		void displayOptionTransparencyP(CWnd *);
		void setDrawMethod(DrawMethod method=drmINIT);
		void setActiveAreas(void *w, LayerTreeView *view);
		void setActiveBoundaries(void *w, LayerTreeView *view);
		bool showAreas;
		bool showBoundaries;
		double areaTransparency;
		HTREEITEM itemTransparentP;
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
}