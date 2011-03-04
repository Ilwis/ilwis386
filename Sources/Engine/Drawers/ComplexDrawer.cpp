#include "Headers\toolspch.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ComplexDrawer::ComplexDrawer() {
	init();
}

ComplexDrawer::ComplexDrawer(DrawerParameters *parms, const String& ty) : type(ty), rootDrawer(parms ? parms->rootDrawer : 0){
	init();
	if ( parms)
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
	specialOptions = sdoNone;
	dirty = true;
	currentIndex = 0;
	editmode = false;
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
		if ( !drw->isSimple())
			((ComplexDrawer *)drw)->clear();
		delete drw;
	}
	drawers.clear();
	postDrawers.clear();
	preDrawers.clear();
	drawersById.clear();
}

RootDrawer *ComplexDrawer::getRootDrawer() {
	return rootDrawer;
}

RootDrawer *ComplexDrawer::getRootDrawer() const{
	return rootDrawer;
}

void ComplexDrawer::addPostDrawer(int order, NewDrawer *drw) {
	if ( !drw )
		return;
	String name("%3d", order);
	map<String, NewDrawer *>::iterator here = postDrawers.find(name);
	if (  here != postDrawers.end())
		delete (*here).second;
	postDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

void ComplexDrawer::addPreDrawer(int order, NewDrawer *drw) {
	if ( !drw )
		return;
	String name("%3d", order);
	map<String, NewDrawer *>::iterator here = preDrawers.find(name);
	if (  here != preDrawers.end())
		delete (*here).second;
	preDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

bool ComplexDrawer::drawPreDrawers(const CoordBounds& cb) const{
	if (!isActive())
		return false;

	long& count = (const_cast<ComplexDrawer *>(this))->currentIndex;
	count = 0;
	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			++count;
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw( cb);
		}
	}
	return true;
}


bool ComplexDrawer::draw( const CoordBounds& cb) const{
	if (!isActive())
		return false;
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex());
	drawPreDrawers(cb);
	

	long& count = (const_cast<ComplexDrawer *>(this))->currentIndex;
	count = 0;
	double total = 0;
	//clock_t start = clock();
	for(int i=0; i < drawers.size(); ++i) {
		++count;
		NewDrawer *drw = drawers[i];
		if (  drw && drw->isActive()) {
			drw->draw( cb);
		}
	}

	drawPostDrawers(cb);

/*	clock_t end = clock();
		total =  1000 *(double)(end - start) / CLOCKS_PER_SEC;
		TRACE(String("drawn %S in %2.2f milliseconds;\n", getName(), total).scVal());*/

	return true;
}

bool ComplexDrawer::drawPostDrawers(const CoordBounds& cb) const{
	if (!isActive())
		return false;

	long& count = (const_cast<ComplexDrawer *>(this))->currentIndex;
	count = 0;
	if ( postDrawers.size() > 0) {
		++count;
		for(map<String,NewDrawer *>::const_iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw( cb);
		}
	}
	return true;
}

String ComplexDrawer::getId() const{
	return id;
}

void ComplexDrawer::prepare(PreparationParameters *parms){
	if ( parms->parentDrawer != 0)
		parentDrawer = parms->parentDrawer;
	if ( parms->rootDrawer !=0)
		rootDrawer = (RootDrawer *)parms->rootDrawer;
}

String ComplexDrawer::addDrawer(NewDrawer *drw) {
	if (! drw)
		return "";
	drawers.push_back(drw);
	drawersById[drw->getId()] = drw;
	return drw->getName();
}

void ComplexDrawer::setDrawer(int index, NewDrawer *drw){
	if ( index >= drawers.size())
		return;
	if ( drawers.at(index) != 0)
		removeDrawer(drawers.at(index)->getId());

	drawers[index] = drw;
	drawersById[drw->getId()] = drw;
}


NewDrawer *ComplexDrawer::getDrawer(const String& did) {
	map<String, NewDrawer *>::iterator cur= drawersById.find(did);
	if ( cur != drawersById.end())
		return (*cur).second;
	return NULL;

}

void ComplexDrawer::insertDrawer(int index, NewDrawer *drw) {
	if ( index < drawers.size())
		drawers.insert(drawers.begin() + index, drw);
}

void ComplexDrawer::removeDrawer(const String& did, bool dodelete) {
	for(int i=0; i < drawers.size(); ++i) {
		if ( drawers.at(i)->getId() == did ) {
			if ( dodelete)
				delete drawers.at(i);
			drawers.erase(drawers.begin() + i);
			drawersById.erase(did);
			break;
		}
	}
	for(map<String,NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw->getId() == did ) {
			preDrawers.erase(cur);
			drawersById.erase(did);
			if ( dodelete)
				delete drw;
			break;
		}
	}
	for(map<String,NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw->getId() == did ) {

			postDrawers.erase(cur);
			int count = drawersById.erase(did);
			if ( dodelete)
				delete drw;
			break;
		}
	}
}

int ComplexDrawer::getDrawerCount(int types) const{
	int count = 0;
	if ( types & dtPRE)
		count += preDrawers.size();
	if ( types & dtPOST)
		count +=postDrawers.size();
	if ( types & dtMAIN)
		count += drawers.size();

	return count;
}

NewDrawer * ComplexDrawer::getDrawer(int index, DrawerType type){
	if ( type == dtMAIN) {
		if ( index < drawers.size()) {
			return drawers.at(index);
		}
	} else  if ( type == dtPRE) {
		String name("%3d", index);
		map<String, NewDrawer *>::iterator here = preDrawers.find(name);
		if (  here != preDrawers.end())
			return (*here).second;
	} else  if ( type == dtPOST ){
		String name("%3d", index);
		map<String, NewDrawer *>::iterator here = postDrawers.find(name);
		if (  here != postDrawers.end())
			return (*here).second;
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

void ComplexDrawer::setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords){
	if ( add)
		specialOptions |= option;
	else {
		specialOptions &= ~option;
	}
	if ( (option & sdoTOCHILDEREN) || (specialOptions & sdoTOCHILDEREN)) {
		for(int i=0; i < drawers.size(); ++i) {
			drawers.at(i)->setSpecialDrawingOptions(option, add, coords);
		}
	}
}

int ComplexDrawer::getSpecialDrawingOption(int opt) const {
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
	for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw) {
			drw->setInfo(yesno);
		}
	}
	for(int i=0; i < drawers.size(); ++i) {
		if ( drawers[i] && drawers[i]->isActive()){
			drawers[i]->setInfo(yesno);
		}
	}
	for(map<String,NewDrawer *>::const_iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		NewDrawer *drw = (*cur).second;
		if ( drw){
			drw->setInfo(yesno);
		}
	}
}

String ComplexDrawer::getInfo(const Coord& crd) const {
	String info;
	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw) {
				String txt = drw->getInfo(crd);
				if ( txt != "") {
					info += info.size() ? "; " + txt : txt;
				}

			}
		}
	}
	for(int i=0; i < drawers.size(); ++i) {
		if ( drawers[i] && drawers[i]->isActive()){
			String txt = drawers[i]->getInfo(crd);
			if ( txt != "") {
				info += info.size() ? "; " + txt : txt;
			}
		}
	}
	if ( postDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw){
				String txt = drw->getInfo(crd);
				if ( txt != "") {
					info += info.size() ? "; " + txt : txt;
				}
			}
		}
	}
	return info;
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
			//PreparationParameters pp((int)parms->type, parms->dc);
			pdrw->prepare(parms);
		}
	}
}


bool ComplexDrawer::is3D() const {
	return threeD;
}

ZValueMaker *ComplexDrawer::getZMaker() const{
	return zmaker;
}

String ComplexDrawer::store(const FileName& fnView, const String& parentSection) const{
	ObjectInfo::WriteElement(parentSection.scVal(),"UiCode",fnView, uiCode);
	ObjectInfo::WriteElement(parentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.scVal(),"DrawMethod",fnView, drm);
	ObjectInfo::WriteElement(parentSection.scVal(),"Transparency",fnView, transparency);
	ObjectInfo::WriteElement(parentSection.scVal(),"Type",fnView, type);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsActive",fnView, active);
	ObjectInfo::WriteElement(parentSection.scVal(),"editable",fnView, editable);
	ObjectInfo::WriteElement(parentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.scVal(),"IsThreeD",fnView, threeD);
	ObjectInfo::WriteElement(parentSection.scVal(),"Name",fnView, name);

	int count = 0;
	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		String currentSection("%S%03d",parentSection,count);
		NewDrawer *drw = (*cur).second;
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(section.scVal(),"Order",fnView, order);
			ObjectInfo::WriteElement(parentSection.scVal(),String("PreDrawer%03d",count++).scVal(),fnView, section);
		}
	}
	ObjectInfo::WriteElement(parentSection.scVal(),"PreDrawerCount",fnView, count);
	int drCount = getDrawerCount();
	count = 0; 
	for(int index = 0; index < drCount; ++index) {
		String currentSection("%S%03d",parentSection,index);
		NewDrawer *drw = drawers[index];
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(parentSection.scVal(),String("Drawer%03d",count++).scVal(),fnView, section);
		}

	}
	ObjectInfo::WriteElement(parentSection.scVal(),"DrawerCount",fnView, count);
	count = 0;
	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		String currentSection("%S%03d",parentSection,count);
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		NewDrawer *drw = (*cur).second;
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(section.scVal(),"Order",fnView, order);
			ObjectInfo::WriteElement(parentSection.scVal(),String("PostDrawer%03d",count++).scVal(),fnView, section);
		}
	}
	ObjectInfo::WriteElement(parentSection.scVal(),"PostDrawerCount",fnView, count);

	zmaker->store(fnView,parentSection);

	return parentSection;

}

void ComplexDrawer::load(const FileName& fnView, const String& parentSection){
	ObjectInfo::ReadElement(parentSection.scVal(),"UiCode",fnView, uiCode);
	ObjectInfo::ReadElement(parentSection.scVal(),"HasInfo",fnView, info);
	int temp;
	ObjectInfo::ReadElement(parentSection.scVal(),"DrawMethod",fnView, temp);
	drm = (ILWIS::NewDrawer::DrawMethod)temp;
	ObjectInfo::ReadElement(parentSection.scVal(),"Transparency",fnView, transparency);
	ObjectInfo::ReadElement(parentSection.scVal(),"Type",fnView, type);
	ObjectInfo::ReadElement(parentSection.scVal(),"IsActive",fnView, active);
	ObjectInfo::ReadElement(parentSection.scVal(),"editable",fnView, editable);
	ObjectInfo::ReadElement(parentSection.scVal(),"HasInfo",fnView, info);
	ObjectInfo::ReadElement(parentSection.scVal(),"IsThreeD",fnView, threeD);
	ObjectInfo::ReadElement(parentSection.scVal(),"Name",fnView, name);

	long count, order;
	String drawerSection;
	ObjectInfo::ReadElement(parentSection.scVal(),"PreDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		ObjectInfo::ReadElement(parentSection.scVal(),String("PreDrawer%03d",i).scVal(),fnView, drawerSection);
		ObjectInfo::ReadElement(drawerSection.scVal(),"Order",fnView, order);
		addPreDrawer(order,loadDrawer(fnView, drawerSection ));
	}

	ObjectInfo::ReadElement(parentSection.scVal(),"DrawerCount",fnView, count);
	for(int i = 0; i < count; ++i) {
		ObjectInfo::ReadElement(parentSection.scVal(),String("Drawer%03d",i).scVal(),fnView, drawerSection);
		addDrawer(loadDrawer(fnView, drawerSection));
	}

	ObjectInfo::ReadElement(parentSection.scVal(),"PostDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		ObjectInfo::ReadElement(parentSection.scVal(),String("PostDrawer%03d",i).scVal(),fnView, drawerSection);
		ObjectInfo::ReadElement(drawerSection.scVal(),"Order",fnView, order);
		addPostDrawer(order,loadDrawer(fnView, drawerSection));
	}
	zmaker->load(fnView, parentSection);
}

NewDrawer *ComplexDrawer::loadDrawer(const FileName& fnView, const String& drawerSection) {
	Array<String> parts;
	Split(drawerSection,parts,"::");
	ILWIS::DrawerParameters dp(rootDrawer, this);
	ILWIS::NewDrawer *drawer = NewDrawer::getDrawer(parts[0], "Ilwis38", &dp);
	drawer->load(fnView,drawerSection);
	return drawer;
}

bool ComplexDrawer::isDirty() const {
	return dirty;
}
void ComplexDrawer::setDirty(bool yesno) {
	dirty = yesno;
}

long ComplexDrawer::getCurrentIndex() const{
	return currentIndex;
}
void ComplexDrawer::setCurrentIndex(long i){
	if ( i >= 0 && i < drawers.size())
		currentIndex = i;	
}

int ComplexDrawer::getDrawerIndex(NewDrawer *drw) {
	for(int index = 0; index < getDrawerCount(); ++index) {
		NewDrawer *dr = getDrawer(index);
		if ( drw && drw->getId() == dr->getId()) {
			return index;
		}
	}
	return iUNDEF;
}

void ComplexDrawer::getDrawerFor(const Feature* feature,vector<NewDrawer *>& featureDrawers) {
	vector<NewDrawer *> draws;
	getDrawers(draws);
	for(int i = 0; i < draws.size(); ++i) {
		NewDrawer *drw = draws.at(i);
		if ( !drw->isSimple()){
			ComplexDrawer *cdrw = (ComplexDrawer *)drw;
			cdrw->getDrawerFor(feature, featureDrawers);
		}
	}
}

void ComplexDrawer::shareVertices(vector<Coord *>& coords) {
}

bool ComplexDrawer::inEditMode() const{
	return editmode;
}

void ComplexDrawer::setEditMode(bool yesno){
	editmode = yesno;
}
//--------------------------------- UI ------------------------------------------------------------------------
//void ComplexDrawer::setActiveMode(void *v,LayerTreeView *tv) {
//	bool value = *(bool *)v;
//	setActive(value);
//	MapCompositionDoc* doc = tv->GetDocument();
//	doc->mpvGetView()->Invalidate();
//
//}







