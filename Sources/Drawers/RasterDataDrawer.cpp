#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterDataDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Drawers\RasterLayerDrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createRasterDataDrawer(DrawerParameters *parms) {
	return new RasterDataDrawer(parms);
}

RasterDataDrawer::RasterDataDrawer(DrawerParameters *parms) : 
	SpatialDataDrawer(parms,"RasterDataDrawer")
{
	setTransparency(rUNDEF);
}

RasterDataDrawer::~RasterDataDrawer(){
}

void RasterDataDrawer::prepare(PreparationParameters *pp){
	SpatialDataDrawer::prepare(pp);
	if ( pp->type == ptALL || pp->type & RootDrawer::ptGEOMETRY) {
		if ( !(pp->type & NewDrawer::ptANIMATION))
			clear();
		BaseMapPtr *bmptr = getBaseMap();
		BaseMap basemap;
		basemap.SetPointer(bmptr);
		RasterLayerDrawer *rsd;
		ILWIS::DrawerParameters dp(getRootDrawer(), this);
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basemap->fnObj);
		switch ( otype) {
			case IlwisObject::iotRASMAP:
				rsd = (RasterLayerDrawer *)NewDrawer::getDrawer("RasterLayerDrawer", pp, &dp);
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
				addLayerDrawer(basemap,pp,rsd);
				break;
		}
	} else {
		if ( pp->type & RootDrawer::ptRENDER) {
			for(int i = 0; i < drawers.size(); ++i) {
				RasterDataDrawer *rsd = (RasterDataDrawer *)drawers.at(i);
				PreparationParameters fp((int)pp->type, 0);
				rsd->prepare(&fp);
			}
		}
	}
}

void RasterDataDrawer::addLayerDrawer(const BaseMap& basemap,PreparationParameters *pp,ILWIS::LayerDrawer *rsd, const String& name) {
	PreparationParameters fp((int)pp->type, 0);
	//fp.rootDrawer = getRootDrawer();
	//fp.parentDrawer = this;
	fp.csy = basemap->cs();
	rsd->setName(name);
	rsd->setRepresentation(basemap->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSource(basemap, getRootDrawer()->getMapCoordBounds());
	rsd->getZMaker()->setDataSourceMap(basemap);
	rsd->addDataSource(basemap.ptr());
	rsd->prepare(&fp);
	addDrawer(rsd);
}

void RasterDataDrawer::addDataSource(void *bmap, int options){
	SpatialDataDrawer::addDataSource(bmap, options);
}

bool RasterDataDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
	SpatialDataDrawer::draw( cbArea);
	return true;
}
