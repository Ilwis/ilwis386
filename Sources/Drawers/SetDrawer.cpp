#include "Headers\toolspch.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Domain\DomainTime.h" 
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

int mycount = 0;



SetDrawer::SetDrawer(DrawerParameters *parms, const String& title) : 
	SpatialDataDrawer(parms,title),
	sourceType(sotUNKNOWN)
{
	setTransparency(1);
}

SetDrawer::~SetDrawer(){
}

String SetDrawer::description() const {
	String sName = getName();
	//String sDescr = (*datasource)->sDescr();
	//if ("" != sDescr) 
	//	sName = String("%S Animated", sName);
	return sName;
}

RangeReal SetDrawer::getMinMax(const ObjectCollection& oc) const{
	RangeReal rrMinMax (0, 255);
	return oc->getRange();
	//for(int i = 0; i < oc->iNrObjects(); ++i) {
	//	if (IOTYPEBASEMAP( oc->fnObject(i))) {
	//		BaseMap mp(oc->fnObject(i));
	//		if (mp.fValid()) {
	//			RangeReal rrMinMaxMap = mp->rrMinMax(BaseMapPtr::mmmSAMPLED);
	//			if (rrMinMaxMap.rLo() > rrMinMaxMap.rHi())
	//				rrMinMaxMap = mp->vr()->rrMinMax();
	//			if (fFirst) {
	//				rrMinMax = rrMinMaxMap;
	//				fFirst = false;
	//			}
	//			else
	//				rrMinMax += rrMinMaxMap;
	//		}
	//	}
	//}
	//return rrMinMax;
}
RangeReal SetDrawer::getMinMax(const MapList& mlist) const{
	RangeReal rrMinMax (0, 255);
	if (mlist->iSize() > 0) {
		if (mlist->map(0)->dm()->pdv()) {
			for (int i = 0; i < mlist->iSize(); ++i) {
				Map mp = mlist->map(i);
				RangeReal rrMinMaxMap = mp->rrMinMax(BaseMapPtr::mmmSAMPLED);
				if (rrMinMaxMap.rLo() > rrMinMaxMap.rHi())
					rrMinMaxMap = mp->vr()->rrMinMax();
				if (i > 0)
					rrMinMax += rrMinMaxMap;
				else
					rrMinMax = rrMinMaxMap;
			}
		} else {
			    ILWIS::LayerDrawer *sdr = (ILWIS::LayerDrawer *)const_cast<SetDrawer *>(this)->getDrawer(0);
				if ( !sdr)
					return RangeReal();
				if (sdr->useAttributeColumn() && sdr->getAtttributeColumn()->dm()->pdv()) {
				for (int i = 0; i < mlist->iSize(); ++i) {
					Map mp = mlist->map(i);
					if (i > 0)
						rrMinMax += sdr->getAtttributeColumn()->vr()->rrMinMax();
					else
						rrMinMax = sdr->getAtttributeColumn()->vr()->rrMinMax();
				}
			}
		}
	}
	return rrMinMax;
}
void SetDrawer::prepare(PreparationParameters *pp){
	SpatialDataDrawer::prepare(pp);
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	if ( sourceType == sotFEATURE ) {
		if ( pp->type & NewDrawer::ptGEOMETRY || pp->type & NewDrawer::ptRESTORE) {
			if ( getName() == "")
				setName(oc->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), this);
			if ( drawers.size() > 0) {
				clear();
			}
			Tranquilizer trq(TR("Adding maps"));
			// Calculate the min/max over the whole maplist. This is used for palette and texture generation.
			rrMinMax = getMinMax(oc);
			Palette * palette;
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				//IlwisObject::iotIlwisObjectType type = IOTYPE();
				if (IOTYPEBASEMAP( oc->fnObject(i))) {
					BaseMap bmp(oc->fnObject(i));
					if ( bmp.fValid()) {
						if ( !rpr.fValid())
							rpr = bmp->dm()->rpr();
						ILWIS::LayerDrawer *drw = createIndexDrawer(i,bmp, dp, pp);
						RasterLayerDrawer * rasterset = dynamic_cast<RasterLayerDrawer*>(drw);
						if (rasterset) {
							if ( rrMinMax.fValid())
								rasterset->setMinMax(rrMinMax);
							if (i == 0)
								palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
							else
								rasterset->SetPalette(palette);
						} 
						drw->setUICode(0);
						trq.fUpdate(i,oc->iNrObjects()); 
					}
				}
			}
		} 
		if ( pp->type && NewDrawer::ptRENDER) {
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				ILWIS::LayerDrawer *sdr = (ILWIS::LayerDrawer *)getDrawer(i);
				if (!sdr)
					continue;
				PreparationParameters prp(*pp);
				prp.index = i;
				prp.csy = sdr->getCoordSystem();
				prp.type = NewDrawer::ptRENDER;
				sdr->prepare(&prp);
			}
		}
	}
	if ( sourceType == sotMAPLIST) {
		bool isRestore = (pp->type & NewDrawer::ptRESTORE) != 0;
		if ( pp->type & NewDrawer::ptGEOMETRY || isRestore) {
			if ( getName() == "Unknown")
				setName(mpl->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), this);
			if ( drawers.size() > 0 && !isRestore) {
				clear();
			}
			// Calculate the min/max over the whole maplist. This is used for palette and texture generation.
			rrMinMax = getMinMax(mpl);
			Palette * palette;
			for(int i = 0; i < mpl->iSize(); ++i) {
				ILWIS::DrawerParameters parms(getRootDrawer(), this);
				Map mp = mpl->map(i);
				if ( !rpr.fValid())
					rpr = mp->dm()->rpr();
				RasterLayerDrawer *rasterset;
				if ( !isRestore) {
					rasterset = (RasterLayerDrawer *)NewDrawer::getDrawer("RasterLayerDrawer", "Ilwis38", &parms);
				} else
					rasterset = (RasterLayerDrawer *)getDrawer(i);
				rasterset->setThreaded(false);
				if ( rrMinMax.fValid())
					rasterset->setMinMax(rrMinMax);
				if (i == 0)
					palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
				else
					rasterset->SetPalette(palette);
				pp->index = i;
				addLayerDrawer(i,mp,pp,rasterset,String("band %d",i));
				getZMaker()->setBounds(getRootDrawer()->getMapCoordBounds());
			}
		}
	}
	if ( pp->type & NewDrawer::pt3D || pp->type & NewDrawer::ptRESTORE) {
		for(int i=0; i < drawers.size(); ++i) {
			pp->index = i;
			drawers.at(i)->prepare(pp);
		}
	}
	setDrawMethod(getDrawer(0)->getDrawMethod());

}
void SetDrawer::setTransparency(double v) {
	for(int i = 0; i < getDrawerCount(); ++i) {
		NewDrawer *drw = getDrawer(i);
		drw->setTransparency(v);
	}
}

LayerDrawer *SetDrawer::createIndexDrawer(int index, const BaseMap& basem,ILWIS::DrawerParameters& dp, PreparationParameters* pp) {
	LayerDrawer *layerDrawer;
	IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basem->fnObj);
	switch ( otype) {
		case IlwisObject::iotPOINTMAP:
			layerDrawer = (LayerDrawer *)NewDrawer::getDrawer("PointLayerDrawer", pp, &dp); 
			addLayerDrawer(index, basem,pp,layerDrawer);
			break;
		case IlwisObject::iotSEGMENTMAP:
			layerDrawer = (LayerDrawer *)NewDrawer::getDrawer("LineLayerDrawer", pp, &dp); 
			addLayerDrawer(index, basem,pp,layerDrawer);
			break;
		case IlwisObject::iotPOLYGONMAP:
			layerDrawer = (LayerDrawer *)NewDrawer::getDrawer("PolygonLayerDrawer", pp, &dp); 
			addLayerDrawer(index, basem,pp,layerDrawer, "Areas");
			break;
		case IlwisObject::iotRASMAP:
			layerDrawer = (LayerDrawer *)NewDrawer::getDrawer("RasterLayerDrawer", pp, &dp); 
			addLayerDrawer(index, basem,pp,layerDrawer);
			break;
	}
	return layerDrawer;
}

void SetDrawer::addLayerDrawer(int index, const BaseMap& basem,PreparationParameters *pp,ILWIS::LayerDrawer *rsd, const String& name, bool post) {
	PreparationParameters fp((int)pp->type | NewDrawer::ptANIMATION, 0);
	fp.csy = basem->cs();
	fp.index= index;
	rsd->setName(name);
	rsd->setRepresentation(basem->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSource(basem, getRootDrawer()->getMapCoordBounds());
	rsd->getZMaker()->setDataSourceMap(basem);
	rsd->addDataSource(basem.ptr());
	rsd->prepare(&fp);
	if (!post) {
		if ( ! (pp->type & NewDrawer::ptRESTORE))
			addDrawer(rsd);
		if ( basem.fValid()) {
			CoordBounds cbMap = basem->cb();
			CoordSystem cs = basem->cs();
			CoordBounds ncb = cbMap;
			if (!cs.fEqual(getRootDrawer()->getCoordinateSystem())) 
				ncb = getRootDrawer()->getCoordinateSystem()->cbConv(cs,cbMap);
			cbBounds += ncb;
		}
	}
}

void SetDrawer::addDataSource(void *data, int options){
	SpatialDataDrawer::addDataSource(data, options);
	IlwisObject::iotIlwisObjectType type = IOTYPE(obj->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION) {
		sourceType = sotFEATURE;
	}
	if ( type == IlwisObject::iotMAPLIST) {
		sourceType = sotMAPLIST;
	}
}

void SetDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	DomainValueRangeStruct dvs = getBaseMap()->dvrs();
	Color clr;
	ILWIS::LayerDrawer *dr = (ILWIS::LayerDrawer *)(const_cast<SetDrawer *>(this)->getDrawer(0));
	if ( dvs.dm()->pdv())
		clr = dr->getDrawingColor()->clrVal(rVal);
	else
		clr = dr->getDrawingColor()->clrRaw((long)rVal, dr->getDrawMethod());
	

	CBrush brushColor(clr);
	CBrush *br = dc->SelectObject(&brushColor);
	dc->Rectangle(rct);
	dc->SelectObject(br);
}


bool SetDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	SpatialDataDrawer::draw( cbArea);
	return true;
}


String SetDrawer::iconName(const String& subtype) const {
	return "Set";
}

void SetDrawer::updateLegendItem() {
	//if ( doLegend)
	//	doLegend->updateLegendItem();
}

