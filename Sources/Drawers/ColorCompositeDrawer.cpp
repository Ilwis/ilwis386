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
	LayerDrawer::prepare(pp);
	
	if (!fSetupDone) {
		setup();
		fSetupDone = true;
	}

	if ( pp->type & NewDrawer::ptRENDER || pp->type & RootDrawer::ptRESTORE) {
		fUsePalette = false;
		textureHeap->RepresentationChanged();
		fLinear = (!getRootDrawer()->fConvNeeded(csy)) && gr()->fLinear();
	}
	if ((pp->type & pt3D) || ((pp->type & ptGEOMETRY || pp->type & ptRESTORE) && demTriangulator != 0)) {
		ZValueMaker * zMaker = getZMaker();
		bool is3DPossible = zMaker->getThreeDPossible();
		if (demTriangulator != 0) {
			delete demTriangulator;
			demTriangulator = 0;
		}
		if (is3DPossible) {
			demTriangulator = new DEMTriangulator(zMaker, mpl[0].ptr(), getRootDrawer(), false);
			if (!demTriangulator->fValid()) {
				delete demTriangulator;
				demTriangulator = 0;
			}
		}
	}
	if (pp->type & ptOFFSCREENSTART) {
		isThreadedBeforeOffscreen = isThreaded;
		isThreaded = false;
		textureHeapBeforeOffscreen = textureHeap;
		DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
		textureHeap = new TextureHeap();
		setData();
	}
	if (pp->type & ptOFFSCREENEND) {
		isThreaded = isThreadedBeforeOffscreen;
		delete textureHeap;
		textureHeap = textureHeapBeforeOffscreen;
	}
	if (pp->type & ptREDRAW)
		textureHeap->ReGenerateAllTextures();
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

String ColorCompositeDrawer::getInfo(const Coord& c) const {
	if ( !hasInfo() || !isActive())
		return "";
	Coord crd = c;
	Map rastermap = mpl[0];
	if (rastermap->cs() != rootDrawer->getCoordinateSystem())
	{
		crd = rastermap->cs()->cConv(rootDrawer->getCoordinateSystem(), c);
	}
	String infos;
	for(int i = 0; i < 3; ++i) {
		rastermap =  mpl[data->ccMaps[i].index];
		String info = rastermap->sValue(crd);
		info=info.sTrimSpaces();
		if ( infos!="")
			infos += "," ; 
		infos += info;
	}
	infos = "(" + infos + ")";

	return infos;
}

String ColorCompositeDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "ColorCompositeDrawer::" + parentSection;
	RasterLayerDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"MapList",fnView,  mpl->fnObj);
	for(int i = 0; i < 3; ++i) {
		String band("Band%d", i);
		long index = data->ccMaps[i].index;
		ObjectInfo::WriteElement(currentSection.c_str(),band.c_str(),fnView,  index);
	}

	return currentSection;
}

void ColorCompositeDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	RasterLayerDrawer::load(fnView, currentSection);
	FileName fnObj;
	ObjectInfo::ReadElement(currentSection.c_str(),"MapList",fnView,  fnObj);
	mpl = MapList(fnObj);
	for(int i = 0; i < 3; ++i) {
		String band("Band%d", i);
		long index;
		ObjectInfo::ReadElement(currentSection.c_str(),band.c_str(),fnView,  index);
		data->ccMaps[i].index = index;
	}
}








