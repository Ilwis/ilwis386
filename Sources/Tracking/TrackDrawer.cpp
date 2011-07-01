#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\LayerDrawer.h"
//#include "Drawers\AnimationDrawer.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Tracking.H"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Tracking\TrackDrawer.h"

using namespace ILWIS;

DrawerInfoVector *createDrawer() {
	DrawerInfoVector *infos = new DrawerInfoVector();
	infos->push_back(new DrawerInfo("TrackSetDrawer","ilwis38",createTrackSetDrawer));

	return infos;
}

ILWIS::NewDrawer *createTrackSetDrawer(DrawerParameters *parms) {
	return new TrackSetDrawer(parms);
}

TrackSetDrawer::TrackSetDrawer(DrawerParameters *parms) : 
	ComplexDrawer(parms,"TrackSetDrawer"),
	linedrawer(0),
	pointdrawer(0)
{
	setDrawMethod(drmSINGLE); // default;
}

TrackSetDrawer::~TrackSetDrawer() {
}

NewDrawer *TrackSetDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("TrackSetDrawer",pp, parms);

}

void TrackSetDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) {
		setDrawMethod(drmSINGLE);

	} else
		drm = method;
}

void TrackSetDrawer::getTrack(vector<Coord>& crds) const{
	crds.resize(coords.size());
	copy(coords.begin(), coords.end(),crds.begin());
}

long TrackSetDrawer::getTrackLength() const {
	return coords.size();
}


String TrackSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "TrackSetDrawer::" + parentSection;
	ComplexDrawer::store(fnView, currentSection);
	trackerDrawProperties.lproperties.store(fnView, currentSection);
	trackerDrawProperties.pproperties.store(fnView, currentSection);
	return currentSection;
}

void TrackSetDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	ComplexDrawer::load(fnView, currentSection);
	trackerDrawProperties.lproperties.load(fnView, currentSection);
	trackerDrawProperties.pproperties.load(fnView, currentSection);
}

bool TrackSetDrawer::draw(const CoordBounds& cb) const {
	return ComplexDrawer::draw(cb);
}


void TrackSetDrawer::prepare(PreparationParameters *parm){
	ComplexDrawer::prepare(parm);
	if ( parm->type & NewDrawer::ptGEOMETRY) {
		if ( getDrawerCount() == 0) {
			DrawerParameters dp(getRootDrawer(), this);
			LineDrawer *drw = (LineDrawer *)NewDrawer::getDrawer("LineDrawer", parm, &dp);
			addDrawer(drw);
			for(int i = 0; i < coords.size(); ++i) {
				PointDrawer *pdrw = (PointDrawer *)NewDrawer::getDrawer("PointDrawer", parm, &dp);
				addDrawer(pdrw);
			}
		}
		((LineDrawer *)getDrawer(0))->addCoords(coords,1);
		for(int i = 0; i < coords.size(); ++i) {
			PointDrawer *pdrw = (PointDrawer *)getDrawer(i+1);
			pdrw->setCoord(coords[i]);
			pdrw->prepare(parm);
		}

	
	}
}
void TrackSetDrawer::addDataSource(void *l,int options){
	vector<Coord> *line = (vector<Coord>  *)l;
	coords.resize(line->size());
	copy(line->begin(), line->end(), coords.begin());
}

GeneralDrawerProperties *TrackSetDrawer::getProperties() {
	return &trackerDrawProperties;
}
//-----------------------------------------------

