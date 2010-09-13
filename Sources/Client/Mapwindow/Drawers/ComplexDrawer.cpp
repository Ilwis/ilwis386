#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\Base\events.h"
#include "Client\ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

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
	threeD=false;
	transparency = 1.0;
	parentDrawer = 0;
	uiCode = NewDrawer::ucALL;
	zmaker = new ILWIS::ZValueMaker();
	itemTransparent = 0;
	specialOptions = sdoNone;
}

String ComplexDrawer::getType() const {
	return type;
}

ComplexDrawer::~ComplexDrawer() {
	delete zmaker;
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

void ComplexDrawer::setSpecialDrawingOptions(SpecialDrawingOptions option, bool add){
	if ( add)
		specialOptions |= option;
	else
		specialOptions &= !option;
}

int ComplexDrawer::getSpecialDrawingOption(SpecialDrawingOptions opt) const {
	if ( opt == sdoNone)
		return specialOptions;
	else
		return specialOptions & opt;
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

void ComplexDrawer::timedEvent(UINT timerid) {
}

void ComplexDrawer::setTransparency(double value){
	if ( (value >= 0.0 && value <= 1.0) || value == rUNDEF)
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

void ComplexDrawer::setInfoMode(void *v,LayerTreeView *tv) {
	bool value = *(bool *)v;
	info = value;
}

void ComplexDrawer::prepareChildDrawers(PreparationParameters *parms) {
	for(int i = 0; i < drawers.size(); ++i) {
		NewDrawer *pdrw = drawers.at(i);
		if ( pdrw) {
			PreparationParameters pp((int)parms->type, parms->dc);
			pdrw->prepare(&pp);
		}
	}
}

HTREEITEM ComplexDrawer::set3D(bool yesno,LayerTreeView  *tv) {
	threeD = yesno;
	HTREEITEM hti = 0;
	for(int i = 0; i < drawers.size(); ++i) {
		ComplexDrawer *pdrw = dynamic_cast<ComplexDrawer *>(drawers.at(i));
		if ( pdrw) {
			pdrw->getZMaker()->setThreeDPossible(yesno);
			pdrw->set3D(yesno, tv);
		}
	}
	return hti;
}
bool ComplexDrawer::is3D() const {
	return threeD;
}

ZValueMaker *ComplexDrawer::getZMaker() {
	return zmaker;
}

void ComplexDrawer::store(const FileName& fnView, const String& parentSection) const{
	ObjectInfo::WriteElement(parentSection.scVal(),"UiCode",fnView, uiCode);
	ObjectInfo::WriteElement(parentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.scVal(),"DrawMethod",fnView, drm);
	ObjectInfo::WriteElement(parentSection.scVal(),"Transparency",fnView, transparency);
	ObjectInfo::WriteElement(parentSection.scVal(),"Type",fnView, type);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsActive",fnView, active);
	ObjectInfo::WriteElement(parentSection.scVal(),"editable",fnView, editable);
	ObjectInfo::WriteElement(parentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsThreeD",fnView, threeD);

	int count = 0;
	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		String currentSection("%S%d",parentSection,count);
		NewDrawer *drw = (*cur).second;
		if ( !drw->isSimple() ) {
			drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(parentSection.scVal(),String("PreDrawer%02d",count++).scVal(),fnView, currentSection);
		}
	}
	ObjectInfo::WriteElement(parentSection.scVal(),"PreDrawerCount",fnView, count);
	int drCount = getDrawerCount();
	count = 0; 
	for(int index = 0; index < drCount; ++index) {
		String currentSection("%S%d",parentSection,index);
		NewDrawer *drw = drawers[index];
		if ( !drw->isSimple() ) {
			drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(parentSection.scVal(),String("Drawer%02d",count++).scVal(),fnView, currentSection);
		}

	}
	ObjectInfo::WriteElement(parentSection.scVal(),"DrawerCount",fnView, count);
	count = 0;
	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		String currentSection("%S%d",parentSection,count);
		NewDrawer *drw = (*cur).second;
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(parentSection.scVal(),String("PostDrawer%02d",count++).scVal(),fnView, section);
		}
	}
	ObjectInfo::WriteElement(parentSection.scVal(),"PostDrawerCount",fnView, count);

	return parentSection;

}

void ComplexDrawer::load(const FileName& fnView, const String& parenSection){
	String currentSection;
	if ( parenSection !="")
		currentSection = parenSection + "::" + getName();
	else
		currentSection = getName();
	int subtype = 0;
	ObjectInfo::ReadElement(currentSection.scVal(),"SubType",fnView, subtype);
	ObjectInfo::ReadElement(currentSection.scVal(),"UiCode",fnView, uiCode);
	ObjectInfo::ReadElement(currentSection.scVal(),"HasInfo",fnView, info);
	int temp;
	ObjectInfo::ReadElement(currentSection.scVal(),"DrawMethod",fnView, temp);
	drm = (ILWIS::NewDrawer::DrawMethod)temp;
	ObjectInfo::ReadElement(currentSection.scVal(),"Transparency",fnView, transparency);
	ObjectInfo::ReadElement(currentSection.scVal(),"Type",fnView, type);
	ObjectInfo::ReadElement(currentSection.scVal(),"IsActive",fnView, active);
	ObjectInfo::ReadElement(currentSection.scVal(),"editable",fnView, editable);
	ObjectInfo::ReadElement(currentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::ReadElement(currentSection.scVal(),"IsThreeD",fnView, threeD);

	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur)
		(*cur).second->load(fnView, currentSection);

	int drCount = getDrawerCount();
	for(int index = 0; index < drCount; ++index)
		drawers[index]->load(fnView, currentSection);

	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur)
		(*cur).second->load(fnView, currentSection);
}


//--------------------------------- UI ------------------------------------------------------------------------
HTREEITEM ComplexDrawer::InsertItem(LayerTreeView *tv, HTREEITEM parent,const String& name,const String& icon, HTREEITEM after) {
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, parent,after);
	return htiDisplayOptions; 
}

HTREEITEM ComplexDrawer::InsertItem(const String& name,const String& icon, DisplayOptionTreeItem *item, int checkstatus, HTREEITEM after){
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = item->getTreeView()->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, item->getParent(), after);
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
	if ( transparency != rUNDEF) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&ComplexDrawer::displayOptionTransparency);
		String transp("Transparency (%d)", 100 * getTransparency());
		itemTransparent = InsertItem(transp,"Transparent", item, -1);
	}
	return parent;
}

void ComplexDrawer::displayOptionTransparency(CWnd *parent) {
	new TransparencyForm(parent, this);
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

//--------------------------------
DisplayOptionsForm2::DisplayOptionsForm2(ComplexDrawer *dr,CWnd *par, const String& title) : 
FormBaseDialog(par,title,fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON),
view((LayerTreeView *)par),
drw(dr)
{
}

int DisplayOptionsForm2::exec() {
	return 1;
}

void DisplayOptionsForm2::updateMapView() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->mpvGetView()->Invalidate();
}
//--------------------------------
TransparencyForm::TransparencyForm(CWnd *wPar, ComplexDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Transparency"),
	transparency(100 *(1.0-dr->getTransparency()))
{
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&TransparencyForm::setTransparency);
	slider->setContinuous(true);
	create();
}

int TransparencyForm::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  TransparencyForm::apply() {
	slider->StoreData();
	drw->setTransparency(1.0 - (double)transparency/100.0);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);
	String transp("Transparency (%d)",transparency);
	TreeItem titem;
	view->getItem(drw->itemTransparent,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);
	
	strcpy(titem.item.pszText,transp.scVal());
	view->GetTreeCtrl().SetItem(&titem.item);
	updateMapView();

}




