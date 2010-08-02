#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Drawers\gpc.h"
#include "Drawers\gpc.c"
#include "drawers\polygondrawer.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : FeatureDrawer(parms,"PolygonDrawer") {
	setDrawMethod(NewDrawer::drmRPR);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : FeatureDrawer(parms,name) {
	setDrawMethod(NewDrawer::drmRPR);
}

PolygonDrawer::~PolygonDrawer() {
}

void PolygonDrawer::draw(bool norecursion) {
	if (triangleStrips.size() == 0)
		return;
	if ( !getDrawerContext()->getCoordBoundsZoom().fContains(cb))
		return;
	setOpenGLColor();
	glShadeModel(GL_FLAT);
	for(int i=0; i < triangleStrips.size(); ++i){
	//glBegin(GL_LINE_STRIP);
	glBegin(GL_TRIANGLE_STRIP);
		for(int j=0; j < triangleStrips.at(i).size(); ++j) {
			Coord c = triangleStrips.at(i).at(j);
			glVertex3d(c.x,c.y,0);
		}
	glEnd();
	}
}

void PolygonDrawer::prepare(PreparationParameters *p){
	FeatureDrawer::prepare(p);
	FeatureSetDrawer *fdr = dynamic_cast<FeatureSetDrawer *>(parentDrawer);
	if (  p->type & ptALL ||  p->type & ptGEOMETRY) {
		CoordSystem csy = fdr->getCoordSystem();
		ILWIS::Polygon *polygon = (ILWIS::Polygon *)feature;
		if ( !polygon)
			return;
		cb = polygon->cbBounds();
		gpc_vertex_list exteriorBoundary;
		vector<gpc_vertex_list> holes;
		bool coordNeedsConversion = drawcontext->getCoordinateSystem()->fnObj == csy->fnObj;

		const LineString *ring = polygon->getExteriorRing();
		exteriorBoundary.num_vertices = ring->getNumPoints() - 1;
		exteriorBoundary.vertex = makeVertexList(ring, coordNeedsConversion,csy);
		holes.resize(polygon->getNumInteriorRing());
		for(int i = 0; i < polygon->getNumInteriorRing(); ++i) {
			const LineString * ring = polygon->getInteriorRingN(i);
			holes[i].num_vertices = ring->getNumPoints() - 1;
			holes[i].vertex = makeVertexList(ring,coordNeedsConversion,csy);
		}
		prepareList(exteriorBoundary, holes);
		for(int i = 0; i < holes.size(); ++i) {
			delete [] holes[i].vertex	;
		}
		delete [] exteriorBoundary.vertex;
	}
	if (  p->type & ptALL || p->type & RootDrawer::ptRENDER) {
		setColor(fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod()));
	}
}

gpc_vertex *PolygonDrawer::makeVertexList(const LineString* ring, bool coordNeedsConversion, const CoordSystem& csy) const{
	int npoints = ring->getNumPoints() - 1;
	gpc_vertex *vertices = new gpc_vertex[npoints];
	for(int j = 0; j < npoints; ++j) {
		Coordinate c = ring->getCoordinateN(j);
		if ( coordNeedsConversion)
			c = drawcontext->getCoordinateSystem()->cConv(csy,c);
		gpc_vertex vertex;
		vertex.x = c.x;
		vertex.y = c.y;
		vertices[j] = vertex;
	}
	return vertices;
}
void PolygonDrawer::prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes) {

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

