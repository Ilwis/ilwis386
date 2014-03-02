#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "drawers\linedrawer.h"
#include "drawers\polygondrawer.h"
#include "Engine\Base\Algorithm\TriangulationAlg\gpc.h"
#include "Drawers\PolygonFeatureDrawer.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createPolygonLayerDrawer(DrawerParameters *parms) {
	return new PolygonLayerDrawer(parms);
}

PolygonLayerDrawer::PolygonLayerDrawer(DrawerParameters *parms) : 
	FeatureLayerDrawer(parms,"PolygonLayerDrawer"), 
	showAreas(true), 
	showBoundaries(true), 
	areaTransparency(1.0),
	usesTriangleFile(true),
	triData(0),
	currentLoc(0),
	triaFileSize(0),
	tesselator(gluNewTess())
{

}
PolygonLayerDrawer::~PolygonLayerDrawer() {
	if ( triData != 0) {
		delete [] triData;
		triData = 0;
	}
	gluDeleteTess(tesselator);
}

NewDrawer *PolygonLayerDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PolygonFeatureDrawer",pp, parms);

}

bool PolygonLayerDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	FeatureLayerDrawer::draw(drawLoop, cbArea);
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex()); // extra offset because of the boundary layer;
	return true;
}
void PolygonLayerDrawer::addDataSource(void *bmap,int options) {
	FeatureLayerDrawer::addDataSource(bmap, options);
	ComplexDrawer *cdr = (ComplexDrawer *)getParentDrawer();
	FeatureLayerDrawer *fdr = (FeatureLayerDrawer *)cdr->getDrawer(100, ComplexDrawer::dtPOST);
	if ( fdr)
		fdr->addDataSource(bmap);

}

bool PolygonLayerDrawer::getTriangleData(long **data, long** count) {
	*data = triData;
	*count = &currentLoc;
	return currentLoc != triaFileSize;
}

void PolygonLayerDrawer::prepare(PreparationParameters *parms) {
	BaseMap *bmap = (BaseMap *)getDataSource();
	FileName fnTriangle((*bmap)->fnObj,".tria#"); // old format, the code is not adapted for backwards compatibility, easiest is just delete it and re-create it
	if (fnTriangle.fExist())
		fnTriangle.fDelete();
	fnTriangle = FileName((*bmap)->fnObj,".tri#");
	if ( (parms->type & RootDrawer::ptGEOMETRY) && fnTriangle.fExist()) {
		if ( triData == 0) { // is already read or not
			ifstream file(fnTriangle.sFullPath().c_str(), ios::in|ios::binary);
			file.is_open();
			long size=1234;
			file.read((char *)(&size), 4);
			triData = new long[size];
			triData[0] = size;
			file.read((char *)(triData + 1),(size - 1)*4);
			currentLoc = 1; // first long is the total size of the file; irrelevant for the rest of the polygons
			triaFileSize = size;
		}
	}

	FeatureLayerDrawer::prepare(parms);

	if ( parms->type & RootDrawer::ptGEOMETRY) {
		if ( usesTriangleFile && triData == 0) {
			if ( !fnTriangle.fExist()) {
				ofstream file(fnTriangle.sFullPath().c_str(), ios::out|ios::binary|ios::ate);
				long cnt = 1;
				file.write((char *)&cnt,4);
				for(int i=0; i < getDrawerCount(); ++i) {
					cnt += ((PolygonFeatureDrawer *)getDrawer(i))->writeTriangleData(file);
				}
				file.seekp(0);
				file.write((char *)&cnt,4);
				file.close();
			}
		}
	}

	if (  parms->type & RootDrawer::ptRENDER || parms->type & ptRESTORE) {
		for(int i=0; i < drawers.size(); ++i) {
			PolygonDrawer *pd = (PolygonDrawer *)drawers.at(i);
			if ( !pd) 
				continue;
			PolygonProperties *props = (PolygonProperties *)pd->getProperties();
			props->drawColor = properties.drawColor;
			props->hatchName = properties.hatchName;
			props->ignoreColor = properties.ignoreColor;
			props->linestyle = properties.linestyle;
			props->thickness = properties.thickness;
		}
	}

}

void PolygonLayerDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT || method == drmNOTSET) {
		if ( useInternalDomain() || !rpr.fValid())
			setDrawMethod(drmMULTIPLE);
		else 
			setDrawMethod(drmRPR);

	} else
		drm = method;
}

double PolygonLayerDrawer::getTransparencyArea() const{
	return areaTransparency;
}

void PolygonLayerDrawer::setTransparencyArea(double v){
	areaTransparency = v;
}

void PolygonLayerDrawer::setShowAreas(bool yesno) {
	showAreas = yesno;
}

void PolygonLayerDrawer::setShowBoundaries(bool yesno){
	showBoundaries = yesno;
}

void PolygonLayerDrawer::setLineStyle(int st) {
	lp.linestyle = st;
}

void PolygonLayerDrawer::setLineThickness(double thick) {
	lp.thickness = thick;
}

void PolygonLayerDrawer::setHatch(const String& name) {
	lp.hatchName = name;
}

void PolygonLayerDrawer::setLineColor(const Color& clr) {
	lp.drawColor = clr;
}

GeneralDrawerProperties *PolygonLayerDrawer::getProperties() {
	return &lp;
}

String PolygonLayerDrawer::getHatchName() const {
	return lp.hatchName;
}


String PolygonLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "PolygonLayerDrawer::" + parentSection;
	FeatureLayerDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"ShowAreas",fnView, showAreas);
	ObjectInfo::WriteElement(currentSection.c_str(),"ShowBoundaries",fnView, showBoundaries);
	ObjectInfo::WriteElement(currentSection.c_str(),"AreaTransparency",fnView, areaTransparency);
	lp.store(fnView, currentSection);

	return currentSection;
}

void PolygonLayerDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	FeatureLayerDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"ShowAreas",fnView, showAreas);
	ObjectInfo::ReadElement(currentSection.c_str(),"ShowBoundaries",fnView, showBoundaries);
	ObjectInfo::ReadElement(currentSection.c_str(),"AreaTransparency",fnView, areaTransparency);
	lp.load(fnView, currentSection);
}

GLUtesselator * PolygonLayerDrawer::getTesselator()
{
	return tesselator;
}








