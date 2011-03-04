#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Drawer_n.h"

using namespace ILWIS;

map<String, DrawerCreate> NewDrawer::drawers;
ILWIS::SVGLoader *ILWIS::NewDrawer::svgContainer=0;//  = new ILWIS::SVGLoader();


ILWIS::NewDrawer *NewDrawer::getDrawer(const String& type, const String& subtype, ILWIS::DrawerParameters *c)  {
	if ( type == "")
		return 0;

	String fullType = type + "|" + subtype;
	if ( c != 0) {
		map<String, DrawerCreate>::const_iterator item = NewDrawer::drawers.find(fullType);
		if ( item != drawers.end())
			return ((*item).second)(c);
	}

	map<String, ILWIS::SVGElement *>::iterator svgItem = getSvgLoader()->find(fullType);
	if ( svgItem != getSvgLoader()->end())
		return (ILWIS::NewDrawer *)(*svgItem).second;
	return NULL;
}

ILWIS::NewDrawer *NewDrawer::getDrawer(const String& type, ILWIS::PreparationParameters *parms, ILWIS::DrawerParameters *c)  {
	if ( type == "")
		return 0;

	if ( parms == 0 || parms->typeMapping.size() == 0)
		return getDrawer(type,"Ilwis38",c);
	map<String,String>::iterator item = parms->typeMapping.find(type);
	String subType =  item == parms->typeMapping.end() ? "Ilwis38" : (*item).second;
	return getDrawer(type,subType,c);
}

void NewDrawer::addDrawer(const String& type, const String& subtype, DrawerCreate func) {
	drawers[type + "|" + subtype] = func;
}

SVGLoader* NewDrawer::getSvgLoader()
{ 
	if ( svgContainer == 0)
		svgContainer = new ILWIS::SVGLoader();
	return svgContainer; 
}

