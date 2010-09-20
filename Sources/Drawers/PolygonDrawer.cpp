#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\Mapwindow\Drawers\Rootdrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "geos\algorithm\CGAlgorithms.h"
#include "drawers\polygondrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"


using namespace ILWIS;

ILWIS::NewDrawer *createPolygonDrawer(DrawerParameters *parms) {
	return new PolygonDrawer(parms);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PolygonDrawer") {
	setDrawMethod(NewDrawer::drmRPR);
	drawColor = SysColor(COLOR_BTNFACE);
}

PolygonDrawer::PolygonDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name) {
	setDrawMethod(NewDrawer::drmRPR);
}

PolygonDrawer::~PolygonDrawer() {
}

bool PolygonDrawer::draw(bool norecursion, const CoordBounds& cbArea) const{
	if (triangleStrips.size() == 0)
		return false;
	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cb))
		return false;

	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glShadeModel(GL_FLAT);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	double zscale = cdrw->getZMaker()->getZScale();
	double zoffset = cdrw->getZMaker()->getOffset();

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}

	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());
	for(int i=0; i < triangleStrips.size(); ++i){
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0; j < triangleStrips.at(i).size(); ++j) {
			Coord c = triangleStrips.at(i).at(j);
			double z = is3D ? c.z : 0;
			glVertex3d(c.x,c.y,z);
		}
		glEnd();
	}
	if ( is3D) {
		glPopMatrix();
	}
	return true;
}

void PolygonDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

HTREEITEM PolygonDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void PolygonDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}



