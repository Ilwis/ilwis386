#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\pointdrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createPointDrawer(DrawerContext *context) {
	return new PointDrawer(context);
}

PointDrawer::PointDrawer(DrawerContext *context) : FeatureDrawer(context,"PointDrawerSimple") {
}

void PointDrawer::draw(){

	glLoadIdentity();	
	glMatrixMode (GL_PROJECTION);
	glOrtho (0, 200, 200, 0, 0, 1);
	glColor3f(1.0,0.0,0.0);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);						// Drawing Using Triangles
		glVertex2f( 0.0f, 1.0f);				// Top
		glVertex2f(-1.0f,-1.0f);				// Bottom Left
		glVertex2f( 1.0f,-1.0f);				// Bottom Right
	glEnd();
	glMatrixMode (GL_MODELVIEW);
}

void PointDrawer::prepare(PreparationType t,CDC *dc){
	if ( t == ptALL || ptGEOMETRY) {
		ILWIS::Point *point = (ILWIS::Point *)feature;
		if ( drawcontext->getCoordinateSystem() == csy) {
			cNorm = *(point->getCoordinate());
		}
		else {
			cNorm = csy->cConv(drawcontext->getCoordinateSystem(), Coord(*(point->getCoordinate())));
		}
	}
}

