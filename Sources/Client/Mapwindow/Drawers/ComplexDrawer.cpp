#include "Client\Headers\formelementspch.h"
#include "Client\Base\events.h"
#include "Client\ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\MapPaneView.h"
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

DrawerContext *ComplexDrawer::getDrawerContext() const{
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

bool ComplexDrawer::draw(bool norecursion, const CoordBounds& cb) const{
	if (!isActive())
		return false;

	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(norecursion, cb);
		}
	}
	if ( !norecursion) {
		for(int i=0; i < drawers.size(); ++i) {
			if ( drawers[i] && drawers[i]->isActive())
				drawers[i]->draw(norecursion, cb);
		}
	}
	if ( postDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(norecursion, cb);
		}
	}
	return true;
}

String ComplexDrawer::getId() const{
	return id;
}

void ComplexDrawer::prepare(PreparationParameters *parms){
	if ( parentDrawer)
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

void ComplexDrawer::getDrawers(vector<NewDrawer *>& allDrawers) {
	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur)
		allDrawers.push_back((*cur).second);

	int drCount = getDrawerCount();
	for(int index = 0; index < drCount; ++index)
		allDrawers.push_back(getDrawer(index));

	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur)
		allDrawers.push_back((*cur).second);
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

HTREEITEM ComplexDrawer::set3D(bool yesno,LayerTreeView  *tv, HTREEITEM parent,SetCheckFunc f) {
	threeD = yesno;
	HTREEITEM hti = 0;
	if ( parent != 0) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,f);
		hti = InsertItem("3D","3D",item,threeD);
	}
	for(int i = 0; i < drawers.size(); ++i) {
		ComplexDrawer *pdrw = dynamic_cast<ComplexDrawer *>(drawers.at(i));
		if ( pdrw) {
			pdrw->set3D(yesno, tv);
		}
	}
	return hti;
}
bool ComplexDrawer::is3D() const {
	return threeD;
}

//----------------------------------------------------------------------------

DisplayOptionsForm::DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title) : 
FormBaseDialog(par,title,fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS),
view((LayerTreeView *)par),
drw(dr)
{
}

void DisplayOptionsForm::OnCancel() {
	apply();
}

int DisplayOptionsForm::exec() {
	return 1;
}

void DisplayOptionsForm::apply() {
}

void DisplayOptionsForm::updateMapView() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->mpvGetView()->Invalidate();
}




