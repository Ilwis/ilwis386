#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\SVGLoader.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PointSetDrawer.h"
#include "Drawers\PointDrawer.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPointSetDrawer(DrawerParameters *parms) {
	return new PointSetDrawer(parms);
}

PointSetDrawer::PointSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"PointSetDrawer")
{
	setDefaultSymbol("open-rectangle");
}

PointSetDrawer::~PointSetDrawer() {
}

NewDrawer *PointSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return IlwWinApp()->getDrawer("PointFeatureDrawer", pp,parms);

}

void PointSetDrawer::prepare(PreparationParameters *parms){
	FeatureSetDrawer::prepare(parms);
	if ( parms->type & NewDrawer::ptRENDER) {
		for(int i = 0; i < getDrawerCount(); ++i) {
			PointDrawer *pdrw = (PointDrawer *)getDrawer(i);
			pdrw->setSymbol(defaultSymbol);
		}
	}
}

void PointSetDrawer::setDefaultSymbol(const String& name) {
	defaultSymbol = name;
}

HTREEITEM PointSetDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = FeatureSetDrawer::configure(tv,parent);
	if ( portrayalItem) {
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,portrayalItem,this,(DisplayOptionItemFunc)&PointSetDrawer::setSymbolization);
		InsertItem(TR("Symbolization"),"set",item);
	}
	return hti;
}

void PointSetDrawer::setSymbolization(CWnd *parent) {
	new PointSymbolizationForm(parent, this);
}

String PointSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = getType() + "::" + parentSection;
	FeatureSetDrawer::store(fnView, currentSection);

	return currentSection;
}

void PointSetDrawer::load(const FileName& fnView, const String& parenSection){
	FeatureSetDrawer::load(fnView, parenSection);
}

void PointSetDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) { 
		if ( useInternalDomain() || !rpr.fValid()) {
			setDrawMethod(drmSINGLE);
		}
		else if ( rpr.fValid()) {
			setDrawMethod(drmRPR);
		}
	}
	else
		drm = method;
}

//-----------------------------------------------------------------
PointSymbolizationForm::PointSymbolizationForm(CWnd *wPar, PointSetDrawer *dr):
	DisplayOptionsForm(dr,wPar,TR("Symbolization")), selection(0)
{
	ILWIS::SVGLoader *loader = IlwWinApp()->getSVGContainer();
	for(map<String, SVGElement *>::iterator cur = loader->begin(); cur != loader->end(); ++cur) {
		String name = (*cur).first;
		name = name.sHead("|");
		names.push_back(name);
	}
	fselect = new FieldOneSelectString(root,TR("Symbols"),&selection, names);
	create();
}

void PointSymbolizationForm::apply(){
	fselect->StoreData();

	String symbol = names[selection];
	PointSetDrawer *psdrw = (PointSetDrawer *)drw;
	psdrw->setDefaultSymbol(symbol);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	psdrw->prepare(&pp);
	updateMapView();
}





