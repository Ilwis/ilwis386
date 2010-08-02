#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "drawers\pointdrawer.h"
#include "drawers\PointSymbolDrawer.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointSymbolDrawer(ILWIS::DrawerParameters *parms) {
	return new PointSymbolDrawer(parms);
}

PointSymbolDrawer::PointSymbolDrawer(DrawerParameters *parms) : PointDrawer(parms,"PointSymbolDrawer") {
}

PointSymbolDrawer::PointSymbolDrawer(DrawerParameters *parms, const String& name) : PointDrawer(parms,name) {
}

void PointSymbolDrawer::draw(bool norecursion){
	PointDrawer::draw(norecursion);
	CoordBounds cb = getDrawerContext()->getCoordBoundsZoom();
	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = 0;
	setOpenGLColor();
	
	double symbolScale = cb.width() / 200;
	glBegin(GL_QUADS);						
		glVertex3f( fx - symbolScale, fy - symbolScale,fz);	
		glVertex3f( fx - symbolScale, fy + symbolScale,fz);	
		glVertex3f( fx + symbolScale, fy + symbolScale,fz);
		glVertex3f( fx + symbolScale, fy - symbolScale,fz);
	glEnd();
}

void PointSymbolDrawer::prepare(PreparationParameters *p){
	PointDrawer::prepare(p);
	if ( p->type == ptALL ||  p->type == ptGEOMETRY) {

	}
}