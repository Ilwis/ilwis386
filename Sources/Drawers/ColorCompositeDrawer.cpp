#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\ColorCompositeDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Representation\Rprclass.h"
#include <GL/glu.h>
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Texture.h"
#include "DEMTriangulator.h"


using namespace ILWIS;

ILWIS::NewDrawer *createColorCompositeDrawer(DrawerParameters *parms) {
	return new ColorCompositeDrawer(parms);
}

ColorCompositeDrawer::ColorCompositeDrawer(DrawerParameters *parms) : 
RasterLayerDrawer(parms,"ColorCompositeDrawer")
{
}


void ColorCompositeDrawer::prepareChildDrawers(PreparationParameters *pp){
	prepare(pp);
}

void ColorCompositeDrawer::prepare(PreparationParameters *pp){
	RasterLayerDrawer::prepare(pp);

}

void ColorCompositeDrawer::setData() const{
	DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
	textureHeap->SetData(mpl, getDrawingColor(), getDrawMethod(), drawcontext->getMaxPaletteSize(), data, rrMinMax, drawcontext);
}


GeoRef ColorCompositeDrawer::gr() const {
	return mpl[0]->gr();
}


void ColorCompositeDrawer::addDataSource(void *bmap, int options){
	IlwisObject *obj = (IlwisObject *)bmap;
	if (IOTYPE((*(obj))->fnObj) == IlwisObject::iotMAPLIST) {
		mpl.SetPointer(obj->pointer());
	}
}

void ColorCompositeDrawer::setDrawMethod(DrawMethod method) {
	drm = drmCOLOR;
}



bool ColorCompositeDrawer::isColorComposite() const {
	return mpl.fValid() && mpl->iSize() >= 3;
}

int ColorCompositeDrawer::getColorCompositeBand(int index) {
	if ( mpl.fValid() && index < 3) {
		return data->ccMaps[index].index;
	}
	return iUNDEF;
}

void ColorCompositeDrawer::setColorCompositeBand(int index, int maplistIndex) {
	if ( mpl.fValid() && index < 3 && maplistIndex < mpl->iSize()) {
		data->ccMaps[index].index = maplistIndex;
	}
}

MapList ColorCompositeDrawer::getMapList() const{
	return mpl;
}

void ColorCompositeDrawer::setColorCompositeRange(int index, const RangeReal& rr){
	if ( data && mpl.fValid()) {
		if ( index < 3)
			data->ccMaps[index].rr = rr;
	}
}

RangeReal ColorCompositeDrawer::getColorCompositeRange(int index){
	if ( data && mpl.fValid()) {
		if ( index < 3)
			return data->ccMaps[index].rr;
	}
	return RangeReal();
}

Color ColorCompositeDrawer::getExceptionColor() const {
	if ( data && mpl.fValid()) {
		return data->exceptionColor;
	}
	return colorUNDEF;
}

void ColorCompositeDrawer::setExceptionColor(const Color& clr){
	if ( data && mpl.fValid()) {
		data->exceptionColor = clr;
	}
}







