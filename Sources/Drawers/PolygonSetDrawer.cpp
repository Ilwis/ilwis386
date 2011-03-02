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
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
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
	usesTriangleFile(true),
	triData(0),
	currentLoc(0)
{

}
PolygonSetDrawer::~PolygonSetDrawer() {
}

NewDrawer *PolygonSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return IlwWinApp()->getDrawer("PolygonFeatureDrawer",pp, parms);

}

bool PolygonSetDrawer::draw( const CoordBounds& cbArea) const {
	FeatureSetDrawer::draw(cbArea);
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex()); // extra offset because of the boundary layer;
	return true;
}
void PolygonSetDrawer::addDataSource(void *bmap,int options) {
	FeatureSetDrawer::addDataSource(bmap, options);
	ComplexDrawer *cdr = (ComplexDrawer *)getParentDrawer();
	FeatureSetDrawer *fdr = (FeatureSetDrawer *)cdr->getDrawer(100, ComplexDrawer::dtPOST);
	if ( fdr)
		fdr->addDataSource(bmap);

}

void PolygonSetDrawer::getTriangleData(long **data, long** count) {
	*data = triData;
	*count = &currentLoc;
}

void PolygonSetDrawer::prepare(PreparationParameters *parms) {
	BaseMap *bmap = (BaseMap *)getDataSource();
	FileName fnTriangle((*bmap)->fnObj,".tria#");
	if ( (parms->type & RootDrawer::ptGEOMETRY) && fnTriangle.fExist()) {
		ifstream file(fnTriangle.sFullPath().scVal(), ios::in|ios::binary);
		file.is_open();
		long size=1234;
		file.read((char *)(&size), 4);
		triData = new long[size];
		triData[0] = size;
		file.read((char *)(triData + 1),(size - 1)*4);
		currentLoc = 1; // first long is the total size of the file; irrelevant for the rest of the polygons
	}

	FeatureSetDrawer::prepare(parms);

	if ( parms->type & RootDrawer::ptGEOMETRY) {
		if ( usesTriangleFile && triData == 0) {
			if ( !fnTriangle.fExist()) {
				ofstream file(fnTriangle.sFullPath().scVal(), ios::out|ios::binary|ios::ate);
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
	if ( triData != 0) {
		delete [] triData;
		triData = 0;
		currentLoc = 0;
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

void PolygonSetDrawer::setShowAreas(bool yesno) {
	showAreas = yesno;
}

void PolygonSetDrawer::setShowBoundaries(bool yesno){
	showBoundaries = yesno;
}

void PolygonSetDrawer::setLineStyle(int st) {
	lp.linestyle = st;
}

void PolygonSetDrawer::setLineThickness(double thick) {
	lp.thickness = thick;
}

void PolygonSetDrawer::setLineColor(const Color& clr) {
	lp.drawColor = clr;
}

GeneralDrawerProperties *PolygonSetDrawer::getProperties() {
	return &lp;
}




