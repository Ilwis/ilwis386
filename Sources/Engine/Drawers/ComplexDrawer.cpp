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
	alpha = 1.0;
	zmaker = new ILWIS::ZValueMaker(this);
	specialOptions = sdoNone;
	dirty = true;
	currentIndex = 0;
	editmode = false;
	selectionColor = Color(255,0,0);
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
	if (  here != postDrawers.end() && here->second != drw) {
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
	if (  here != preDrawers.end() && here->second != drw) {
		drawersById.erase(here->second->getId());
		delete (*here).second;
	}
	preDrawers[name] = drw;
	drawersById[drw->getId()] = drw;
}

bool ComplexDrawer::drawPreDrawers(const DrawLoop drawLoop, const CoordBounds& cb) const{
	if (!isActive())
		return false;

	if ( preDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(drawLoop, cb);
		}
	}
	return true;
}


bool ComplexDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cb) const{
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex());
	if (!isActive())
		return false;
	glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);
	drawPreDrawers(drawLoop, cb);

	for(int i=0; i < drawers.size(); ++i) {
		NewDrawer *drw = drawers[i];
		if (  drw && drw->isActive()) {
			drw->draw(drawLoop, cb);
		}
	}

	drawPostDrawers(drawLoop, cb);

	return true;
}

bool ComplexDrawer::drawPostDrawers(const DrawLoop drawLoop, const CoordBounds& cb) const{
	if (!isActive())
		return false;

	if ( postDrawers.size() > 0) {
		for(map<String,NewDrawer *>::const_iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
			NewDrawer *drw = (*cur).second;
			if ( drw)
				drw->draw(drawLoop, cb);
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

double ComplexDrawer::getAlpha() const{
	return alpha;
}

void ComplexDrawer::timedEvent(UINT timerid) {
}

void ComplexDrawer::setAlpha(double value){
	if ( (value >= 0.0 && value <= 1.0) || value == rUNDEF)
		alpha = value;
	else
		throw ErrorObject(String("Wrong alpha value %d", value));
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

String ComplexDrawer::store(const FileName& fnView, const String& section) const{
	ObjectInfo::WriteElement(section.c_str(),"HasInfo",fnView, info);
	ObjectInfo::WriteElement(section.c_str(),"DrawMethod",fnView, drm);
	ObjectInfo::WriteElement(section.c_str(),"Alpha",fnView, alpha);
	ObjectInfo::WriteElement(section.c_str(),"Type",fnView, type);
	if (subType != "" && subType != "ilwis38")
		ObjectInfo::WriteElement(section.c_str(),"SubType",fnView, subType);
	ObjectInfo::WriteElement(section.c_str(),"IsActive",fnView, active);
	ObjectInfo::WriteElement(section.c_str(),"editable",fnView, editable);
	ObjectInfo::WriteElement(section.c_str(),"Name",fnView, name);
	ObjectInfo::WriteElement(section.c_str(),"SpecialOptions",fnView, specialOptions);

	int count = 0;
	for(DrawerIter_C cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		String currentSection("%S%S",section,order);
		NewDrawer *drw = (*cur).second;
		if ( drw && !drw->isSimple() ) {
			String subSection = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(subSection.c_str(),"Order",fnView, order);
			ObjectInfo::WriteElement(section.c_str(),String("PreDrawer%03d",count++).c_str(),fnView, subSection);
		}
	}
	ObjectInfo::WriteElement(section.c_str(),"PreDrawerCount",fnView, count);
	int drCount = getDrawerCount();
	count = 0; 
	for(int index = 0; index < drCount; ++index) {
		String currentSection("%S%03d",section,index);
		NewDrawer *drw = drawers[index];
		if ( drw && !drw->isSimple() ) {
			String subSection = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(section.c_str(),String("Drawer%03d",count++).c_str(),fnView, subSection);
		}

	}
	ObjectInfo::WriteElement(section.c_str(),"DrawerCount",fnView, count);
	count = 0;
	for(DrawerIter_C cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		String order = String("%03d", (*cur).first.sHead("|").iVal());
		String currentSection("%S%S",section,order);
		NewDrawer *drw = (*cur).second;
		if ( drw && !drw->isSimple() ) {
			String subSection = drw->store(fnView, currentSection);
			ObjectInfo::WriteElement(subSection.c_str(),"Order",fnView, order);
			ObjectInfo::WriteElement(section.c_str(),String("PostDrawer%03d",count++).c_str(),fnView, subSection);
		}
	}
	ObjectInfo::WriteElement(section.c_str(),"PostDrawerCount",fnView, count);

	zmaker->store(fnView,section + ":ZValues");

	return section;
}

void ComplexDrawer::load(const FileName& fnView, const String& section){
	ObjectInfo::ReadElement(section.c_str(),"HasInfo",fnView, info);
	int temp;
	ObjectInfo::ReadElement(section.c_str(),"DrawMethod",fnView, temp);
	drm = (ILWIS::NewDrawer::DrawMethod)temp;
	ObjectInfo::ReadElement(section.c_str(),"Alpha",fnView, alpha);
	ObjectInfo::ReadElement(section.c_str(),"Type",fnView, type);
	ObjectInfo::ReadElement(section.c_str(),"IsActive",fnView, active);
	ObjectInfo::ReadElement(section.c_str(),"editable",fnView, editable);
	ObjectInfo::ReadElement(section.c_str(),"Name",fnView, name);
	ObjectInfo::ReadElement(section.c_str(),"SpecialOptions",fnView, specialOptions);

	long count, order;
	String drawerSection;
	ObjectInfo::ReadElement(section.c_str(),"PreDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		String subSection("PreDrawer%03d",i);
		ObjectInfo::ReadElement(section.c_str(),subSection.c_str(),fnView, drawerSection);
		ObjectInfo::ReadElement(drawerSection.c_str(),"Order",fnView, order);
		addPreDrawer(order,loadDrawer(fnView, drawerSection ));
	}

	ObjectInfo::ReadElement(section.c_str(),"DrawerCount",fnView, count);
	for(int i = 0; i < count; ++i) {
		ObjectInfo::ReadElement(section.c_str(),String("Drawer%03d",i).c_str(),fnView, drawerSection);
		addDrawer(loadDrawer(fnView, drawerSection));
	}

	ObjectInfo::ReadElement(section.c_str(),"PostDrawerCount",fnView, count);
	for(int i = 0; i < count ; ++i) {
		String subSection("PostDrawer%03d",i);
		ObjectInfo::ReadElement(section.c_str(),subSection.c_str(),fnView, drawerSection);
		ObjectInfo::ReadElement(drawerSection.c_str(),"Order",fnView, order);
		addPostDrawer(order,loadDrawer(fnView, drawerSection));
	}

	zmaker->load(fnView, section + ":ZValues");
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
		try {
			drawer->load(fnView,drawerSection);
		} catch (ErrorObject& err) {
			delete drawer;
			drawer = 0;
			err.Show();
		}
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

void ComplexDrawer::select(bool yesno) {
	for(int index = 0; index < getDrawerCount(); ++index) {
		NewDrawer *dr = getDrawer(index);
		if ( dr)
			dr->select(yesno);
	}
}

void ComplexDrawer::removeDrawersUsing(NewDrawer * drw) {
	vector<NewDrawer *> allDrawers;
	getDrawers(allDrawers);
	for(int i = 0; i < allDrawers.size(); ++i) {
		ComplexDrawer * cdrw = dynamic_cast<ComplexDrawer*>(allDrawers[i]);
		if (cdrw)
			cdrw->removeDrawersUsing(drw);
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







