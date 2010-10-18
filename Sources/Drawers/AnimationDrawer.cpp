#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\TimeGraphSlider.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Domain\DomainTime.h" 
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\ValueSlicer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

int AnimationDrawer::timerIdCounter=5000;
int mycount = 0;
#define REAL_TIME_INTERVAL 10

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
	animcontrol(0), 
	useTime(false),
	mapIndex(0)
{
	setTransparency(1);
}

AnimationDrawer::~AnimationDrawer(){
	delete datasource;
	delete featurelayer;
}

String AnimationDrawer::description() const {
	String sName = getName();
	String sDescr = (*datasource)->sDescr();
	if ("" != sDescr) 
		sName = String("%S Animated", sName);
	return sName;
}

void AnimationDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	if ( sourceType == sotFEATURE ) {
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			BaseMap basemap((*datasource)->fnObj);
			setName(basemap->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			featurelayer = (FeatureLayerDrawer *)IlwWinApp()->getDrawer("FeatureLayerDrawer", "Ilwis38", &parms);
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
			}
			addDrawer(featurelayer);

		} 
	}
	if ( sourceType == sotMAPLIST) {
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			MapList mlist((*datasource)->fnObj);
			setName(mlist->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			// Calculate the min/max over the whole maplist. This is used for palette and texture generation.
			RangeReal rrMinMax (0, 255);
			if (mlist->iSize() > 0) {
				if (mlist->map(0)->dm()->pdv()) {
					for (int i = 0; i < mlist->iSize(); ++i) {
						Map mp = mlist->map(i);
						RangeReal rrMinMaxMap = mp->rrMinMax();
						if (rrMinMaxMap.rLo() >= rrMinMaxMap.rHi())
							rrMinMaxMap = mp->vr()->rrMinMax();
						if (i > 0)
							rrMinMax += rrMinMaxMap;
						else
							rrMinMax = rrMinMaxMap;
					}
				} else if (mlist->map(0)->fTblAtt() && attColumn.fValid() && attColumn->dm()->pdv()) {
					for (int i = 0; i < mlist->iSize(); ++i) {
						Map mp = mlist->map(i);
						if (i > 0)
							rrMinMax += attColumn->vr()->rrMinMax();
						else
							rrMinMax = attColumn->vr()->rrMinMax();
					}
				}
			}
			Palette * palette;
			for(int i = 0; i < mlist->iSize(); ++i) {
				ILWIS::DrawerParameters parms(getRootDrawer(), this);
				Map mp = mlist->map(i);
				RasterSetDrawer *rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
				rasterset->setThreaded(false);
				rasterset->setMinMax(rrMinMax);
				if (i == 0)
					palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
				else
					rasterset->SetPalette(palette);
				addSetDrawer(mp,pp,rasterset);
				rasterset->setActive(i == 0 ? true : false);
			}
		}
	}
	if ( pp->type & NewDrawer::pt3D) {
		for(int i=0; i < drawers.size(); ++i)
			drawers.at(i)->prepare(pp);
	}

}
void AnimationDrawer::setTransparency(double v) {
	for(int i = 0; i < getDrawerCount(); ++i) {
		NewDrawer *drw = getDrawer(i);
		drw->setTransparency(v);
	}
}

void AnimationDrawer::addSetDrawer(const BaseMap& basemap,PreparationParameters *pp,SetDrawer *rsd, const String& name) {
	PreparationParameters fp((int)pp->type | NewDrawer::ptANIMATION, 0);
	fp.rootDrawer = getRootDrawer();
	fp.parentDrawer = this;
	fp.csy = basemap->cs();
	rsd->setName(name);
	rsd->setRepresentation(basemap->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSourceMap(basemap);
	rsd->getZMaker()->setDataSourceMap(basemap);
	rsd->addDataSource(basemap.ptr());
	rsd->prepare(&fp);
	addDrawer(rsd);
}

void AnimationDrawer::addDataSource(void *data, int options){
	IlwisObject *obj = dynamic_cast<IlwisObject *>((IlwisObject *)data);
	if ( obj) {
		datasource = new IlwisObject(*obj);
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
		if  (type == IlwisObject::iotSEGMENTMAP || 
			 type == IlwisObject::iotPOINTMAP || 
			 type == IlwisObject::iotPOLYGONMAP) {
			 sourceType = sotFEATURE;
			 AbstractMapDrawer::addDataSource(data);
		}
		if ( type == IlwisObject::iotMAPLIST) {
			sourceType = sotMAPLIST;
			MapList mlist((*datasource)->fnObj);
			AbstractMapDrawer::addDataSource((void *)&(mlist->map(0)));
		}
	}

}

HTREEITEM AnimationDrawer::configure(LayerTreeView  *tv, HTREEITEM displayOptionsLastItem){
	ComplexDrawer::configure(tv,displayOptionsLastItem);
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationControl);
	InsertItem(TR("Run"),"History",item2);

	item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationSourceUsage);
	InsertItem(TR("Source usage"),".mpv",item2);

	//item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,0);
	HTREEITEM portrayalItem = InsertItem(tv, displayOptionsLastItem, TR("Portrayal"),"Colors");

	DisplayOptionTreeItem * itemSlicing = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&AnimationDrawer::animationSlicing);
	InsertItem(TR("Interactive Slicing"),"Slicing",itemSlicing);

	DisplayOptionTreeItem * itemSelect = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&AnimationDrawer::animationSelection);
	InsertItem(TR("Selection"),"SelectArea",itemSelect);


	return displayOptionsLastItem;

}
void AnimationDrawer::animationSlicing(CWnd *parent) {
	new AnimationSlicing(parent,this);
}

void AnimationDrawer::animationSelection(CWnd *parent) {
	new AnimationSelection(parent,this);
}

bool AnimationDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	AbstractMapDrawer::draw(norecursion, cbArea);
	return true;
}

void AnimationDrawer::animationControl(CWnd *parent) {
	animcontrol = new AnimationControl(parent, this);
}

void AnimationDrawer::animationSourceUsage(CWnd *parent) {
	new AnimationSourceUsage(parent, this);
}

void AnimationDrawer::timedEvent(UINT _timerid) {
    ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	MapCompositionDoc *doc = getRootDrawer()->getDrawerContext()->getDocument();
	bool redraw = false;
	if ( timerid == _timerid) {
		if ( useTime) {
			redraw = timerPerTime();
		} else {
			redraw = timerPerIndex();
		}
		if ( redraw) {
			if ( animcontrol)
				animcontrol->PostMessage(ID_TIME_TICK,mapIndex, TRUE);
			doc->mpvGetView()->Invalidate();
		}
	}
}

bool AnimationDrawer::timerPerIndex() {
	if ( featurelayer){
		if ( names.size() > 0 && mapIndex < names.size()-1) {
			featurelayer->getDrawer(mapIndex)->setActive(false);
			featurelayer->getDrawer(++mapIndex)->setActive(true);
		} else {
			if (loop) {
				featurelayer->getDrawer(mapIndex)->setActive(false);
				mapIndex = 0;
				featurelayer->getDrawer(0)->setActive(false);
			}
		}
	}
	if ( sourceType == sotMAPLIST) {
		MapList mlist;
		mlist.SetPointer(datasource->pointer());
		if ( mlist->iSize() > 0 && mapIndex < mlist->iSize() - 1) {
			getDrawer(mapIndex)->setActive(false);
			getDrawer(++mapIndex)->setActive(true);
		} else {
			if (loop) {
				getDrawer(mapIndex)->setActive(false);
				mapIndex = 0;
				getDrawer(0)->setActive(true);
			}
		}
	}
	return true;
}

bool AnimationDrawer::timerPerTime() {
	if ( (double)timestep == rUNDEF || (double)timestep == 0.0)
		return false;

	MapList mpl;
	mpl.SetPointer(datasource->pointer());
	Column col = mpl->tblAtt()->col(colTime);
	ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
	double steps = 1000.0 / REAL_TIME_INTERVAL;
	bool redraw = false;
	double currentTime = col->rrMinMax().rLo() +  timestep * (double)index / steps;
	if ( mapIndex < col->iRecs() -1 && col->rValue(mapIndex) < currentTime){
		getDrawer(mapIndex)->setActive(false);
		getDrawer(++mapIndex)->setActive(true);
		redraw = true;
	} else {
		if (loop && mapIndex >= mpl->iSize() -1 ) {
			getDrawer(mapIndex)->setActive(false);
			mapIndex = 0;
			index = 0;
			getDrawer(0)->setActive(true);
			redraw = true;
		}
	}
	++index;

	return redraw;

}

String AnimationDrawer::iconName(const String& subtype) const {
	return "Animation";
}

void AnimationDrawer::setMapIndex(int ind) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	for(int i =0 ; i < drawers.size(); ++i)
		getDrawer(i)->setActive(false);

	mapIndex = ind;
}

//---------------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationControl, DisplayOptionsForm2)
	ON_MESSAGE(ID_TIME_TICK, OnTimeTick)
END_MESSAGE_MAP()

LRESULT AnimationControl::OnTimeTick( WPARAM wParam, LPARAM lParam ) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->getDrawerCount() - 1 == (int)wParam )
		st->Hide();

	if ( lParam == TRUE)
		graphSlider->setIndex(wParam);
	else
		adrw->setMapIndex(wParam);
	return 1;
}

AnimationControl::AnimationControl(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, "Time"), fgTime(0)
{
	initial = true;
	FieldGroup *fg = new FieldGroup(root, true);
	fg->SetBevelStyle(FormEntry::bsRAISED);
	FlatIconButton *fi1 = new FlatIconButton(fg,"Begin","",(NotifyProc)&AnimationControl::begin, FileName());
	fbBegin = fi1;
	FlatIconButton *fi2 = new FlatIconButton(fg,"Pause","",(NotifyProc)&AnimationControl::pause, FileName());
	fi2->Align(fi1,AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"Run","",(NotifyProc)&AnimationControl::run, FileName());
	fi1->Align(fi2,AL_AFTER,-10);
	fi2 = new FlatIconButton(fg,"Stop","",(NotifyProc)&AnimationControl::stop, FileName());
	fi2->Align(fi1, AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"End","",(NotifyProc)&AnimationControl::end, FileName());
	fi1->Align(fi2, AL_AFTER,-10);
	cbTime = new CheckBox(root,TR("Use Time"), &adr->useTime);
	cbTime->Align(fbBegin, AL_UNDER);
	cbTime->SetCallBack((NotifyProc)&AnimationControl::setTimingMode);

	frtime = new FieldReal(root,"Interval", &adr->interval,ValueRangeReal(0.1,1000,0.1));
	frtime->SetCallBack((NotifyProc)&AnimationControl::setTiming);
	frtime->Align(cbTime, AL_UNDER);

	setTimeElements(cbTime);
	setSlider(cbTime);
	st = new StaticText(root,"Preparing . . .");
	

	create();

}

int AnimationControl::setTimingMode(Event *ev) {
	cbTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	frtime->Hide();
	fgTime->Hide();
	if ( adrw->useTime) {
		fgTime->Show();
		IlwisObject *source = adrw->datasource;
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			Column col = mpl->tblAtt()->col(adrw->colTime);
			TimeInterval interval(col->rrMinMax().rLo(),col->rrMinMax().rHi());
			graphSlider->setTimeInterval(interval);
			graphSlider->setTimes(col);
		}
	} else {
		frtime->Show();
		graphSlider->setTimeInterval(TimeInterval());
	}
	stop(0);
	run(0);
	return 1;
}

void AnimationControl::setTimeElements(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	IlwisObject *source = adrw->datasource;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
		IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			fgTime = new FieldGroup(entry,true);
			fcolTime = new FieldColumn(fgTime,TR("Time attributes"),mpl->tblAtt(),&adrw->colTime,dmTIME);
			fcolTime->SetCallBack((NotifyProc)&AnimationControl::changeTimeColumn);
			ftime = new FieldTime(fgTime,"",&adrw->timestep, 0, ILWIS::Time::mDURATION);
			ftime->SetWidth(20);
			ftime->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			StaticText *stTime = new StaticText(fgTime,TR("per second"));
			ftime->Align(fcolTime, AL_AFTER, -10);
			stTime->Align(ftime, AL_AFTER, -5);
			fgTime->Align(entry, AL_UNDER);
		}
	} 
}

int AnimationControl::changeDuration(Event *ev) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->colTime != "" && adrw->useTime) {
		ftime->StoreData();
	}
	return 1;
}
int AnimationControl::changeTimeColumn(Event *e) {
	fcolTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->colTime != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = adrw->datasource;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
			IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
		}
		if ( type ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			if ( mpl->fTblAtt()) {
				Column col = mpl->tblAtt()->col(adrw->colTime);
				adrw->timestep = calcNiceStep((col->rrMinMax().rHi() - col->rrMinMax().rLo()) / mpl->iSize());
				ftime->SetVal(adrw->timestep, ILWIS::Time::mDURATION);
			}
		} 
		
	}
	return 1;
}

double AnimationControl::calcNiceStep(Duration time) {
	int yr = 4712 + time.get(Time::tpYEAR);
	int mnt = time.get(Time::tpMONTH);
	int dy = time.get(Time::tpDAYOFMONTH);
	int hr = time.get(Time::tpHOUR);
	int min = time.get(Time::tpMINUTE);
	int sec = time.get(Time::tpSECOND);

	if ( yr > 10) {
		int l10 = log10((double)yr);
		int p10 = pow(10.0, l10);
		int y = (int)(yr / p10) * p10;
		int rest = yr - y;
		if ( rest * 2 > p10 )
			yr = y + 5 * p10 / 10;
		else
			yr = y;
	}
	if ( yr > 0) {
		dy = hr = min = sec = 0;
		if ( mnt > 6)
			mnt = 6;
		else 
			mnt = 0;
	} else if ( mnt > 0) {
		hr = min = sec = 0;
	} else if ( dy > 0) {
		min = sec = 0;
	}
	if ( yr > 5) {
		mnt = 0;
	}

	return Duration(String("P%04dY%02dM%02dDT%02dH%02dM%02ds",yr,mnt,dy,hr,min,sec));
	
}
void AnimationControl::setSlider(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	IlwisObject *source = adrw->datasource;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	RangeInt setRange;
	graphSlider = new TimeGraphSlider(root, setRange);
	graphSlider->SetWidth(165);
	if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
		IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		setRange = RangeInt(0, mpl->iSize());
		if ( mpl->fTblAtt()) {
			fcol = new FieldColumn(root,TR("Reference Attribute"),mpl->tblAtt(),&colName,dmVALUE);
			fcol->SetCallBack((NotifyProc)&AnimationControl::changeColum);
		}
	}
	graphSlider->Align(entry, AL_UNDER, 23);

}

int AnimationControl::changeColum(Event *) {
	fcol->StoreData();
	if ( colName != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = adrw->datasource;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		Table tbl;
		if (  type ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			tbl = mpl->tblAtt();
		}
		graphSlider->setSourceTable(tbl);
		graphSlider->setSourceColumn(colName);
	}
	return 1;

}
int AnimationControl::setTiming(Event *ev) {
	frtime->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	
	run(0);
	return 1;
}

int AnimationControl::stop(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	andr->setMapIndex(0);
	andr->index = 0;
	andr->timerid = iUNDEF;
	return 1;
}

int AnimationControl::pause(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	return 1;
}

int AnimationControl::end(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->mapIndex = andr->drawers.size();
	andr->index = 0;
	return 1;
}

int AnimationControl::run(Event  *ev) {
	frtime->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	if ( andr->timerid != iUNDEF) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	}else{
		andr->timerid = AnimationDrawer::timerIdCounter++;
	}
	if ( andr->useTime) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->timerid,REAL_TIME_INTERVAL,0);
	} else {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->timerid,andr->interval * 1000.0,0);
	}
	updateMapView();
	return 1;
}

int AnimationControl::begin(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->setMapIndex(0);
	andr->index = 0;
	return 1;
}

void AnimationControl::shutdown(int iReturn) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->animcontrol = 0;
	return DisplayOptionsForm2::shutdown();
}
//----------------------------------------------------------
AnimationSlicing::AnimationSlicing(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Slicing"))
{
	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&AnimationSlicing::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/2);


	create();
}

int AnimationSlicing::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"2");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
		}
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->Invalidate();
	}
	return 1;
}

//----------------------------------------------------------
AnimationSelection::AnimationSelection(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Selection"))
{
	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
	vs->setLowColor(GetSysColor(COLOR_3DFACE));
	vs->setHighColor(GetSysColor(COLOR_3DFACE));
	vs->setNumberOfBounds(3);
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&AnimationSelection::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/2);

	create();

	vs->setBoundColor(1,Color(120,230,0));
}

int AnimationSelection::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"3");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
		}
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->Invalidate();
	}
	return 1;
}
//----------------------------------------------------------
AnimationSourceUsage::AnimationSourceUsage(CWnd *par, AnimationDrawer *ldr) 
	: DisplayOptionsForm2(ldr, par, "Time"), mcs(0), rg(0)
{
	if ( ldr->sourceType == AnimationDrawer::sotFEATURE) {
		BaseMap basemap((*(ldr->datasource))->fnObj);
		if ( basemap->fTblAtt()) {
			new StaticText(root, TR("Columns to be used"));
			mcs = new MultiColumnSelector(root,basemap->tblAtt().ptr(), dmVALUE);
			RadioGroup *rg = new RadioGroup(root,TR("Type of use"),&columnUsage);
			new RadioButton(rg, TR("As Z value"));
			new RadioButton(rg, TR("As feature size"));
			new RadioButton(rg, TR("As Coordinates"));
				
		}
	}
  create();
}

int  AnimationSourceUsage::exec() {
	if ( rg) rg->StoreData();
	if ( mcs) {
		//mcs->StoreData();
		IntBuf selectedIndexes;
		mcs->iGetSelected(selectedIndexes);
		AnimationDrawer *andr = (AnimationDrawer *)drw;
		for(int  i =0; i < selectedIndexes.iSize(); ++i) {
			andr->names.push_back(mcs->sName(i));
		}

	}
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();

	return 1;
}
