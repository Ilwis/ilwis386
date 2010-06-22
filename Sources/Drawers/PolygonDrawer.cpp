#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "drawers\polygondrawer.h"
#include "geos\algorithm\CGAlgorithms.h"

using namespace ILWIS;

void CALLBACK vertexCallback(void *vertex)
{
	const double *pointer;

	pointer = (double *) vertex;
	double x = pointer[0];
	double y = pointer[1];
	double z = pointer[2];
	double r = pointer[3];
	double g = pointer[4];
	double b = pointer[5];
	double t = pointer[6];
	glColor4d(r,g,b,t);
	glVertex3d(x,y,z);
}

void CALLBACK combineCallback(double coords[3], 
							  double *vertex_data[4],
							  float weight[4], double **dataOut )
{
	double *vertex;
	int i;

	vertex = (double *) malloc(7 * sizeof(double));
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	for (i = 3; i < 7; i++)
		vertex[i] = weight[0] * vertex_data[0][i] 
	+ weight[1] * vertex_data[1][i]
	+ weight[2] * vertex_data[2][i] 
	+ weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}

void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK endCallback(void)
{
	glEnd();
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const byte *estring;

	estring = gluErrorString(errorCode);
	fprintf (stderr, "Tessellation Error: %s\n", estring);
	//exit (0);
}


ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : FeatureDrawer(parms,"PolygonDrawer"), exterior(0),tesselator(0),listIndex(0) {
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : FeatureDrawer(parms,name), exterior(0),tesselator(0),listIndex(0) {
}

PolygonDrawer::~PolygonDrawer() {
	delete exterior;
	gluDeleteTess(tesselator);
	glDeleteLists(listIndex,1);
}

void PolygonDrawer::draw(bool norecursion) {
	if ( listIndex == 0)
		prepareList();
	glCallList(listIndex);
}

typedef double*  pdouble;

void PolygonDrawer::prepare(PreparationParameters *p){
	FeatureDrawer::prepare(p);
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(p->parentDrawer);
	BaseMap bm = fdr->getBaseMap();
	CoordSystem csy = fdr->getBaseMap()->cs();
	ILWIS::Polygon *polygon = (ILWIS::Polygon *)feature;
	FileName fn = drawcontext->getCoordinateSystem()->fnObj;
	if ( drawcontext->getCoordinateSystem()->fnObj == csy->fnObj) {
		const LineString *line = polygon->getExteriorRing();
		exteriorPoints = line->getNumPoints() - 1;
		exterior = new pdouble[exteriorPoints];
			CoordinateSequence *seq = line->getCoordinates();
			int pp = geos::algorithm::CGAlgorithms::isCCW(line->getCoordinates());
		 for(int i = 0; i <exteriorPoints; ++i)  {
			//Coordinate c = line->getCoordinateN(exteriorPoints - i - 1);
			Coordinate c = line->getCoordinateN(i);
			exterior[i] = new double[7];
			exterior[i][0] = c.x;
			exterior[i][1] = c.y;
			exterior[i][2] = c.z == rUNDEF ? 0 : c.z;
			exterior[i][3] = (double)color1.red() / 255.0;
			exterior[i][4] = (double)color1.green() / 255.0;
			exterior[i][5] = (double)color1.blue() / 255.0;
			exterior[i][6] = 1;
		}
		for(int i = 0; i < polygon->getNumInteriorRing(); ++i) {
			const LineString * ring = polygon->getInteriorRingN(i);
			CoordinateSequence *seq = ring->getCoordinates();
			int pp = geos::algorithm::CGAlgorithms::isCCW(seq);
			
			int npoints = ring->getNumPoints() - 1;
			holePoints.push_back(npoints);
			double **hole = new pdouble[npoints];
			for(int j = 0; j < npoints; ++j) {
				Coordinate c = ring->getCoordinateN(npoints - j - 1);
				//Coordinate c = ring->getCoordinateN(j);
				hole[j] = new double[7];
				hole[j][0] = c.x;
				hole[j][1] = c.y;
				hole[j][2] =  c.z == rUNDEF ? 0 : c.z;
				hole[j][3] = 1;
				hole[j][4] = 1;
				hole[j][5] = 1;
				hole[j][6] = 0;

			}
			holes.push_back(hole);
		}
	}
	//	else {
	//		//const LineString *line = polygon->getExteriorRing();
	//		//CoordinateSequence *seq = line->getCoordinates();
	//		//exterior = new CoordinateArraySequence(seq->size());
	//		//for(int i = 0; i < seq->size(); ++i) {
	//		//	Coordinate c = seq->getAt(i);
	//		//	exterior->setAt(csy->cConv(drawcontext->getCoordinateSystem(), c),i);
	//		//}
	//		//delete seq;
	//		//for(int i = 0; i < polygon->getNumInteriorRing(); ++i) {
	//		//	const LineString * hole = polygon->getInteriorRingN(i);
	//		//	CoordinateSequence *seq = line->getCoordinates();
	//		//	CoordinateSequence *points = new CoordinateArraySequence(hole->getNumPoints());
	//		//	for(int j=0; j < seq->size(); ++j) {
	//		//		Coordinate c = seq->getAt(i);
	//		//		points->setAt(csy->cConv(drawcontext->getCoordinateSystem(), c),j);
	//		//	}
	//		//	delete seq;
	//		//	holes.push_back(points);
	//		//}
	//	}
}

void PolygonDrawer::prepareList() {

	if( tesselator)
		gluDeleteTess(tesselator);	

	listIndex = glGenLists(2);
	tesselator = gluNewTess();

	gluTessCallback(tesselator, GLU_TESS_VERTEX,
		(void (__stdcall *) ()) &vertexCallback);
	gluTessCallback(tesselator, GLU_TESS_BEGIN,
		(void (__stdcall *) ()) &beginCallback);
	gluTessCallback(tesselator, GLU_TESS_END,
		(void (__stdcall *) ()) &endCallback);
	gluTessCallback(tesselator, GLU_TESS_ERROR,
		(void (__stdcall *) ()) &errorCallback);
	//gluTessCallback(tesselator, GLU_TESS_COMBINE,
	//	(void (__stdcall *) ()) &combineCallback);
	gluTessProperty(tesselator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
	glNewList(listIndex, GL_COMPILE);
	glShadeModel(GL_FLAT);
	glColor3f(1.0,0.0,0.0);
	gluTessBeginPolygon(tesselator,NULL);
	gluTessBeginContour(tesselator);
	for(int i = 0 ; i < exteriorPoints  ; ++i) {
		gluTessVertex(tesselator, exterior[i], exterior[i]);
	}
	gluTessEndContour(tesselator);

	for(int i = 0 ; i < holes.size(); ++i) {
		gluTessBeginContour(tesselator);
		for(int j = 0 ; j < holePoints[j]; ++j)
			gluTessVertex(tesselator,holes[i][j],holes[i][j]);
		gluTessEndContour(tesselator);
	}
	gluTessEndPolygon(tesselator);
	glEndList();
}

void PolygonDrawer::fff() {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "An OpenGL error has occured: " << gluErrorString(error) << std::endl;
	}

}