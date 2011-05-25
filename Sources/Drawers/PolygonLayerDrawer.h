#pragma once


ILWIS::NewDrawer *createPolygonLayerDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class _export PolygonLayerDrawer : public FeatureLayerDrawer {
	friend class TransparencyFormP;
	friend class BoundaryLineStyleForm;
	public:
		ILWIS::NewDrawer *createPolygonLayerDrawer(DrawerParameters *parms);
		NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		bool draw( const CoordBounds& cbArea) const;

		PolygonLayerDrawer(DrawerParameters *parms);
		virtual ~PolygonLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap,int options);
		double getTransparencyArea() const;
		void setTransparencyArea(double v);
		bool getShowAreas() const { return showAreas; }
		bool getShowBoundaries() const { return showBoundaries; }
		void setLineStyle(int st);
		void setLineThickness(double thick);
		void setLineColor(const Color& clr);
		void setShowBoundaries(bool yesno);
		void setShowAreas(bool yesno);
		void getTriangleData(long **data,long** loc);
		GeneralDrawerProperties *getProperties() ;
		

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void setDrawMethod(DrawMethod method=drmINIT);
	
		bool showAreas;
		bool showBoundaries;
		double areaTransparency;
		LineProperties lp;
		bool usesTriangleFile;
		long *triData;
		long currentLoc;
		
	};
}