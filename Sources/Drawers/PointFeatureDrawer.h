#pragma once

ILWIS::NewDrawer *createPointFeatureDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class PointFeatureDrawer : public PointDrawer {
		public:
		PointFeatureDrawer(ILWIS::DrawerParameters *parms);
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addDataSource(void *f,int options=0);
		Feature *getFeature() const;
	protected:
		PointFeatureDrawer(DrawerParameters *parms, const String& name);
		Feature *feature;
		CoordBounds cb;
	};
}