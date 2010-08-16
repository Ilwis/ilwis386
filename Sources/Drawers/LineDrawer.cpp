#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "drawers\linedrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineDrawer(DrawerParameters *parms) {
	return new LineDrawer(parms);
}

LineDrawer::LineDrawer(DrawerParameters *parms) : 
	SimpleDrawer(parms,"LineDrawer"),
		thickness(1.0)
{
}

LineDrawer::LineDrawer(DrawerParameters *parms, const String& name) : 
	SimpleDrawer(parms,name)
{
	drawColor = SysColor(COLOR_WINDOWTEXT);
}

LineDrawer::~LineDrawer() {
	clear();
}

HTREEITEM LineDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
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
	if ( !getDrawerContext()->getCoordBoundsZoom().fContains(cb))
		return false;

	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	bool is3D = getDrawerContext()->is3D();
	double fakeZ = getDrawerContext()->getFakeZ();

	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());
	glLineWidth(thickness);

	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		glBegin(GL_LINE_STRIP);

		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			c.z = is3D ? fakeZ : 0;
			glVertex3d( c.x, c.y, c.z);	
		}
		glEnd();
	}
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



