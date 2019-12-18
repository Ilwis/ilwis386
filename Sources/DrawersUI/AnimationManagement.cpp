#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldonesl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Base\DataObjects\DT.H"
#include "Engine\Domain\DomainTime.h"
#include "Client\FormElements\TimeGraphSlider.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Mainwind.hs"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "client\mapwindow\drawers\DrawerTool.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\GLToMovie.h"
#include "Drawers\AnimationDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "AnimationManagement.h"


using namespace ILWIS;

#define REAL_TIME_INTERVAL 100

//--------------------------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationPropertySheet, CPropertySheet)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(ILWM_UPDATE_ANIM, command)
END_MESSAGE_MAP()

AnimationPropertySheet::AnimationPropertySheet() :
CPropertySheet(TR("Animation Management").c_str()), activeIndex(-1)
{
}

BOOL AnimationPropertySheet::OnInitDialog()
{
	return CPropertySheet::OnInitDialog();
}

void AnimationPropertySheet::OnDestroy() {
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 1);
}

LRESULT AnimationPropertySheet::command(WPARAM wp, LPARAM lp) {
	if ( (int)wp & pRun) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(3);
		page->DataChanged((Event *)1);
		page = (FormBasePropertyPage *)GetPage(2);
		page->DataChanged((Event *)1);
		page = (FormBasePropertyPage *)GetPage(1);
		page->DataChanged((Event *)1);
		page = (FormBasePropertyPage *)GetPage(0);
		page->DataChanged((Event *)1);
	}
	if ( (int)wp & pSynchornization) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(1);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pProgress) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(1);
		page->DataChanged((Event*)1);
		page = (FormBasePropertyPage *)GetPage(2);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pTimedEvent) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(2);
		page->DataChanged((Event*)2);
		/*AnimationProperties *prop = getActiveAnimation();
		if ( prop) {
			prop->animBar->updateTime(prop);
		}*/
		AnimationRun *pageRun = dynamic_cast<AnimationRun *>(GetPage(0));
		pageRun->timed();
	}
	if ( (int)wp == pRemove) {
		AnimationDrawer *adrw = (AnimationDrawer *) (void *)lp;
		removeAnimation(adrw);
	}
	return 0;
}

AnimationProperties* AnimationPropertySheet::findAnimationProps(AnimationDrawer * drw) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == drw->getId()) {
			return &animations[i];
		}
	}
	return 0;
}

void AnimationPropertySheet::addAnimation(const AnimationProperties& props) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == props.drawer->getId()) {
			return;
		}
	}
	animations.push_back(props);
	props.drawer->manager = this;
	PostMessage(ILWM_UPDATE_ANIM,pAll);
}

void AnimationPropertySheet::removeAnimation(AnimationDrawer * drw) {
	{
		ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
		for(int i=0; i < animations.size(); ++i) {
			if ( animations[i].drawer->getId() == drw->getId()) {
				if ( i < activeIndex)
					activeIndex--;
				else if ( i == activeIndex) { // removing the active animation
					activeIndex = -1;
				}
				animations[i].drawer->manager = 0;
				if ( animations[i].animBar && animations[i].animBar->m_hWnd && ::IsWindow(animations[i].animBar->m_hWnd))
					animations[i].animBar->ShowWindow(SW_HIDE);
				if (animations[i].drawer->getTimerId() != iUNDEF && animations[i].drawer->getTimerId() != SLAVE_TIMER_ID)
					animations[i].mdoc->mpvGetView()->KillTimer(animations[i].drawer->getTimerId());
				animations.erase(animations.begin() + i);
				break;
			}
		}
	}
	FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(3);
	page->DataChanged((Event *)0);
	page = (FormBasePropertyPage *)GetPage(2);
	page->DataChanged((Event *)3);
	page = (FormBasePropertyPage *)GetPage(1);
	page->DataChanged((Event *)1);
	page = (FormBasePropertyPage *)GetPage(0);
	page->DataChanged((Event *)3);
	PostMessage(ILWM_UPDATE_ANIM,pAll);
}

AnimationProperties *AnimationPropertySheet::getActiveAnimation(){
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( activeIndex >= 0 && activeIndex < animations.size()) {
		return &(animations[activeIndex]);
	}
	else {
		if ( animations.size() > 0) {
			activeIndex = 0;
			return &(animations[activeIndex]);
		}
	}
	return 0;
}

AnimationProperties *AnimationPropertySheet::getAnimation(int i) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( i < animations.size()) {
		return &animations[i];
	}
	return 0;
}

void AnimationPropertySheet::setActiveAnimation(AnimationDrawer * drw) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == drw->getId()) {
			activeIndex = i;
			PostMessage(ILWM_UPDATE_ANIM, pRun);
			return;
		}
	}
	activeIndex = -1;
}

int AnimationPropertySheet::getActiveIndex() const {
	return activeIndex;
}

int AnimationPropertySheet::getAnimationCount() const{
	return animations.size();
}

void AnimationPropertySheet::OnClose() {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	ShowWindow(SW_HIDE);
}

void AnimationPropertySheet::OnSysCommand(UINT nID, LPARAM p) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SendMessage(WM_CLOSE);
		return;
	}
	CPropertySheet::OnSysCommand(nID, p);
}
//--------------------------------------
AnimationRun::AnimationRun(AnimationPropertySheet& sheet)
: FormBasePropertyPage(TR("Run").c_str())
, propsheet(sheet)
, animIndex(0)
, fps(1)
, saveToAvi(false)
, movieRecorder(0)
{
	FieldGroup *fgRest = new FieldGroup(root, true);
	foAnimations = new FieldOneSelect(fgRest,&animIndex);
	foAnimations->SetCallBack((NotifyProc)&AnimationRun::changeActive);
	foAnimations->SetWidth(100);
	foAnimations->SetIndependentPos();
	sliderFps = new FieldRealSliderEx(fgRest,"Frame rate(fps)", &fps,ValueRange(RangeReal(0,5),0.1),false);
	if (fgRest->childlist().size() >= 3) { // change the allowed value range of the FieldReal so that the user can switch to another tab, with a framerate that exceeds 5.
		FieldReal * edit = dynamic_cast<FieldReal*>(fgRest->childlist()[2]->childlist()[0]); // the FieldReal has nestled itself into fgRest; this is a mistake in FieldRealSliderEx, but I will not change that now because it has side effects, and FieldRealSliderEx is used alot.
		edit->setValueRange(ValueRange(RangeReal(0,100),0.1));
	}
	sliderFps->SetCallBack((NotifyProc)&AnimationRun::speed);
	sliderFps->Align(foAnimations, AL_UNDER);
	FieldBlank *fb = new FieldBlank(fgRest);
	fb->Align(sliderFps, AL_UNDER);
	FieldGroup *fg = new FieldGroup(root, true);
	fg->SetBevelStyle(FormEntry::bsRAISED);
	FlatIconButton *fiTemp;
	FlatIconButton *fi1 = new FlatIconButton(fg,"Begin","",(NotifyProc)&AnimationRun::begin, FileName());
	fiTemp = fi1;
	FlatIconButton *fi2 = new FlatIconButton(fg,"OneFrameMinus","",(NotifyProc)&AnimationRun::frameMinus, FileName());
	fi2->Align(fi1, AL_AFTER);
	fiPause = new FlatIconButton(fg,"Pause","",(NotifyProc)&AnimationRun::pause, FileName());
	fiPause->Align(fi2,AL_AFTER,-10);
	fiRun = new FlatIconButton(fg,"Run","",(NotifyProc)&AnimationRun::run, FileName());
	fiRun->Align(fi2,AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"Stop","",(NotifyProc)&AnimationRun::stop, FileName());
	fi1 ->Align(fiRun, AL_AFTER,-10);
	fi2 = new FlatIconButton(fg,"OneFramePlus","",(NotifyProc)&AnimationRun::framePlus, FileName());
	fi2->Align(fi1, AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"End","",(NotifyProc)&AnimationRun::end, FileName());
	fi1->Align(fi2, AL_AFTER,-10);
	FieldGroup *fgAvi = new FieldGroup(fg);
	cbAvi = new CheckBox(fgAvi,TR("Save to avi"),&saveToAvi);
	cbAvi->SetCallBack((NotifyProc)&AnimationRun::checkAvi);
	fldAviName = new FieldString(fgAvi,TR("Filename"),&fnAvi);
	fldAviName->Align(cbAvi, AL_AFTER);
	fgAvi->Align(fiTemp, AL_UNDER);
	fgAvi->SetIndependentPos();
	fg->Align(fgRest, AL_UNDER);
	create();
	fEndInitialization = true;
}

AnimationRun::~AnimationRun() {
	if ( movieRecorder) {
		delete movieRecorder;
		movieRecorder = 0;
	}
}
void AnimationRun::timed() {
	if (saveToAvi && movieRecorder && fnAvi != "") {
		AnimationProperties *prop = propsheet.getActiveAnimation();
		if ( prop) {
			prop->mdoc->rootDrawer->getDrawerContext()->TakeContext();
			movieRecorder->RecordFrame();
			prop->mdoc->rootDrawer->getDrawerContext()->ReleaseContext();
		}
	}
}

int AnimationRun::changeActive(Event *ev) {
	if ( !fEndInitialization )
		return 1;
	foAnimations->StoreData();
	checkAvi(0);
	int sel = foAnimations->ose->GetCurSel();
	if ( sel != -1) {
		AnimationProperties *prop = propsheet.getAnimation(sel);
		propsheet.setActiveAnimation(prop->drawer);
	}
	return 1;
}

int AnimationRun::checkAvi(Event *ev) {
	if ( !fEndInitialization )
		return 1;
	cbAvi->StoreData();
	if ( saveToAvi)
		fldAviName->Show();
	else
		fldAviName->Hide();
	return 1;
}

void AnimationRun::startAvi() {
	if ( movieRecorder) {
		delete movieRecorder;
		movieRecorder = 0;
	}

	fldAviName->StoreData();
	AnimationProperties *prop = propsheet.getActiveAnimation();
	CRect rct;
	prop->mdoc->mpvGetView()->GetClientRect(rct);
	int width = rct.Width();
	int height = rct.Height();


	FileName fn(fnAvi,".avi");
	movieRecorder = new CGLToMovie(fn.sFullPath().c_str(), width, height, 24, mmioFOURCC('C','V','I','D'),1);
	//movieRecorder = new CGLToMovie(fn.sFullPath().c_str(), width, height, 24,mmioFOURCC('d','i','v','x'));
}

void AnimationRun::stopAvi() {
	if (movieRecorder)
		delete movieRecorder;
	movieRecorder = 0;
}

void AnimationRun::refreshTimer() {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return;
	bool fRunning = (props->drawer->getTimerId() != iUNDEF) && (props->drawer->getTimerId() != SLAVE_TIMER_ID);
	if (fRunning) {
		if (props->drawer->getTimerId() == SLAVE_TIMER_ID)
			return;
		run(0); // kill and set the timer with the new frequency
	}
}

int AnimationRun::speed(Event *ev) {
	if ( !fEndInitialization )
		return 1;
	sliderFps->StoreData();
	AnimationProperties *prop = propsheet.getActiveAnimation();
	if ( !prop)
		return 0;
	AnimationDrawer *andr = prop->drawer;
	if ( fps == 0 || fps == rUNDEF)
		return 1;
	andr->setInterval(1.0 / fps);
	refreshTimer();
	return 1;
}

int AnimationRun::DataChanged(Event* ev) {
	int code = (int)ev;
	if ( GetSafeHwnd() && ( code == 1 || code == 3)) {
		AnimationProperties *props =0;
		int index = 0;
		animIndex = propsheet.getActiveIndex();
		foAnimations->ose->ResetContent();
		while((props = propsheet.getAnimation(index++)) != 0) {
			foAnimations->ose->AddString(String("%S.Animation",props->drawer->getName()).c_str());
		}
		if ( animIndex >= 0)
			foAnimations->ose->SetCurSel(animIndex);
		else {
			foAnimations->ose->SetCurSel(0);
			animIndex = 0;
		}
		props = propsheet.getActiveAnimation();
		if (props) {
			if (props->drawer->getInterval() > 0 && props->drawer->getInterval() != rUNDEF) {
				fps = 1.0 / props->drawer->getInterval();
				sliderFps->SetVal(fps);
			}
		}
	}
	if (code == 3) { // animationRemoved
		if ( saveToAvi) {
			stopAvi();
		}
	}
	UpdateUIState();
	return 1;
}

int AnimationRun::frameMinus(Event *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	int index = props->drawer->getMapIndex();
	--index;
	if ( index < 0)
		index = props->drawer->getActiveMaps().size() - 1;
	props->drawer->setMapIndex(index);
	propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
	return 1;
}

int AnimationRun::framePlus(Event *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	int index = props->drawer->getMapIndex();
	++index;
	if ( index == props->drawer->getActiveMaps().size())
		index = 0;
	props->drawer->setMapIndex(index);
	propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
	return 1;
}

int AnimationRun::stop(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	if ( saveToAvi) {
		stopAvi();
	}
	if (props->drawer->getTimerId() == SLAVE_TIMER_ID) {
		int index = 0;
		AnimationProperties *propsIt;
		while((propsIt = propsheet.getAnimation(index)) != 0) {
			propsIt->drawer->removeSlave(props->drawer);
			++index;
		}
	} else
		props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
	props->drawer->setTimerId(iUNDEF);
	props->drawer->setMapIndex(0);
	UpdateUIState();
	propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
	props->mdoc->mpvGetView()->Invalidate();
	return 1;
}

int AnimationRun::pause(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	if (props->drawer->getTimerId() == SLAVE_TIMER_ID) {
		int index = 0;
		AnimationProperties *propsIt;
		while((propsIt = propsheet.getAnimation(index)) != 0) {
			propsIt->drawer->removeSlave(props->drawer);
			++index;
		}
	} else
		props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
	props->drawer->setTimerId(iUNDEF);
	UpdateUIState();
	return 1;
}

int AnimationRun::end(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->drawer->setMapIndex(props->drawer->getDrawerCount() - 1);
	propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
	return 1;
}

int AnimationRun::run(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	if ( props->drawer->getTimerId() != iUNDEF) {
		if (props->drawer->getTimerId() == SLAVE_TIMER_ID) {
			int index = 0;
			AnimationProperties *propsIt;
			while((propsIt = propsheet.getAnimation(index)) != 0) {
				propsIt->drawer->removeSlave(props->drawer);
				++index;
			}
			int timeIdC = AnimationDrawer::getTimerIdCounter(true); // assign a new timer-id
			props->drawer->setTimerId(timeIdC);
		} else
			props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId()); // keep same timer-id as before
	} else {
		int timeIdC = AnimationDrawer::getTimerIdCounter(true); // assign a new timer-id
		props->drawer->setTimerId(timeIdC);
	}
	if (props->drawer->getUseTime()) {
		// kill all other timers that have useTime == true
		int index = 0;
		AnimationProperties *propsIt;
		while((propsIt = propsheet.getAnimation(index)) != 0) {
			if (propsIt != props && propsIt->drawer->getUseTime() && propsIt->drawer->getTimerId() != iUNDEF && propsIt->drawer->getTimerId() != SLAVE_TIMER_ID) {
				propsIt->mdoc->mpvGetView()->KillTimer(propsIt->drawer->getTimerId());
				propsIt->drawer->setTimerId(iUNDEF);
			}
			props->drawer->removeSlave(propsIt->drawer); // clear our own slave list
			++index;
		}
	}
	if ( props->drawer->getUseTime())
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(), props->drawer->getInterval() * REAL_TIME_INTERVAL,0);
	else
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(), props->drawer->getInterval() * 1000.0,0);
	if (saveToAvi)
		startAvi();
	UpdateUIState();
	props->mdoc->mpvGetView()->Invalidate();
	return 1;
}

int AnimationRun::begin(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->drawer->setMapIndex(0);
	propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pTimedEvent);
	return 1;
}

void AnimationRun::UpdateUIState() {
	bool fRunning = false;
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (props)
		fRunning = (props->drawer->getTimerId() != iUNDEF) && (props->drawer->getTimerId() != SLAVE_TIMER_ID);
	if (fRunning) {
		fiPause->Show();
		fiRun->Hide();
		cbAvi->disable();
		fldAviName->Disable();
	} else {
		fiPause->Hide();
		fiRun->Show();
		cbAvi->enable();
		fldAviName->Enable();
	}
}

//-------------------------------------------------------------------
AnimationSynchronization::AnimationSynchronization(AnimationPropertySheet& sheet) : 
FormBasePropertyPage(TR("Synchronization").c_str()), 
propsheet(sheet),
offset1(0),
choiceSlave1(-1),
choiceMaster(-1),
step1(1),
foSlave1(0),
fgMaster(0),
fgSlaveIndex(0),
fgSlaveTime(0),
stMaster(0),
fiSlaveStep(0),
fiSlave1I(0),
initial(true)
{
	fgMaster = new FieldGroup(root, true);
	stMaster = new StaticText(fgMaster, TR("Master animation :                                                    "),true);
	stMaster->SetIndependentPos();
	StaticText *st2 = new StaticText(fgMaster, TR("Slave animation 1"));
	st2->Align(stMaster, AL_UNDER);
	foSlave1 = new FieldOneSelect(fgMaster,&choiceSlave1);
	foSlave1->SetWidth(100);
	foSlave1->Align(st2, AL_AFTER);
	setTimerPerIndex(st2);
	setTimerPerTime(st2);
	create();
}

void AnimationSynchronization::setTimerPerIndex(FormEntry *anchor) {
	fgSlaveIndex  = new FieldGroup(fgMaster);		
	fiSlave1I = new FieldInt(fgSlaveIndex, TR("Offset"),&offset1);
	fiSlave1I->SetWidth(10);
	fiSlave1I->Align(anchor, AL_UNDER);
	fiSlaveStep = new FieldReal(fgSlaveIndex, TR("Step"),&step1,ValueRange(0.01,100,0));
	fiSlaveStep->Align(fiSlave1I, AL_AFTER);
	fiSlaveStep->SetWidth(10);
	fgSlaveIndex->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(fgSlaveIndex);
	fb->Align(fiSlave1I, AL_UNDER);
	PushButton *pb = new PushButton(fgSlaveIndex,TR("Synchronize"),(NotifyProc)&AnimationSynchronization::synchronize);
}

void AnimationSynchronization::setTimerPerTime(FormEntry *anchor) {
	year = 0; month = 0; day = 0; hour = 0; minute = 0;
	fgSlaveTime = new FieldGroup(fgMaster);
	fgSlaveTime->SetIndependentPos();
	FieldInt *fiYr = new FieldInt(fgSlaveTime,TR("Offset(YMDhm)"),&year);
	fiYr->Align(anchor,AL_UNDER);
	FieldInt *fiMonth = new FieldInt(fgSlaveTime,"",&month);
	fiMonth->Align(fiYr,AL_AFTER,-5);
	fiMonth->SetWidth(8);
	FieldInt *fiDay = new FieldInt(fgSlaveTime,"",&day);
	fiDay->Align(fiMonth,AL_AFTER,-5);
	fiDay->SetWidth(8);
	FieldInt *fiHour = new FieldInt(fgSlaveTime,"",&hour);
	fiHour->Align(fiDay,AL_AFTER,-5);
	fiHour->SetWidth(8);
	FieldInt *fiMinute = new FieldInt(fgSlaveTime,"",&minute);
	fiMinute->Align(fiHour,AL_AFTER,-5);
	fiMinute->SetWidth(8);
	fgSlaveTime->Hide();
}

BOOL AnimationSynchronization::OnInitDialog()
{
	BOOL v = FormBasePropertyPage::OnInitDialog();
	AnimationProperties *prop = propsheet.getActiveAnimation();
	if (!prop)
		return v;

	bool useMasterTime = prop->drawer->getUseTime();
	if ( useMasterTime) {
		fgSlaveTime->Show();
		fgSlaveIndex->Hide();
	} else {
		fgSlaveTime->Hide();
		fgSlaveIndex->Show();
	}
	return v;
}

int AnimationSynchronization::DataChanged(Event*ev) {
	int code = (int)ev;
	if ( GetSafeHwnd() &&( initial || code == 1)) {
		initial = false;
		AnimationProperties *prop = propsheet.getActiveAnimation();
		int index = 0;
		choiceMaster = propsheet.getActiveIndex();
		AnimationProperties *props =  prop;
		if ( props) {
			bool useMasterTime = props->drawer->getUseTime();
			if ( useMasterTime) {
				fgSlaveTime->Show();
				fgSlaveIndex->Hide();
			} else {
				fgSlaveTime->Hide();
				fgSlaveIndex->Show();
			}
			foSlave1->ose->ResetContent();
			while((props = propsheet.getAnimation(index)) != 0) {
				bool slaveTime = props->drawer->getUseTime();
				if ( useMasterTime == slaveTime) {
					foSlave1->ose->AddString(String("%S.Animation",props->drawer->getName()).c_str());
					foSlave1->ose->SetItemData(index, (DWORD_PTR)props);
				}
				++index;
			}
			offset1 = 0;
			fiSlave1I->SetVal(0);
			String name = prop->drawer->getName();
			String v = TR("Master animation %S");
			stMaster->SetVal(String(v.c_str(),name));
		} else {
			fgSlaveTime->Hide();
			fgSlaveIndex->Show();
			foSlave1->ose->ResetContent();
		}
	} else if (code > 1000) {
		if ( ev->message() == Notify(CBN_SELCHANGE)) {
			choiceMaster =  propsheet.getActiveIndex();
			choiceSlave1 = foSlave1->ose->GetCurSel();
			AnimationProperties *masterDrawer = propsheet.getActiveAnimation();
			AnimationProperties *slaveDrawer = choiceSlave1 != -1 ? (AnimationProperties *)foSlave1->ose->GetItemData(choiceSlave1) : 0;
			if ( (masterDrawer && slaveDrawer)) {
				double sizeSlave = slaveDrawer->drawer->getActiveMaps().size();
				double sizeMaster = masterDrawer->drawer->getActiveMaps().size();
				step1 = sizeSlave / sizeMaster;
				fiSlaveStep->SetVal(step1);
			}
		}
	}
	return 1;
}

int AnimationSynchronization::synchronize(Event*) {
	fgMaster->StoreData();
	choiceMaster =  propsheet.getActiveIndex();
	choiceSlave1 = foSlave1->ose->GetCurSel();
	AnimationProperties *masterDrawer = propsheet.getActiveAnimation();
	AnimationProperties *slaveDrawer = choiceSlave1 != -1 ? (AnimationProperties *)foSlave1->ose->GetItemData(choiceSlave1) : 0;
	if ( (masterDrawer && slaveDrawer)) {
		if (masterDrawer->drawer->getId() == slaveDrawer->drawer->getId()) {
			throw ErrorObject(TR("Master and Slave must be different"));
		}
		int oldTimerId = slaveDrawer->drawer->getTimerId();
		slaveDrawer->mdoc->mpvGetView()->KillTimer(oldTimerId);
		slaveDrawer->drawer->setTimerId(SLAVE_TIMER_ID);
		double sizeSlave = slaveDrawer->drawer->getActiveMaps().size();
		double sizeMaster = masterDrawer->drawer->getActiveMaps().size();
		SlaveProperties props(slaveDrawer->drawer,offset1,step1);
		masterDrawer->drawer->addSlave(props);
	}
	return 1;
}

//---------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationProgress, FormBasePropertyPage)
	ON_MESSAGE(ID_TIME_TICK, OnTimeTick)
	ON_MESSAGE(ID_CLEAN_FORM, OnCleanForm)
END_MESSAGE_MAP()

AnimationProgress::AnimationProgress(AnimationPropertySheet& sheet) : FormBasePropertyPage(TR("Threshold Marking").c_str()), propsheet(sheet), form(0)
{
	fgMaster = new FieldGroup(root, true);
	stMaster = new StaticText(fgMaster, TR("Selected Animation                                               "), true);
	stMaster->SetIndependentPos();
	fcol = new FieldColumn(fgMaster,TR("Reference Attribute"),0,&colName,dmVALUE);
	fcol->SetCallBack((NotifyProc)&AnimationProgress::changeColumn);
	fcol->Hide();
	PushButton *pb = new PushButton(fgMaster,TR("Graph Treshold"),(NotifyProc)&AnimationProgress::graphProperties);
	pb->Align(fcol, AL_AFTER);
	RangeInt setRange = RangeInt(1, 100);
	graphSlider = new TimeGraphSlider(fgMaster, setRange);
	graphSlider->SetWidth(180);
	graphSlider->SetHeight(110);
	graphSlider->Align(fcol, AL_UNDER);
	//useTimeAttribute =  adr->getUseTime();
	create();
	fEndInitialization = true;
}

int AnimationProgress::graphProperties(Event *ev){
	fcol->StoreData();
	AnimationProperties *props = propsheet.getActiveAnimation();
	if ( props && tbl.fValid())
		form = new GraphPropertyForm(this,graphSlider, tbl->col(colName),props);
	return 1;
}

LRESULT AnimationProgress::OnCleanForm( WPARAM wParam, LPARAM lParam ) {
	form = 0;
	return 1;
}

LRESULT AnimationProgress::OnTimeTick( WPARAM wParam, LPARAM lParam ) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if ( !props)
		return 0;

	graphSlider->setIndex(props->drawer->getActiveMaps()[wParam]);
	props->drawer->setMapIndex(wParam);
	return 1;
}

int AnimationProgress::DataChanged(Event*ev) {
	int code = (int)ev;
	if ( code == 3) {
		fcol->FillWithColumns((TablePtr *)0);
		tbl = Table();
		if ( form && form->m_hWnd && ::IsWindow(form->m_hWnd))
			form->PostMessage(WM_CLOSE);
		form = 0;
		graphSlider->setSourceTable(tbl);
		graphSlider->setTimeInterval(TimeInterval());
	}
	if ( code == 1) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( !props)
			return 0;
			/*	TimeInterval interval(col->rrMinMax().rLo(),col->rrMinMax().rHi());
				graphSlider->setTimeInterval(interval);
				graphSlider->setTimes(col);*/
		AnimationDrawer *adrw = props->drawer;
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		int number=iUNDEF;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type ==IlwisObject::iotMAPLIST ) {
			MapList mpl((*source)->fnObj);
			number = mpl->iSize();
			tbl = mpl->tblAtt();		
		}
		if ( type == IlwisObject::iotOBJECTCOLLECTION){
			ObjectCollection oc((*source)->fnObj);
			number = oc->iNrObjects();
			tbl = oc->tblAtt();
		}
		if ( props->drawer->getUseTime()) {
			Column col = tbl->col(props->drawer->getTimeColumn());
			if ( col.fValid()) {
				TimeInterval interval(col->rrMinMax().rLo(),col->rrMinMax().rHi());
				graphSlider->setTimeInterval(interval);
				graphSlider->setTimes(col);
			}
		} else {
			RangeInt setRange = RangeInt(1, number);
			graphSlider->setRecordRange(setRange);
			graphSlider->setTimeInterval(TimeInterval());
		}
		String name = props->drawer->getName();
		String v = TR("Selected Animation: %S");
		stMaster->SetVal(String(v.c_str(),name));
	}
	if ( tbl.fValid()) {
		fcol->Show();
		fcol->FillWithColumns(&tbl);
		graphSlider->setSourceTable(tbl);
	} else
		fcol->Hide();

	if (code == 2) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( props == 0)
			return 1;
		AnimationDrawer *adrw = props->drawer;
		graphSlider->setIndex(adrw->getActiveMaps()[adrw->getMapIndex()]);
		//animBar.updateTime(String("index : %d",adrw->getMapIndex()));
	}
	return 1;
}

int AnimationProgress::changeColumn(Event *) {
	if ( !fEndInitialization )
		return 1;
	fcol->StoreData();
	if ( colName != "") {
		graphSlider->setSourceColumn(colName);
	}
	return 1;

}
//---------------------------------------------
BEGIN_MESSAGE_MAP(GraphPropertyForm, DisplayOptionsForm)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

GraphPropertyForm::GraphPropertyForm(CWnd *wPar,TimeGraphSlider *slider, const Column& _col, AnimationProperties *_props) : 
DisplayOptionsForm(_props->drawer, wPar, TR("Thresholds")), col(_col), graph(slider), threshold(0), type(0), props(_props) {
	bool above;
	graph->getThreshold(threshold, above);
	if (threshold != rUNDEF) {
		type = above ? 1 : 2;
	} else {
		threshold = 0;
		calcMed(col);
	}
	view = props->mdoc->ltvGetView();
	color = ((LayerDrawer *)props->drawer->getDrawer(0))->getDrawingColor()->getTresholdColor();
	color.alpha() = 255 - color.alpha(); // inverse the alpha, for FieldColor
	RadioGroup *rg = new RadioGroup(root, TR("Threshold type"),&type);
	RadioButton *rb = new RadioButton(rg,TR("No threshold"));
	rb = new RadioButton(rg,TR("Above threshold"));
	frr = new FieldReal(rb, "",&threshold);
	frr->Align(rb, AL_AFTER);
	rb = new RadioButton(rg,TR("Below threshold"));
	frr = new FieldReal(rb, "",&threshold);
	frr->Align(rb, AL_AFTER);
	fc = new FieldColor(root,TR("Treshold color"),&color);
	fc->SetCallBack((NotifyProc)&GraphPropertyForm::changeColor);
	slider->setLinkedWindow(props->animBar);

	create();
}

int GraphPropertyForm::changeColor(Event *ev) {
	if (initial)
		return 1;
	fc->StoreData();
	apply();

	return 1;
}

void GraphPropertyForm::calcMed(const Column& col) {
	if ( col->dm()->pdv()) {	
		RangeReal mm = col->rrMinMax();
		threshold = mm.rLo() + mm.rWidth() * 0.8;
	}
}

FormEntry *GraphPropertyForm::CheckData() {
	return 0;
}

void GraphPropertyForm::apply() {
	root->StoreData();
	if ( type == 0)
		graph->setThreshold(rUNDEF, true); // no threshold
	else if ( type == 1)
		graph->setThreshold(threshold, true); // above
	else if ( type == 2)
		graph->setThreshold(threshold, false); // below
	if ( props ) {
		RangeReal rr;
		if ( type == 1) {
			rr = RangeReal(threshold, 1e307);
		} else if (type == 2) {
			rr = RangeReal(-1e307, threshold); 
		}

		Color clr (color);
		clr.alpha() = 255 - clr.alpha(); // inverse the alpha again, for displaying
		for(int i=0; i < props->drawer->getDrawerCount(); ++i) {
			LayerDrawer *ldr = (LayerDrawer *)props->drawer->getDrawer(i);
			ldr->getDrawingColor()->setTresholdColor(clr);
			ldr->getDrawingColor()->setTresholdRange(rr);
		}
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		props->drawer->prepareChildDrawers(&pp);
		updateMapView();
	}
}

void GraphPropertyForm::OnClose() {
	GetParent()->PostMessage(ID_CLEAN_FORM);
	shutdown();
}

//---------------------------------------------
RealTimePage::RealTimePage(ILWIS::AnimationPropertySheet &sheet, AnimationRun * _animationRun)
: FormBasePropertyPage(TR("Real time progress").c_str())
, propsheet(sheet)
, useTimeAttribute(false)
, animationRun(_animationRun)
{
	year = month = day = hour = minute = 0;

	fgMaster = new FieldGroup(root, true);

	stMaster = new StaticText(fgMaster, TR("Selected Animation                                               "), true);
	stMaster->SetIndependentPos();

	cbTime = new CheckBox(fgMaster,TR("Use Time Attribute"),&useTimeAttribute);
	cbTime->SetCallBack((NotifyProc)&RealTimePage::setTimingMode);
	cbTime->Align(stMaster, AL_UNDER);

	fgTime = new FieldGroup(cbTime);
	fgTime->Align(cbTime, AL_UNDER);

	fcolTime = new FieldColumn(fgTime,"",0,&timeColName,dmTIME);
	fcolTime->SetCallBack((NotifyProc)&RealTimePage::changeTimeColumn);
	new FieldBlank(fgTime);
	FieldGroup *fg2 = new FieldGroup(fgTime);
	fiYr = new FieldInt(fg2,TR("Period(YMDHm) / tick"),&year);
	fiYr->Align(fcolTime,AL_UNDER);
	fiMonth = new FieldInt(fg2,"",&month);
	fiMonth->Align(fiYr,AL_AFTER,-5);
	fiMonth->SetWidth(8);
	fiDay = new FieldInt(fg2,"",&day);
	fiDay->Align(fiMonth,AL_AFTER,-5);
	fiDay->SetWidth(8);
	fiHour = new FieldInt(fg2,"",&hour);
	fiHour->Align(fiDay,AL_AFTER,-5);
	fiHour->SetWidth(8);
	fiMinute = new FieldInt(fg2,"",&minute);
	fiMinute->Align(fiHour,AL_AFTER,-5);
	fiMinute->SetWidth(8);
	fg2->Align(fcolTime, AL_UNDER);
	fg2->SetIndependentPos();
	fiYr->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMinute->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMonth->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiDay->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMinute->SetCallBack((NotifyProc)&RealTimePage::changeDuration);

	fgTime->Hide();
	create();
	fEndInitialization = true;
}

int RealTimePage::DataChanged(Event*ev) {
	int code = (int)ev;
	if ( GetSafeHwnd()) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( !props) {
			fcolTime->FillWithColumns((TablePtr *)0);
			tbl = Table();
			cbTime->SetVal(false);
			fgTime->Hide();
			return 0;
		}
		AnimationDrawer *adrw = props->drawer;
		if (code == 1) { // master animation changed; code != 1 means a callback was called
			useTimeAttribute = adrw->getUseTime();
			cbTime->SetVal(useTimeAttribute);
			if (useTimeAttribute) {
				fgTime->Show();
				ILWIS::Duration t(adrw->getTimeStep());
				fiYr->SetVal(t.get(ILWIS::Time::tpYEAR));
				fiMonth->SetVal(t.get(ILWIS::Time::tpMONTH));
				fiDay->SetVal(t.get(ILWIS::Time::tpDAYOFMONTH));
				fiHour->SetVal(t.get(ILWIS::Time::tpHOUR));
				fiMinute->SetVal(t.get(ILWIS::Time::tpMINUTE));
			} else
				fgTime->Hide();
		}

		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type ==IlwisObject::iotMAPLIST ) {
			MapList mpl((*source)->fnObj);
			tbl = mpl->tblAtt();		
		}
		if ( type == IlwisObject::iotOBJECTCOLLECTION){
			ObjectCollection oc((*source)->fnObj);
			tbl = oc->tblAtt();
		}
		if ( tbl.fValid())
			fcolTime->FillWithColumns(&tbl);

	/*	if ( tbl.fValid() && fgTime->fShow() == false)
			fgTime->Show();
		else if ( fgTime->fShow())
			fgTime->Hide();*/

		String name = props->drawer->getName();
		String v = TR("Selected Animation: %S");
		stMaster->SetVal(String(v.c_str(),name));
	}	

	return 1;
}

int RealTimePage::changeDuration(Event *ev) {
	if ( !fEndInitialization )
		return 1;
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	AnimationDrawer *adrw = props->drawer;
	if ( adrw->getTimeColumn() != "" && adrw->getUseTime()) {
		fiYr->StoreData();
		fiMonth->StoreData();
		fiDay->StoreData();
		fiHour->StoreData();
		fiMinute->StoreData();
		if ( year ==0 && month ==0 && day == 0 && hour ==0 && minute == 0)
			return 1;
		Duration dur(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));
		adrw->setTimeStep(dur);
	}
	animationRun->refreshTimer();
	return 1;
}

int RealTimePage::changeTimeColumn(Event *e) {
	if ( !fEndInitialization )
		return 1;
	fcolTime->StoreData();
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	AnimationDrawer *adrw = props->drawer;
	if ( timeColName != "") {
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		if ( IOTYPEBASEMAP((*source)->fnObj)) {
				ObjectCollection oc((*source)->fnObj);
		}
		if ( IOTYPE((*source)->fnObj) ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			if ( mpl->fTblAtt()) {
				Column col = mpl->tblAtt()->col(timeColName);
				if ( col.fValid()) {
					double timeStep = calcNiceStep((col->rrMinMax().rHi() - col->rrMinMax().rLo()) / ((mpl->iSize() > 1) ? (mpl->iSize() - 1) : 1));
					ILWIS::Duration t(timeStep);
					String s = t.toString(true,ILWIS::Time::mDATETIME);
					fiYr->SetVal(t.get(ILWIS::Time::tpYEAR));
					fiMonth->SetVal(t.get(ILWIS::Time::tpMONTH));
					fiDay->SetVal(t.get(ILWIS::Time::tpDAYOFMONTH));
					fiHour->SetVal(t.get(ILWIS::Time::tpHOUR));
					fiMinute->SetVal(t.get(ILWIS::Time::tpMINUTE));
					adrw->setTimeStep(timeStep);
					adrw->setTimeColumn(col);
					adrw->setUseTime(useTimeAttribute);
				}
			}
		} 
		
	}
	return 1;
}

double RealTimePage::calcNiceStep(Duration time) {
	year = time.get(Time::tpYEAR);
	month = time.get(Time::tpMONTH);
	day = time.get(Time::tpDAYOFMONTH);
	hour = time.get(Time::tpHOUR);
	minute = time.get(Time::tpMINUTE);

	if ( year > 10) {
		int l10 = log10((double)year);
		int p10 = pow(10.0, l10);
		int y = (int)(year / p10) * p10;
		int rest = year - y;
		if ( rest * 2 > p10 )
			year = y + 5 * p10 / 10;
		else
			year = y;
	}
	if ( year > 0) {
		day = hour = minute = 0;
		if ( month > 6)
			month = 6;
		else 
			month = 0;
	} else if (month == 12) {
		year += 1;
		month = day = minute = hour = 0;
	}
	else if ( month > 0) {
		hour = minute = 0;
	} if ( month > 3) {
		day = 0;
	}
	else if ( day > 0) {
		minute = 0;
	}
	if ( year > 5) {
		month = 0;
	}
	fiYr->SetVal(year);
	fiMonth->SetVal(month);
	fiDay->SetVal(day);
	fiHour->SetVal(hour);
	fiMinute->SetVal(minute);

	return Duration(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));	
}

int RealTimePage::setTimingMode(Event *ev) {
	if ( !fEndInitialization )
		return 1;
	cbTime->StoreData();
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	AnimationDrawer *adrw = props->drawer;
	adrw->setUseTime(useTimeAttribute);
	if ( useTimeAttribute) {
		fcolTime->StoreData();
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		MapListPtr * pmpl = dynamic_cast<MapListPtr*>(source->pointer());
		if (pmpl) {
			if ( pmpl->fTblAtt()) {
				Column col = pmpl->tblAtt()->col(timeColName);
				if ( col.fValid()) {
					propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pProgress);
				} 
			}
		} else {
			ObjectCollectionPtr * poc = dynamic_cast<ObjectCollectionPtr*>(source->pointer());
			if ( poc->fTblAtt()) {
				Column col = poc->tblAtt()->col(timeColName);
				if ( col.fValid()) {
					propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pProgress);
				}
			}
		}
		if ((props->drawer->getTimerId() != iUNDEF) && (props->drawer->getTimerId() != SLAVE_TIMER_ID)) {
			// kill all other timers that have useTime == true
			int index = 0;
			AnimationProperties *propsIt;
			while((propsIt = propsheet.getAnimation(index)) != 0) {
				if (propsIt != props && propsIt->drawer->getUseTime() && propsIt->drawer->getTimerId() != iUNDEF && propsIt->drawer->getTimerId() != SLAVE_TIMER_ID) {
					propsIt->mdoc->mpvGetView()->KillTimer(propsIt->drawer->getTimerId());
					propsIt->drawer->setTimerId(iUNDEF);
				}
				propsIt->drawer->removeSlave(props->drawer); // remove ourselves from another drawer's slave list
				props->drawer->removeSlave(propsIt->drawer); // also clear our own slave list
				++index;
			}
		}
	} else {
		propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pProgress);
	}
	int index = props->drawer->getMapIndex();
	props->drawer->setMapIndex(index); // change the frame text on top of the MapWindow back to "index" (instead of "date")
	animationRun->refreshTimer();
	return 1;
}

//---------------------------
//--------------------------------------------------------------

BEGIN_MESSAGE_MAP(AnimationBar, CToolBar)
	ON_EN_SETFOCUS(ID_AnimationBar,OnSetFocus)
	ON_EN_KILLFOCUS(ID_AnimationBar,OnKillFocus)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(ID_NOTIFY_ME, OnChangeColor)
	ON_MESSAGE(ILWM_UPDATE_ANIM,OnUpdateAnimMessages)
END_MESSAGE_MAP()

AnimationBar::AnimationBar() 
{
	fActive = false;
	isMarked = false;
	red.CreateSolidBrush(RGB(255,0,0)); 

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// Since design guide says toolbars are fixed height so is the font.
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");
	fnt.CreateFontIndirect(&logFont);
}

LRESULT AnimationBar::OnChangeColor(WPARAM wp, LPARAM lp) {
	isMarked = wp;
	ed.Invalidate();

	return 1;
}

LRESULT AnimationBar::OnUpdateAnimMessages(WPARAM p1, LPARAM p2) {
	if ( !p1)
		return 1;

	FileName fn = *(FileName *)(void *)(p1);
	updateTime();

	return 1;
}

HBRUSH AnimationBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CToolBar::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC) {
		if (pWnd->GetDlgCtrlID() == ID_AnimationBar && isMarked)
		{
			 pDC->SetBkColor(RGB(255,0,0));
			 hbr = (HBRUSH)red;
		}
	}
	return hbr;
}

AnimationBar::~AnimationBar()
{

}

void AnimationBar::Create(CWnd* pParent,const AnimationProperties& props)
{
	int iWidth = 180;

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		              CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	CToolBar::CreateEx(pParent, dwCtrlStyle, dwStyle, rectBB, ID_AnimationBar);
	UINT ai[2];
	ai[0] = ID_AnimationBar;
	ai[1] = ID_SEPARATOR;
	SetButtons(ai,2);
	CRect rect;
	GetItemRect(0, &rect);
	SetButtonInfo(1, ID_AnimationBar, TBBS_SEPARATOR, iWidth - rect.Width());

	rect.top = 3;
	rect.bottom -= 2;
	rect.right = rect.left + iWidth;
	ed.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|WS_DISABLED,rect,this,ID_AnimationBar);
	ed.SetFont(&fnt);
	ed.SetWindowText(String("index : 1").c_str());
	SendMessage(DM_SETDEFID,IDOK);

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	SetBarStyle(GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);

	SetWindowText(TR("Animation").c_str());
	animation = props;

	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)m_hWnd, 1);
}

void AnimationBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	MapWindow *mw = dynamic_cast<MapWindow *>(pParent);
	if ( mw) {
		if ( mw->isFullScreen())
			ShowWindow(SW_HIDE);
		else
			ShowWindow(SW_SHOW);
	}
}

void AnimationBar::OnSetFocus()
{
	fActive = true;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(false);
}

void AnimationBar::OnKillFocus()
{
	fActive = false;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(true);
}


void AnimationBar::updateTime(/*const AnimationProperties* props*/) // called by AnimationBarEdit
{
	if ( animation.drawer->getUseTime()) {
		String tmstring = setTimeString();
		if ( ed.GetSafeHwnd())
			ed.SetWindowText(tmstring.c_str());
	}
	else {
		if ( ed.GetSafeHwnd())
			ed.SetWindowText(String("index : %d",1 + animation.drawer->getCurrentIndex()).c_str());
	}
}

String AnimationBar::setTimeString(/*const AnimationProperties* props*/) {

	int ind = animation.drawer->getMapIndex();
	String timestring;
	IlwisObject *source = (IlwisObject *)animation.drawer->getDataSource();
	MapList mpl((*source)->fnObj);
	if ( mpl->fTblAtt()) {
		Column timeCol = mpl->tblAtt()->col(animation.drawer->getTimeColumn());
		if ( timeCol.fValid()) {
			Duration timestep = animation.drawer->getTimeStep();
			double steps = 1000.0 / REAL_TIME_INTERVAL;
			//double currentTime = mpl->tblAtt()->col(colTime)->rrMinMax().rLo() +  timestep * (double)index / steps;
			double currentTime = timeCol->rValue(ind + 1);
			ILWIS::Time ct(currentTime);
			timestring = ct.toString(true,timeCol->dm()->pdtime()->getMode());
		}
	}
	return String("index %d : %S", 1 + ind, timestring);
}




