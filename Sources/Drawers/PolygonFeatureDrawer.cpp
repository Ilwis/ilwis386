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
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Drawers\gpc.h"
#include "Drawers\gpc.c"
#include "drawers\polygondrawer.h"
#include "Drawers\PolygonFeatureDrawer.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonFeatureDrawer(DrawerParameters *parms) {
	return new PolygonFeatureDrawer(parms);
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms) : PolygonDrawer(parms,"PolygonFeatureDrawer") {
	setDrawMethod(NewDrawer::drmRPR);
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms, const String& name) : PolygonDrawer(parms,name) {
	setDrawMethod(NewDrawer::drmRPR);
}

PolygonFeatureDrawer::~PolygonFeatureDrawer() {
}

bool PolygonFeatureDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	return PolygonDrawer::draw(norecursion, cbArea);
}

void PolygonFeatureDrawer::addDataSource(void *f, int options) {
	feature = (Feature *)f;
}

void PolygonFeatureDrawer::prepare(PreparationParameters *p){
	PolygonDrawer::prepare(p);
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
		drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
	}
}

gpc_vertex *PolygonFeatureDrawer::makeVertexList(const LineString* ring, bool coordNeedsConversion, const CoordSystem& csy) const{
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
void PolygonFeatureDrawer::prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes) {

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

