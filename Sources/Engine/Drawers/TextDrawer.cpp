#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\drawer_n.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createTextLayerDrawer(DrawerParameters *parms) {
	return new TextLayerDrawer(parms);
}

TextLayerDrawer::TextLayerDrawer(DrawerParameters *parms) : 
	ComplexDrawer(parms,"TextLayerDrawer"),
	font(0),
	clrText(colorUNDEF),
	fontScale(1)
{
}

TextLayerDrawer::~TextLayerDrawer() {
	delete font;
}

void  TextLayerDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
	if ( font)
		font->prepare(pp);
}

OpenGLText *TextLayerDrawer::getFont() const {
	return font;
}
void TextLayerDrawer::setFont(OpenGLText *f) {
	if ( font) {
		delete font;
		font = f;
	} else {
		font = f;
		if (font != 0 && !clrText.fEqual(colorUNDEF)) { // only set the color the first time, and only when the color originates from a MapView::load().
			font->setColor(clrText);
			clrText = colorUNDEF; // reset it so it isn't used after MapView::load()
		}
	}
}

void TextLayerDrawer::setFontScale(double f){
	fontScale = f;
	getFont()->setHeight(12 * fontScale);
}

double TextLayerDrawer::getFontScale() const{
	return fontScale;
}

String TextLayerDrawer::store(const FileName& fnView, const String& section) const{
	ComplexDrawer::store(fnView, section);
	if (font)
		ObjectInfo::WriteElement(section.c_str(),"ColorText",fnView, font->getColor());
	ObjectInfo::WriteElement(section.c_str(),"Scale",fnView, fontScale);
	
	return section;

}
void TextLayerDrawer::load(const FileName& fnView, const String& section){
	ComplexDrawer::load(fnView, section);
	Color clr;
	if (ObjectInfo::ReadElement(section.c_str(),"ColorText",fnView, clr))
		clrText = clr; // cache this until the setFont() call
	ObjectInfo::ReadElement(section.c_str(),"Scale",fnView, fontScale);
}
//---------------------------------------------------

ILWIS::NewDrawer *createTextDrawer(DrawerParameters *parms) {
	return new TextDrawer(parms);
}

TextDrawer::TextDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"TextDrawer"){
}

TextDrawer::~TextDrawer() {
}

void  TextDrawer::prepare(PreparationParameters *pp){
	SimpleDrawer::prepare(pp);
}

bool TextDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	TextLayerDrawer *set = dynamic_cast<TextLayerDrawer *>(parentDrawer);
	if (set) {
		OpenGLText * font = set->getFont();
		if ( (font != 0) && ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && font->getColor().alpha() == 255) || (drawLoop == drl3DTRANSPARENT && font->getColor().alpha() != 255)))
			font->renderText(drawLoop, c, text);
	}
	return true;
}

void TextDrawer::addDataSource(void *v) {
	if ( v != 0) {
		text = *((String *)v);
	}
}

void TextDrawer::setCoord(const Coord& crd) {
	c = crd;
}

void TextDrawer::setText(const Coord& crd, const String& txt) {
	c = crd;
	text = txt;

}

double TextDrawer::getHeight() const {
	TextLayerDrawer *set = (TextLayerDrawer *)parentDrawer;
	if ( set->getFont()) {
		return set->getFont()->getHeight();
	}
	return 0;
}

CoordBounds TextDrawer::getTextExtent() const{
	TextLayerDrawer *set = (TextLayerDrawer *)parentDrawer;
	if ( set->getFont()) {
		return set->getFont()->getTextExtent(text);
	}
	return CoordBounds();

}

