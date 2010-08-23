#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "drawers\pointdrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PointDrawer") {
	drawColor = SysColor(COLOR_WINDOWTEXT);
}

PointDrawer::PointDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name) {
}

void PointDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

bool PointDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	if ( cNorm.fUndef())
		return false;
	CoordBounds cbZoom = getDrawerContext()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cNorm))
		return false;

	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	ZValueMaker *zmaker = cdrw->getZMaker();
	bool is3D = getDrawerContext()->is3D() && zmaker->getThreeDPossible();
	double zscale = zmaker->getZScale();
	double zoffset = zmaker->getOffset();

	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = is3D ? cNorm.z : 0;;

	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}
	
	double symbolScale = cbZoom.width() / 200;
	glBegin(GL_QUADS);						
		glVertex3f( fx - symbolScale, fy - symbolScale,fz);	
		glVertex3f( fx - symbolScale, fy + symbolScale,fz);	
		glVertex3f( fx + symbolScale, fy + symbolScale,fz);
		glVertex3f( fx + symbolScale, fy - symbolScale,fz);
	glEnd();

	if ( is3D) {
		glPopMatrix();
	}

	return true;
}

HTREEITEM PointDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void PointDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}

