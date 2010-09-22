#include "Headers\toolspch.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "Client\ilwis.h"
#include "Engine\base\system\engine.h"
#include <map>
#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Client\Mapwindow\Drawers\SVGElements.h"


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

map<String, Color> SVGElement::svgcolors;

SVGElement::SVGElement() : eheight(0), ewidth(0){
	if ( svgcolors.size() == 0) {
		initSvgData();
	}
}

void SVGElement::initSvgData() {
	svgcolors["red"] = Color(255,0,0);
	svgcolors["green"] = Color(0,255,0);
	svgcolors["blue"] = Color(0,0,255);
	svgcolors["black"] = Color(0,0,0);
	svgcolors["white"] = Color(255,255,255);

}

Color SVGElement::getColor(const String& name) const{
	map<String, Color>::const_iterator cur;
	if (( cur = svgcolors.find(name)) != svgcolors.end()) {
		return (*cur).second;
	}
	return Color(0,0,0);
}

String SVGElement::getAttributeValue(XERCES_CPP_NAMESPACE::DOMNamedNodeMap *map, const String& key) const{
	XMLCh *str = XERCES_CPP_NAMESPACE::XMLString::transcode(key.scVal());
	XERCES_CPP_NAMESPACE::DOMNode* attnode = map->getNamedItem(str);
	delete str;
	if ( attnode) {
		String value = CString(attnode->getTextContent());
		return value;
	}
	return "";
}

SVGElement::SVGElement(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name) {
}

void SVGElement::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

bool SVGElement::draw(bool norecursion, const CoordBounds& cbArea) const {
	return true;
}

//---------------------------------------------------

