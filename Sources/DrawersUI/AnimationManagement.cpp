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
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(0);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pSynchornization) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(1);
		page->DataChanged((Event*)1);
	}
	if ( (int)wp & pProgress) {
		FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(2);
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
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(int i =0; i < animations.size(); ++i) {
		if ( animations[i].drawer->getId() == drw->getId()) {
			if ( i < activeIndex)
				activeIndex--;
			if ( i == activeIndex)
				activeIndex = -1;
			FormBasePropertyPage *page = (FormBasePropertyPage *)GetPage(2);
			page->DataChanged((Event *)3);
			//animations[i].animBar->stop();
			animations[i].drawer->manager = 0;
			animations.erase(animations.begin() + i);
			page = (FormBasePropertyPage *)GetPage(0);
			page->DataChanged((Event *)3);
		}
	}
	PostMessage(ILWM_UPDATE_ANIM,pAll);
}

AnimationProperties *AnimationPropertySheet::getActiveAnimation(){
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	if ( activeIndex >= 0) {
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
AnimationRun::AnimationRun(AnimationPropertySheet& sheet) : FormBasePropertyPage(TR("Run").c_str()), propsheet(sheet), animIndex(0), fps(1),saveToAvi(false),movieRecorder(0)
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
	FlatIconButton *fiTemp;
	FlatIconButton *fi1 = new FlatIconButton(fg,"Begin","",(NotifyProc)&AnimationRun::begin, FileName());
	fiTemp = fi1;
	FlatIconButton *fi2 = new FlatIconButton(fg,"OneFrameMinus","",(NotifyProc)&AnimationRun::frameMinus, FileName());
	fi2->Align(fi1, AL_AFTER);
	fi1 = new FlatIconButton(fg,"Pause","",(NotifyProc)&AnimationRun::pause, FileName());
	fi1->Align(fi2,AL_AFTER,-10);
	fi2 = new FlatIconButton(fg,"Run","",(NotifyProc)&AnimationRun::run, FileName());
	fi2->Align(fi1,AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"Stop","",(NotifyProc)&AnimationRun::stop, FileName());
	fi1->Align(fi2, AL_AFTER,-10);
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
	}
	return 1;
}

int AnimationRun::frameMinus(Event *ev) {
	pause(0);
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	int index = props->drawer->getMapIndex();
	--index;
	if ( index < 0)
		index = props->drawer->getActiveMaps().size() - 1;
	props->drawer->setMapIndex(index);
	props->mdoc->mpvGetView()->Invalidate();
	return 1;
}

int AnimationRun::framePlus(Event *ev) {
	pause(0);
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	int index = props->drawer->getMapIndex();
	++index;
	if ( index == props->drawer->getActiveMaps().size())
		index = 0;
	props->drawer->setMapIndex(index);
	props->mdoc->mpvGetView()->Invalidate();
	return 1;
}

int AnimationRun::stop(Event  *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	if ( saveToAvi) {
		stopAvi();
	}
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
	if ( saveToAvi) {
			startAvi();
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
FormBasePropertyPage(TR("Synchronization").c_str()), 
propsheet(sheet),
offset1(0),
choiceSlave1(-1),
choiceMaster(-1),
step1(1),
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
	FieldInt *fiSlave1I = new FieldInt(fgSlaveIndex, TR("Offset"),&offset1);
	fiSlave1I->SetWidth(10);
	fiSlave1I->Align(anchor, AL_UNDER);
	FieldReal *fiSlaveStep = new FieldReal(fgSlaveIndex, TR("Step"),&step1,ValueRange(0.01,100));
	fiSlaveStep->Align(fiSlave1I, AL_AFTER);
	fiSlaveStep->SetWidth(10);
	fgSlaveIndex->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(fgSlaveIndex);
	fb->Align(fiSlave1I, AL_UNDER);
	PushButton *pb = new PushButton(fgSlaveIndex,TR("Synchronize"),(NotifyProc)&AnimationSynchronization::synchronize);
}

void AnimationSynchronization::setTimerPerTime(FormEntry *anchor) {
	year =1; month = 2; hour = 3; minute = 4;
	fgSlaveTime = new FieldGroup(fgMaster);
	FieldInt *fiYr = new FieldInt(fgSlaveTime,TR("Offset(YMDHm)"),&year);
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
		AnimationProperties *prop = propsheet.getActiveAnimation();;
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
			String name = prop->drawer->getName();
			String v = TR("Master animation %S");
			stMaster->SetVal(String(v.c_str(),name));
			initial = false;
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
		SlaveProperties props(slaveDrawer->drawer,offset1, sizeSlave/ sizeMaster);
		masterDrawer->drawer->addSlave(props);
		slaveDrawer->drawer->setOffset(props.slaveOffset);
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

	RangeInt setRange = RangeInt(0, 100);
	graphSlider = new TimeGraphSlider(fgMaster, setRange);
	graphSlider->SetWidth(180);
	graphSlider->SetHeight(110);
	graphSlider->Align(fcol, AL_UNDER);


	//useTimeAttribute =  adr->getUseTime();


	create();
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

	return 11;
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
		if ( form)
			form->PostMessage(WM_CLOSE);
		form = 0;
		graphSlider->setSourceTable(tbl);
	}
	if ( GetSafeHwnd() || code == 1) {
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
			RangeInt setRange = RangeInt(0, number);
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


int AnimationProgress::changeColumn(Event *) {
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
DisplayOptionsForm(_props->drawer, wPar, TR("Thresholds")), col(_col), graph(slider), gtThreshold(0), ltThreshold(0), type(0), props(_props) {
	calcMad(col);
	view = props->mdoc->ltvGetView();
	color = ((LayerDrawer *)props->drawer->getDrawer(0))->getDrawingColor()->getTresholdColor();
	RadioGroup *rg = new RadioGroup(root, TR("Threshold type"),&type);
	RadioButton *rb = new RadioButton(rg,TR("Above threshold"));
	frr = new FieldReal(rb, "",&gtThreshold);
	frr->Align(rb, AL_AFTER);
	rb = new RadioButton(rg,TR("Below threshold"));
	frr = new FieldReal(rb, "",&ltThreshold);
	frr->Align(rb, AL_AFTER);
	fc = new FieldColor(root,TR("Treshold color"),&color);
	fc->SetCallBack((NotifyProc)&GraphPropertyForm::changeColor);
	slider->setLinkedWindow(props->animBar);

	create();

}

int GraphPropertyForm::changeColor(Event *ev) {
	fc->StoreData();
	oldRange = RangeReal();
	apply();

	return 1;
}

void GraphPropertyForm::calcMad(const Column& col) {
	if ( col->dm()->pdv()) {
	
		RangeReal mm = col->rrMinMax();
		double tr = mm.rLo() + mm.rWidth() * 0.8;
		gtThreshold = tr;
		ltThreshold = tr ;

	}
}

FormEntry *GraphPropertyForm::CheckData() {
	return 0;
}
void GraphPropertyForm::apply() {
	root->StoreData();
	if ( type == 0)
		graph->setThreshold(gtThreshold);
	if ( type == 1)
		graph->setThreshold(ltThreshold);
	if ( props ) {
		RangeReal rr;
		if ( type == 0) {
			rr = RangeReal(gtThreshold, 1e307);
		} else {
			rr = RangeReal(-1e307,ltThreshold); 
		}
	//	if ( rr != oldRange) {
			for(int i=0; i < props->drawer->getDrawerCount(); ++i) {
				LayerDrawer *ldr = (LayerDrawer *)props->drawer->getDrawer(i);
				ldr->getDrawingColor()->setTresholdColor(color);
				ldr->getDrawingColor()->setTresholdRange(rr);
			}
			PreparationParameters pp(NewDrawer::ptRENDER, 0);
			props->drawer->prepareChildDrawers(&pp);
		//	oldRange = rr;
			updateMapView();
	//	}
	}
}

void GraphPropertyForm::OnClose() {
	GetParent()->PostMessage(ID_CLEAN_FORM);

	shutdown();

}

//---------------------------------------------
RealTimePage::RealTimePage(ILWIS::AnimationPropertySheet &sheet) :FormBasePropertyPage(TR("Real time progress").c_str()), propsheet(sheet), useTimeAttribute(false) 
{
	year = month = day = hour = minute = 0;

	fgMaster = new FieldGroup(root, true);

	stMaster = new StaticText(fgMaster, TR("Selected Animation                                               "), true);
	stMaster->SetIndependentPos();

	cbTime = new CheckBox(fgMaster,TR("Use Time Attribute"),&useTimeAttribute);
	cbTime->SetCallBack((NotifyProc)&RealTimePage::setTimingMode);
	cbTime->Align(stMaster, AL_UNDER);

	fgTime = new FieldGroup(fgMaster);

	fcolTime = new FieldColumn(fgTime,"",0,&timeColName,dmTIME);
	fcolTime->SetCallBack((NotifyProc)&RealTimePage::changeTimeColumn);
	new FieldBlank(fgTime);
	FieldGroup *fg2 = new FieldGroup(fgTime);
	fiYr = new FieldInt(fg2,TR("Period(YMDHm)/ sec"),&year);
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
	fg2->Align(fgMaster, AL_UNDER);
	fg2->SetIndependentPos();
	fiYr->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMinute->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMonth->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiDay->SetCallBack((NotifyProc)&RealTimePage::changeDuration);
	fiMinute->SetCallBack((NotifyProc)&RealTimePage::changeDuration);

	fgTime->Hide();
	create();
}

int RealTimePage::DataChanged(Event*ev) {
	int code = (int)ev;
	if ( GetSafeHwnd() || code == 1) {
		AnimationProperties *props = propsheet.getActiveAnimation();
		if ( !props)
			return 0;
		AnimationDrawer *adrw = props->drawer;
		useTimeAttribute =  adrw->getUseTime();
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

void RealTimePage::changeTimer(bool isRealTime, AnimationProperties *props) {
	if ( isRealTime){
		props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(),REAL_TIME_INTERVAL,0);
		props->mdoc->mpvGetView()->Invalidate();
	} else {
		props->mdoc->mpvGetView()->KillTimer(props->drawer->getTimerId());
		props->mdoc->mpvGetView()->SetTimer(props->drawer->getTimerId(),props->drawer->getInterval() * 1000.0,0);
	}
}

int RealTimePage::changeDuration(Event *ev) {
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	AnimationDrawer *adrw = props->drawer;
	if ( adrw->getTimeColumn() != "" && adrw->getUseTime()) {
		fiYr->StoreData();
		fiMinute->StoreData();
		fiMonth->StoreData();
		fiDay->StoreData();
		fiHour->StoreData();
		if ( year ==0 && month ==0 && day == 0 && hour ==0 && minute == 0)
			return 1;
		Duration dur(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));
		adrw->setTimeStep(dur);
		changeTimer(true, props);
	} else {
		changeTimer(false, props);
	}
	return 1;
}
int RealTimePage::changeTimeColumn(Event *e) {
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
					double timeStep = calcNiceStep((col->rrMinMax().rHi() - col->rrMinMax().rLo()) / mpl->iSize());
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
	cbTime->StoreData();
	fcolTime->StoreData();
	AnimationProperties *props = propsheet.getActiveAnimation();
	if (!props)
		return 0;
	AnimationDrawer *adrw = props->drawer;
	adrw->setUseTime(useTimeAttribute);
	if ( fgTime)
		fgTime->Hide();
	if ( adrw->getUseTime()) {
		if ( fgTime)
			fgTime->Show();
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			Column col = mpl->tblAtt()->col(colName);
			if ( col.fValid()) {
				propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pProgress);
			} 
		}
	} else {

		propsheet.PostMessage(ILWM_UPDATE_ANIM, AnimationPropertySheet::pProgress);
	}
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
	SetButtonInfo(1, ID_AnimationBar,	TBBS_SEPARATOR, iWidth - rect.Width());

	rect.top = 3;
	rect.bottom -= 2;
	rect.right = rect.left + iWidth;
	ed.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|WS_DISABLED,rect,this,ID_AnimationBar);
	ed.SetFont(&fnt);
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
			ed.SetWindowText(String("%S",tmstring).c_str());
	}
	else {
		if ( ed.GetSafeHwnd())
			ed.SetWindowText(String("index : %d",animation.drawer->getCurrentIndex()).c_str());
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
			double currentTime = timeCol->rValue(ind);
			ILWIS::Time ct(currentTime);
			timestring += ":" + ct.toString(true,timeCol->dm()->pdtime()->getMode());
		}
	}
	return String("index %d : %S",ind, timestring);
}




