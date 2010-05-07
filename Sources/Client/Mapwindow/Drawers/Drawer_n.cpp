#include "headers\toolspch.h"
#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"

using namespace ILWIS;

AbstractDrawer::AbstractDrawer(DrawerParameters *parms, const String& ty) : type(ty), drawcontext(parms->context){
	GUID gd;
	CoCreateGuid(&gd);
	WCHAR buf[39];
	::StringFromGUID2(gd,buf,39);
	CString str(buf);
	id = str;
}

String AbstractDrawer::getType() const {
	return type;
}

AbstractDrawer::~AbstractDrawer() {
	clear();
}

void AbstractDrawer::clear() {
	for(int i=0; i < drawers.size(); ++i)
		delete drawers[i];
	drawers.clear();
}

DrawerContext *AbstractDrawer::getDrawerContext() {
	return drawcontext;
}

void AbstractDrawer::draw(bool norecursion){
	if ( !norecursion) {
		for(int i=0; i < drawers.size(); ++i)
			drawers[i]->draw();
	}
}

void AbstractDrawer::prepare(PreparationType t,CDC *dc){
	for(int i=0; i < drawers.size(); ++i)
		drawers[i]->prepare(t,dc);
}

String AbstractDrawer::addDrawer(NewDrawer *drw) {
	drawers.push_back(drw);
	drawersById[drw->getId()] = drw;
	return drw->getId();
}

void AbstractDrawer::removeDrawer(const String& did) {
	for(int i=0; i < drawers.size(); ++i) {
		if ( drawers.at(i)->getId() == did ) {
			delete drawers.at(i);
			drawers.erase(drawers.begin() + i);
			map<String, NewDrawer *>::iterator cur= drawersById.find(did);
			if ( cur != drawersById.end())
				drawersById.erase(cur);
			break;
		}
	}
}

String AbstractDrawer::getId() const{
	return id;
}

NewDrawer *AbstractDrawer::getDrawer(const String& did) {
	map<String, NewDrawer *>::iterator cur= drawersById.find(did);
	if ( cur != drawersById.end())
		return (*cur).second;
	return NULL;

}
//----------------------------------------------------------------------------





