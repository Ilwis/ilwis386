
#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
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
	bool is3D = getRootDrawer()->is3D();
	double fakeZ = getRootDrawer()->getFakeZ();
	double z = is3D ? fakeZ : 0;
	glColor4d(0,0.5,1,1);
	glBegin(GL_LINE_STRIP);
	glVertex3d(c1.x, c1.y,z);
	glVertex3d(c1.x, c2.y,z);
	glVertex3d(c2.x, c2.y,z);
	glVertex3d(c2.x, c1.y,z);
	glVertex3d(c1.x, c1.y,z);
	glEnd();

	glColor4d(0,0.2,0.5,0.1);
	glBegin(GL_QUADS);
	glVertex3d(c1.x, c1.y,z);
	glVertex3d(c1.x, c2.y,z);
	glVertex3d(c2.x, c2.y,z);
	glVertex3d(c2.x, c1.y,z);
	glVertex3d(c1.x, c1.y,z);
	glEnd();

	return true;
}

void SelectionRectangle::calcWorldCoordinates(const CRect & rctZoom) {
	c1 = getRootDrawer()->screenToWorld(RowCol(rctZoom.top,rctZoom.left));
	c2 = getRootDrawer()->screenToWorld(RowCol(rctZoom.bottom,rctZoom.right));
}

void SelectionRectangle::prepare(PreparationType t,CDC *dc){

}

CoordBounds SelectionRectangle::getWorldCoordinates() const {
	return CoordBounds(c1,c2);
}

