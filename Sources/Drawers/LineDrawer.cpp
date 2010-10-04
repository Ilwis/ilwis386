#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "drawers\linedrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineDrawer(DrawerParameters *parms) {
	return new LineDrawer(parms);
}

LineDrawer::LineDrawer(DrawerParameters *parms) : 
SimpleDrawer(parms,"LineDrawer"),
thickness(1.0),
linestyle(0xFFFF)
{
}

LineDrawer::LineDrawer(DrawerParameters *parms, const String& name) : 
SimpleDrawer(parms,name),
linestyle(0xFFFF),
thickness(1.0)
{
	drawColor = SysColor(COLOR_WINDOWTEXT);
}

LineDrawer::~LineDrawer() {
	clear();
}

void LineDrawer::clear() {
	for(int i = 0; i < lines.size(); ++i) {
		delete lines.at(i);
	}
	lines.clear();
}

bool LineDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	if (lines.size() == 0)
		return false;
	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cb))
		return false;

	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
	bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilled);
	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	double zscale, zoffset, fakez;


	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());
	glLineWidth(thickness);
	if (linestyle != 0xFFFF) {
		glEnable (GL_LINE_STIPPLE);
		glLineStipple(1,linestyle);
	}

	if ( is3D) {
		zscale = cdrw->getZMaker()->getZScale();
		zoffset = cdrw->getZMaker()->getOffset();
		fakez = getRootDrawer()->getFakeZ();
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		glBegin(GL_LINE_STRIP);
		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			double z = is3D ? c.z : 0;
			glVertex3d( c.x, c.y, z);
		}
		glEnd();
		if ( is3D) {
			if ( extrusion) {
				glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), extrTransparency);
				Coord cOld;
				Coord cStart = points->getAt(0);
				for(int i=0; i<points->size(); ++i) {
					Coordinate c = points->getAt(i);
					if ( !cOld.fUndef()) {
						glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), extrTransparency);
						drawExtrusion(cOld, c, fakez, filledExtr);
						glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());
						glBegin(GL_LINE_STRIP);
						glVertex3d(c.x,c.y,c.z);
						glVertex3d(c.x,c.y,fakez);
						glEnd();
					}
					cOld = c;
				}
				//drawExtrusion(cOld, cStart, cOld.z, filledExtr); 
			}
		}
	}
	if ( is3D) {
		glPopMatrix();
	}
	glDisable (GL_LINE_STIPPLE);
	glLineWidth(1);
	return true;
}

void LineDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

void LineDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}

void LineDrawer::setThickness(float t) {
	thickness = t;
}

void LineDrawer::setLineStyle(int style) {
	linestyle = style;
}





