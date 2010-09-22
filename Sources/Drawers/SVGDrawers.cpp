#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\SVGElements.h"
#include "drawers\svgdrawers.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;


ILWIS::NewDrawer *createSVGRectangleElement(DrawerParameters *parms) {
	return new SVGRectangleElement(parms);
}

void addSVGDrawers(DrawerInfoVector *infos) {
	infos->push_back(new DrawerInfo("Rectangle","ilwis38",createSVGRectangleElement));
}

SVGRectangleElement::SVGRectangleElement(ILWIS::DrawerParameters *parms) : 
SVGElement(parms, "Rectangle"),
rx(0), 
ry(0), 
rwidth(0), 
rheight(0), 
fillColor(Color(128,128,128)), 
strokeColor(Color(0,0,0))
{
	id = "Rectangle";
}

SVGRectangleElement::SVGRectangleElement(ILWIS::DrawerParameters *parms, const String& name) :
SVGElement(parms, name),
rx(0), 
ry(0), 
rwidth(0), 
rheight(0), 
fillColor(Color(128,128,128)), 
strokeColor(Color(0,0,0))
{
}

void SVGRectangleElement::parse(DOMNode* node) {
	XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map = node->getAttributes();
	if ( map) {
		id = getAttributeValue(map,"id");
		String sfill = getAttributeValue(map, "fill");
		if ( sfill != "")
			fillColor = getColor(sfill);
		
		String sstroke = getAttributeValue(map, "stroke");
		if ( sstroke != "")
			strokeColor = getColor(sstroke);
		String sx = getAttributeValue(map, "x");
		if ( sx != "")
			rx = sx.iVal();
		String sy = getAttributeValue(map, "y");
		if ( sy != "")
			ry = sy.iVal(); 
		String swidth = getAttributeValue(map, "width");
		if ( swidth != "")
			rwidth = swidth.iVal();
		String sheight = getAttributeValue(map, "height");
		if ( sheight != "")
			rheight = sheight.iVal();
		ewidth = rwidth;
		eheight = rheight;
	}
}

bool SVGRectangleElement::draw(bool norecursion, const CoordBounds& cb) const {
	//glTranslated(cb.cMin.x, cb.cMin.y,0,
	glColor4d(fillColor.redP(), fillColor.greenP(), fillColor.blueP(),0);
	//glBegin(GL_QUADS);						
	//	glVertex3f( fx - symbolScale, fy - symbolScale,fz);	
	//	glVertex3f( fx - symbolScale, fy + symbolScale,fz);	
	//	glVertex3f( fx + symbolScale, fy + symbolScale,fz);
	//	glVertex3f( fx + symbolScale, fy - symbolScale,fz);
	//glEnd();
	return true;
}

void SVGRectangleElement::prepare(PreparationParameters *p){
	SVGElement::prepare(p);
}








