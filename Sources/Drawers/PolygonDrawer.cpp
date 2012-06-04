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

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PolygonDrawer"), boundary(0), showArea(0), hatch(0) {
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = Color(100,200,255);
	areaTransparency = 1;
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), boundary(0), showArea(true), hatch(0) {
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
	if ( !cbZoom.fValid())
		asQuad = false;

	glShadeModel(GL_FLAT);

	bool is3D = getRootDrawer()->is3D();
	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	double zscale = cdrw->getZMaker()->getZScale();
	double zoffset = cdrw->getZMaker()->getOffset();
	double z0 = cdrw->getZMaker()->getZ0(is3D);

    // A stipple pattern of a fly
	GLubyte fly[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x80, 0x01, 0xC0, 0x06, 0xC0, 0x03, 0x60,
    0x04, 0x60, 0x06, 0x20, 0x04, 0x30, 0x0C, 0x20,
    0x04, 0x18, 0x18, 0x20, 0x04, 0x0C, 0x30, 0x20,
    0x04, 0x06, 0x60, 0x20, 0x44, 0x03, 0xC0, 0x22,
    0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
    0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
    0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
    0x66, 0x01, 0x80, 0x66, 0x33, 0x01, 0x80, 0xCC,
    0x19, 0x81, 0x81, 0x98, 0x0C, 0xC1, 0x83, 0x30,
    0x07, 0xe1, 0x87, 0xe0, 0x03, 0x3f, 0xfc, 0xc0,
    0x03, 0x31, 0x8c, 0xc0, 0x03, 0x33, 0xcc, 0xc0,
    0x06, 0x64, 0x26, 0x60, 0x0c, 0xcc, 0x33, 0x30,
    0x18, 0xcc, 0x33, 0x18, 0x10, 0xc4, 0x23, 0x08,
    0x10, 0x63, 0xC6, 0x08, 0x10, 0x30, 0x0c, 0x08,
    0x10, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x08};
	if ( hatch) {
		glEnable(GL_POLYGON_STIPPLE);          	// Enable POLYGON STIPPLE
		glPolygonStipple(hatch); 
	}

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
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
	if ( hatch) {
		glDisable(GL_POLYGON_STIPPLE);
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
	if (  p->type & ptRESTORE || p->type & RootDrawer::ptRENDER) {
		if ( properties.hatchName != 0 ) {
			if ( properties.hatchName == "None") {
				hatch = 0;
				return;
			}
			if ( properties.hatchName == sUNDEF) // leave it unchanged
				return;
			const SVGLoader *loader = NewDrawer::getSvgLoader();
			SVGLoader::const_iterator cur = loader->find(properties.hatchName);
			if ( cur == loader->end())
				return;
			hatch = (*cur).second->getHatch();
		}
	}

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

GeneralDrawerProperties *PolygonDrawer::getProperties(){
	return &properties;
}

//----------------------------------------
PolygonProperties::PolygonProperties() : hatchName(sUNDEF) {
}

PolygonProperties::PolygonProperties(PolygonProperties *p) : LineProperties(p), hatchName(p->hatchName) {
}

String PolygonProperties::store(const FileName& fnView, const String& parentSection) const{
	LineProperties::store(fnView,parentSection);
	ObjectInfo::WriteElement(parentSection.c_str(),"HatchName",fnView, hatchName);
	return parentSection;
}

void PolygonProperties::load(const FileName& fnView, const String& parentSection){
	LineProperties::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.c_str(),"HatchName",fnView, hatchName);
}