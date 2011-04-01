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
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

int AnimationDrawer::timerIdCounter=5000;
int mycount = 0;
#define REAL_TIME_INTERVAL 100
#define BOX_DRAWER_ID 497

ILWIS::NewDrawer *createAnimationDrawer(DrawerParameters *parms) {
	return new AnimationDrawer(parms);
}

AnimationDrawer::AnimationDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"AnimationDrawer"),
	timerid(iUNDEF),
	interval(1.0),
	datasource(0),
	sourceType(sotUNKNOWN),
	featurelayer(0),
	loop(true),
	index(0),
	useTime(false),
	mapIndex(0),
	offset(0)
{
	setTransparency(1);
	last = 0;
}

AnimationDrawer::~AnimationDrawer(){
	delete datasource;
	delete featurelayer;
	datasource = 0;
	featurelayer = 0;
}

String AnimationDrawer::description() const {
	String sName = getName();
	String sDescr = (*datasource)->sDescr();
	if ("" != sDescr) 
		sName = String("%S Animated", sName);
	return sName;
}

RangeReal AnimationDrawer::getMinMax(const MapList& mlist) const{
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
				SetDrawer *sdr = (SetDrawer *)const_cast<AnimationDrawer *>(this)->getDrawer(0);
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

void AnimationDrawer::removeSelectionDrawers() {
	for(int i = 0; i < getDrawerCount(); ++i) {
		SetDrawer *sdrw = (SetDrawer *) getDrawer(i);
		NewDrawer *drwPost = sdrw->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
		if ( drwPost)
			sdrw->removeDrawer(drwPost->getId());
	}
}

void AnimationDrawer::addSelectionDrawers(const Representation& rpr) {
	if ( sourceType == sotMAPLIST) {
		MapList mlist((*datasource)->fnObj);
		RangeReal rrMinMax = getMinMax(mlist);
		Palette * palette;
		removeSelectionDrawers();
		for(int i = 0; i < getDrawerCount(); ++i) {
			RasterSetDrawer *drw = (RasterSetDrawer*)getDrawer(i);
			ILWIS::DrawerParameters parms(getRootDrawer(), this);
			Map mp = mlist->map(i);
			RasterSetDrawer *rasterset = (RasterSetDrawer *)NewDrawer::getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
			rasterset->setUICode(0);
			rasterset->setThreaded(false);
			rasterset->setRepresentation(rpr);
			rasterset->setMinMax(rrMinMax);
			if (i == 0)
				palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
			else
				rasterset->SetPalette(palette);
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			addSetDrawer(mp,&pp,rasterset,String("overlay %d",i), true);
			drw->addPostDrawer(RSELECTDRAWER,rasterset);
			rasterset->setActive(i == 0 ? true : false);

		}
	}
}

SetDrawer *AnimationDrawer::createIndexDrawer(const BaseMap& basem,ILWIS::DrawerParameters& dp, PreparationParameters* pp) {
	FeatureSetDrawer *fsd;
	IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basem->fnObj);
	switch ( otype) {
		case IlwisObject::iotPOINTMAP:
			fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("PointSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd);
			break;
		case IlwisObject::iotSEGMENTMAP:
			fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("LineSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd);
			break;
		case IlwisObject::iotPOLYGONMAP:
			fsd = (FeatureSetDrawer *)NewDrawer::getDrawer("PolygonSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd, "Areas");
			break;
	}
	return fsd;
}

void AnimationDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	if ( sourceType == sotFEATURE ) {
		ObjectCollection oc((*datasource)->fnObj);
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			if ( getName() == "")
				setName(oc->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			Tranquilizer trq(TR("Adding maps"));
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				IlwisObject::iotIlwisObjectType type = IOTYPE(oc->fnObject(i));
				if (type == IlwisObject::iotPOINTMAP || type == IlwisObject::iotPOLYGONMAP || type ==IlwisObject::iotSEGMENTMAP) {
					BaseMap bmp(oc->fnObject(i));
					if ( bmp.fValid()) {
						if ( !rpr.fValid())
							rpr = bmp->dm()->rpr();
						
						SetDrawer *drw = createIndexDrawer(bmp, dp, pp);
						drw->setUICode(0);
						drw->setActive(i == 0 ? true : false);
						trq.fUpdate(i,oc->iNrObjects()); 
					}
				}
			}
	/*		featurelayer = (FeatureLayerDrawer *)IlwWinApp()->getDrawer("FeatureLayerDrawer", "Ilwis38", &parms);
			featurelayer->setActive(true);
			featurelayer->addDataSource(&basemap);
			if ( basemap->fTblAtt()) {
				for(int i = 0; i< names.size(); ++i) {
					PreparationParameters parms(NewDrawer::ptGEOMETRY | NewDrawer::ptANIMATION);
					featurelayer->prepare(&parms);
				}
				vector<NewDrawer *> allDrawers;
				featurelayer->getDrawers(allDrawers);
				for(int i=0; i < allDrawers.size(); ++i) {
					FeatureSetDrawer *fset = (FeatureSetDrawer *)allDrawers.at(i);
					fset->setActive(i == 0 ? true : false);
					fset->getZMaker()->setTable(basemap->tblAtt(), names.at(i));
					fset->getZMaker()->setThreeDPossible(true);
				}
			}*/

		} 
		if ( pp->type && NewDrawer::ptRENDER) {
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				SetDrawer *sdr = (SetDrawer *)getDrawer(i);
				sdr->prepare(pp);
			}
		}
	}
	if ( sourceType == sotMAPLIST) {
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			MapList mlist((*datasource)->fnObj);
			if ( getName() == "Unknown")
				setName(mlist->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			// Calculate the min/max over the whole maplist. This is used for palette and texture generation.
			rrMinMax = getMinMax(mlist);
			Palette * palette;
			for(int i = 0; i < mlist->iSize(); ++i) {
				ILWIS::DrawerParameters parms(getRootDrawer(), this);
				Map mp = mlist->map(i);
				if ( !rpr.fValid())
					rpr = mp->dm()->rpr();
				RasterSetDrawer *rasterset = (RasterSetDrawer *)NewDrawer::getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
				rasterset->setThreaded(false);
				if ( rrMinMax.fValid())
					rasterset->setMinMax(rrMinMax);
				if (i == 0)
					palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
				else
					rasterset->SetPalette(palette);
				addSetDrawer(mp,pp,rasterset,String("band %d",i));
				getZMaker()->setBounds(getRootDrawer()->getMapCoordBounds());
				rasterset->setActive(i == 0 ? true : false);
			}
		}
	}
	if ( pp->type & NewDrawer::pt3D) {
		for(int i=0; i < drawers.size(); ++i)
			drawers.at(i)->prepare(pp);
	}
	setDrawMethod(getDrawer(0)->getDrawMethod());

}
void AnimationDrawer::setTransparency(double v) {
	for(int i = 0; i < getDrawerCount(); ++i) {
		NewDrawer *drw = getDrawer(i);
		drw->setTransparency(v);
	}
}

void AnimationDrawer::addSetDrawer(const BaseMap& basem,PreparationParameters *pp,SetDrawer *rsd, const String& name, bool post) {
	//MessageBox(0,"Add set drawer","", MB_OK);
	PreparationParameters fp((int)pp->type | NewDrawer::ptANIMATION, 0);
	fp.rootDrawer = getRootDrawer();
	fp.parentDrawer = this;
	fp.csy = basem->cs();
	rsd->setName(name);
	rsd->setRepresentation(basem->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSource(basem, getRootDrawer()->getMapCoordBounds());
	rsd->getZMaker()->setDataSourceMap(basem);
	RangeReal tempRange = basem->dvrs().rrMinMax();
	if ( tempRange.fValid()) {
		RangeReal rr = getZMaker()->getRange();
		rr += tempRange.rLo();
		rr += tempRange.rHi();
		getZMaker()->setRange(rr);
	}
	rsd->addDataSource(basem.ptr());
	rsd->prepare(&fp);
	if (!post)
		addDrawer(rsd);
}

void* AnimationDrawer::getDataSource() const {
	return (void *)datasource;
}

void AnimationDrawer::addDataSource(void *data, int options){
	IlwisObject *obj = dynamic_cast<IlwisObject *>((IlwisObject *)data);
	if ( obj) {
		datasource = new IlwisObject(*obj);
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
		if  (type == IlwisObject::iotSEGMENTMAP || 
			 type == IlwisObject::iotPOINTMAP || 
			 type == IlwisObject::iotPOLYGONMAP ) {
			 sourceType = sotFEATURE;
			 AbstractMapDrawer::addDataSource(data);
		}
		if ( type == IlwisObject::iotOBJECTCOLLECTION) {
			sourceType = sotFEATURE;
			ObjectCollection oc((*datasource)->fnObj);
			AbstractMapDrawer::addDataSource((void *)&(oc->ioObj(0)));
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				activeMaps.push_back(i);
			}
		}
		if ( type == IlwisObject::iotMAPLIST) {
			sourceType = sotMAPLIST;
			MapList mlist((*datasource)->fnObj);
			AbstractMapDrawer::addDataSource((void *)&(mlist->map(0)));
			for(int i = 0; i < mlist->iSize(); ++i) {
				activeMaps.push_back(i);
			}
		}
	}
}

void AnimationDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	DomainValueRangeStruct dvs = getBaseMap()->dvrs();
	Color clr;
	SetDrawer *dr = (SetDrawer *)(const_cast<AnimationDrawer *>(this)->getDrawer(0));
	if ( dvs.dm()->pdv())
		clr = dr->getDrawingColor()->clrVal(rVal);
	else
		clr = dr->getDrawingColor()->clrRaw((long)rVal, dr->getDrawMethod());
	

	CBrush brushColor(clr);
	CBrush *br = dc->SelectObject(&brushColor);
	dc->Rectangle(rct);
	dc->SelectObject(br);
}


bool AnimationDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	AbstractMapDrawer::draw( cbArea);
	return true;
}

void AnimationDrawer::animationDefaultView() {
	MapList mlist;
	mlist.SetPointer(datasource->pointer());
	for(int i =0 ; i < drawers.size(); ++i) {
		SetDrawer *sdr = (SetDrawer *)drawers.at(i);
		NewDrawer *drPost;
		if ( (drPost = sdr->getDrawer(RSELECTDRAWER,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);
		if ( (drPost = sdr->getDrawer(BOX_DRAWER_ID,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);

		if ( mlist[i]->dm()->pdi()) {
			sdr->setDrawMethod(NewDrawer::drmIMAGE);
		} else {
			sdr->setRepresentation(mlist[i]->dm()->rpr());
			sdr->setStretchRangeReal(mlist[i]->rrPerc1());
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
	}
}

bool AnimationDrawer::timerPerIndex() {
	if ( sourceType == sotFEATURE){
		ObjectCollection oc((*datasource)->fnObj);
		if ( oc->iNrObjects() > 0 && mapIndex < activeMaps.size() - 1) {
			getDrawer(activeMaps[mapIndex])->setActive(false);
			getDrawer(activeMaps[++mapIndex])->setActive(true);
		} else {
			if (loop) {
				getDrawer(activeMaps[mapIndex])->setActive(false);
				mapIndex = 0;
				getDrawer(activeMaps[0])->setActive(true);
			}
		}
	}
	if ( sourceType == sotMAPLIST) {
		MapList mlist;
		mlist.SetPointer(datasource->pointer());
		int nmaps = activeMaps.size();
		getDrawer(activeMaps[(mapIndex) % nmaps])->setActive(false);
		getDrawer(activeMaps[(mapIndex + 1) % nmaps])->setActive(true);
		mapIndex = (mapIndex+1)  % nmaps;
	
	}
	for(int i=0; i < slaves.size(); ++i) {
		SlaveProperties& props = slaves.at(i);
		if ( props.threshold > 1.0){
			props.slave->timedEvent(SLAVE_TIMER_ID);
			props.threshold -= 1.0;
		}
		props.threshold += props.slaveStep;

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
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl;
		mpl.SetPointer(datasource->pointer());
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
//
//String AnimationDrawer::timeString(const MapList& mpl, int index) {
//	double steps = 1000.0 / REAL_TIME_INTERVAL;
//	double currentTime = mpl->tblAtt()->col(colTime)->rrMinMax().rLo() +  timestep * (double)index / steps;
//	ILWIS::Time ct(currentTime);
//	String timestring = ct.toString(true,mpl->tblAtt()->col(colTime)->dm()->pdtime()->getMode());
//	return timestring;
//}

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
}

void AnimationDrawer::updateLegendItem() {
	//if ( doLegend)
	//	doLegend->updateLegendItem();
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