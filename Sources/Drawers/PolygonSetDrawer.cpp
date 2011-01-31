#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
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
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms) {
	return new PolygonSetDrawer(parms);
}

PolygonSetDrawer::PolygonSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"PolygonSetDrawer"), showAreas(true), showBoundaries(true), areaTransparency(1.0)
{
	//PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER , 0);
	//pp.csy = ((AbstractMapDrawer *)getParentDrawer())->getBaseMap()->cs();
	//FeatureSetDrawer *fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("LineSetDrawer", &pp, parms); // boundaries 
	//fsd->setUICode(NewDrawer::ucNOINFO | NewDrawer::ucNOMASK);
	//fsd->setName("Boundaries");
	//fsd->setSingleColor(Color(0,0,0));
	//fsd->setDrawMethod(NewDrawer::drmSINGLE);
	//BaseMap bmp;
	//bmp.SetPointer(((AbstractMapDrawer *)getParentDrawer())->getBaseMap());
	//fsd->getZMaker()->setSpatialSourceMap(bmp);
	//fsd->getZMaker()->setDataSourceMap(bmp);
	////fsd->prepare(&pp);
	//((ComplexDrawer *)getParentDrawer())->addPostDrawer(100,fsd);
	//setName("Areas");
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
	item = new DisplayOptionTreeItem(tv,parent,this,
				(SetCheckFunc)&PolygonSetDrawer::setActiveAreas);

				
	HTREEITEM itemAreas = InsertItem("Areas",".mpa",item,(int)showAreas);

	item = new DisplayOptionTreeItem(tv,itemAreas,this,(DisplayOptionItemFunc)&PolygonSetDrawer::displayOptionTransparencyP);
	String transp("Transparency (%d)", 100 * getTransparency());
	itemTransparentP = InsertItem(transp,"Transparent", item, -1);
	return hti;
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
			PolygonDrawer *pdr = (PolygonDrawer *)drawers.at(i);
			pdr->boundariesActive(showBoundaries);
			pdr->areasActive(showAreas);
			pdr->setTransparencyArea(areaTransparency);

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
