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