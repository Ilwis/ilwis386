#pragma once

ILWIS::NewDrawer *createPointFeatureDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export PointFeatureDrawer : public PointDrawer {
		public:
		PointFeatureDrawer(ILWIS::DrawerParameters *parms);
		PointFeatureDrawer();
		virtual bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addDataSource(void *f,int options=0);
		virtual Feature *getFeature() const;
		void* getDataSource() const;
	protected:
		PointFeatureDrawer(DrawerParameters *parms, const String& name);
		Feature *feature;
		long getRaw() const;
		CoordBounds cb;
		bool fRawEnabled;
		bool fColorVisible;
		bool fContainedInStretchRange;
	};
}