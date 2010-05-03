
#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SelectionRectangle.h"
#include "Client\Mapwindow\MapPaneView.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSelectionRectangle(DrawerParameters *parms) {
	return new SelectionRectangle(parms);
}

SelectionRectangle::SelectionRectangle(DrawerParameters *parms) : 
AbstractDrawer(parms, "SelectionRectangle")
{
	/*c1 = Coord(-0.4, -0.2);
	c2 = Coord(+0.6, +0.2);*/
	delete parms;
}

SelectionRectangle::~SelectionRectangle() {
}

void SelectionRectangle::draw(bool norecursion ){
	glBegin(GL_LINE_STRIP);
	glVertex2d(c1.x, c1.y);
	glVertex2d(c1.x, c2.y);
	glVertex2d(c2.x, c2.y);
	glVertex2d(c2.x, c1.y);
	glVertex2d(c1.x, c1.y);
	glEnd();
}

void SelectionRectangle::calcWorldCoordinates(const CRect & rctZoom) {
	/*int port[4];
	glGetIntegerv(GL_VIEWPORT, port);
	double deltax1 = rctZoom.left - port[0];
	double deltay1 = rctZoom.top - port[1];
	double deltax2 = deltax1 + rctZoom.Width();
	double deltay2 = deltay1 + rctZoom.Height();
	c1.x = (2.0*deltax1/port[2]) - 1.0;
	c1.y = 1.0 - (2.0*deltay1/port[3]);
	c2.x = (2.0*deltax2/port[2]) - 1.0;
	c2.y = 1.0 - (2.0*deltay2/port[3]);*/
	c1 = getDrawerContext()->screenToWorld(RowCol(rctZoom.top,rctZoom.left));
	c2 = getDrawerContext()->screenToWorld(RowCol(rctZoom.bottom,rctZoom.right));
}

void SelectionRectangle::prepare(PreparationType t,CDC *dc){

}

CoordBounds SelectionRectangle::getWorldCoordinates() const {
	return CoordBounds(c1,c2);
}

