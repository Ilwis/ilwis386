#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\Rootdrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "geos\algorithm\CGAlgorithms.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PolygonDrawer"), boundary(0), showArea(0), hatch(0), hatchInverse(0) {
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = Color(100,200,255);
	backgroundColor = colorUNDEF;
	areaTransparency = 1;
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), boundary(0), showArea(true), hatch(0), hatchInverse(0) {
	setDrawMethod(NewDrawer::drmRPR);
	areaTransparency = 1;
}

PolygonDrawer::~PolygonDrawer() {
	delete boundary;
}

bool PolygonDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if (triangleStrips.size() == 0 || !fActive)
		return false;
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cb))
			return false;
    bool asQuad =  cbZoom.getArea() * 0.00001 > const_cast<PolygonDrawer *>(this)->cb.getArea() ? true : false;
	if ( !cbZoom.fValid())
		asQuad = false;

	glShadeModel(GL_FLAT);

	double alpha = (1.0 - drawColor.transparencyP()) * getTransparency() * areaTransparency;
	if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
		bool is3D = getRootDrawer()->is3D();
		ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
		double zscale = cdrw->getZMaker()->getZScale();
		double zoffset = cdrw->getZMaker()->getOffset();
		double z0 = cdrw->getZMaker()->getZ0(is3D);

 		if ( hatch) {
			glEnable(GL_POLYGON_STIPPLE);          	// Enable POLYGON STIPPLE
			glPolygonStipple(hatch); 
		}

		if ( is3D) {
			glPushMatrix();
			glScaled(1,1,zscale);
			glTranslated(0,0,zoffset);
		}

		if ( label && label->getParentDrawer()->isActive()) {
			Coord c = label->coord();
			//double xshift = cbZoom.width() / 250.0;
			//double yshift = cbZoom.height() / 250.0;
			//c += Coord(xshift,yshift);
			c.z = z0;
			label->setCoord(c);
		}
		glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), alpha);
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
 		if ( hatch && backgroundColor != colorUNDEF ) {
			glPolygonStipple(hatchInverse); 
			double alphaBG = (1.0 - backgroundColor.transparencyP()) * getTransparency() * areaTransparency;
			glColor4f(backgroundColor.redP(),backgroundColor.greenP(), backgroundColor.blueP(), alphaBG);
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
		}
		if ( is3D) {
			glPopMatrix();
		}
		if ( hatch) {
			glDisable(GL_POLYGON_STIPPLE);
		}
	}

	if ( boundary && showBoundary && (!asQuad || !showArea)) {
		boundary->draw(drawLoop, cbArea);
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

void PolygonDrawer::select(bool yesno) {
	if (yesno)
		specialOptions |= NewDrawer::sdoSELECTED;
	else
		specialOptions &= ~NewDrawer::sdoSELECTED;
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