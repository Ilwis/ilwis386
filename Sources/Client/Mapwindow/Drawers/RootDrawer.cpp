#include "headers\toolspch.h"
#include "Client\Ilwis.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\CanvasBackgroundDrawer.h"
#include "RootDrawer.h"

using namespace ILWIS;

RootDrawer::RootDrawer() {
    drawcontext = new ILWIS::DrawerContext(this);
	ILWIS::DrawerParameters dp(drawcontext, this);
	ILWIS::PreparationParameters pp(RootDrawer::ptALL,0);
	addPreDrawer(1,IlwWinApp()->getDrawer("CanvasBackgroundDrawer", &pp, &dp));
	addPostDrawer(900,IlwWinApp()->getDrawer("MouseClickInfoDrawer", &pp, &dp));
}

RootDrawer::~RootDrawer() {
	delete drawcontext;
}
void  RootDrawer::prepare(PreparationParameters *pp){
	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {
		if ( getDrawerContext()->initOpenGL(pp->dc)) {
			DrawerParameters dp(getDrawerContext(), this);
		}
	}
	if ( !(pp->type & RootDrawer::ptINITOPENGL))
		ComplexDrawer::prepare(pp);
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
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ComplexDrawer::draw(norecursion);

}

void RootDrawer::addDataSource(void *) {
}








