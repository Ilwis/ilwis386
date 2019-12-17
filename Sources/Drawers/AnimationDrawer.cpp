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
index(0),
useTime(false),
mapIndex(0),
manager(0)
{
	setTransparency(1);
}

AnimationDrawer::~AnimationDrawer(){
	if ( manager && manager->GetSafeHwnd() != 0) {
		manager->SendMessage(ILWM_UPDATE_ANIM,128,(long)this);
	}
}

String AnimationDrawer::description() const {
	String sName = getName();
	IlwisObjectPtr *obj = getObject();
	String sDescr = (obj != 0) ? obj->sDescr() : "";
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
	IlwisObjectPtr *obj = getObject();
	if ( obj) {
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

bool AnimationDrawer::draw(int drawerIndex, const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	SetDrawer::draw(drawerIndex, drawLoop, cbArea);
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
	}
}

bool AnimationDrawer::timerPerIndex() {
	{
		ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
		int nmaps = activeMaps.size();
		int cindex = min(nmaps - 1, mapIndex % nmaps); // some safeguards for corrupt data.
		int nindex = min(nmaps - 1,(mapIndex + 1) % nmaps);
		getDrawer(activeMaps[cindex])->setActive(false);
		getDrawer(activeMaps[nindex])->setActive(true);
		mapIndex = nindex;
		currentIndex = activeMaps[mapIndex];
	}
	IlwisObjectPtr *obj = getObject();
	for(int i=0; i < slaves.size(); ++i) {
		SlaveProperties& props = slaves.at(i);
		int slaveMapIndex = (int)(mapIndex * props.slaveStep + min(0.5, props.slaveStep / 2.0) + props.slaveOffset) % props.slave->getActiveMaps().size();
		if (props.slave->getMapIndex() != slaveMapIndex)
			props.slave->setMapIndex(slaveMapIndex);
	}
	if ( obj)
		getEngine()->SendMessage(ILWM_UPDATE_ANIM,(WPARAM)&(obj->fnObj), mapIndex); 

	return true;
}

bool AnimationDrawer::activeOnTime(const Column& col, double currentTime) {
	int newMapIndex = 0;
	while ((newMapIndex < activeMaps.size()) && (col->rValue(activeMaps[newMapIndex] + 1) <= currentTime)) // change this to find "nearest" map in maplist
		++newMapIndex;
	if (newMapIndex > 0)
		--newMapIndex;
	bool fChanged = false;
	{
		ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
		if (newMapIndex != mapIndex) {
			getDrawer(activeMaps[mapIndex])->setActive(false);
			mapIndex = newMapIndex;
			getDrawer(activeMaps[mapIndex])->setActive(true);
			currentIndex = activeMaps[mapIndex];
			fChanged = true;
		}
	}
	if (fChanged) {
		IlwisObjectPtr *obj = getObject();
		if ( obj) 
			getEngine()->SendMessage(ILWM_UPDATE_ANIM,(WPARAM)&(obj->fnObj), mapIndex); 
		return true;
	} else
		return false;
}

bool AnimationDrawer::timerPerTime() {
	if ( (double)timestep == rUNDEF || (double)timestep == 0.0)
		return false;

	bool redraw = false;
	IlwisObjectPtr *obj = getObject();
	if (obj) {
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(obj->fnObj);
		if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
		}
		if ( type ==IlwisObject::iotMAPLIST) {
			Column col = mpl->tblAtt()->col(colTime);
			ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
			int totmaps = getDrawerCount();
			if (totmaps > 1)
				duration = duration * totmaps / double(totmaps - 1); // enlarge the duration with one timestep, to ensure that in "real time" mode the last frame shows as much time as the other frames
			double steps = 1000.0 / REAL_TIME_INTERVAL;
			double offset = timestep * (double)index / steps;
			if (offset > duration) {
				offset = 0;
				index = 0;
			}
			double lowtime = col->rrMinMax().rLo();
			double currentTime = lowtime + offset;
			redraw = activeOnTime(col, currentTime);
			SendTimeMessage(currentTime, long(this));
		}
	}

	++index;
	return redraw;
}

void AnimationDrawer::SetTime(ILWIS::Time time, long sender)
{
	if (sender == (long) this)
		return;
	if (useTime) {
		IlwisObjectPtr *obj = getObject();
		if (obj) {
			IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(obj->fnObj);
			if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
			} else if ( type ==IlwisObject::iotMAPLIST) {
				Column col = mpl->tblAtt()->col(colTime);
				bool redraw = activeOnTime(col, time);
				if ( redraw)
					getRootDrawer()->getDrawerContext()->doDraw();
				ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
				double frameTime = time;
				int totmaps = getDrawerCount();
				if (totmaps > 1)
					duration = duration * totmaps / double(totmaps - 1); // enlarge the duration with one timestep, to ensure that in "real time" mode the last frame shows as much time as the other frames
				double steps = 1000.0 / REAL_TIME_INTERVAL;
				double lowtime = col->rrMinMax().rLo();
				index = ceil(steps * (frameTime - lowtime) / timestep);
			}
		}
	}
}

void AnimationDrawer::setTimeStep(ILWIS::Duration dur) 
{
	timestep = dur; 
}

ILWIS::Duration AnimationDrawer::getTimeStep() const {
	return timestep;
}

String AnimationDrawer::iconName(const String& subtype) const {
	return "Animation";
}

void AnimationDrawer::setMapIndex(int ind) {
	{
		ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
		getDrawer(currentIndex)->setActive(false);
		int nmaps = activeMaps.size();
		mapIndex = ind % nmaps;
		currentIndex = activeMaps[mapIndex];
		getDrawer(currentIndex)->setActive(true);
	}
	getRootDrawer()->getDrawerContext()->doDraw();
	if (useTime) {
		IlwisObjectPtr *obj = getObject();
		if (obj) {
			IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(obj->fnObj);
			if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
			} else if ( type ==IlwisObject::iotMAPLIST) {
				Column col = mpl->tblAtt()->col(colTime);
				ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
				double frameTime = col->rValue(currentIndex + 1);
				int totmaps = getDrawerCount();
				if (totmaps > 1)
					duration = duration * totmaps / double(totmaps - 1); // enlarge the duration with one timestep, to ensure that in "real time" mode the last frame shows as much time as the other frames
				double steps = 1000.0 / REAL_TIME_INTERVAL;
				double lowtime = col->rrMinMax().rLo();
				index = ceil(steps * (frameTime - lowtime) / timestep);				
				SendTimeMessage(frameTime, long(this));
			}
		}
	} else {
		for(int i=0; i < slaves.size(); ++i) {
			SlaveProperties& props = slaves.at(i);
			int slaveMapIndex = (int)(mapIndex * props.slaveStep + min(0.5, props.slaveStep / 2.0) + props.slaveOffset) % props.slave->getActiveMaps().size();
			if (props.slave->getMapIndex() != slaveMapIndex)
				props.slave->setMapIndex(slaveMapIndex);
		}
	}

	IlwisObjectPtr *obj = getObject();
	if (obj)
		getEngine()->SendMessage(ILWM_UPDATE_ANIM,(WPARAM)&(obj->fnObj), mapIndex);
}


int AnimationDrawer::getTimerIdCounter(bool increase) {
	if ( increase)
		++timerIdCounter;

	return timerIdCounter;
}

void AnimationDrawer::addSlave(const SlaveProperties& props) {
	for(int i =0; i < slaves.size(); ++i) {
		if ( slaves.at(i).slave->getId() == props.slave->getId()) {
			slaves.erase(slaves.begin() + i);
			break;
		}
	}
	slaves.push_back(props);
	props.slave->setTimerId(SLAVE_TIMER_ID);
	props.slave->slaves.clear(); // clean slaves list in new slave
	int slaveMapIndex = (int)(mapIndex * props.slaveStep + min(0.5, props.slaveStep / 2.0) + props.slaveOffset) % props.slave->getActiveMaps().size();
	if (props.slave->getMapIndex() != slaveMapIndex)
		props.slave->setMapIndex(slaveMapIndex);
}

void AnimationDrawer::removeSlave(AnimationDrawer *drw) {
	for(int i =0; i < slaves.size(); ++i) {
		if ( slaves.at(i).slave->getId() == drw->getId()) {
			drw->setTimerId(iUNDEF);
			slaves.erase(slaves.begin() + i);
		}
	}
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

void AnimationDrawer::load(const FileName& fnView, const String& section){
	String currentSection = section;
	SetDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"Interval",fnView, interval);
	ObjectInfo::ReadElement(currentSection.c_str(),"UseTime",fnView, useTime);
	ObjectInfo::ReadElement(currentSection.c_str(),"TimeColumn",fnView, colTime);
	ObjectInfo::ReadElement(currentSection.c_str(),"MapIndex",fnView, mapIndex);
}

String AnimationDrawer::store(const FileName& fnView, const String& section) const{
	String currentSection = section + ":Animation";
	SetDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Interval",fnView, interval);
	ObjectInfo::WriteElement(currentSection.c_str(),"UseTime",fnView, useTime);
	ObjectInfo::WriteElement(currentSection.c_str(),"TimeColumn",fnView, colTime);
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

void AnimationDrawer::setTresholdRange(const RangeReal& rr, bool single){
	for(int i=0; i < drawers.size(); ++i) {
		RasterLayerDrawer *rsd = dynamic_cast<RasterLayerDrawer *> (drawers[i]);
		if ( rsd) {
			rsd->getDrawingColor()->setTresholdRange(rr, single);
			return;
		}
	}
}