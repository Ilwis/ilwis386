#pragma once

#include <gl/gl.h>
#include <gl/glu.h>

namespace ILWIS {
class Polygon;

class _export Triangulator {
public:
	Triangulator(GLUtesselator * _tesselator);
	void getTriangulation(const vector<Coord>& crds, vector<pair<unsigned int, vector<Coord>>>& triangleStrips);

protected:
	GLUtesselator * tesselator;
	static void __stdcall tessBeginData(GLenum type, void * polygon_data);
	static void __stdcall tessEdgeFlag(GLboolean);
	static void __stdcall tessEnd();
	static void __stdcall tessVertexData(void * vertex_data, void * polygon_data);
	static void __stdcall tessCombineData(GLdouble coords[3], void *d[4], GLfloat w[4], void **dataOut, void * polygon_data);
	static void __stdcall tessError(GLenum errno);
};

class _export MapPolygonTriangulator : public Triangulator {
public:
	MapPolygonTriangulator(GLUtesselator * _tesselator);
	virtual ~MapPolygonTriangulator();

	void getTriangulation(ILWIS::Polygon *polygon,	vector<pair<unsigned int, vector<Coord>>>& triangleStrips);
	void getTriangulation(long *buffer, long* count, vector<pair<unsigned int, vector<Coord>>>& triangleStrips);
	long writeTriangleData(ofstream& file, vector<pair<unsigned int, vector<Coord>>>& triangleStrips);
};

}