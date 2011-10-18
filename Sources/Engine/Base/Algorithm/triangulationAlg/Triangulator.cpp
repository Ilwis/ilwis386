#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Engine\Base\Algorithm\triangulationAlg\gpc.h"
#include "Engine\Base\Algorithm\TriangulationAlg\Triangulator.h"

using namespace ILWIS;

//-----------------------------------------------------------
Triangulator::Triangulator() {

}

void Triangulator::getTriangulation(const vector<Coord>& crds, vector<vector<Coord> >& triangleStrips) {
	gpc_vertex_list exteriorBoundary;
	vector<gpc_vertex_list> holes;

	vector<Coordinate> coords(crds.size());
	for(int  i =0 ; i < crds.size(); ++i)
		coords[i] = crds[i];
	CoordinateArraySequence seq;
	seq.setPoints(coords);
	GeometryFactory fact;
	LineString *ring = fact.createLineString(seq);
	exteriorBoundary.num_vertices = ring->getNumPoints() - 1;
	exteriorBoundary.vertex = makeVertexList(ring);
	prepareList(exteriorBoundary, holes,triangleStrips);
	delete [] exteriorBoundary.vertex;

}

gpc_vertex *Triangulator::makeVertexList(const LineString* ring) const{
	int npoints = ring->getNumPoints() - 1;
	gpc_vertex *vertices = new gpc_vertex[npoints];
	for(int j = 0; j < npoints; ++j) {
		Coordinate c = ring->getCoordinateN(j);
		gpc_vertex vertex;
		vertex.x = c.x;
		vertex.y = c.y;
		vertices[j] = vertex;
	}
	return vertices;
}
void Triangulator::prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes, vector<vector<Coord> >& triangleStrips) {

	triangleStrips.clear();
	gpc_polygon polygon;
	polygon.contour = 0;
	polygon.hole = 0;
	polygon.num_contours = 0;

	gpc_add_contour(&polygon,&exteriorBoundary,0);

	for(int i = 0 ; i < holes.size(); ++i) {
		gpc_add_contour(&polygon,&(holes[i]),1);
	}

	gpc_tristrip tristrip;
	tristrip.num_strips = 0;
	tristrip.strip = 0;
	gpc_polygon_to_tristrip(&polygon, &tristrip);

	long count = 2;
	//count += tristrip.num_strips;
	for(int i = 0; i < tristrip.num_strips; ++i) {
		gpc_vertex_list list = tristrip.strip[i];
		int n = list.num_vertices;
		count += n * 2 * 3 + 1;
	}
	triangleStrips.resize(tristrip.num_strips);
	for(int i = 0; i < tristrip.num_strips; ++i) {
		gpc_vertex_list list = tristrip.strip[i];
		int n = list.num_vertices;
		triangleStrips[i].resize(n);
		for(int j = 0; j < n; ++j) {
			gpc_vertex b = list.vertex[j];
			triangleStrips.at(i)[j] = (Coord(b.x, b.y));
		}
	}
}

//-------------------------------------------------------------------------------
MapPolygonTriangulator::MapPolygonTriangulator() : Triangulator() {
}

void MapPolygonTriangulator::getTriangulation(ILWIS::Polygon *polygon, vector<vector<Coord> >& triangleStrips) {
	gpc_vertex_list exteriorBoundary;
	vector<gpc_vertex_list> holes;

	const LineString *ring = polygon->getExteriorRing();
	exteriorBoundary.num_vertices = ring->getNumPoints() - 1;
	exteriorBoundary.vertex = makeVertexList(ring);
	holes.resize(polygon->getNumInteriorRing());
	for(int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		const LineString * ring = polygon->getInteriorRingN(i);
		holes[i].num_vertices = ring->getNumPoints() - 1;
		holes[i].vertex = makeVertexList(ring);
	}
	prepareList(exteriorBoundary, holes,triangleStrips);
	for(int i = 0; i < holes.size(); ++i) {
		delete [] holes[i].vertex	;
	}
	delete [] exteriorBoundary.vertex;
}

void MapPolygonTriangulator::prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes, vector<vector<Coord> >& triangleStrips) {

	triangleStrips.clear();
	gpc_polygon polygon;
	polygon.contour = 0;
	polygon.hole = 0;
	polygon.num_contours = 0;

	gpc_add_contour(&polygon,&exteriorBoundary,0);

	for(int i = 0 ; i < holes.size(); ++i) {
		gpc_add_contour(&polygon,&(holes[i]),1);
	}

	gpc_tristrip tristrip;
	tristrip.num_strips = 0;
	tristrip.strip = 0;
	gpc_polygon_to_tristrip(&polygon, &tristrip);

	long count = 2;
	//count += tristrip.num_strips;
	for(int i = 0; i < tristrip.num_strips; ++i) {
		gpc_vertex_list list = tristrip.strip[i];
		int n = list.num_vertices;
		count += n * 2 * 3 + 1;
	}
	trianglePol = new long[ count ]; // number of pointer plus one long indicating howmany pointers + one for totalsize of block
	trianglePol[0] = count;
	trianglePol[1] = tristrip.num_strips;
	count = 2;
	triangleStrips.resize(tristrip.num_strips);
	for(int i = 0; i < tristrip.num_strips; ++i) {
		gpc_vertex_list list = tristrip.strip[i];
		int n = list.num_vertices;
		trianglePol[count++] = n;
		triangleStrips[i].resize(n);
		for(int j = 0; j < n; ++j) {
			gpc_vertex b = list.vertex[j];
			triangleStrips.at(i)[j] = (Coord(b.x, b.y));
			((double *)(trianglePol + count))[j*3] = b.x;
			((double*)(trianglePol + count))[j*3 + 1] = b.y;
			((double*)(trianglePol + count))[j*3 + 2] = 0;
		}
		count += n * 2 *3;
	}
}


long MapPolygonTriangulator::writeTriangleData(ofstream& file) {
	file.write((char *)trianglePol, trianglePol[0]*4);
	return trianglePol[0];
}

void MapPolygonTriangulator::getTriangulation(long *buffer, long* count, const CoordSystem& csData, const CoordSystem& csView, vector<vector<Coord> >& triangleStrips) {
	long number;
	number = buffer[*count];
	trianglePol = new long[number];
	trianglePol[0] = number;
	memcpy(trianglePol,buffer + *count,number * 4);
	long current = 2;
	triangleStrips.resize(trianglePol[1]);
	bool coordNeedsConversion = csData->fnObj != csView->fnObj;
	for(int i = 0; i < trianglePol[1]; ++i) {
		int n = trianglePol[current++];
		triangleStrips[i].resize(n);
		for(int j = 0; j < n; ++j) {
			double x = ((double *)(trianglePol + current))[j*3];
			double y = ((double *)(trianglePol + current))[j*3 + 1];
			double z = ((double *)(trianglePol + current))[j*3 + 2];
			Coord c(x,y,z);
			if ( coordNeedsConversion)
				c = csView->cConv(csData,c);
			
			triangleStrips.at(i)[j] = c;
		}
		current += n * 2 *3;
	}
	*count += number;
}
