#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldonesl.h"
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
#include "Engine\Drawers\AbstractMapDrawer.h"
//#include "Drawers\FeatureLayerDrawer.h"
//#include "Drawers\SetDrawer.h"
//#include "Drawers\FeatureSetDrawer.h"
//#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
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
CPropertySheet(TR("Animation Management").scVal()), activeIndex(-1)
{
}

BOOL AnimationPropertySheet::OnInitDialog()
{
	return CPropertySheet::OnInitDialog();
}

LRESULT AnimationPropertySheet::command(WPARAM wp, LPARAM lp) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( (int)wp & pRun) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(0);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pSynchornization) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(1);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pTimedEvent) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(2);
		page->DataChanged((Event*)2);
	}
	return 0;
}

void AnimationPropertySheet::addAnimation(const AnimationProperties& props) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == props.drawer->getId())
			return;
	}
	animations.push_back(props);
	PostMessage(ILWM_UPDATE_ANIM,pAll);
}

void AnimationPropertySheet::removeAnimation(AnimationDrawer * drw) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == drw->getId()) {
			if ( i < activeIndex)
				activeIndex--;
			if ( i == activeIndex)
				activeIndex = -1;
			animations.erase(animations.begin() + i);
		}
	}
	PostMessage(ILWM_UPDATE_ANIM,pAll);
}

AnimationProperties *AnimationPropertySheet::getActiveAnimation(){
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( activeIndex >= 0)
		return &(animations[activeIndex]);
	return 0;
}

AnimationProperties *AnimationPropertySheet::getAnimation(int i) {
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( i < animations.size())
		return &animations[i];
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
//------------------------------------


AnimationRun::AnimationRun(AnimationPropertySheet& sheet) : FormBasePropertyPage(TR("Run").scVal()), propsheet(sheet), animIndex(0), fps(1)
{
	FieldGroup *fgRest = new FieldGroup(root, true);
	foAnimations = new FieldOneSelect(fgRest,&animIndex);
	foAnimations->SetCallBack((NotifyProc)&AnimationRun::changeActive);
	foAnimations->SetWidth(100);
	foAnimations->SetIndependentPos();
	sliderFps = new FieldRealSliderEx(fgRest,"Frame rate(fps)", &fps,ValueRange(RangeReal(0,5),0.1),false);
	sliderFps->SetCallBack((NotifyProc)&AnimationRun::speed);
	sliderFps->Align(foAnimations, AL_UNDER);
	FieldBlank *fb = new FieldBlank(fgRest);
	fb->Align(sliderFps, AL_UNDER);
	FieldGroup *fg = new FieldGroup(root, true);
	fg->SetBevelStyle(FormEntry::bsRAISED);
	FlatIconButton *fi1 = new FlatIconButton(fg,"Begin","",(NotifyProc)&AnimationRun::begin, FileName());
	//fi1->Align(fb, AL_UNDER);
	FlatIconButton *fi2 = new FlatIconButton(fg,"Pause","",(NotifyProc)&AnimationRun::pause, FileName());
	fi2->Align(fi1,AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"Run","",(NotifyProc)&AnimationRun::run, FileName());
	fi1->Align(fi2,AL_AFTER,-10);
	fi2 = new FlatIconButton(fg,"Stop","",(NotifyProc)&AnimationRun::stop, FileName());
	fi2->Align(fi1, AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"End","",(NotifyProc)&AnimationRun::end, FileName());
	fi1->Align(fi2, AL_AFTER,-10);
	fg->Align(fgRest, AL_UNDER);
	create();
}

int AnimationRun::changeActive(Event *ev) {
	foAnimations->StoreData();
	int sel = foAnimations->ose->GetCurSel();
	if ( sel != -1) {
		AnimationProperties *prop = propsheet.getAnimation(sel);
		propsheet.setActiveAnimation(prop->drawer);
	}
	return 1;
}

int AnimationRun::speed(Event *ev) {
	sliderFps->StoreData();
	AnimationProperties *prop = propsheet.getActiveAnimation();
	if ( !prop)
		return 0;
	AnimationDrawer *andr = prop->drawer;
	if ( fps == 0 || fps == rUNDEF)
		return 1;
	andr->setInterval(1.0 / fps);
	run(0);
	return 1;
}

int AnimationRun::DataChanged(Event* ev) {
	int code = (int)ev;
	if ( GetSafeHwnd() && code == 1) {
		AnimationProperties *props =0;
		int index = 0;
		animIndex = propsheet.getActiveIndex();
		foAnimations->ose->ResetContent();
		while((props = propsheet.getAnimation(index++)) != 0) {
			foAnimations->ose->AddString(String("%S.Animation",props->drawer->getName()).scVal());
		}
		if ( animIndex >= 0)
			foAnimations->ose->SetCurSel(animIndex);
	}
	return 1;
}

int AnimationRun::stop(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
	KillTimer(props->drawer->getTimerId());
	props->drawer->setMapIndex(0);
	props->drawer->setIndex(0);
	props->drawer->setTimerId(iUNDEF);
	return 1;
}

int AnimationRun::pause(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
	KillTimer(props->drawer->getTimerId());
	return 1;
}

int AnimationRun::end(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->drawer->setMapIndex(props->drawer->getDrawerCount());
	props->drawer->setIndex(0);
	return 1;
}

int AnimationRun::run(Event  *ev) {
	//sliderFps->StoreData();
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	if ( props->drawer->getTimerId() != iUNDEF) {
		props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
		KillTimer(props->drawer->getTimerId());
	}else{
		int timeIdC = AnimationDrawer::getTimerIdCounter(true);
		props->drawer->setTimerId(timeIdC);
	}
	if ( props->drawer->getUseTime()) {
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(),REAL_TIME_INTERVAL,0);
		SetTimer(props->drawer->getTimerId(),REAL_TIME_INTERVAL,0);
	} else {
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(),props->drawer->getInterval() * 1000.0,0);
		SetTimer(props->drawer->getTimerId(),props->drawer->getInterval() * 1000.0,0);
	}
	props->mdoc->mpvGetView()->Invalidate();
	return 1;
}

int AnimationRun::begin(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	props->drawer->setMapIndex(0);
	props->drawer->setIndex(0);
	return 1;
}
//-------------------------------------------------------------------
AnimationSynchronization::AnimationSynchronization(AnimationPropertySheet& sheet) : 
FormBasePropertyPage(TR("Synchronization").scVal()), 
propsheet(sheet),
offset1(0),
choiceSlave1(-1),
choiceMaster(-1),
step1(1),
initial(true)
{
	fgMaster = new FieldGroup(root, true);
	StaticText *st = new StaticText(fgMaster, TR("Master animation"));
	foMaster = new FieldOneSelect(fgMaster,&choiceMaster);
	foMaster->SetWidth(60);
	foMaster->Align(st, AL_AFTER);
	//FieldGroup *fgSlave1 = new FieldGroup(root, true);
	StaticText *st2 = new StaticText(fgMaster, TR("Slave animation 1"));
	st2->Align(st, AL_UNDER);
	foSlave1 = new FieldOneSelect(fgMaster,&choiceSlave1);
	foSlave1->SetWidth(60);
	foSlave1->Align(st2, AL_AFTER);
	FieldInt *fiSlave1 = new FieldInt(fgMaster, TR("Offset"),&offset1);
	fiSlave1->SetWidth(10);
	fiSlave1->Align(foSlave1, AL_AFTER);
	FieldReal *fiSlaveStep = new FieldReal(fgMaster, TR("Step"),&step1,ValueRange(0.01,100));
	fiSlaveStep->Align(fiSlave1, AL_AFTER);
	fiSlaveStep->SetWidth(10);
	//fgMaster->Align(fgMaster, AL_UNDER);
	new FieldBlank(root);
	PushButton *pb = new PushButton(root,TR("Synchronize"),(NotifyProc)&AnimationSynchronization::synchronize);
	//pb->Align(fgMaster, AL_UNDER);
	create();
}

int AnimationSynchronization::DataChanged(Event*ev) {
	int code = (int)ev;
	if ( GetSafeHwnd() &&( initial || code == 1)) {
		AnimationProperties *props =0;
		int index = 0;
		choiceMaster = propsheet.getActiveIndex();
		foMaster->ose->ResetContent();
		foSlave1->ose->ResetContent();
		while((props = propsheet.getAnimation(index)) != 0) {
			foMaster->ose->AddString(String("%S.Animation",props->drawer->getName()).scVal());
			foMaster->ose->SetItemData(index, (DWORD_PTR)props);
			foSlave1->ose->AddString(String("%S.Animation",props->drawer->getName()).scVal());
			foSlave1->ose->SetItemData(index, (DWORD_PTR)props);
			++index;
		}
		if ( choiceMaster >= 0)
			foMaster->ose->SetCurSel(choiceMaster);
		initial = false;
	}
	return 1;
}

int AnimationSynchronization::synchronize(Event*) {
	fgMaster->StoreData();
	choiceMaster = foMaster->ose->GetCurSel();
	choiceSlave1 = foSlave1->ose->GetCurSel();
	AnimationProperties *masterDrawer = choiceMaster != -1 ? (AnimationProperties *)foMaster->ose->GetItemData(choiceMaster) : 0;
	AnimationProperties *slaveDrawer = choiceSlave1 != -1 ? (AnimationProperties *)foSlave1->ose->GetItemData(choiceSlave1) : 0;
	if ( (masterDrawer && slaveDrawer)) {
		if (masterDrawer->drawer->getId() == slaveDrawer->drawer->getId()) {
			throw ErrorObject(TR("Master and Slave must be different"));
		}
		int oldTimerId = slaveDrawer->drawer->getTimerId();
		slaveDrawer->mdoc->mpvGetView()->KillTimer(oldTimerId);
		slaveDrawer->drawer->setTimerId(SLAVE_TIMER_ID);
		SlaveProperties props(slaveDrawer->drawer,offset1,step1);
		masterDrawer->drawer->addSlave(props);
		slaveDrawer->drawer->setOffset(props.slaveOffset);
	}


	return 1;
}
//---------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationProgress, FormBasePropertyPage)
	ON_MESSAGE(ID_TIME_TICK, OnTimeTick)
END_MESSAGE_MAP()

AnimationProgress::AnimationProgress(AnimationPropertySheet& sheet) : FormBasePropertyPage(TR("Progress Control").scVal()), propsheet(sheet)
{
	fgMaster = new FieldGroup(root, true);
	RangeInt setRange = RangeInt(0, 100);
	graphSlider = new TimeGraphSlider(fgMaster, setRange);
	graphSlider->SetWidth(180);
	//graphSlider->Align(entry, AL_UNDER, 23);{
	create();
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
	if ( GetSafeHwnd() || code == 1) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( !props)
			return 0;
		AnimationDrawer *adrw = props->drawer;
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		int number=iUNDEF;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type ==IlwisObject::iotMAPLIST ) {
			MapList mpl((*source)->fnObj);
			number = mpl->iSize();
		
		}
		if ( type == IlwisObject::iotOBJECTCOLLECTION){
			ObjectCollection oc((*source)->fnObj);
			number = oc->iNrObjects();
		}
		RangeInt setRange = RangeInt(0, number);
		graphSlider->setRecordRange(setRange);
	}
	if (GetSafeHwnd() && code == 2) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( props == 0)
			return 1;
		AnimationDrawer *adrw = props->drawer;
		graphSlider->setIndex(adrw->getActiveMaps()[adrw->getMapIndex()]);
		//animBar.updateTime(String("index : %d",adrw->getMapIndex()));
	}
	return 1;
}