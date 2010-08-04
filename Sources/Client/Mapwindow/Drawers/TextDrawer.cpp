#include "Client\Headers\formelementspch.h"
#include "Client\Base\OpenGLFont.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "client\mapwindow\drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Engine\Representation\Rprclass.h"

using namespace ILWIS;


TextSetDrawer::TextSetDrawer(DrawerParameters *parms, const String& type) : 
	ComplexDrawer(parms,type), 
	font(0),
	iFHeight(100 * RepresentationClass::iSIZE_FACTOR),
	sFaceName("Arial"),
	iFWeight(FW_BOLD),
	iFStyle(0),
	iWidth(0)
{
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

HTREEITEM TextSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = ComplexDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&TextSetDrawer::displayOptionsText);
	HTREEITEM itemText = InsertItem(String("Text(%S)",sFaceName), "Text",item,-1);
	InsertItem(tv,itemText,String("Font : %S",sFaceName),"Text");
	InsertItem(tv,itemText,String("Size : %d",iFHeight),"Calculationsingle");
	String weightString =  iFWeight >= 500 ? "Bold" : "Normal";
	InsertItem(tv,itemText,String("Weight : %S",weightString),"Calculationsingle");


	return hti;
}

void TextSetDrawer::displayOptionsText(CWnd *parent) {
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
