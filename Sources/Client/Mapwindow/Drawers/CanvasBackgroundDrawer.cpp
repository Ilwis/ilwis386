#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\CanvasBackgroundDrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createCanvasBackgroundDrawer(DrawerParameters *parms) {
	return new CanvasBackgroundDrawer(parms);
}

CanvasBackgroundDrawer::CanvasBackgroundDrawer(DrawerParameters *parms) : ComplexDrawer(parms,"CanvasBackgroundDrawer"){
	id = name = "CanvasBackgroundDrawer";
}

CanvasBackgroundDrawer::~CanvasBackgroundDrawer() {
}
void  CanvasBackgroundDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);

}

void CanvasBackgroundDrawer::draw(bool norecursion){
	CoordBounds cb = getDrawerContext()->getCoordBoundsView();
	glColor3d(0.7,0.7,0.7);
	double z =0.0;
	glBegin(GL_QUADS);						
		glVertex3f(cb.MinX(), cb.MinY(),z);				
		glVertex3f(cb.MinX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MinY(),z);
	glEnd();

	cb = getDrawerContext()->getMapCoordBounds();

	glColor3f(1.0,1.0,1.0);
	glBegin(GL_QUADS);						
		glVertex3f(cb.MinX(), cb.MinY(),z);				
		glVertex3f(cb.MinX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MinY(),z);
	glEnd();
}

