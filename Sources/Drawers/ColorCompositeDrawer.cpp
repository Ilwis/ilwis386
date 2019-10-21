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
		fSameGeoRef = getRootDrawer()->getGeoReference().fValid() && gr().fValid() && (getRootDrawer()->getGeoReference() == gr());
		fLinear = fSameGeoRef || ((!getRootDrawer()->fConvNeeded(csy)) && gr()->fLinear());
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

void ColorCompositeDrawer::init() const
{
	// fetch the image's coordinate bounds
	DrawerContext* drawcontext = (getRootDrawer())->getDrawerContext();
	data->maxTextureSize = drawcontext->getMaxTextureSize();
	int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN); // maximum X size of client area (regardless of current viewport)
	int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN); // maximum Y size of client area
	if (iXScreen < data->maxTextureSize) // prevent making textures that are larger than the screen, it is totally unnecessary and a big performance and memory hit
		data->maxTextureSize = iXScreen;
	if (iYScreen < data->maxTextureSize)
		data->maxTextureSize = iYScreen;
	if ( !gr()->rcSize().fUndef()) {
		data->imageWidth = gr()->rcSize().Col;
		data->imageHeight = gr()->rcSize().Row;
	} else if ( mpl.fValid()) {
		data->imageWidth = mpl->rcSize().Col;
		data->imageHeight = mpl->rcSize().Row;
	}

	double log2width = log((double)data->imageWidth)/log(2.0);
	log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	data->width = pow(2, log2width);
	double log2height = log((double)data->imageHeight)/log(2.0);
	log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	data->height = pow(2, log2height);

	setData();

	if (fPaletteOwner && fUsePalette) {
		palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);
		palette->Refresh();
		getRootDrawer()->getDrawerContext()->setActivePalette(0);
	}
	data->init = true;
}

void ColorCompositeDrawer::setData() const{
	DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
	textureHeap->SetData(mpl, getDrawingColor(), getDrawMethod(), drawcontext->getMaxPaletteSize(), data, rrMinMax, drawcontext);
}

const GeoRef & ColorCompositeDrawer::gr() const {
	return mpl[0]->gr();
}


void ColorCompositeDrawer::addDataSource(void *bmap, int options){
	IlwisObject *obj = (IlwisObject *)bmap;
	if (IOTYPE((*(obj))->fnObj) == IlwisObject::iotMAPLIST) {
		mpl.SetPointer(obj->pointer());
		long iMaxIndex = mpl->iSize() - 1;
		for (int i = 0; i < 3; ++i) {
			data->ccMaps[i].index = min(data->ccMaps[i].index, iMaxIndex);
		}
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

String ColorCompositeDrawer::store(const FileName& fnView, const String& section) const{
	String layerSection = RasterLayerDrawer::store(fnView, section);// + ":ColorComposite";	
	String currentSection = layerSection + ":ColorComposite";
	ObjectInfo::WriteElement(currentSection.c_str(),"MapList",fnView,  mpl->fnObj);
	for(int i = 0; i < 3; ++i) {
		String band("Band%d", i);
		ObjectInfo::WriteElement(currentSection.c_str(),band.c_str(),fnView, data->ccMaps[i].index);
		ObjectInfo::WriteElement(currentSection.c_str(),(band + "_StretchReal").c_str(),fnView, data->ccMaps[i].rr);
	}

	return layerSection;
}

void ColorCompositeDrawer::load(const FileName& fnView, const String& section){
	String currentSection = section;
	RasterLayerDrawer::load(fnView, currentSection);
	currentSection += ":ColorComposite";
	FileName fnObj;
	ObjectInfo::ReadElement(currentSection.c_str(),"MapList",fnView,  fnObj);
	mpl = MapList(fnObj);
	for(int i = 0; i < 3; ++i) {
		String band("Band%d", i);
		ObjectInfo::ReadElement(currentSection.c_str(),band.c_str(),fnView, data->ccMaps[i].index);
		ObjectInfo::ReadElement(currentSection.c_str(),(band + "_StretchReal").c_str(),fnView, data->ccMaps[i].rr);
	}
}

void *ColorCompositeDrawer::getDataSource() const{
	return 0; // no annotation-legend for colorcomposite
}







