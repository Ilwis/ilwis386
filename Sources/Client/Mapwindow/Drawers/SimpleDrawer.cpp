#include "Client\Headers\formelementspch.h"
#include "Client\Base\events.h"
#include "Client\ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

SimpleDrawer::SimpleDrawer() {
	init();
}

SimpleDrawer::SimpleDrawer(DrawerParameters *parms, const String& ty) : type(ty), rootDrawer(parms ? parms->rootDrawer : 0){
	init();
	if ( parms)
		parentDrawer = parms->parent;
}


SimpleDrawer::~SimpleDrawer() {
}

void SimpleDrawer::init() {
	GUID gd;
	CoCreateGuid(&gd);
	WCHAR buf[39];
	::StringFromGUID2(gd,buf,39);
	CString str(buf);
	id = str;
	name = "Unknown";
	editable = true;
	info = false;
	parentDrawer = 0;
	extrTransparency = 0.2;
	specialOptions = NewDrawer::sdoNone;
	fActive = true;
	isSupportingDrawer = false;
}

String SimpleDrawer::getType() const {
	return type;
}

String SimpleDrawer::getId() const{
	return id;
}

void SimpleDrawer::prepare(PreparationParameters *parms){
	if ( parms->parentDrawer != 0)
		parentDrawer = parms->parentDrawer;
	rootDrawer = parentDrawer->getRootDrawer();
}


String SimpleDrawer::getName() const {
	return name;
}

void SimpleDrawer::setName(const String& n) {
	name = n;
}

bool  SimpleDrawer::isEditable() const{
	return editable;
}
void  SimpleDrawer::setEditable(bool yesno){
	editable = yesno;
}

bool SimpleDrawer::hasInfo() const {
	return info;
}

void SimpleDrawer::setInfo(bool yesno) {
	info = yesno;
}

NewDrawer *SimpleDrawer::getParentDrawer() const {
	return parentDrawer;
}

void SimpleDrawer::drawExtrusion(const Coord& c1, const Coord& c2, double z, int option) const {
	Coord c3 = c1;
	Coord c4 = c2;
	c3.z = z;
	c4.z = z;
	if ( option & NewDrawer::sdoFilled) {
		glBegin(GL_QUADS); // temporary, should be changed when svg symbols are there						
			glVertex3f( c1.x, c1.y, c1.z);	
			glVertex3f( c2.x, c2.y, c2.z);	
			glVertex3f( c4.x, c4.y, c4.z);
			glVertex3f( c3.x, c3.y, c3.z);
		glEnd();
	} else if ( option & NewDrawer::sdoOpen){
		glBegin(GL_LINE_STRIP); // temporary, should be changed when svg symbols are there						
			glVertex3f( c1.x, c1.y, c1.z);	
			glVertex3f( c2.x, c2.y, c2.z);	
			glVertex3f( c4.x, c4.y, c4.z);
			glVertex3f( c3.x, c3.y, c3.z);
		glEnd();
	} else if ( option & NewDrawer::sdoFootPrint) {
		//double z0 = ((ComplexDrawer *)getParentDrawer())->getZMaker()->getZ0(getRootDrawer()->is3D());
			glBegin(GL_LINE_STRIP);
				glVertex3f( c1.x, c1.y, z);	
				glVertex3f( c2.x, c2.y, z);
			glEnd();
	}
}

String SimpleDrawer::getInfo(const Coord& crd) const {
	return "";
}

void SimpleDrawer::setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords){
	if ( add)
		specialOptions |= option;
	else
		specialOptions &= ~option;
}

int SimpleDrawer::getSpecialDrawingOption(int opt) const {
	if ( opt == sdoNone)
		return specialOptions;
	else
		return specialOptions & opt;
}

void SimpleDrawer::setExtrustionTransparency(double v) {
	extrTransparency = v;
}







