#pragma once

ILWIS::NewDrawer *createPolygonFeatureDrawer(ILWIS::DrawerParameters *parms);


namespace ILWIS{
	class MapPolygonTriangulator;

class _export PolygonFeatureDrawer : public PolygonDrawer {
	public:
		PolygonFeatureDrawer(ILWIS::DrawerParameters *parms);
		PolygonFeatureDrawer();
		~PolygonFeatureDrawer();
		virtual bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addDataSource(void *f,int options=0);
		virtual Feature *getFeature() const;
		void* getDataSource() const;
		long writeTriangleData(ofstream& file);
	protected:
		PolygonFeatureDrawer(DrawerParameters *parms, const String& name);

		Feature *feature;
		MapPolygonTriangulator *tri;

	};
}