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

void PointDrawer::draw(bool norecursion){
	CoordBounds cb = getDrawerContext()->getCoordBounds();
	double fx = -1.0 + 2 * ( cNorm.x - cb.cMin.x ) / cb.width();
	double fy = -1.0 + 2 * ( cNorm.y - cb.cMin.y ) / cb.height();
	double fz = 0;
	CoordBounds canvas = getDrawerContext()->getCanvas();
	fx *= canvas.width() / 2.0;
	fy *= canvas.height() / 2.0;
    
	glLoadIdentity();	
	glMatrixMode (GL_PROJECTION);
	glColor3f(1.0,0.0,0.0);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, 0.0);
	double symbolScale = 0.01;
	glBegin(GL_QUADS);						
		glVertex3f( fx - symbolScale, fy - symbolScale,fz);	
		glVertex3f( fx - symbolScale, fy + symbolScale,fz);	
		glVertex3f( fx + symbolScale, fy + symbolScale,fz);
		glVertex3f( fx + symbolScale, fy - symbolScale,fz);
	glEnd();
	glMatrixMode (GL_MODELVIEW);
}

void PointDrawer::prepare(PreparationType t,CDC *dc){
	if ( t == ptALL || t == ptGEOMETRY) {
		ILWIS::Point *point = (ILWIS::Point *)feature;
		FileName fn = drawcontext->getCoordinateSystem()->fnObj;
		if ( drawcontext->getCoordinateSystem()->fnObj == csy->fnObj) {
			cNorm = *(point->getCoordinate());
		}
		else {
			cNorm = csy->cConv(drawcontext->getCoordinateSystem(), Coord(*(point->getCoordinate())));
		}
	}
}

