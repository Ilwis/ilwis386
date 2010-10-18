#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\SVGElements.h"
#include "Drawers\SVGDrawers.h"
#include "drawers\pointdrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SVGElements.h"
#include "drawers\svgdrawers.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PointDrawer") ,drw(0) {
	drawColor = SysColor(COLOR_WINDOWTEXT);
}

PointDrawer::PointDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), drw(0){
}

PointDrawer::~PointDrawer() {
	delete drw;
}

void PointDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
	if ( drw == 0){
		drw = new SVGElement();
	}
}

bool PointDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	if ( cNorm.fUndef())
		return false;
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cNorm))
		return false;

	bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
	bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilled);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	double fakez = getRootDrawer()->getFakeZ();

	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = is3D ? cNorm.z : 0;;

	double symbolScale = cbZoom.width() / 200;
	CoordBounds cb(Coord(fx - symbolScale, fy - symbolScale,fz), Coord(fx + symbolScale, fy + symbolScale,fz));
	drw->drawSVG(cb, this,fz);


	if ( is3D) {
		if ( extrusion) {
			if (!filledExtr) {
				glBegin(GL_LINE_STRIP) ;
				glVertex3d(cNorm.x,cNorm.y,0);
				glVertex3d(cNorm.x, cNorm.y,fz);
				glEnd();
			} else {
				glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), extrTransparency);
				drawExtrusion(Coord(fx - symbolScale, fy - symbolScale,fz),Coord(fx - symbolScale, fy + symbolScale, fz),fakez,true); 
				drawExtrusion(Coord(fx - symbolScale, fy + symbolScale,fz),Coord(fx + symbolScale, fy + symbolScale, fz),fakez, true);
				drawExtrusion(Coord(fx + symbolScale, fy + symbolScale,fz),Coord(fx + symbolScale, fy - symbolScale, fz),fakez, true); 
				drawExtrusion(Coord(fx + symbolScale, fy - symbolScale,fz),Coord(fx - symbolScale, fy - symbolScale, fz),fakez, true); 
			}
		}

	}

	return true;
}

HTREEITEM PointDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void PointDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}

Color PointDrawer::getDrawColor() const {
	return drawColor;
}

void PointDrawer::setSymbol(const String& sym) {
	if( drwId != "") {
		drw->removeDrawer(drwId,false);
	}
	symbol = sym;
	drwId = drw->addDrawer(IlwWinApp()->getDrawer(symbol));
}

String PointDrawer::getSymbol() const {
	return symbol;
}



