
#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SelectionRectangle.h"
#include "Client\Mapwindow\MapPaneView.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSelectionRectangle(DrawerParameters *parms) {
	return new SelectionRectangle(parms);
}

SelectionRectangle::SelectionRectangle(DrawerParameters *parms) : 
SimpleDrawer(parms, "SelectionRectangle")
{
}

SelectionRectangle::~SelectionRectangle() {
}

bool SelectionRectangle::draw(bool norecursion , const CoordBounds& cb) const{
	glBegin(GL_LINE_STRIP);
	glVertex2d(c1.x, c1.y);
	glVertex2d(c1.x, c2.y);
	glVertex2d(c2.x, c2.y);
	glVertex2d(c2.x, c1.y);
	glVertex2d(c1.x, c1.y);
	glEnd();

	return true;
}

void SelectionRectangle::calcWorldCoordinates(const CRect & rctZoom) {
	c1 = getDrawerContext()->screenToWorld(RowCol(rctZoom.top,rctZoom.left));
	c2 = getDrawerContext()->screenToWorld(RowCol(rctZoom.bottom,rctZoom.right));
}

void SelectionRectangle::prepare(PreparationType t,CDC *dc){

}

CoordBounds SelectionRectangle::getWorldCoordinates() const {
	return CoordBounds(c1,c2);
}

