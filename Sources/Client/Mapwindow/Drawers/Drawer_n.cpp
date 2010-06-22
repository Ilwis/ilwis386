#include "headers\toolspch.h"
#include "Client\Base\events.h"
#include "Client\ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
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
	name = "Unknown";
	drm = drmRPR;
	editable = true;
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

void AbstractDrawer::prepare(PreparationParameters *parms){
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

int AbstractDrawer::getDrawerCount() const{
	return drawers.size();
}
NewDrawer * AbstractDrawer::getDrawer(int index){
	if ( index < drawers.size()) {
		return drawers.at(index);
	}
	return NULL;
}

String AbstractDrawer::getName() const {
	return name;
}

bool AbstractDrawer::isActive() const {
	return active;
}

void AbstractDrawer::setActive(bool yesno){
	active = yesno;
}

NewDrawer::DrawMethod AbstractDrawer::getDrawMethod() const{
	return drm;
}
void AbstractDrawer::setDrawMethod(DrawMethod method) {
	drm = method;
}

bool  AbstractDrawer::isEditable() const{
	return editable;
}
void  AbstractDrawer::setEditable(bool yesno){
	editable = yesno;
}

bool AbstractDrawer::hasInfo() const {
	return info;
}

void AbstractDrawer::setInfo(bool yesno) {
	info = yesno;
}

HTREEITEM AbstractDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	String sName = String("Info");
	int iImg = IlwWinApp()->iImage("info");
	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, parent);
	tv->GetTreeCtrl().SetCheck(htiDisplayOptions, active);
	return parent;
}
//----------------------------------------------------------------------------





