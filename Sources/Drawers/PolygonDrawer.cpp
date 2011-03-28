#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\Rootdrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "geos\algorithm\CGAlgorithms.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PolygonDrawer"), boundary(0), showArea(0) {
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = Color(100,200,255);
	areaTransparency = 1;
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), boundary(0), showArea(true) {
	setDrawMethod(NewDrawer::drmRPR);
	areaTransparency = 1;
}

PolygonDrawer::~PolygonDrawer() {
	delete boundary;
}

bool PolygonDrawer::draw( const CoordBounds& cbArea) const{
	if (triangleStrips.size() == 0 || !fActive)
		return false;
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cb))
			return false;
    bool asQuad =  cbZoom.getArea() * 0.00001 > const_cast<PolygonDrawer *>(this)->cb.getArea() ? true : false;
	glShadeModel(GL_FLAT);

	bool is3D = getRootDrawer()->is3D();
	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	double zscale = cdrw->getZMaker()->getZScale();
	double zoffset = cdrw->getZMaker()->getOffset();
	double z0 = cdrw->getZMaker()->getZ0(is3D);

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset + z0);
	}
	double tr = (1.0 - drawColor.transparencyP()) * getTransparency() * areaTransparency;
	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), tr);
	if ( asQuad && showArea) {
		glBegin(GL_QUADS);
		glVertex3d(cb.cMin.x, cb.cMin.y, 0);
		glVertex3d(cb.cMin.x, cb.cMax.y, 0);
		glVertex3d(cb.cMax.x, cb.cMax.y, 0);
		glVertex3d(cb.cMax.x, cb.cMin.y, 0);
		glEnd();
	} else {
	for(int i=0; i < triangleStrips.size() && showArea; ++i){
			glBegin(GL_TRIANGLE_STRIP);
			//glBegin(GL_LINE_STRIP);
			for(int j=0; j < triangleStrips.at(i).size(); ++j) {
				Coord c = triangleStrips.at(i).at(j);
				double z = cdrw->getZMaker()->getThreeDPossible() & is3D ? c.z : z0;
				glVertex3d(c.x,c.y,z);
			}
			glEnd();
		}
	}
	if ( is3D) {
		glPopMatrix();
	}
	if ( boundary && showBoundary && (!asQuad || !showArea)) {
		boundary->draw( cbArea);
	}
	return true;
}

void PolygonDrawer::setSpecialDrawingOptions(int option, bool add, const vector<int>& coords){
	SimpleDrawer::setSpecialDrawingOptions(option,add,coords);
	if ( boundary)
		boundary->setSpecialDrawingOptions(option, add, coords);
}

void PolygonDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

HTREEITEM PolygonDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void PolygonDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}

LineDrawer *PolygonDrawer::getBoundaryDrawer(){
	return boundary;
}

void PolygonDrawer::areasActive(bool yesno) {
	showArea = yesno;
}

void PolygonDrawer::boundariesActive(bool active) {
	showBoundary = active;
}

void PolygonDrawer::setTransparencyArea(double v) {
	areaTransparency = v;
}

void PolygonDrawer::setlineStyle(int st){
	LineProperties *lp = (LineProperties *)boundary->getProperties();
	lp->linestyle = st;
}
void PolygonDrawer::setlineThickness(double th){
	LineProperties *lp = (LineProperties *)boundary->getProperties();
	lp->thickness = th;
}

void PolygonDrawer::setLineColor(const Color& clr) {
	LineProperties *lp = (LineProperties *)boundary->getProperties();
	lp->drawColor = clr;
}




