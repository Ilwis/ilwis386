#pragma once

#include <gl/glu.h>

ILWIS::NewDrawer *createPolygonLayerDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class _export PolygonLayerDrawer : public FeatureLayerDrawer {
	friend class TransparencyFormP;
	friend class BoundaryLineStyleForm;
	public:
		ILWIS::NewDrawer *createPolygonLayerDrawer(DrawerParameters *parms);
		NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const;
		bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;

		PolygonLayerDrawer(DrawerParameters *parms);
		virtual ~PolygonLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		void addDataSource(void *bmap,int options);
		double getAreaAlpha() const;
		void setAreaAlpha(double v);
		bool getShowAreas() const { return showAreas; }
		bool getShowBoundaries() const { return showBoundaries; }
		void setLineStyle(int st);
		void setLineThickness(double thick);
		void setLineColor(const Color& clr);
		void setShowBoundaries(bool yesno);
		void setShowAreas(bool yesno);
		void setHatch(const String& name);
		String getHatchName() const;
		bool getTriangleData(long **data,long** loc);
		GeneralDrawerProperties *getProperties();
		GLUtesselator * getTesselator();

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void setDrawMethod(DrawMethod method=drmINIT);
	
		bool showAreas;
		bool showBoundaries;
		double areaAlpha;
		PolygonProperties lp;
		bool usesTriangleFile;
		long *triData;
		long currentLoc;
		int triaFileSize;
		PolygonProperties properties;
		GLUtesselator * tesselator;		
	};
}