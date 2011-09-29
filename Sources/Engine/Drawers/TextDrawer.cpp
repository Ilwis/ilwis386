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
	iWidth(0)
{
}

TextLayerDrawer::~TextLayerDrawer() {
	delete font;
}

void  TextLayerDrawer::prepare(PreparationParameters *pp){ 
	ComplexDrawer::prepare(pp);
}

OpenGLText *TextLayerDrawer::getFont() const {
	return font;
}
void TextLayerDrawer::setFont(OpenGLText *f) {
	if ( font)
		delete font;
	font = f;
}

//HTREEITEM TextLayerDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
//	HTREEITEM hti = ComplexDrawer::configure(tv,parent);
//	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&TextLayerDrawer::displayOptionsText);
//	HTREEITEM itemText = InsertItem(String("Text(%S)",sFaceName), "Text",item,-1);
//	InsertItem(tv,itemText,String("Font : %S",sFaceName),"Text");
//	InsertItem(tv,itemText,String("Size : %d",iFHeight),"Calculationsingle");
//	String weightString =  iFWeight >= 500 ? "Bold" : "Normal";
//	InsertItem(tv,itemText,String("Weight : %S",weightString),"Calculationsingle");
//
//
//	return hti;
//}

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

bool TextDrawer::draw( const CoordBounds& cbArea) const{
	TextLayerDrawer *set = (TextLayerDrawer *)parentDrawer;
	if ( set->getFont()) {
		set->getFont()->renderText(c, text);
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

double TextDrawer::getHeight() const {
	TextLayerDrawer *set = (TextLayerDrawer *)parentDrawer;
	if ( set->getFont()) {
		return set->getFont()->getHeight();
	}
	return 0;
}
