#include "Client\Headers\formelementspch.h"
#include "Client\Base\OpenGLFont.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h" 
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"




using namespace ILWIS;
ILWIS::NewDrawer *createMouseClickInfoDrawer(DrawerParameters *parms) {
	return new MouseClickInfoDrawer(parms);
}

MouseClickInfoDrawer::MouseClickInfoDrawer(DrawerParameters *parms) : 
	TextSetDrawer(parms,"MouseClickInfoDrawer"),
	hasText(false)
{
	name = id = "MouseClickInfoDrawer";
	setFont(new OpenGLFont("c:\\windows\\fonts\\arial.ttf", 36));
}

MouseClickInfoDrawer::~MouseClickInfoDrawer() {
}

void  MouseClickInfoDrawer::prepare(PreparationParameters *pp){
	TextSetDrawer::prepare(pp);
	sInfo = "";
	for(int i =0; i < maps.size(); ++i) {
		BaseMap bm = maps[i];
		sInfo += bm->sValue(activePoint);
	}
}

bool MouseClickInfoDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	if (getFont() && !activePoint.fUndef())
		getFont()->renderText(activePoint, sInfo);
	return true;
}

void MouseClickInfoDrawer::addDataSource(void *v) {
	BaseMap *pbm = dynamic_cast<BaseMap *>((BaseMap *)v);
	if ( pbm)
		maps.push_back(*pbm);
}

void MouseClickInfoDrawer::setActivePoint(const Coord& c) {
	activePoint = c;
}




