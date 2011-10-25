#pragma once

ILWIS::NewDrawer *createPolygonFeatureDrawer(ILWIS::DrawerParameters *parms);


namespace ILWIS{
	class MapPolygonTriangulator;

class _export PolygonFeatureDrawer : public PolygonDrawer {
	public:
		PolygonFeatureDrawer(ILWIS::DrawerParameters *parms);
		~PolygonFeatureDrawer();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addDataSource(void *f,int options=0);
		Feature *getFeature() const;
		void* getDataSource() const;
		long writeTriangleData(ofstream& file);
		//void readTriangleData(long* data, long* count, bool coordConversion, const CoordSystem& csy);
	protected:
		PolygonFeatureDrawer(DrawerParameters *parms, const String& name);
		//void prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes);
		//gpc_vertex *makeVertexList(const LineString* ring) const;

		Feature *feature;
//		long *trianglePol;
		MapPolygonTriangulator *tri;

	};
}