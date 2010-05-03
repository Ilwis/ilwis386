#include "headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "RootDrawer.h"

using namespace ILWIS;

RootDrawer::RootDrawer() {
    drawcontext = new ILWIS::DrawerContext;
}

RootDrawer::~RootDrawer() {
	delete drawcontext;
}
void  RootDrawer::prepare(PreparationType t,CDC *dc){
	bool v1 = t & RootDrawer::ptINITOPENGL;
	bool v2 = t & RootDrawer::ptALL;
	if ( dc && (  v1 || v2 ))
		getDrawerContext()->initOpenGL(dc);
	if ( !(t & RootDrawer::ptINITOPENGL))
		AbstractDrawer::prepare(t,dc); 
}

void RootDrawer::setCoordSystem(const CoordSystem& cs, bool overrule){
	if ( drawcontext)
		drawcontext->setCoordinateSystem(cs, overrule);
}
void RootDrawer::addCoordBounds(const CoordBounds& cb, bool overrule){
	if ( drawcontext)
		drawcontext->setCoordBoundsView(cb, overrule);
}

void RootDrawer::draw(bool norecursion){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	
	double z =0.0;
	glMatrixMode(GL_MODELVIEW);
	double aspect = getDrawerContext()->getAspectRatio();
	RowCol port = getDrawerContext()->getViewPort();
	double shiftx = 1.0, shifty = 1.0;
	if ( aspect < 1.0) {
		
	}
	CoordBounds cb = getDrawerContext()->toWorld(getDrawerContext()->getCoordBoundsView());

	glLoadIdentity();
	glBegin(GL_QUADS);						
		glColor3f(1.0,1.0,1.0);
		glVertex3f(cb.MinX(), cb.MinY(),z);				
		glVertex3f(cb.MinX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MaxY(),z);				
		glVertex3f(cb.MaxX(), cb.MinY(),z);
	glEnd();

	if ( !norecursion) {
		for(int i=0; i < drawers.size(); ++i)
			drawers[i]->draw();
	}
	glFlush();
}

void RootDrawer::setDataSource(void *) {
}




