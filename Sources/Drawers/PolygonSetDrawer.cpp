#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Map\basemap.h"
#include "Client\Editors\Utils\line.h"
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
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\gpc.h"
#include "Drawers\PolygonFeatureDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms) {
	return new PolygonSetDrawer(parms);
}

PolygonSetDrawer::PolygonSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"PolygonSetDrawer"), 
	showAreas(true), 
	showBoundaries(true), 
	areaTransparency(1.0),
	linecolor(Color(0,0,0)),
	linestyle(ldtNone),
	linethickness(1.0)
{

}
PolygonSetDrawer::~PolygonSetDrawer() {
}

NewDrawer *PolygonSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return IlwWinApp()->getDrawer("PolygonFeatureDrawer",pp, parms);

}

void PolygonSetDrawer::addDataSource(void *bmap,int options) {
	FeatureSetDrawer::addDataSource(bmap, options);
	ComplexDrawer *cdr = (ComplexDrawer *)getParentDrawer();
	FeatureSetDrawer *fdr = (FeatureSetDrawer *)cdr->getDrawer(100, ComplexDrawer::dtPOST);
	if ( fdr)
		fdr->addDataSource(bmap);

}

HTREEITEM PolygonSetDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = FeatureSetDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,
				(SetCheckFunc)&PolygonSetDrawer::setActiveBoundaries);
	HTREEITEM itemBoundaries = InsertItem("Boundaries",".mps",item,(int)showBoundaries);

	item = new DisplayOptionTreeItem(tv,itemBoundaries,this,(DisplayOptionItemFunc)&PolygonSetDrawer::displayOptionSetLineStyle);
	InsertItem("Line style","LineStyle", item, -1);

	item = new DisplayOptionTreeItem(tv,parent,this,
				(SetCheckFunc)&PolygonSetDrawer::setActiveAreas);				
	HTREEITEM itemAreas = InsertItem("Areas",".mpa",item,(int)showAreas);

	item = new DisplayOptionTreeItem(tv,itemAreas,this,(DisplayOptionItemFunc)&PolygonSetDrawer::displayOptionTransparencyP);
	String transp("Transparency (%d)", 100 * getTransparency());
	itemTransparentP = InsertItem(transp,"Transparent", item, -1);
	return hti;
}

void PolygonSetDrawer::displayOptionSetLineStyle(CWnd *parent) {
	new BoundaryLineStyleForm(parent, this);
}

void PolygonSetDrawer::displayOptionTransparencyP(CWnd *p) {
	new TransparencyFormP(p, this);
}

void PolygonSetDrawer::setActiveAreas(void *v, LayerTreeView *view) {
	bool value = *(bool *)v;
	showAreas = value;
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	prepare(&parm);
	getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetDrawer::setActiveBoundaries(void *v, LayerTreeView *view) {
	bool value = *(bool *)v;
	showBoundaries = value;
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	prepare(&parm);
	getRootDrawer()->getDrawerContext()->doDraw();
}

void PolygonSetDrawer::prepare(PreparationParameters *parms) {
	SetDrawer::test_count = 0;
	FeatureSetDrawer::prepare(parms);
	if ( parms->type & RootDrawer::ptRENDER){
		for(int i=0; i < drawers.size(); ++i) {
			PolygonFeatureDrawer *pdr = (PolygonFeatureDrawer *)drawers.at(i);
			pdr->boundariesActive(showBoundaries);
			pdr->areasActive(showAreas);
			pdr->setTransparencyArea(areaTransparency);
			switch(linestyle) {
			case ldtDot:
				pdr->setlineStyle(0xAAAA); break;
			case ldtDash:
				pdr->setlineStyle(0xF0F0); break;
			case ldtDashDot:
				pdr->setlineStyle(0x6B5A); break;
			case ldtDashDotDot:
				pdr->setlineStyle(0x56B5); break;
			default:
				pdr->setlineStyle(0xFFFF);
			}
			pdr->setLineColor(linecolor);
			pdr->setlineThickness(linethickness);
			for(int j =0 ; j < parms->filteredRaws.size(); ++j) {
				int raw = parms->filteredRaws[j];
				if ( pdr->getFeature()->rValue() == abs(raw)) {
					pdr->setActive(raw > 0);
				}
			}
		}
	}
}

void PolygonSetDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) {
		if ( useInternalDomain() || !rpr.fValid())
			setDrawMethod(drmMULTIPLE);
		else 
			setDrawMethod(drmRPR);

	} else
		drm = method;
}

double PolygonSetDrawer::getTransparencyArea() const{
	return areaTransparency;
}

void PolygonSetDrawer::setTransparencyArea(double v){
	areaTransparency = v;
}

//------------------------------------------------
TransparencyFormP::TransparencyFormP(CWnd *wPar, PolygonSetDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Transparency"),
transparency(100 *(1.0-dr->getTransparencyArea()))
{
	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&TransparencyFormP::setTransparency);
	slider->setContinuous(true);
	create();
}

int TransparencyFormP::setTransparency(Event *ev) {
	apply();
	return 1;
}

void  TransparencyFormP::apply() {
	slider->StoreData();
	PolygonSetDrawer *pdrw = (PolygonSetDrawer *)drw;
	pdrw->setTransparencyArea(1.0 - (double)transparency/100.0);
	/*PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);*/
	String transp("Transparency (%d)",transparency);
	TreeItem titem;
	view->getItem(pdrw->itemTransparentP,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

	strcpy(titem.item.pszText,transp.scVal());
	view->GetTreeCtrl().SetItem(&titem.item);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	pdrw->prepare(&parm);
	updateMapView();

}

//-----------------------------
BoundaryLineStyleForm::BoundaryLineStyleForm(CWnd *par, PolygonSetDrawer *pdr) 
	: DisplayOptionsForm(pdr, par, TR("Line Style"))
{

  fi = new FieldReal(root, TR("Line thickness"), &pdr->linethickness, ValueRange(1.0,100.0));
  flt = new FieldLineType(root, SDCUiLineType, &pdr->linestyle);
  fc = new FieldColor(root, TR("Line color"),&pdr->linecolor);

  create();
}

void  BoundaryLineStyleForm::apply() {
	fi->StoreData();
	flt->StoreData();
	fc->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}