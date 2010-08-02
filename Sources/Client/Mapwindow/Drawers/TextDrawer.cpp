#include "headers\toolspch.h"
#include "Client\Base\OpenGLFont.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "client\mapwindow\drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"

using namespace ILWIS;


TextSetDrawer::TextSetDrawer(DrawerParameters *parms, const String& type) : ComplexDrawer(parms,type), font(0){
}

TextSetDrawer::~TextSetDrawer() {
	delete font;
}

void  TextSetDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
}

OpenGLFont *TextSetDrawer::getFont() const {
	return font;
}
void TextSetDrawer::setFont(OpenGLFont *f) {
	if ( font)
		delete font;
	font = f;
}

//---------------------------------------------------

TextDrawer::TextDrawer(DrawerParameters *parms, const String& type) : SimpleDrawer(parms,type){
}

TextDrawer::TextDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"TextDrawer"){
}

TextDrawer::~TextDrawer() {
}

void  TextDrawer::prepare(PreparationParameters *pp){
	SimpleDrawer::prepare(pp);
}

void TextDrawer::draw(bool norecursion){
	TextSetDrawer *set = (TextSetDrawer *)parentDrawer;
	if ( set->getFont()) {
		set->getFont()->renderText(c, text);
	}
}

void TextDrawer::addDataSource(void *v) {
	if ( v != 0) {
		text = *((String *)v);
	}
}
