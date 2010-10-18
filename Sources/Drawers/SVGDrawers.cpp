#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\SVGElements.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "drawers\pointdrawer.h"
#include "drawers\svgdrawers.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;


ILWIS::NewDrawer *createSVGRectangle(DrawerParameters *parms) {
	return new SVGRectangle(parms);
}

SVGRectangle::SVGRectangle(ILWIS::DrawerParameters *parms) : 
SVGElement(parms, "filled-rectangle")
{
	name = id = "filled-rectangle";
}

SVGRectangle::SVGRectangle(ILWIS::DrawerParameters *parms, const String& name) :
SVGElement(parms, name)
{
}

void SVGRectangle::parse(DOMNode* node) {
	SVGElement::parse(node);
	if ( fillColor == colorUNDEF)
		name = id = "open-rectangle";
}

void SVGRectangle::drawSVG(const CoordBounds& cbElement,const NewDrawer *dr, double z) const {
	PointDrawer *pdrw = (PointDrawer *)dr;
	double scalex = ewidth != 0 ? cbElement.width() / ewidth : 1;
	double scaley = eheight !=0 ? cbElement.height() / eheight : 1;
	double w =  ewidth == 0 ? cbElement.width() : rwidth;
	double h = eheight == 0 ? cbElement.height() : rheight;
	glPushMatrix();
	glTranslated(cbElement.cMin.x, cbElement.cMin.y,0);

	Color fcolor = fillColor == colorUSERDEF ? pdrw->getDrawColor() : fillColor;
	double transp = opacity * pdrw->getTransparency();
	glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), transp);

	if ( fillColor != colorUNDEF) {
		glBegin(GL_QUADS);						
		glVertex3f( 0, 0,z);	
		glVertex3f( 0,h * scalex,z);	
		glVertex3f( w*scaley, h * scalex,z);
		glVertex3f( w*scaley, 0,z);
		glEnd();
	}
	glColor4d(strokeColor.redP(), strokeColor.greenP(), strokeColor.blueP(), transp);
	glBegin(GL_LINE_STRIP);						
	glVertex3f( 0, 0,z);	
	glVertex3f( 0,h * scaley,z);	
	glVertex3f( w*scalex, h * scaley,z);
	glVertex3f( w*scalex, 0,z);
	glVertex3f( 0, 0,z);
	glEnd();
	glPopMatrix();
}

//----------------------------------------------
ILWIS::NewDrawer *createSVGEllipse(DrawerParameters *parms) {
	return new SVGEllipse(parms);
}

SVGEllipse::SVGEllipse(ILWIS::DrawerParameters *parms) : 
SVGElement(parms, "filled-circle"),
cx(iUNDEF),
cy(iUNDEF),
radius(0),
radiusx(0),
radiusy(0)
{
	name = id = "filled-circle";
}

SVGEllipse::SVGEllipse(ILWIS::DrawerParameters *parms, const String& name) :
SVGElement(parms, name),
radius(0),
radiusx(0),
radiusy(0)
{
	cx = cy = iUNDEF;
}

void SVGEllipse::drawSVG(const CoordBounds& cbElement,const NewDrawer *dr, double fz) const {
	PointDrawer *pdrw = (PointDrawer *)dr;
	double scalex = ewidth > 0 ? cbElement.width() / ewidth : 1;
	double scaley = eheight > 0 ? cbElement.height() / eheight : 1;
	double rx =  ewidth <= 0 ? cbElement.width()/2.0 : radiusx;
	double ry = eheight <= 0 ? cbElement.height()/2.0 : radiusy;
	double lcx = cx == iUNDEF ? cbElement.width() / 2.0 : cx;
	double lcy = cy == iUNDEF ? cbElement.height() / 2.0 : cy;
	double r = min(rx,ry);

	glPushMatrix();
	glTranslated(cbElement.cMin.x, cbElement.cMin.y,0);

	Color fcolor = fillColor == colorUSERDEF ? pdrw->getDrawColor() : fillColor;
	double transp = opacity * pdrw->getTransparency();
	glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), transp);
	int i;
	int sections = 20; //number of triangles to use to estimate a circle
	// (a higher number yields a more perfect circle)
	double twoPi =  2.0 * M_PI;
	if ( r != 0)
		rx = ry = r * min(scalex,scaley);
	else if ( rx != 0 && ry != 0) {
		rx *= max(scalex, scaley);
		ry *= max(scaley, scaley);
	}


	if ( fillColor != colorUNDEF) {
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(lcx, lcy, fz); // origin
		for(i = 0; i <= sections;i++) { // make $section number of circles
			glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
				lcy + ry* sin(i * twoPi / sections), fz);
		}
		glEnd();
	}

	Color scolor = strokeColor == colorUNDEF ? pdrw->getDrawColor() : strokeColor;
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);
	glBegin(GL_LINE_LOOP);
	for(i = 0; i <= sections;i++) { // make $section number of circles
		glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
			lcy + ry* sin(i * twoPi / sections), fz);
	}
	glEnd();
	glPopMatrix();
}

void SVGEllipse::parse(DOMNode* node) {
	SVGElement::parse(node);
	XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map = node->getAttributes();
	if ( map) {
		String sradius = getAttributeValue(map, "r");
		if ( sradius != "")
			radius = sradius.rVal();
		String sradiusx = getAttributeValue(map, "rx");
		if ( sradiusx != "")
			radiusx= sradiusx.rVal();
		String sradiusy = getAttributeValue(map, "ry");
		if ( sradiusy != "")
			radiusy= sradiusy.rVal();
		String scx = getAttributeValue(map, "cx");
		if ( scx != "")
			cx = scx.iVal();
		String scy = getAttributeValue(map, "cy");
		if ( scy != ""){
			cy = scy.iVal();
		}
		if ( ewidth == 0)
			ewidth = cx + max(radius, radiusx);
		if ( eheight == 0)
			eheight = cy + max(radius, radiusy);
		if ( cx == iUNDEF && ewidth > 0)
			cx = ewidth / 2.0;
		if ( cy == iUNDEF && eheight > 0)
			cy = eheight / 2.0;
		if ( fillColor == colorUNDEF) {
			name = id = radius != 0 ? "open-circle" : "open-ellipse";
		} else  {
			name = id = radius != 0 ? "filled-circle" : "filled-ellipse";
		}
	}

}
//-----------------------------------------------

//-----------------------------------------------
void addSVGDrawers(DrawerInfoVector *infos) {
	infos->push_back(new DrawerInfo("filled-rectangle","ilwis38",createSVGRectangle));
	infos->push_back(new DrawerInfo("open-rectangle","ilwis38",createSVGRectangle));
	infos->push_back(new DrawerInfo("filled-circle","ilwis38",createSVGEllipse));
	infos->push_back(new DrawerInfo("open-circle","ilwis38",createSVGEllipse));
	infos->push_back(new DrawerInfo("open-ellipse","ilwis38",createSVGEllipse));
	infos->push_back(new DrawerInfo("filled-ellipse","ilwis38",createSVGEllipse));
}









