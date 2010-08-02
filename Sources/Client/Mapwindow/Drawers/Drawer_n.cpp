#include "Client\Headers\formelementspch.h"
#include "Client\Base\events.h"
#include "Client\ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"

#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"

using namespace ILWIS;

ComplexDrawer::ComplexDrawer() {
	init();
}

ComplexDrawer::ComplexDrawer(DrawerParameters *parms, const String& ty) : type(ty), drawcontext(parms->context){
	init();
	parentDrawer = parms->parent;
}

void ComplexDrawer::init() {
	GUID gd;
	CoCreateGuid(&gd);
	WCHAR buf[39];
	::StringFromGUID2(gd,buf,39);
	CString str(buf);
	id = str;
	name = "Unknown";
	drm = drmNOTSET;
	editable = true;
	active = true;
	info = false;
	transparency = 1.0;
	parentDrawer = 0;
	uiCode = NewDrawer::ucALL;;
}

String ComplexDrawer::getType() const {
	return type;
}

ComplexDrawer::~ComplexDrawer() {
	clear();
}

void ComplexDrawer::clear() {
	for(map<String,NewDrawer *>::iterator cur = drawersById.begin(); cur != drawersById.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		delete drw;
	}
	drawers.clear();
	postDrawers.clear();
	preDrawers.clear();
	drawersById.clear();
}

DrawerContext *ComplexDrawer::getDrawerContext() {
	return drawcontext;
}

void ComplexDrawer::addPostDrawer(int order, NewDrawer *drw) {
	if ( !drw )
		return;
	String name("%3d|%S", order, drw->getName());
	map<String, NewDrawer *>::iterator here = postDrawers.find(name);
	if (  here != postDrawers.end())
		delete (*here).second;
	postDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

void ComplexDrawer::addPreDrawer(int order, NewDrawer *drw) {
	if ( !drw )
		return;
	String name("%3d|%S", order, drw->getName());
	map<String, NewDrawer *>::iterator here = preDrawers.find(name);
	if (  here != preDrawers.end())
		delete (*here).second;
	preDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

void ComplexDrawer::draw(bool norecursion){
	if (!isActive())
		return;

	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(norecursion);
		}
	}
	if ( !norecursion) {
		for(int i=0; i < drawers.size(); ++i) {
			if ( drawers[i] && drawers[i]->isActive())
				drawers[i]->draw();
		}
	}
	if ( postDrawers.size() > 0) {
		for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(norecursion);
		}
	}
}

String ComplexDrawer::getId() const{
	return id;
}

void ComplexDrawer::prepare(PreparationParameters *parms){
	setTransparency(parentDrawer->getTransparency());
	for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		(*cur).second->prepare(parms);
	}
	for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		(*cur).second->prepare(parms);
	}

}

String ComplexDrawer::addDrawer(NewDrawer *drw) {
	drawers.push_back(drw);
	drawersById[drw->getId()] = drw;
	return drw->getName();
}

NewDrawer *ComplexDrawer::getDrawer(const String& did) {
	map<String, NewDrawer *>::iterator cur= drawersById.find(did);
	if ( cur != drawersById.end())
		return (*cur).second;
	return NULL;

}

void ComplexDrawer::removeDrawer(const String& did) {
	for(int i=0; i < drawers.size(); ++i) {
		if ( drawers.at(i)->getId() == did ) {
			delete drawers.at(i);
			drawers.erase(drawers.begin() + i);
			drawersById.erase(did);
			break;
		}
	}
	for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw->getId() == did ) {
			preDrawers.erase(did);
			drawersById.erase(did);
			delete drw;
			break;
		}
	}
	for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw->getId() == did ) {
			postDrawers.erase(did);
			drawersById.erase(did);
			delete drw;
			break;
		}
	}
}

NewDrawer *ComplexDrawer::getRootDrawer() const{
	NewDrawer *current = parentDrawer;
	while (current != 0)
		current = current->getParentDrawer();
	return parentDrawer;
}

int ComplexDrawer::getDrawerCount() const{
	return drawers.size();
}
NewDrawer * ComplexDrawer::getDrawer(int index){
	if ( index < drawers.size()) {
		return drawers.at(index);
	}
	return NULL;
}

String ComplexDrawer::getName() const {
	return name;
}

void ComplexDrawer::setName(const String& n) {
	name = n;
}

bool ComplexDrawer::isActive() const {
	return active;
}

void ComplexDrawer::setActive(bool yesno){
	active = yesno;
}

NewDrawer::DrawMethod ComplexDrawer::getDrawMethod() const{
	return drm;
}
void ComplexDrawer::setDrawMethod(DrawMethod method) {
	drm = method;
}

bool  ComplexDrawer::isEditable() const{
	return editable;
}
void  ComplexDrawer::setEditable(bool yesno){
	editable = yesno;
}

bool ComplexDrawer::hasInfo() const {
	return info;
}

void ComplexDrawer::setInfo(bool yesno) {
	info = yesno;
}

NewDrawer *ComplexDrawer::getParentDrawer() const {
	return parentDrawer;
}

double ComplexDrawer::getTransparency() const{
	return transparency;
}

int ComplexDrawer::getUICode() const {
	return uiCode;
}
void ComplexDrawer::setUICode(int code) {
	uiCode = code;
}

void ComplexDrawer::setTransparency(double value){
	if ( value >= 0.0 && value <= 1.0)
		transparency = value;
	else
		throw ErrorObject(String("Wrong transparency value %d", value));
}


HTREEITEM ComplexDrawer::InsertItem(LayerTreeView *tv, HTREEITEM parent,const String& name,const String& icon) {
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, parent);
	return htiDisplayOptions; 
}

HTREEITEM ComplexDrawer::InsertItem(const String& name,const String& icon, DisplayOptionTreeItem *item, int checkstatus){
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = item->getTreeView()->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, item->getParent());
	item->setTreeItem(htiDisplayOptions);
	if ( checkstatus >=0) {
		item->getTreeView()->GetTreeCtrl().SetCheck(htiDisplayOptions, checkstatus );
	}
	item->getTreeView()->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);

	return htiDisplayOptions;
}

//HTREEITEM ComplexDrawer::InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon,int checkStatus,DisplayOptionItemFunc fun,SetChecks *checks){
//	int iImg = IlwWinApp()->iImage(icon);
//	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, parent);
//	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv, this, fun,htiDisplayOptions,checks);
//	if ( checkStatus >=0) {
//		//item->SwitchCheckBox((bool)checkStatus);
//		tv->GetTreeCtrl().SetCheck(htiDisplayOptions, checkStatus );
//	}
//	tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);
//
//	return htiDisplayOptions;
//}
//HTREEITEM ComplexDrawer::InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon,bool checkStatus, SetCheckFunc fun,DisplayOptionItemFunc fun2, NewDrawer *altHandler){
//	int iImg = IlwWinApp()->iImage(icon);
//	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, parent);
//	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv, this, fun, fun2, 0, altHandler);
//	tv->GetTreeCtrl().SetCheck(htiDisplayOptions, checkStatus );
//	item->SwitchCheckBox(checkStatus);
//	tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);
//
//	return htiDisplayOptions;
//}

HTREEITEM ComplexDrawer::findTreeItemByName(LayerTreeView  *tv, HTREEITEM parent, const String& name) const {
	HTREEITEM currentItem = tv->GetTreeCtrl().GetNextItem(parent, TVGN_CHILD);
	while(currentItem != 0) {
	  TVITEM item;
      TCHAR szText[1024];
      item.hItem = currentItem;
      item.mask = TVIF_TEXT | TVIF_HANDLE;
      item.pszText = szText;
      item.cchTextMax = 1024;

      BOOL bWorked = tv->GetTreeCtrl().GetItem(&item);
	  if ( name == item.pszText )
		  return item.hItem;
	  currentItem =tv->GetTreeCtrl().GetNextItem(currentItem, TVGN_NEXT);
	}
	return 0;
}

HTREEITEM ComplexDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	//for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
	//	(*cur).second->configure(tv,parent);
	//}
	//for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
	//	(*cur).second->configure(tv,parent);
	//}
	return parent;
}

void ComplexDrawer::setInfoMode(void *v,LayerTreeView *tv) {
	bool value = *(bool *)v;
	info = value;
}

void ComplexDrawer::prepareChildDrawers(PreparationParameters *parms) {
	for(int i = 0; i < drawers.size(); ++i) {
		NewDrawer *pdrw = drawers.at(i);
		PreparationParameters pp((int)parms->type, parms->dc);
		pdrw->prepare(&pp);
	}
}

//----------------------------------------------------------------------------





