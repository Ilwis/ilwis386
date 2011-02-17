#pragma once

ILWIS::NewDrawer *createPolygonFeatureDrawer(ILWIS::DrawerParameters *parms);


namespace ILWIS{

class PolygonFeatureDrawer : public PolygonDrawer {
	public:
		PolygonFeatureDrawer(ILWIS::DrawerParameters *parms);
		~PolygonFeatureDrawer();
		virtual bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addDataSource(void *f,int options=0);
		Feature *getFeature() const;
		void* getDataSource() const;
		long writeTriangleData(ofstream& file);
		void readTriangleData(long* data, long* count);
	protected:
		PolygonFeatureDrawer(DrawerParameters *parms, const String& name);
		void prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes);
		gpc_vertex *makeVertexList(const LineString* ring, bool coordConversion, const CoordSystem& csy) const;

		Feature *feature;
		long *trianglePol;

	};
}