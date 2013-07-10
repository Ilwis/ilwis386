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
	iFHeight(100 * 3),
	sFaceName("Arial"),
	iFWeight(FW_BOLD),
	iFStyle(0),
	iWidth(0),
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
	if ( font)
		delete font;
	font = f;
}

void TextLayerDrawer::displayOptionsText(CWnd *parent) {
	CWindowDC wdc(parent);
	LOGFONT lf;
	lf.lfHeight = MulDiv(iFHeight, GetDeviceCaps(wdc.GetSafeHdc(), LOGPIXELSY), 720);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = iFWeight;
	lf.lfItalic =(BYTE)(iFStyle&FS_ITALIC) ;
	lf.lfUnderline = (BYTE)(iFStyle&FS_UNDERLINE);
	lf.lfStrikeOut = (BYTE)(iFStyle&FS_STRIKEOUT);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
	lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH;
	strcpy(lf.lfFaceName, sFaceName.sVal());

    CHOOSEFONT chf;

    chf.Flags = CF_INITTOLOGFONTSTRUCT;
    chf.iPointSize = iFHeight; // fs->pointSize();
    chf.lStructSize = sizeof(CHOOSEFONT);
    chf.hwndOwner = parent->m_hWnd;
    chf.lpLogFont = &lf;
    chf.hDC = 0; //*mdw->mppn->prDisplay();
    chf.Flags |= CF_BOTH| CF_PRINTERFONTS|CF_EFFECTS;//|CF_SHOWHELP;
    chf.rgbColors = 0;
    chf.lCustData = 0;
    chf.lpfnHook = 0;
    chf.lpTemplateName = 0;
    chf.hInstance = 0;
    chf.lpszStyle = 0;
    chf.nFontType = 0;
    chf.nSizeMin = 0;
    chf.nSizeMax = 0;
    if (ChooseFont(&chf))
    {
        iFHeight = lf.lfHeight;
        iFHeight = chf.iPointSize; // fs->pointSize();
        iFWeight = lf.lfWeight;
        iFStyle = lf.lfItalic    ? FS_ITALIC : 0  
					           | lf.lfStrikeOut ? FS_STRIKEOUT : 0
										 | lf.lfUnderline ? FS_UNDERLINE : 0;
        sFaceName = lf.lfFaceName;
    }
}

void TextLayerDrawer::setFontScale(double f){
	fontScale = f;
	getFont()->setHeight(12 * fontScale);
}

double TextLayerDrawer::getFontScale() const{
	return fontScale;
}

String TextLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = parentSection;
	ComplexDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"ColotText",fnView, clrText);
	ObjectInfo::WriteElement(currentSection.c_str(),"FaceName",fnView, sFaceName);
	ObjectInfo::WriteElement(currentSection.c_str(),"Height",fnView, iFHeight);
	ObjectInfo::WriteElement(currentSection.c_str(),"Weight",fnView, iFWeight);
	ObjectInfo::WriteElement(currentSection.c_str(),"Style",fnView, iFStyle);
	ObjectInfo::WriteElement(currentSection.c_str(),"Width",fnView, iWidth);
	ObjectInfo::WriteElement(currentSection.c_str(),"Scale",fnView, fontScale);
	
	return currentSection;

}
void TextLayerDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	ComplexDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"ColotText",fnView, clrText);
	ObjectInfo::ReadElement(currentSection.c_str(),"FaceName",fnView, sFaceName);
	ObjectInfo::ReadElement(currentSection.c_str(),"Height",fnView, iFHeight);
	ObjectInfo::ReadElement(currentSection.c_str(),"Weight",fnView, iFWeight);
	ObjectInfo::ReadElement(currentSection.c_str(),"Style",fnView, iFStyle);
	ObjectInfo::ReadElement(currentSection.c_str(),"Width",fnView, iWidth);
	ObjectInfo::ReadElement(currentSection.c_str(),"Scale",fnView, fontScale);
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
			font->renderText(c, text);
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

