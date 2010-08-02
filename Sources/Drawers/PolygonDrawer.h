#pragma once

ILWIS::NewDrawer *createPolygonDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

class PolygonDrawer : public FeatureDrawer {
	public:
		PolygonDrawer(ILWIS::DrawerParameters *parms);
		~PolygonDrawer();
		virtual void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
	protected:
		PolygonDrawer(DrawerParameters *parms, const String& name);
		void prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes);
		gpc_vertex *makeVertexList(const LineString* ring, bool coordConversion, const CoordSystem& csy) const;

		vector<vector<Coord> > triangleStrips;

	};
}