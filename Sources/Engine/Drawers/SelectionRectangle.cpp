#include "Headers\toolspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SelectionRectangle.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSelectionRectangle(DrawerParameters *parms) {
	return new SelectionRectangle(parms);
}

SelectionRectangle::SelectionRectangle(DrawerParameters *parms) : 
SimpleDrawer(parms, "SelectionRectangle"),
clr(Color(0,100,255))
{
}

SelectionRectangle::~SelectionRectangle() {
}

bool SelectionRectangle::draw( const CoordBounds& cb) const{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	bool is3D = getRootDrawer()->is3D();
	double fakeZ = getRootDrawer()->getFakeZ();
	double z = is3D ? fakeZ : 0;
	glColor4d(clr.redP(),clr.greenP(),clr.blueP(),clr.alphaP());
	glBegin(GL_LINE_STRIP);
	glVertex3d(c1.x, c1.y,z);
	glVertex3d(c1.x, c2.y,z);
	glVertex3d(c2.x, c2.y,z);
	glVertex3d(c2.x, c1.y,z);
	glVertex3d(c1.x, c1.y,z);
	glEnd();

	glColor4d(clr.redP() / 4.0,clr.greenP() / 4.0,clr.blueP() / 4.0,clr.alphaP() * 0.1);
	glBegin(GL_QUADS);
	glVertex3d(c1.x, c1.y,z);
	glVertex3d(c1.x, c2.y,z);
	glVertex3d(c2.x, c2.y,z);
	glVertex3d(c2.x, c1.y,z);
	glVertex3d(c1.x, c1.y,z);
	glEnd();
	glDisable(GL_BLEND);

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

void SelectionRectangle::setColor(const Color& _clr) {
	clr = _clr;
}

