#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "drawers\pointdrawer.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : FeatureDrawer(parms,"PointDrawerSimple") {
	delete parms;
}

void PointDrawer::draw(bool norecursion){
	CoordBounds cb = getDrawerContext()->getCoordBoundsZoom();
	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = 0;
	
	glColor3f(1.0,0.0,0.0);
	double symbolScale = cb.width() / 200;
	glBegin(GL_QUADS);						
		glVertex3f( fx - symbolScale, fy - symbolScale,fz);	
		glVertex3f( fx - symbolScale, fy + symbolScale,fz);	
		glVertex3f( fx + symbolScale, fy + symbolScale,fz);
		glVertex3f( fx + symbolScale, fy - symbolScale,fz);
	glEnd();
	//glMatrixMode (GL_MODELVIEW);
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

