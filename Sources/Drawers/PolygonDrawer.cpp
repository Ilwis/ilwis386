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

PolygonDrawer::PolygonDrawer(DrawerParameters *parms)
: SimpleDrawer(parms,"PolygonDrawer")
, boundary(0)
, showArea(false)
, showAreasAsQuads(false)
, showBoundariesWhenQuads(true)
, hatch(0)
, hatchInverse(0)
{
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = Color(100,200,255);
	backgroundColor = colorUNDEF;
	areaAlpha = 1;
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name)
: SimpleDrawer(parms,name)
, boundary(0)
, showArea(true)
, showAreasAsQuads(false)
, showBoundariesWhenQuads(true)
, hatch(0)
, hatchInverse(0)
{
	setDrawMethod(NewDrawer::drmRPR);
	areaAlpha = 1;
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

	bool asQuad = false;
	if (showAreasAsQuads) {
		GLint viewport[4];
		double modelview[16];
		double projection[16];
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );
		double posX1, posY1, posZ1;
		double posX2, posY2, posZ2;
		double posX3, posY3, posZ3;
		double posX4, posY4, posZ4;
		gluProject(cb.cMin.x, cb.cMin.y, 0.0, modelview, projection, viewport, &posX1, &posY1, &posZ1);
		gluProject(cb.cMin.x, cb.cMax.y, 0.0, modelview, projection, viewport, &posX2, &posY2, &posZ2);
		gluProject(cb.cMax.x, cb.cMax.y, 0.0, modelview, projection, viewport, &posX3, &posY3, &posZ3);
		gluProject(cb.cMax.x, cb.cMin.y, 0.0, modelview, projection, viewport, &posX4, &posY4, &posZ4);
		double posXMin = min(posX1, min(posX2, min(posX3, posX4)));
		double posXMax = max(posX1, max(posX2, max(posX3, posX4)));
		double posYMin = min(posY1, min(posY2, min(posY3, posY4)));
		double posYMax = max(posY1, max(posY2, max(posY3, posY4)));
		//bool asQuad = 9.0 > (sqr(posXMax - posXMin) + sqr(posYMax - posYMin));
		asQuad = 9.0 > ((posXMax - posXMin) * (posYMax - posYMin));
	}

	glShadeModel(GL_FLAT);

	double alpha = (drawColor.alphaP()) * getAlpha() * areaAlpha;
	if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
		bool is3D = getRootDrawer()->is3D();
		ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
		double zscale = cdrw->getZMaker()->getZScale();
		double zoffset = cdrw->getZMaker()->getOffset();

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
			c.z = 0;
			label->setCoord(c);
		}
		glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), alpha);
		if (showArea) {
			if (asQuad) {
				glBegin(GL_QUADS);
				glVertex3d(cb.cMin.x, cb.cMin.y, 0);
				glVertex3d(cb.cMin.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMin.y, 0);
				glEnd();
			} else {
				for(int i=0; i < triangleStrips.size(); ++i) {
					glBegin(triangleStrips[i].first);
					//glBegin(GL_LINE_STRIP);
					for(int j=0; j < triangleStrips[i].second.size(); ++j) {
						Coord c = triangleStrips[i].second[j];
						double z = cdrw->getZMaker()->getThreeDPossible() & is3D ? c.z : 0;
						glVertex3d(c.x,c.y,z);
					}
					glEnd();
				}
			}
		}
 		if ( hatch && !backgroundColor.fEqual(colorUNDEF)) {
			glPolygonStipple(hatchInverse); 
			double alphaBG = backgroundColor.alphaP() * getAlpha() * areaAlpha;
			glColor4f(backgroundColor.redP(),backgroundColor.greenP(), backgroundColor.blueP(), alphaBG);
			if ( asQuad && showArea) {
				glBegin(GL_QUADS);
				glVertex3d(cb.cMin.x, cb.cMin.y, 0);
				glVertex3d(cb.cMin.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMin.y, 0);
				glEnd();
			} else {
				for(int i=0; i < triangleStrips.size() && showArea; ++i) {
					glBegin(triangleStrips[i].first);
					//glBegin(GL_LINE_STRIP);
					for(int j=0; j < triangleStrips[i].second.size(); ++j) {
						Coord c = triangleStrips[i].second[j];
						double z = cdrw->getZMaker()->getThreeDPossible() & is3D ? c.z : 0;
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

	glShadeModel(GL_SMOOTH);

	if ( boundary && showBoundary) {
		glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 1) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 1) * 0.0005);
		if (!asQuad || !showArea)
			boundary->draw(drawLoop, cbArea);
		else if (asQuad && showBoundariesWhenQuads) {
			ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
			bool is3D = getRootDrawer()->is3D(); 
			bool is3DPossible = cdrw->getZMaker()->getThreeDPossible();
			double zscale, zoffset;
			double alpha = boundary->getAlpha();

			glColor4f(((LineProperties*)boundary->getProperties())->drawColor.redP(),((LineProperties*)boundary->getProperties())->drawColor.greenP(), ((LineProperties*)boundary->getProperties())->drawColor.blueP(),alpha );
			glLineWidth(((LineProperties*)boundary->getProperties())->thickness);
			if (((LineProperties*)boundary->getProperties())->linestyle != 0xFFFF) {
				glEnable (GL_LINE_STIPPLE);
				glLineStipple(1,((LineProperties*)boundary->getProperties())->linestyle);
			}

			if ( is3D) {
				zscale = cdrw->getZMaker()->getZScale();
				zoffset = cdrw->getZMaker()->getOffset();
				glPushMatrix();
				glScaled(1,1,zscale);
				glTranslated(0,0,zoffset);
			}
			if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
				glBegin(GL_LINE_STRIP);
				glVertex3d(cb.cMin.x, cb.cMin.y, 0);
				glVertex3d(cb.cMin.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMax.y, 0);
				glVertex3d(cb.cMax.x, cb.cMin.y, 0);
				glVertex3d(cb.cMin.x, cb.cMin.y, 0);
				glEnd();
			}
			if ( is3D) {
				glPopMatrix();
			}

			glDisable (GL_LINE_STIPPLE);
			glLineWidth(1);

		}
		glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);
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

void PolygonDrawer::setBoundaryDrawer(LineDrawer * _boundary)
{
	boundary = _boundary;
}

void PolygonDrawer::areasActive(bool yesno) {
	showArea = yesno;
}

void PolygonDrawer::boundariesActive(bool active) {
	showBoundary = active;
}

void PolygonDrawer::setAsQuads(bool yesno) {
	showAreasAsQuads = yesno;
}

void PolygonDrawer::setAsQuadsBoundaries(bool yesno) {
	showBoundariesWhenQuads = yesno;
}

void PolygonDrawer::setAreaAlpha(double v) {
	areaAlpha = v;
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
	if (boundary)
		boundary->select(yesno);
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