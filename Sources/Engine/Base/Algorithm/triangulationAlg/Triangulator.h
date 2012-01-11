#pragma once

namespace ILWIS {
class Polygon;
class RootDrawer;

class _export Triangulator {
public:
	Triangulator();
	void getTriangulation(const vector<Coord>& crds, vector<vector<Coord> >& triangleStrips);

protected:
	gpc_vertex *makeVertexList(const LineString* ring) const;
	virtual void prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes,	vector<vector<Coord> >& triangleStrips);


};

class _export MapPolygonTriangulator : public Triangulator {
public:
	MapPolygonTriangulator();

	void getTriangulation(ILWIS::Polygon *polygon,	vector<vector<Coord> >& triangleStrips);
	void getTriangulation(long *buffer, long* count, const CoordSystem& csData, const RootDrawer* rootDrawer, vector<vector<Coord> >& triangleStrips);
	virtual void prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes,	vector<vector<Coord> >& triangleStrips);
	long writeTriangleData(ofstream& file);

private:
	long *trianglePol;

};

}