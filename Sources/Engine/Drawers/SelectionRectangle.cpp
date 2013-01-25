#include "Headers\toolspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createSelectionRectangle(DrawerParameters *parms) {
	return new SelectionRectangle(parms);
}

SelectionRectangle::SelectionRectangle(DrawerParameters *parms) : 
SimpleDrawer(parms, "SelectionRectangle"),
clr(Color(0,100,255))
{
	isSupportingDrawer = true;
}

SelectionRectangle::~SelectionRectangle() {
}

bool SelectionRectangle::draw( const CoordBounds& cb) const{
	glPushMatrix(); // GL_MODELVIEW
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	RowCol pixArea (getRootDrawer()->getViewPort());
	glOrtho(0,pixArea.Col,pixArea.Row,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	double z = 0;
	glDisable(GL_DEPTH_TEST);
	glColor4d(clr.redP(),clr.greenP(),clr.blueP(),clr.alphaP());
	glBegin(GL_LINE_STRIP);
	glVertex3d(rectangle.left, rectangle.top, z);
	glVertex3d(rectangle.left, rectangle.bottom, z);
	glVertex3d(rectangle.right, rectangle.bottom, z);
	glVertex3d(rectangle.right, rectangle.top, z);
	glVertex3d(rectangle.left, rectangle.top, z);
	glEnd();

	glColor4d(clr.redP() / 4.0,clr.greenP() / 4.0,clr.blueP() / 4.0,clr.alphaP() * 0.1);
	glBegin(GL_QUADS);
	glVertex3d(rectangle.left, rectangle.top, z);
	glVertex3d(rectangle.left, rectangle.bottom, z);
	glVertex3d(rectangle.right, rectangle.bottom, z);
	glVertex3d(rectangle.right, rectangle.top, z);
	glVertex3d(rectangle.left, rectangle.top, z);
	glEnd();
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // maintain the original state for incidental gluUnProject calls
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	return true;
}

void SelectionRectangle::setRectangle(const CRect & rct) {
	rectangle = rct;
}

void SelectionRectangle::prepare(PreparationType t,CDC *dc){

}

void SelectionRectangle::setColor(const Color& _clr) {
	clr = _clr;
}

