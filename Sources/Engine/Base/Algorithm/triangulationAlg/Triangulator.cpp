#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Engine\Base\Algorithm\TriangulationAlg\Triangulator.h"
#include "Engine\Drawers\RootDrawer.h"

using namespace ILWIS;

//-----------------------------------------------------------
// struct so that x, y, z are consecutive in memory, to make glu tess happy

struct CoordinateStruct {
	double x;
	double y;
	double z;
	CoordinateStruct()
	{
	};
	CoordinateStruct(double _x, double _y)
		: x(_x)
		, y(_y)
		, z(0)
	{			
	};
	void setCoordinate(double _x, double _y)
	{
		x = _x;
		y = _y;
		z = 0;
	};
};

struct TriangulationData {
	vector<CoordinateStruct> coordinates;
	vector<CoordinateStruct*> coordinatePtrs;
	vector<pair<unsigned int, vector<Coord>>>& triangleStrips;
	TriangulationData(vector<pair<unsigned int, vector<Coord>>>& _triangleStrips)
		: triangleStrips(_triangleStrips)
	{
	}
	~TriangulationData()
	{
		for (vector<CoordinateStruct*>::iterator coordinatePtr = coordinatePtrs.begin(); coordinatePtr != coordinatePtrs.end(); ++coordinatePtr)
			delete(*coordinatePtr);
	}
};

//-----------------------------------------------------------
Triangulator::Triangulator(GLUtesselator * _tesselator)
: tesselator(_tesselator)
{
	gluTessCallback(tesselator, GLU_TESS_BEGIN_DATA, (void (__stdcall *)())&tessBeginData);
	//gluTessCallback(tesselator, GLU_TESS_EDGE_FLAG, (void (__stdcall *)()) tessEdgeFlag);
	gluTessCallback(tesselator, GLU_TESS_VERTEX_DATA, (void (__stdcall *)())tessVertexData);
	//gluTessCallback(tesselator, GLU_TESS_END, (void (__stdcall *)())tessEnd);
	gluTessCallback(tesselator, GLU_TESS_COMBINE_DATA, (void (__stdcall *)())tessCombineData);
	//gluTessCallback(tesselator, GLU_TESS_ERROR, (void (__stdcall *)())tessError);
	gluTessNormal(tesselator, 0, 0, 1);
}

void Triangulator::getTriangulation(const vector<Coord>& crds, vector<pair<unsigned int, vector<Coord>>>& triangleStrips) {
	TriangulationData triangulationData (triangleStrips);

	gluTessBeginPolygon(tesselator, &triangulationData);
	gluTessBeginContour(tesselator);

	triangulationData.coordinates.resize(crds.size());
	long coords_size = 0;
	for (int i = 0; i < crds.size(); ++i) { // closed or open ring? (nr points or nr points -1?)
		const Coord & c = crds[i];
		triangulationData.coordinates[coords_size].setCoordinate(c.x, c.y);
		gluTessVertex(tesselator, (double*)&(triangulationData.coordinates[coords_size]), (void*)&(triangulationData.coordinates[coords_size]));
		++coords_size;
	}
	gluTessEndContour(tesselator);
	gluTessEndPolygon(tesselator);
}

void __stdcall Triangulator::tessBeginData(GLenum type, void * polygon_data)
{
	((TriangulationData*)polygon_data)->triangleStrips.push_back(pair<unsigned int, vector<Coord>>(type, vector<Coord>()));
}

void __stdcall Triangulator::tessEdgeFlag(GLboolean)
{
}

void __stdcall Triangulator::tessEnd()
{
}

void __stdcall Triangulator::tessVertexData(void * vertex_data, void * polygon_data)
{
	((TriangulationData*)polygon_data)->triangleStrips[((TriangulationData*)polygon_data)->triangleStrips.size() - 1].second.push_back(Coord(((CoordinateStruct*)vertex_data)->x, ((CoordinateStruct*)vertex_data)->y));
}

void __stdcall Triangulator::tessCombineData(GLdouble coords[3], void *d[4], GLfloat w[4], void **dataOut, void * polygon_data)
{
	TriangulationData * triangulationData = (TriangulationData*)polygon_data;
	*dataOut = new CoordinateStruct(coords[0], coords[1]);
	triangulationData->coordinatePtrs.push_back((CoordinateStruct*)(*dataOut));
}

void __stdcall Triangulator::tessError(GLenum errno)
{
	switch(errno) {
		case GLU_TESS_COORD_TOO_LARGE:
			printf("GLU_TESS_COORD_TOO_LARGE\n");
			break;
		case GLU_TESS_NEED_COMBINE_CALLBACK:
			printf("GLU_TESS_NEED_COMBINE_CALLBACK\n");
			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------------

MapPolygonTriangulator::MapPolygonTriangulator(GLUtesselator * _tesselator)
: Triangulator(_tesselator)
, trianglePol(0)
{
}

MapPolygonTriangulator::~MapPolygonTriangulator()
{
	delete [] trianglePol;
}

void MapPolygonTriangulator::getTriangulation(ILWIS::Polygon *polygon, vector<pair<unsigned int, vector<Coord>>>& triangleStrips) {

	const LineString *ring = polygon->getExteriorRing();

	TriangulationData triangulationData (triangleStrips);

	gluTessBeginPolygon(tesselator, &triangulationData);
	gluTessBeginContour(tesselator);

	long coords_size = polygon->getExteriorRing()->getNumPoints();
	for(int i = 0; i < polygon->getNumInteriorRing(); ++i)
		coords_size += polygon->getInteriorRingN(i)->getNumPoints();
	triangulationData.coordinates.resize(coords_size);
	coords_size = 0;

	for (int i = 0; i < ring->getNumPoints(); ++i) { // closed or open ring?
		const Coordinate & c = ring->getCoordinateN(i);
		triangulationData.coordinates[coords_size].setCoordinate(c.x, c.y);
		gluTessVertex(tesselator, (double*)&(triangulationData.coordinates[coords_size]), (void*)&(triangulationData.coordinates[coords_size]));
		++coords_size;
	}
	gluTessEndContour(tesselator);
	for(int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		gluTessBeginContour(tesselator);
		const LineString * ring = polygon->getInteriorRingN(i);
		for (int j = 0; j < ring->getNumPoints(); ++j) { // closed or open ring?
			const Coordinate & c = ring->getCoordinateN(j);
			triangulationData.coordinates[coords_size].setCoordinate(c.x, c.y);
			gluTessVertex(tesselator, (double*)&(triangulationData.coordinates[coords_size]), (void*)&(triangulationData.coordinates[coords_size]));
			++coords_size;
		}
		gluTessEndContour(tesselator);
	}
	gluTessEndPolygon(tesselator);

	long count = 2; // first "long" is "total nr of longs", second "long" is "total nr of strips"
	for(int i = 0; i < triangleStrips.size(); ++i) {
		int n = triangleStrips[i].second.size();
		count += n * 2 * 3 + 2; // nr strips * 2 longs per double * 3 components per coordinate (x, y, z) + one long for type of strip + one long for nr vertices in strip
	}
	trianglePol = new long[ count ]; // number of pointer plus one long indicating howmany pointers + one for totalsize of block
	trianglePol[0] = count; // "total nr of longs"
	trianglePol[1] = triangleStrips.size(); // "total nr of strips"
	count = 2; // first "long" is "total nr of longs", second "long" is "total nr of strips"
	for(int i = 0; i < triangleStrips.size(); ++i) {
		int n = triangleStrips[i].second.size();
		trianglePol[count++] = triangleStrips[i].first; // type of strip
		trianglePol[count++] = n; // nr vertices in strip
		for(int j = 0; j < n; ++j) {
			((double *)(trianglePol + count))[j*3] = triangleStrips[i].second[j].x;
			((double*)(trianglePol + count))[j*3 + 1] = triangleStrips[i].second[j].y;
			((double*)(trianglePol + count))[j*3 + 2] = 0;
		}
		count += n * 2 *3;
	}
}

long MapPolygonTriangulator::writeTriangleData(ofstream& file) {
	file.write((char *)trianglePol, trianglePol[0]*4);
	return trianglePol[0];
}

void MapPolygonTriangulator::getTriangulation(long *buffer, long* count, vector<pair<unsigned int, vector<Coord>>>& triangleStrips) {
	long number = buffer[*count];
	trianglePol = new long[number];
	trianglePol[0] = number;
	memcpy(trianglePol, buffer + *count, number * 4);
	long current = 2; // first "long" is "total nr of longs", second "long" is "total nr of strips"
	triangleStrips.resize(trianglePol[1]);
	for(int i = 0; i < trianglePol[1]; ++i) {
		triangleStrips[i].first = trianglePol[current++]; // type of strip
		int n = trianglePol[current++]; // nr vertices in strip
		triangleStrips[i].second.resize(n);
		for(int j = 0; j < n; ++j) {
			double x = ((double *)(trianglePol + current))[j*3];
			double y = ((double *)(trianglePol + current))[j*3 + 1];
			double z = ((double *)(trianglePol + current))[j*3 + 2];
			triangleStrips[i].second[j] = Coord(x,y,z);
		}
		current += n * 2 *3;
	}
	*count += number; // "seek" the triangulation data buffer to the next polygon
}
