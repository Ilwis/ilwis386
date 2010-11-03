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
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPolygonSetDrawer(DrawerParameters *parms) {
	return new PolygonSetDrawer(parms);
}

PolygonSetDrawer::PolygonSetDrawer(DrawerParameters *parms) : 
	FeatureSetDrawer(parms,"PolygonSetDrawer")
{
	PreparationParameters pp(NewDrawer::ptALL, 0);
	pp.csy = ((AbstractMapDrawer *)getParentDrawer())->getBaseMap()->cs();
	FeatureSetDrawer *fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("LineSetDrawer", &pp, parms); // boundaries 
	fsd->setUICode(NewDrawer::ucNOINFO | NewDrawer::ucNOMASK);
	fsd->setName("Boundaries");
	fsd->setSingleColor(Color(0,0,0));
	fsd->setDrawMethod(NewDrawer::drmSINGLE);
	BaseMap bmp;
	bmp.SetPointer(((AbstractMapDrawer *)getParentDrawer())->getBaseMap());
	fsd->getZMaker()->setSpatialSourceMap(bmp);
	fsd->prepare(&pp);
	((ComplexDrawer *)getParentDrawer())->addPostDrawer(100,fsd);
}

PolygonSetDrawer::~PolygonSetDrawer() {
}

NewDrawer *PolygonSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return IlwWinApp()->getDrawer("PolygonFeatureDrawer",pp, parms);

}

HTREEITEM PolygonSetDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = FeatureSetDrawer::configure(tv,parent);
	return hti;
}

void PolygonSetDrawer::prepare(PreparationParameters *parms) {
	SetDrawer::test_count = 0;
	FeatureSetDrawer::prepare(parms);
	TRACE(String("%d", SetDrawer::test_count).scVal());
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
