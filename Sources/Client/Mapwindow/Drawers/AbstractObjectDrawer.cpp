#include "Client\Headers\formelementspch.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h" 
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Engine\Representation\Rprclass.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

void displayOptionSubItem(NewDrawer *drw, CWnd *parent) {
	new TransparencyForm(parent, (AbstractObjectDrawer *)drw);
}


AbstractObjectDrawer::AbstractObjectDrawer(DrawerParameters *parms, const String& name) : 
	AbstractDrawer(parms,name),
	iFHeight(100 * RepresentationClass::iSIZE_FACTOR),
	sFaceName("Arial"),
	iFWeight(FW_BOLD),
	iFStyle(0),
	iWidth(0),
	hasText(false),
	transparency(0),
	object(0)
{
}

AbstractObjectDrawer::~AbstractObjectDrawer() {
	delete object;
}

void AbstractObjectDrawer::prepare(PreparationParameters *pp){
	AbstractDrawer::prepare(pp);
}

IlwisObject AbstractObjectDrawer::getObject() const {
	return IlwisObject::obj(obj->fnObj);
}

int AbstractObjectDrawer::getTransparency() const{
	return transparency;
}

void AbstractObjectDrawer::setTransperency(int value) {
	transparency = value;
}

void AbstractObjectDrawer::setDataSource(void *bmap, int options) {
	IlwisObject *o = (IlwisObject *)bmap;
	object = new IlwisObject(*o);
	obj = object->pointer();
	name = obj->sName();

}

HTREEITEM AbstractObjectDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = AbstractDrawer::configure(tv,parent);
	String sName = String("Transparency");
	int iImg = IlwWinApp()->iImage("Transparent");
	HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, parent);
	tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTreeItem(tv, this, displayOptionSubItem));
	iImg = IlwWinApp()->iImage("CalculationMultiple");
	tv->GetTreeCtrl().InsertItem(String("Value : %d",transparency).scVal(), iImg, iImg, htiDisplayOptions);

	sName = String("Text(%S)",sFaceName);
	iImg = IlwWinApp()->iImage("Text");
	htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, parent);
	tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTreeItem(tv, this, (DisplayOptionItemFunc)ILWIS::displayOptionsText));
	tv->GetTreeCtrl().InsertItem(String("Font : %S",sFaceName).scVal(), iImg, iImg, htiDisplayOptions);
	iImg = IlwWinApp()->iImage("Calculationsingle");
	tv->GetTreeCtrl().InsertItem(String("Size : %d",iFHeight).scVal(), iImg, iImg, htiDisplayOptions);
	String weightString = "Normal";
	if ( iFWeight >= 500)
		weightString = "Bold";
	tv->GetTreeCtrl().InsertItem(String("Weight : %S",weightString).scVal(), iImg, iImg, htiDisplayOptions);


	return hti;
}

//--------------------------------
TransparencyForm::TransparencyForm(CWnd *wPar, AbstractObjectDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Transparency"),
	transparency(dr->getTransparency())
{
	new FieldInt(root, "Transparency(0-100)", &transparency, RangeInt(0,100), true);
	create();
}

void  TransparencyForm::apply() {

	//FormBaseDialog::OnCancel();
}
//--------------------------------
void ILWIS::displayOptionsText(NewDrawer &drw, CWnd *parent) {
	AbstractObjectDrawer& drawer = (AbstractObjectDrawer&)drw;
	CWindowDC wdc(parent);
	LOGFONT lf;
	lf.lfHeight = MulDiv(drawer.iFHeight, GetDeviceCaps(wdc.GetSafeHdc(), LOGPIXELSY), 720);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = drawer.iFWeight;
	lf.lfItalic =(BYTE)(drawer.iFStyle&FS_ITALIC) ;
	lf.lfUnderline = (BYTE)(drawer.iFStyle&FS_UNDERLINE);
	lf.lfStrikeOut = (BYTE)(drawer.iFStyle&FS_STRIKEOUT);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
	lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH;
	strcpy(lf.lfFaceName, drawer.sFaceName.sVal());

    CHOOSEFONT chf;

    chf.Flags = CF_INITTOLOGFONTSTRUCT;
    chf.iPointSize = drawer.iFHeight; // fs->pointSize();
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
        drawer.iFHeight = lf.lfHeight;
        drawer.iFHeight = chf.iPointSize; // fs->pointSize();
        drawer.iFWeight = lf.lfWeight;
        drawer.iFStyle = lf.lfItalic    ? FS_ITALIC : 0  
					           | lf.lfStrikeOut ? FS_STRIKEOUT : 0
										 | lf.lfUnderline ? FS_UNDERLINE : 0;
        drawer.sFaceName = lf.lfFaceName;
    }
}

DisplayOptionsForm::DisplayOptionsForm(AbstractObjectDrawer *dr,CWnd *par, const String& title) : 
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

void DisplayOptionsForm::updateViews() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->ChangeState();
	doc->UpdateAllViews(0,0);
}






