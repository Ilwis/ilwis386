#include "Headers\toolspch.h"
#include "Headers\messages.h"
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
#include "Drawers\AnimationDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

int AnimationDrawer::timerIdCounter=5000;
#define REAL_TIME_INTERVAL 100
#define BOX_DRAWER_ID 497

ILWIS::NewDrawer *createAnimationDrawer(DrawerParameters *parms) {
	return new AnimationDrawer(parms);
}

AnimationDrawer::AnimationDrawer(DrawerParameters *parms) : 
	SetDrawer(parms,"AnimationDrawer"),
	timerid(iUNDEF),
	interval(1.0),
	loop(true),
	index(0),
	useTime(false),
	mapIndex(0),
	offset(0),
	manager(0)
{
	setTransparency(1);
	last = 0;
}

AnimationDrawer::~AnimationDrawer(){
	if ( manager && manager->GetSafeHwnd() != 0) {
		manager->SendMessage(ILWM_UPDATE_ANIM,128,(long)this);
	}
}

String AnimationDrawer::description() const {
	String sName = getName();
	String sDescr = obj->sDescr();
	if ("" != sDescr) 
		sName = String("%S Animated", sName);
	return sName;
}

void AnimationDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY || pp->type & NewDrawer::ptRESTORE) {
		if ( getDrawerCount() > 0) {
			for(int i=0; i < getDrawerCount(); ++i)
				getDrawer(i)->setActive(i == 0 ? true : false);
		}
	}
}


void AnimationDrawer::addDataSource(void *data, int options){
	SetDrawer::addDataSource(data, options);
	if ( obj.fValid()) {
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(obj->fnObj);
		if  (type == IlwisObject::iotSEGMENTMAP || 
			 type == IlwisObject::iotPOINTMAP || 
			 type == IlwisObject::iotPOLYGONMAP ) {
			 sourceType = sotFEATURE;
		}
		else if ( sourceType == sotFEATURE) {
			activeMaps.clear();
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				activeMaps.push_back(i);
			}
		}
		if ( sourceType == sotMAPLIST) {
			activeMaps.clear();
			for(int i = 0; i < mpl->iSize(); ++i) {
				activeMaps.push_back(i);
			}
		}
	}
}

bool AnimationDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
	SetDrawer::draw( drawerIndex, cbArea);
	return true;
}

void AnimationDrawer::animationDefaultView() {

	for(int i =0 ; i < drawers.size(); ++i) {
		ILWIS::LayerDrawer *sdr = (ILWIS::LayerDrawer *)drawers.at(i);
		NewDrawer *drPost;
		if ( (drPost = sdr->getDrawer(RSELECTDRAWER,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);
		if ( (drPost = sdr->getDrawer(BOX_DRAWER_ID,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);

		if ( mpl[i]->dm()->pdi()) {
			sdr->setDrawMethod(NewDrawer::drmIMAGE);
		} else {
			sdr->setRepresentation(mpl[i]->dm()->rpr());
			sdr->setStretchRangeReal(mpl[i]->rrPerc1());
		}
		PreparationParameters pp(NewDrawer::ptRENDER);
		sdr->prepare(&pp);
	}
	getRootDrawer()->getDrawerContext()->doDraw();


}

void AnimationDrawer::timedEvent(UINT _timerid) {
    ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	bool redraw = false;
	if ( timerid == _timerid) {
		if ( useTime) {
			redraw = timerPerTime();
		} else {
			redraw = timerPerIndex();
		}
		if ( redraw) {
			getRootDrawer()->getDrawerContext()->doDraw();
		}
		currentIndex = activeMaps[mapIndex];
	}
}

bool AnimationDrawer::timerPerIndex() {
	int nmaps = getDrawerCount();
	int cindex = min(nmaps - 1, mapIndex % nmaps); // some safeguards for corrupt data.
	int nindex = min(nmaps - 1,(mapIndex + 1) % nmaps);
	getDrawer(activeMaps[cindex])->setActive(false);
	getDrawer(activeMaps[nindex])->setActive(true);
	mapIndex = nindex;
	IlwisObjectPtr *obj = getObject();
	if ( obj) 
		getEngine()->SendMessage(ILWM_UPDATE_ANIM,(WPARAM)&(obj->fnObj), mapIndex); 
	for(int i=0; i < slaves.size(); ++i) {
		SlaveProperties& props = slaves.at(i);
		props.threshold += props.slaveStep;
		if ( props.threshold >= 1.0){
			props.slave->timedEvent(SLAVE_TIMER_ID);
			props.threshold -= 1.0;
		}


	}
	return true;
}

bool AnimationDrawer::activeOnTime(const Column& col, double currentTime) {
	if ( mapIndex < activeMaps.size() - 1 && col->rValue(activeMaps[mapIndex]) < currentTime){
		getDrawer(activeMaps[mapIndex])->setActive(false);
		++mapIndex;
		getDrawer(activeMaps[mapIndex])->setActive(true);
		return true;
	} else {
		if (loop && mapIndex >= activeMaps.size() -1 && currentTime >= col->rValue(col->iRecs() - 1)) {
			getDrawer(activeMaps[mapIndex])->setActive(false);
			mapIndex = 0;
			index = 0;
			getDrawer(activeMaps[0])->setActive(true);
			return true;
		}
	}
	return false;
}
bool AnimationDrawer::timerPerTime() {
	if ( (double)timestep == rUNDEF || (double)timestep == 0.0)
		return false;

	bool redraw = false;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(obj->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		Column col = mpl->tblAtt()->col(colTime);
		ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
		double steps = 1000.0 / REAL_TIME_INTERVAL;
		double offset =  timestep * (double)index / steps;
		double lowtime = col->rrMinMax().rLo();
		double currentTime = lowtime + offset;
		redraw = activeOnTime(col, currentTime);
		for(int i=0; i < slaves.size(); ++i) {
			SlaveProperties& props = slaves.at(i);
			props.slave->timedEvent(SLAVE_TIMER_ID);

		}
	}
	
	++index;

	return redraw;

}

void AnimationDrawer::setTimeStep(ILWIS::Duration dur) 
{
	//MapList mpl;
	//mpl.SetPointer(datasource->pointer());
	//Column col = mpl->tblAtt()->col(colTime);
	//if ( useTime && col.fValid() && (double)timestep > 0) {
	//	index = 0;
	//	if ( (double)timestep < 300.0  )
	//		mapIndex = 13;
	//	double steps = 1000.0 / REAL_TIME_INTERVAL;
	//	double baseTime = col->rrMinMax().rLo();
	//	double offset = 0;
	//	double currentTime = col->rValue(activeMaps[mapIndex]);
	//	while ( currentTime > (baseTime + offset)){
	//		offset =  timestep * (double)index / steps;
	//		++index;
	//	}

	//}
	index = 0;
	setMapIndex(0);
	timestep = dur; 
}

ILWIS::Duration AnimationDrawer::getTimeStep() const {
	return timestep;
}

String AnimationDrawer::iconName(const String& subtype) const {
	return "Animation";
}

void AnimationDrawer::setMapIndex(int ind) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	for(int i =0 ; i < drawers.size(); ++i)
		getDrawer(i)->setActive(false);

	int nmaps = activeMaps.size();
	mapIndex = (ind)  % nmaps;
	getDrawer(activeMaps[mapIndex])->setActive(true);
	for(int i =0; i < slaves.size(); ++i) {
		AnimationDrawer *slave = slaves[i].slave;
		if ( slave) {
			slave->setMapIndex(ind);
		}
	}
	currentIndex = activeMaps[mapIndex];
	getEngine()->SendMessage(ILWM_UPDATE_ANIM,(WPARAM)&(obj->fnObj), mapIndex); 
}


int AnimationDrawer::getTimerIdCounter(bool increase) {
	if ( increase)
		++timerIdCounter;

	return timerIdCounter;
}

void AnimationDrawer::addSlave(const SlaveProperties& pr) {
	for(int i =0; i < slaves.size(); ++i) {
		if ( slaves.at(i).slave->getId() == pr.slave->getId())
			return;
	}
	slaves.push_back(pr);
	pr.slave->setTimerId(SLAVE_TIMER_ID);
}

void AnimationDrawer::removeSlave(AnimationDrawer *drw) {
	for(int i =0; i < slaves.size(); ++i) {
		if ( slaves.at(i).slave->getId() == drw->getId()) {
			drw->setTimerId(iUNDEF);
			slaves.erase(slaves.begin() + i);
		}
	}
}

int AnimationDrawer::getOffset() const {
	return offset;
}

void AnimationDrawer::setOffset(int off) {
	offset = off;
	mapIndex = (mapIndex + offset) % activeMaps.size();
}

void AnimationDrawer::setTimeColumn(const Column& col) {
	if ( col->dm()->dmt() == dmtTIME ) {
		colTime = col->sName();
		useTime = true;
	} else {
		useTime = false;
	}
}

void AnimationDrawer::setUseTime(bool yesno) {
	if ( colTime != "") {
		useTime = yesno;
	} else {
		useTime = false;
	}
}

void AnimationDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	SetDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"Interval",fnView, interval);
	ObjectInfo::ReadElement(currentSection.c_str(),"UseTime",fnView, useTime);
	ObjectInfo::ReadElement(currentSection.c_str(),"TimeColumn",fnView, colTime);
	ObjectInfo::ReadElement(currentSection.c_str(),"Offset",fnView, offset);
	ObjectInfo::ReadElement(currentSection.c_str(),"MapIndex",fnView, mapIndex);
}

String AnimationDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "AnimationDrawer::" + parentSection;
	SetDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Interval",fnView, interval);
	ObjectInfo::WriteElement(currentSection.c_str(),"UseTime",fnView, useTime);
	ObjectInfo::WriteElement(currentSection.c_str(),"TimeColumn",fnView, colTime);
	ObjectInfo::WriteElement(currentSection.c_str(),"Offset",fnView, offset);
	ObjectInfo::WriteElement(currentSection.c_str(),"MapIndex",fnView, mapIndex);

	return currentSection;
}

void AnimationDrawer::setTresholdColor(const Color&clr){
	for(int i=0; i < drawers.size(); ++i) {
		RasterLayerDrawer *rsd = dynamic_cast<RasterLayerDrawer *> (drawers[i]);
		if ( rsd) {
			rsd->getDrawingColor()->setTresholdColor(clr);
			return;
		}
	}
}