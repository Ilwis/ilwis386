#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Drawers\RasterSetDrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createRasterLayerDrawer(DrawerParameters *parms) {
	return new RasterLayerDrawer(parms);
}

RasterLayerDrawer::RasterLayerDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"RasterLayerDrawer")
{
	setTransparency(rUNDEF);
}

RasterLayerDrawer::~RasterLayerDrawer(){
}

void RasterLayerDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	if ( pp->type == ptALL || pp->type & RootDrawer::ptGEOMETRY) {
		if ( !(pp->type & NewDrawer::ptANIMATION))
			clear();
		BaseMapPtr *bmptr = getBaseMap();
		BaseMap basemap;
		basemap.SetPointer(bmptr);
		RasterSetDrawer *rsd;
		ILWIS::DrawerParameters dp(getRootDrawer(), this);
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basemap->fnObj);
		switch ( otype) {
			case IlwisObject::iotRASMAP:
				rsd = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", pp, &dp);
				RangeReal rrMinMax (0, 255);
				Domain dm = basemap->dm();
				if (dm.fValid() && (dm->pdbit() || dm->pdbool()))
					rrMinMax = RangeReal(1,2);
				else if ( basemap->dm()->pdv()) {
					rrMinMax = basemap->rrMinMax(BaseMapPtr::mmmCALCULATE); // not mmmSAMPLED here, to get a more accurate result, otherwise there's a high chance of artifacts since the sampling is only done on this one band
					if (rrMinMax.rLo() > rrMinMax.rHi())
						rrMinMax = basemap->vr()->rrMinMax();
				} else {
					if (  rsd->useAttributeColumn() && rsd->getAtttributeColumn()->dm()->pdv()) {

						rrMinMax = rsd->getAtttributeColumn()->vr()->rrMinMax();
					}
				}
				rsd->setMinMax(rrMinMax);
				rsd->SetPaletteOwner(); // this set has the only available palette
				addSetDrawer(basemap,pp,rsd);
				break;
		}
	} else {
		if ( pp->type & RootDrawer::ptRENDER) {
			for(int i = 0; i < drawers.size(); ++i) {
				RasterSetDrawer *rsd = (RasterSetDrawer *)drawers.at(i);
				PreparationParameters fp((int)pp->type, 0);
				rsd->prepare(&fp);
			}
		}
	}
}

void RasterLayerDrawer::addSetDrawer(const BaseMap& basemap,PreparationParameters *pp,SetDrawer *rsd, const String& name) {
	PreparationParameters fp((int)pp->type, 0);
	fp.rootDrawer = getRootDrawer();
	fp.parentDrawer = this;
	fp.csy = basemap->cs();
	rsd->setName(name);
	rsd->setRepresentation(basemap->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSourceMap(basemap);
	rsd->getZMaker()->setDataSourceMap(basemap);
	rsd->addDataSource(basemap.ptr());
	rsd->prepare(&fp);
	addDrawer(rsd);
}

void RasterLayerDrawer::addDataSource(void *bmap, int options){
	AbstractMapDrawer::addDataSource(bmap, options);
}

HTREEITEM RasterLayerDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	if ( getUICode() == 0)
		return parent;
	return AbstractMapDrawer::configure(tv,parent);
}

bool RasterLayerDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	AbstractMapDrawer::draw(norecursion, cbArea);
	return true;
}
