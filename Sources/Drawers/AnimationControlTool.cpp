#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\TimeGraphSlider.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\AnimationControlTool.h"
#include "Drawers\AnimationTool.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"

using namespace ILWIS;

#define REAL_TIME_INTERVAL 100
 
DrawerTool *createAnimationControlTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationControlTool(zv, view, drw);
}

AnimationControlTool::AnimationControlTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnimationControlTool", zv, view, drw),animControl(0)
{
}

AnimationControlTool::~AnimationControlTool() {
	delete animControl;
}

bool AnimationControlTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return dynamic_cast<AnimationTool *>(tool) != 0;
}

HTREEITEM AnimationControlTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item2->setDoubleCickAction(this, (DTDoubleClickActionFunc) &AnimationControlTool::animationControl);
	htiNode = insertItem(TR("Run"),"History",item2);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void AnimationControlTool::animationControl() {
	if ( animControl == 0) {
		animControl = new AnimationControl(tree, (AnimationDrawer *)drawer);
	}
	animControl->ShowWindow(SW_SHOW);
}

String AnimationControlTool::getMenuString() const {
	return TR("Animation Control");
}
//---------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationControl, DisplayOptionsForm2)
	//ON_MESSAGE(ID_TIME_TICK, OnTimeTick)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void AnimationControl::OnTimer(UINT timerID) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	graphSlider->setIndex(adrw->getActiveMaps()[adrw->getMapIndex()]);
	animBar.updateTime(String("index : %d",adrw->getMapIndex()));
}

LRESULT AnimationControl::OnTimeTick( WPARAM wParam, LPARAM lParam ) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->getDrawerCount() - 1 == (int)wParam )
		st->Hide();

	if ( lParam == TRUE) {
		graphSlider->setIndex(adrw->getActiveMaps()[wParam]);
	}
	else {
		adrw->setMapIndex(wParam);
	}
	return 1;
}

AnimationControl::AnimationControl(CWnd *par, AnimationDrawer *adr) 
: DisplayOptionsForm2(adr, par, "Time",fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsHIDEONCLOSE), 
	fgTime(0), useTimeAttribute(false)
{
	initial = true;
	IlwisObject *source = (IlwisObject *)adr->getDataSource();
	fps = 1.0 / adr->getInterval();
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
		IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			fcol = new FieldColumn(root,TR("Reference Attribute"),mpl->tblAtt(),&colName,dmVALUE);
			fcol->SetCallBack((NotifyProc)&AnimationControl::changeColum);
		}
	}
	//if ( type ==IlwisObject::iotOBJECTCOLLECTION) {
	//	ObjectCollection oc((*datasource)->fnObj);
	//	if ( mpl->fTblAtt()) {
	//		fcol = new FieldColumn(root,TR("Reference Attribute"),mpl->tblAtt(),&colName,dmVALUE);
	//		fcol->SetCallBack((NotifyProc)&AnimationControl::changeColum);
	//	}
	//}

	useTimeAttribute =  adr->getUseTime();
	cbTime = new CheckBox(root,TR("Use Time Attribute"),&useTimeAttribute);
	cbTime->SetCallBack((NotifyProc)&AnimationControl::setTimingMode);

	
	/*frtime = new FieldReal(root,TR("Interval"), &adr->interval,ValueRangeReal(0.1,1000,0.1));
	frtime->SetCallBack((NotifyProc)&AnimationControl::speed(Event *ev));
	frtime->Align(cbTime, AL_UNDER);*/
	setTimeElements(cbTime);
	setSlider(cbTime);
	sliderFps = new FieldRealSliderEx(root,"Frame rate(fps)", &fps,ValueRange(RangeReal(0,5),0.1),false);
	sliderFps->Align(graphSlider, AL_UNDER,-25);
	sliderFps->SetCallBack((NotifyProc)&AnimationControl::speed);
	new FieldBlank(root);

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
	st = new StaticText(root,"Preparing . . .");	
	st->Align(fg, AL_AFTER);

	MapWindow *parent = (MapWindow *)drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->GetParent();

	if ( animBar.GetSafeHwnd() == 0) {
		animBar.Create(parent);
		CRect rect;
		parent->barScale.GetWindowRect(&rect);
		rect.OffsetRect(1,0);
		parent->DockControlBar(&animBar,AFX_IDW_DOCKBAR_TOP, rect);
	}

	create();

}

int AnimationControl::speed(Event *ev) {
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	if ( fps == 0 || fps == rUNDEF)
		return 1;
	andr->setInterval(1.0 / fps);
	run(0);
	return 1;
}

int AnimationControl::setTimingMode(Event *ev) {
	cbTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	adrw->setUseTime(useTimeAttribute);
	sliderFps->Hide();
	if ( fgTime)
		fgTime->Hide();
	if ( adrw->getUseTime()) {
		sliderFps->Hide();
		if ( fgTime)
			fgTime->Show();
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			Column col = mpl->tblAtt()->col(adrw->getTimeColumn());
			TimeInterval interval(col->rrMinMax().rLo(),col->rrMinMax().rHi());
			graphSlider->setTimeInterval(interval);
			graphSlider->setTimes(col);
		}
	} else {
		sliderFps->Show();
		graphSlider->setTimeInterval(TimeInterval());
	}
	stop(0);
	run(0);
	return 1;
}

void AnimationControl::setTimeElements(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	IlwisObject *source = (IlwisObject *)adrw->getDataSource();
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc((*source)->fnObj);
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			fgTime = new FieldGroup(entry);
			timeColName = adrw->getTimeColumn();
			fcolTime = new FieldColumn(fgTime,"",mpl->tblAtt(),&timeColName,dmTIME);
			fcolTime->SetCallBack((NotifyProc)&AnimationControl::changeTimeColumn);
			fgTime->Align(entry, AL_AFTER);
			FieldGroup *fg2 = new FieldGroup(fgTime);
			fiYr = new FieldInt(fg2,TR("Period(YMDHm)/ sec"),&year);
			fiYr->Align(fg2,AL_AFTER);
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
			fg2->Align(entry, AL_UNDER);
			fg2->SetIndependentPos();
			fiYr->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMinute->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMonth->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiDay->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMinute->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
		}
	} 
}

int AnimationControl::changeDuration(Event *ev) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->getTimeColumn() != "" && adrw->getUseTime()) {
		fiYr->StoreData();
		fiMinute->StoreData();
		fiMonth->StoreData();
		fiDay->StoreData();
		fiMinute->StoreData();
		
		Duration dur(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		adrw->setTimeStep(dur);
	}
	return 1;
}
int AnimationControl::changeTimeColumn(Event *e) {
	fcolTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->getTimeColumn() != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
			IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
				ObjectCollection oc((*source)->fnObj);
		}
		if ( type ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			if ( mpl->fTblAtt()) {
				Column col = mpl->tblAtt()->col(adrw->getTimeColumn());
				adrw->setTimeStep(calcNiceStep((col->rrMinMax().rHi() - col->rrMinMax().rLo()) / mpl->iSize()));
				//sliderFps->SetVal(adrw->timestep);

			}
		} 
		
	}
	return 1;
}

double AnimationControl::calcNiceStep(Duration time) {
	year = 4712 + time.get(Time::tpYEAR);
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
	} else if ( month > 0) {
		hour = minute = 0;
	} else if ( day > 0) {
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
void AnimationControl::setSlider(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
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
	graphSlider = new TimeGraphSlider(root, setRange);
	graphSlider->SetWidth(180);
	graphSlider->Align(entry, AL_UNDER, 23);

}

int AnimationControl::changeColum(Event *) {
	fcol->StoreData();
	if ( colName != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = (IlwisObject *)adrw->getDataSource();
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
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	
	run(0);
	return 1;
}

int AnimationControl::stop(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->getTimerId());
	KillTimer(andr->getTimerId());
	andr->setMapIndex(0);
	andr->setIndex(0);
	andr->setTimerId(iUNDEF);
	return 1;
}

int AnimationControl::pause(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->getTimerId());
	KillTimer(andr->getTimerId());
	return 1;
}

int AnimationControl::end(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->setMapIndex(andr->getDrawerCount());
	andr->setIndex(0);
	return 1;
}

int AnimationControl::run(Event  *ev) {
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	if ( andr->getTimerId() != iUNDEF) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->getTimerId());
		KillTimer(andr->getTimerId());
	}else{
		andr->setTimerId(AnimationDrawer::timerIdCounter++);
	}
	if ( andr->getUseTime()) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->getTimerId(),REAL_TIME_INTERVAL,0);
		this->SetTimer(andr->getTimerId(),REAL_TIME_INTERVAL,0);
	} else {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->getTimerId(),andr->getInterval() * 1000.0,0);
		this->SetTimer(andr->getTimerId(),andr->getInterval() * 1000.0,0);
	}
	updateMapView();
	return 1;
}

int AnimationControl::begin(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->setMapIndex(0);
	andr->setIndex(0);
	return 1;
}

void AnimationControl::shutdown(int iReturn) {
	return DisplayOptionsForm2::shutdown();
}
//-------------------
//---------------------------------------------------

BEGIN_MESSAGE_MAP(AnimationBar, CToolBar)
	ON_EN_SETFOCUS(ID_AnimationBar,OnSetFocus)
	ON_EN_KILLFOCUS(ID_AnimationBar,OnKillFocus)
END_MESSAGE_MAP()

AnimationBar::AnimationBar()
{
	fActive = false;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// Since design guide says toolbars are fixed height so is the font.
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");
	fnt.CreateFontIndirect(&logFont);
}

AnimationBar::~AnimationBar()
{
}

void AnimationBar::Create(CWnd* pParent)
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

	SetWindowText(TR("Animation").scVal());
}

void AnimationBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	if (fActive)
		return;
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


void AnimationBar::updateTime(const String& s) // called by AnimationBarEdit
{

	ed.SetWindowText(s.scVal());
}
