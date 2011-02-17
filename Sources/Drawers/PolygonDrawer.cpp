#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\Drawers\Rootdrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PolygonDrawer"), boundary(0), showArea(0) {
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = SysColor(COLOR_BTNFACE);
	areaTransparency = 1;
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), boundary(0), showArea(true) {
	setDrawMethod(NewDrawer::drmRPR);
	areaTransparency = 1;
}

PolygonDrawer::~PolygonDrawer() {
	delete boundary;
}

bool PolygonDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	if (triangleStrips.size() == 0 || !fActive)
		return false;
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cb))
			return false;
    bool asQuad =  cbZoom.getArea() * 0.00001 > const_cast<PolygonDrawer *>(this)->cb.getArea() ? true : false;
	glShadeModel(GL_FLAT);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	double zscale = cdrw->getZMaker()->getZScale();
	double zoffset = cdrw->getZMaker()->getOffset();

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}

	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), (1.0 - drawColor.transparencyP()) * getTransparency() * areaTransparency);
	if ( asQuad) {
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
				double z = is3D ? c.z : 0;
				glVertex3d(c.x,c.y,z);
			}
			glEnd();
		}
	}
	if ( is3D) {
		glPopMatrix();
	}
	if ( boundary && showBoundary && !asQuad) {
		boundary->draw(norecursion, cbArea);
	}
	return true;
}

void PolygonDrawer::setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords){
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
	boundary->setLineStyle(st);
}
void PolygonDrawer::setlineThickness(double th){
	boundary->setThickness(th);
}

void PolygonDrawer::setLineColor(const Color& clr) {
	boundary->setDrawColor(clr);
}




