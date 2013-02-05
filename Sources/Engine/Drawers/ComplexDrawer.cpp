#include "Headers\toolspch.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ComplexDrawer::ComplexDrawer() : parentDrawer(0), managed(0) {
	init();
}

ComplexDrawer::ComplexDrawer(DrawerParameters *parms, const String& ty) : type(ty),parentDrawer(0){
	setDrawerParameters(parms);
	init();
}

ComplexDrawer::ComplexDrawer(DrawerParameters *parms) : type("Container"), parentDrawer(0){
	setDrawerParameters(parms);
	init();
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
	uiCode = NewDrawer::ucALL;
	zmaker = new ILWIS::ZValueMaker(this);
	specialOptions = sdoNone;
	dirty = true;
	currentIndex = 0;
	editmode = false;
	selectionColor = Color(255,0,0);
	valid = true;
	selectable = false;
}

void ComplexDrawer::setDrawerParameters(DrawerParameters *parms){
	if ( parms) {
		parentDrawer = parms->parent;
		rootDrawer = parms->rootDrawer;
		managed = parms->managed;
	}
}

String ComplexDrawer::getType() const {
	return type;
}

String ComplexDrawer::getSubType() const {
	return subType;
}

void ComplexDrawer::setSubType(const String & sSubType) {
	subType = sSubType;
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
		if ( !drw->isManaged())
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
	if (  here != postDrawers.end()) {
		drawersById.erase(here->second->getId());
		delete (*here).second;
	}
	postDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

void ComplexDrawer::addPreDrawer(int order, NewDrawer *drw) {
	if ( !drw )
		return;
	String name("%3d", order);
	map<String, NewDrawer *>::iterator here = preDrawers.find(name);
	if (  here != preDrawers.end()) {
		drawersById.erase(here->second->getId());
		delete (*here).second;
	}
	preDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

bool ComplexDrawer::drawPreDrawers(const CoordBounds& cb) const{
	if (!isActive())
		return false;

	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
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
	

	double total = 0;
	//clock_t start = clock();
	for(int i=0; i < drawers.size(); ++i) {
		NewDrawer *drw = drawers[i];
		if (  drw && drw->isActive()) {
			drw->draw( cb);
		}
	}

	drawPostDrawers(cb);

/*	clock_t end = clock();
		total =  1000 *(double)(end - start) / CLOCKS_PER_SEC;
		TRACE(String("drawn %S in %2.2f milliseconds;\n", getName(), total).c_str());*/

	return true;
}

bool ComplexDrawer::drawPostDrawers(const CoordBounds& cb) const{
	if (!isActive())
		return false;

	if ( postDrawers.size() > 0) {
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
	//if ( parms->parentDrawer != 0)
	//	parentDrawer = parms->parentDrawer;
	//if ( parms->rootDrawer !=0)
	//	rootDrawer = (RootDrawer *)parms->rootDrawer;
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
	if ( index <= drawers.size())
		drawers.insert(drawers.begin() + index, drw);
	drawersById[drw->getId()] = drw;
}

void ComplexDrawer::removeDrawer(const String& did, bool dodelete) {
	for(int i=0; i < drawers.size(); ++i) {
		NewDrawer *drw = drawers.at(i);
		if ( drw && drw->getId() == did ) {
			drawers.erase(drawers.begin() + i);
			drawersById.erase(did);
			if ( dodelete)
				delete drw;
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

NewDrawer * ComplexDrawer::getDrawer(int index, int type) const{
	//if ( drawers.size() == 0)
	//	return 0;
	if ( type == dtMAIN) {
		if ( index < drawers.size()) {
			return drawers.at(index);
		}
	}  else if ( type == dtPRE) {
		String name("%3d", index);
		map<String, NewDrawer *>::const_iterator here = preDrawers.find(name);
		if (  here != preDrawers.end())
			return (*here).second;
	} if ( type == dtPOST ){
		String name("%3d", index);
		map<String, NewDrawer *>::const_iterator here = postDrawers.find(name);
		if (  here != postDrawers.end())
			return (*here).second;
	} if (type & dtPOLYGONLAYER) {
		NewDrawer *drw = drawers.at(index);
		if ( drw->getType() == "PolygonLayerDrawer")
			return drw;
	} if (type & dtSEGMENTLAYER) {
		NewDrawer *drw = drawers.at(index);
		if ( drw->getType() == "LineLayerDrawer")
			return drw;
	} if (type & dtPOINTLAYER) {
		NewDrawer *drw = drawers.at(index);
		if ( drw->getType() == "PointLayerDrawer")
			return drw;
	} if (type & dtRASTERLAYER) {
		NewDrawer *drw = drawers.at(index);
		if ( drw->getType() == "RasterLayerDrawer")
			return drw;
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

void ComplexDrawer::setSpecialDrawingOptions(int option, bool add, const vector<int>& coords){
	if ( add)
		specialOptions |= option;
	else {
		specialOptions &= ~option;
	}
	if ( (option & sdoTOCHILDEREN) || (specialOptions & sdoTOCHILDEREN)) {
		for(int i=0; i < drawers.size(); ++i) {
			if ( drawers.at(i))
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


ZValueMaker *ComplexDrawer::getZMaker() const{
	return zmaker;
}

String ComplexDrawer::store(const FileName& fnView, const String& parentSection) const{
	ObjectInfo::WriteElement(parentSection.c_str(),"UiCode",fnView, uiCode);
	ObjectInfo::WriteElement(parentSection.c_str(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.c_str(),"DrawMethod",fnView, drm);
	ObjectInfo::WriteElement(parentSection.c_str(),"Transparency",fnView, transparency);
	ObjectInfo::WriteElement(parentSection.c_str(),"Type",fnView, type);
	if (subType != "" && subType != "ilwis38")
		ObjectInfo::WriteElement(parentSection.c_str(),"SubType",fnView, subType);
	ObjectInfo::WriteElement(parentSection.c_str(),"IsActive",fnView, active);
	ObjectInfo::WriteElement(parentSection.c_str(),"editable",fnView, editable);
	ObjectInfo::WriteElement(parentSection.c_str(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(parentSection.c_str(),"Name",fnView, name);
	ObjectInfo::WriteElement(parentSection.c_str(),"SpecialOptions",fnView, specialOptions);

	int count = 0;
	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		String currentSection("%S%03d",parentSection,order);
		NewDrawer *drw = (*cur).second;
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(section.c_str(),"Order",fnView, order);
			ObjectInfo::WriteElement(parentSection.c_str(),String("PreDrawer%03d",count++).c_str(),fnView, section);
		}
	}
	ObjectInfo::WriteElement(parentSection.c_str(),"PreDrawerCount",fnView, count);
	int drCount = getDrawerCount();
	count = 0; 
	for(int index = 0; index < drCount; ++index) {
		String currentSection("%S%03d",parentSection,index);
		NewDrawer *drw = drawers[index];
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(parentSection.c_str(),String("Drawer%03d",count++).c_str(),fnView, section);
		}

	}
	ObjectInfo::WriteElement(parentSection.c_str(),"DrawerCount",fnView, count);
	count = 0;
	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		String currentSection("%S%S",parentSection,order);
		NewDrawer *drw = (*cur).second;
		if ( !drw->isSimple() ) {
			String section = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(section.c_str(),"Order",fnView, order);
			ObjectInfo::WriteElement(parentSection.c_str(),String("PostDrawer%03d",count++).c_str(),fnView, section);
		}
	}
	ObjectInfo::WriteElement(parentSection.c_str(),"PostDrawerCount",fnView, count);

	zmaker->store(fnView,parentSection);

	return parentSection;

}

void ComplexDrawer::load(const FileName& fnView, const String& parentSection){
	ObjectInfo::ReadElement(parentSection.c_str(),"UiCode",fnView, uiCode);
	ObjectInfo::ReadElement(parentSection.c_str(),"HasInfo",fnView, info);
	int temp;
	ObjectInfo::ReadElement(parentSection.c_str(),"DrawMethod",fnView, temp);
	drm = (ILWIS::NewDrawer::DrawMethod)temp;
	ObjectInfo::ReadElement(parentSection.c_str(),"Transparency",fnView, transparency);
	ObjectInfo::ReadElement(parentSection.c_str(),"Type",fnView, type);
	ObjectInfo::ReadElement(parentSection.c_str(),"IsActive",fnView, active);
	ObjectInfo::ReadElement(parentSection.c_str(),"editable",fnView, editable);
	ObjectInfo::ReadElement(parentSection.c_str(),"HasInfo",fnView, info);
	ObjectInfo::ReadElement(parentSection.c_str(),"Name",fnView, name);
	ObjectInfo::ReadElement(parentSection.c_str(),"SpecialOptions",fnView, specialOptions);

	long count, order;
	String drawerSection;
	ObjectInfo::ReadElement(parentSection.c_str(),"PreDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		ObjectInfo::ReadElement(drawerSection.c_str(),"Order",fnView, order);
		ObjectInfo::ReadElement(parentSection.c_str(),String("PreDrawer%03d",order).c_str(),fnView, drawerSection);
		addPreDrawer(order,loadDrawer(fnView, drawerSection ));
	}

	ObjectInfo::ReadElement(parentSection.c_str(),"DrawerCount",fnView, count);
	for(int i = 0; i < count; ++i) {
		ObjectInfo::ReadElement(parentSection.c_str(),String("Drawer%03d",i).c_str(),fnView, drawerSection);
		addDrawer(loadDrawer(fnView, drawerSection));
	}

	ObjectInfo::ReadElement(parentSection.c_str(),"PostDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		String section("PostDrawer%03d",i);
		ObjectInfo::ReadElement(parentSection.c_str(),section.c_str(),fnView, drawerSection);
		ObjectInfo::ReadElement(drawerSection.c_str(),"Order",fnView, order);
		addPostDrawer(order,loadDrawer(fnView, drawerSection));
	}

	zmaker->load(fnView, parentSection);
	setSpecialDrawingOptions(specialOptions | NewDrawer::sdoTOCHILDEREN, true);
}

NewDrawer *ComplexDrawer::loadDrawer(const FileName& fnView, const String& drawerSection) {
	String sType, sSubType;
	ObjectInfo::ReadElement(drawerSection.c_str(),"Type",fnView, sType);
	if (!ObjectInfo::ReadElement(drawerSection.c_str(),"SubType",fnView, sSubType))
		sSubType = "Ilwis38";
	ILWIS::DrawerParameters dp(rootDrawer, this);
	ILWIS::NewDrawer *drawer = NewDrawer::getDrawer(sType, sSubType, &dp);
	if (drawer) {
		drawer->load(fnView,drawerSection);
	}
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

bool ComplexDrawer::isValid() const{
	return valid;
}
void ComplexDrawer::setValid(bool yesno) {
	valid = yesno;
}

void ComplexDrawer::select(bool yesno) {
	for(int index = 0; index < getDrawerCount(); ++index) {
		NewDrawer *dr = getDrawer(index);
		if ( dr)
			dr->select(yesno);
	}
}

//--------------------------------- UI ------------------------------------------------------------------------
//void ComplexDrawer::setActiveMode(void *v,LayerTreeView *tv) {
//	bool value = *(bool *)v;
//	setActive(value);
//	MapCompositionDoc* doc = tv->GetDocument();
//	doc->mpvGetView()->Invalidate();
//
//}







