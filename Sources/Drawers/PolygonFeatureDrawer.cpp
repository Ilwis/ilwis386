#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\featuredatadrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Base\Algorithm\triangulation\gpc.h"
#include "Engine\Base\Algorithm\Triangulation\Triangulator.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\PolygonFeatureDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonFeatureDrawer(DrawerParameters *parms) {
	return new PolygonFeatureDrawer(parms);
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms) : PolygonDrawer(parms,"PolygonFeatureDrawer") {
	setDrawMethod(NewDrawer::drmRPR);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER , 0);
	boundary = (LineDrawer *)NewDrawer::getDrawer("LineFeatureDrawer", &pp, parms);
	tri = new MapPolygonTriangulator();
}

PolygonFeatureDrawer::PolygonFeatureDrawer(DrawerParameters *parms, const String& name) : PolygonDrawer(parms,name) {
	setDrawMethod(NewDrawer::drmRPR);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER , 0);
	boundary = (LineDrawer *)NewDrawer::getDrawer("LineFeatureDrawer", &pp, parms);
	tri = new MapPolygonTriangulator();
}

PolygonFeatureDrawer::~PolygonFeatureDrawer() {
	delete tri;
}

bool PolygonFeatureDrawer::draw( const CoordBounds& cbArea) const {
	return PolygonDrawer::draw( cbArea);
}

void PolygonFeatureDrawer::addDataSource(void *f, int options) {
	feature = (Feature *)f;
	if ( boundary)
		boundary->addDataSource(feature);
}

Feature *PolygonFeatureDrawer::getFeature() const {
	return feature;
}

void* PolygonFeatureDrawer::getDataSource() const{
	return (void *) feature;
}


void PolygonFeatureDrawer::prepare(PreparationParameters *p){
	PolygonDrawer::prepare(p);
	PolygonLayerDrawer *fdr = dynamic_cast<PolygonLayerDrawer *>(parentDrawer);
	if (  p->type & ptGEOMETRY || p->type & ptRESTORE) {
		CoordSystem csy = fdr->getCoordSystem();
		if ( boundary) {
			boundary->prepare(p);
		}
		ILWIS::Polygon *polygon = (ILWIS::Polygon *)feature;
		if ( !polygon)
			return;
		cb = polygon->cbBounds();
		bool coordNeedsConversion = getRootDrawer()->getCoordinateSystem()->fnObj != csy->fnObj;
		if ( coordNeedsConversion) {
			Coord c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy,cb.cMin);
			Coord c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy,cb.cMax);
			cb = CoordBounds(c1,c2);
		}
		cb.getArea(); // initializes the area
		long *data;
		long *count;
		bool firstTime = fdr->getTriangleData(&data, &count);
		if ( data) {
			if ( firstTime)
				tri->getTriangulation(data, count, csy, getRootDrawer()->getCoordinateSystem(),triangleStrips);
			//readTriangleData(data, count, coordNeedsConversion, csy);
		} else {
			tri->getTriangulation(polygon,triangleStrips);
	/*		gpc_vertex_list exteriorBoundary;
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
			prepareList(exteriorBoundary, holes);
			for(int i = 0; i < holes.size(); ++i) {
				delete [] holes[i].vertex	;
			}
			delete [] exteriorBoundary.vertex;*/
		}

	}
	if ( p->type & NewDrawer::pt3D || p->type & ptRESTORE) {
		ZValueMaker *zmaker = ((ComplexDrawer *)parentDrawer)->getZMaker();
		for(int i = 0; i < triangleStrips.size(); ++i) {
			for(int j = 0; j < triangleStrips.at(i).size(); ++j) {
				Coord &c = triangleStrips.at(i).at(j);
				double zv = zmaker->getValue(c,feature);
				c.z = zv;
			}
		}
		if ( boundary) {
			boundary->prepare(p);
			boundary->setSupportingDrawer(true);

		}
	}
	if (  p->type & ptRESTORE || p->type & RootDrawer::ptRENDER) {
		extrTransparency = fdr->getExtrusionTransparency();
		drawColor = fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod());
		if ( boundary) {
			LineProperties *lp = (LineProperties *)fdr->getProperties();
			boundariesActive(fdr->getShowBoundaries());
			areasActive(fdr->getShowAreas());
			setTransparencyArea(fdr->getTransparencyArea());
			((LineProperties *)boundary->getProperties())->linestyle = lp->linestyle;
			((LineProperties *)boundary->getProperties())->thickness = lp->thickness;
			for(int j =0 ; j < p->filteredRaws.size(); ++j) {
				int raw = p->filteredRaws[j];
				if ( getFeature()->rValue() == abs(raw)) {
					setActive(raw > 0);
				}
			}
			boundary->prepare(p);
			boundary->setSupportingDrawer(true);
			((LineProperties *)boundary->getProperties())->drawColor = lp->drawColor;
		}
	}
}

gpc_vertex *PolygonFeatureDrawer::makeVertexList(const LineString* ring) const{
	//int npoints = ring->getNumPoints() - 1;
	//gpc_vertex *vertices = new gpc_vertex[npoints];
	//for(int j = 0; j < npoints; ++j) {
	//	Coordinate c = ring->getCoordinateN(j);
	//	gpc_vertex vertex;
	//	vertex.x = c.x;
	//	vertex.y = c.y;
	//	vertices[j] = vertex;
	//}
	//return vertices;
	return 0;
}
void PolygonFeatureDrawer::prepareList(gpc_vertex_list& exteriorBoundary, vector<gpc_vertex_list>& holes) {

	//triangleStrips.clear();
	//gpc_polygon polygon;
	//polygon.contour = 0;
	//polygon.hole = 0;
	//polygon.num_contours = 0;

	//gpc_add_contour(&polygon,&exteriorBoundary,0);

	//for(int i = 0 ; i < holes.size(); ++i) {
	//	gpc_add_contour(&polygon,&(holes[i]),1);
	//}

	//gpc_tristrip tristrip;
	//tristrip.num_strips = 0;
	//tristrip.strip = 0;
	//gpc_polygon_to_tristrip(&polygon, &tristrip);

	//long count = 2;
	////count += tristrip.num_strips;
	//for(int i = 0; i < tristrip.num_strips; ++i) {
	//	gpc_vertex_list list = tristrip.strip[i];
	//	int n = list.num_vertices;
	//	LayerDrawer::test_count += n;
	//	count += n * 2 * 3 + 1;
	//}
	//trianglePol = new long[ count ]; // number of pointer plus one long indicating howmany pointers + one for totalsize of block
	//trianglePol[0] = count;
	//trianglePol[1] = tristrip.num_strips;
	//count = 2;
	//triangleStrips.resize(tristrip.num_strips);
	//for(int i = 0; i < tristrip.num_strips; ++i) {
	//	gpc_vertex_list list = tristrip.strip[i];
	//	int n = list.num_vertices;
	//	trianglePol[count++] = n;
	//	triangleStrips[i].resize(n);
	//	for(int j = 0; j < n; ++j) {
	//		gpc_vertex b = list.vertex[j];
	//		triangleStrips.at(i)[j] = (Coord(b.x, b.y));
	//		((double *)(trianglePol + count))[j*3] = b.x;
	//		((double*)(trianglePol + count))[j*3 + 1] = b.y;
	//		((double*)(trianglePol + count))[j*3 + 2] = 0;
	//	}
	//	count += n * 2 *3;
	//}
}

long PolygonFeatureDrawer::writeTriangleData(ofstream& file) {
	//file.write((char *)trianglePol, trianglePol[0]*4);
	//return trianglePol[0];
	return tri->writeTriangleData(file);
}

void PolygonFeatureDrawer::readTriangleData(long *buffer, long* count, bool coordConversion, const CoordSystem& csy) {
	//long number;
	//number = buffer[*count];
	//trianglePol = new long[number];
	//trianglePol[0] = number;
	//memcpy(trianglePol,buffer + *count,number * 4);
	//long current = 2;
	//triangleStrips.resize(trianglePol[1]);
	//for(int i = 0; i < trianglePol[1]; ++i) {
	//	int n = trianglePol[current++];
	//	triangleStrips[i].resize(n);
	//	for(int j = 0; j < n; ++j) {
	//		double x = ((double *)(trianglePol + current))[j*3];
	//		double y = ((double *)(trianglePol + current))[j*3 + 1];
	//		double z = ((double *)(trianglePol + current))[j*3 + 2];
	//		Coord c(x,y,z);
	//		if ( coordConversion)
	//			c = getRootDrawer()->getCoordinateSystem()->cConv(csy,c);
	//		
	//		triangleStrips.at(i)[j] = c;
	//	}
	//	current += n * 2 *3;
	//}
	//*count += number;
}

