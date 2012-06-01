#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "TimeZoomTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "SpaceTimeCubeTool.h"
#include "TimeBounds.h"

DrawerTool *createTimeZoomTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TimeZoomTool(zv, view, drw);
}

TimeZoomTool::TimeZoomTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("TimeZoomTool",zv, view, drw)
, stc(0)
, timeZoomForm(0)
{
}

TimeZoomTool::~TimeZoomTool() {
	if (timeZoomForm)
		delete timeZoomForm;
}

bool TimeZoomTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	return false;
}

HTREEITEM TimeZoomTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;

	SpaceTimeCubeTool *stct = dynamic_cast<SpaceTimeCubeTool *>(parentTool);
	if ( stct )
		stc = stct->getSpaceTimeCube();

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&TimeZoomTool::displayOptionTimeZoom);
	//ILWIS::Time tMin = stc->getTimeBoundsZoom()->tMin();
	//ILWIS::Time tMax = stc->getTimeBoundsZoom()->tMax();
	//String text("TimeZoom (%S,%S)", tMin.toString(), tMax.toString());
	String text("TimeZoom");
	htiNode = insertItem(text,"Valuerange", item);

	return htiNode;
}

void TimeZoomTool::displayOptionTimeZoom() {
	CRect rect;
	bool fRestorePosition = false;
	if (timeZoomForm) {
		fRestorePosition = true;
		timeZoomForm->GetWindowRect(&rect);
		delete timeZoomForm;
		timeZoomForm = 0;
	}
	if (stc->fUseSpaceTimeCube() && stc->getTimeBoundsFullExtent()->fValid()) {
		timeZoomForm = new TimeZoomForm(tree, (ComplexDrawer *)drawer, htiNode, stc, *this);
		if (fRestorePosition)
			timeZoomForm->SetWindowPos(tree, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOREPOSITION);
	}
}

void TimeZoomTool::setFormAutoDeleted()
{
	timeZoomForm = 0;
}

String TimeZoomTool::getMenuString() const {
	return TR("TimeZoomTool");
}


//---------------------------------------------------
TimeZoomForm::TimeZoomForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti, SpaceTimeCube * _stc, TimeZoomTool & _timeZoomTool)
: DisplayOptionsForm(dr,wPar,"Time Zoom")
, maxSlider(1000)
, htiTimeZoom(hti)
, stc(_stc)
, timeZoomTool(_timeZoomTool)
, fInCallback(false)
{
	tMin = stc->getTimeBoundsFullExtent()->tMin();
	tMax = 	stc->getTimeBoundsFullExtent()->tMax();
	calcSliderFromMinMax();

	String sFromTime = tMin.toString();
	String sToTime = tMax.toString();
	StaticText * stRange = new StaticText(root, "Range: from");
	stRange->SetIndependentPos();
	FieldString * fsFrom = new FieldString(root, "", &sFromTime);
	fsFrom->SetWidth(60);
	fsFrom->Align(stRange, AL_AFTER);
	fsFrom->SetIndependentPos();
	StaticText * stTo = new StaticText(root, "to");
	stTo->Align(fsFrom, AL_AFTER);
	stTo->SetIndependentPos();
	FieldString * fsTo = new FieldString(root, "", &sToTime);
	fsTo->SetWidth(60);
	fsTo->Align(stTo, AL_AFTER);
	fsTo->SetIndependentPos();

	ftFrom = new FieldTime(root, "From", &tMin, 0, ILWIS::Time::mDATETIME);
	ftFrom->Align(stRange, AL_UNDER);
	sliderFrom = new FieldIntSlider(root, &timeZoomFrom, ValueRange(0, maxSlider), TBS_HORZ); // TBS_VERT|TBS_AUTOTICKS|TBS_RIGHT);
	sliderFrom->SetCallBack((NotifyProc)&TimeZoomForm::sliderFromCallBack);
	sliderFrom->setContinuous(true);
	sliderFrom->Align(ftFrom, AL_AFTER);

	ftTo = new FieldTime(root, "To", &tMax, 0, ILWIS::Time::mDATETIME);
	ftTo->Align(ftFrom, AL_UNDER);
	sliderTo = new FieldIntSlider(root, &timeZoomTo, ValueRange(0, maxSlider), TBS_HORZ);
	sliderTo->SetCallBack((NotifyProc)&TimeZoomForm::sliderToCallBack);
	sliderTo->setContinuous(true);
	sliderTo->Align(ftTo, AL_AFTER);
	create();
	fsFrom->SetReadOnly(true);
	fsTo->SetReadOnly(true);
}

void TimeZoomForm::calcSliderFromMinMax() {
	ILWIS::Time tMinFull = stc->getTimeBoundsFullExtent()->tMin();
	ILWIS::Time tMaxFull = stc->getTimeBoundsFullExtent()->tMax();
	ILWIS::Time tWidth = tMaxFull - tMinFull;
	timeZoomFrom = (tMin - tMinFull) * maxSlider / tWidth;
	timeZoomTo = (tMax - tMaxFull) * maxSlider / tWidth + maxSlider;
}

void TimeZoomForm::calcMinMaxFromSlider() {
	tMin = stc->getTimeBoundsFullExtent()->tMin();
	tMax = stc->getTimeBoundsFullExtent()->tMax();
	ILWIS::Time tWidth = tMax - tMin;
	tMin = tMin + (ILWIS::Time)(timeZoomFrom * tWidth / maxSlider);
	tMax = tMax - (ILWIS::Time)((maxSlider - timeZoomTo) * tWidth / maxSlider);
}

int TimeZoomForm::sliderFromCallBack(Event *ev) {
	if (fInCallback)
		return 1;
	fInCallback = true;
	sliderFrom->StoreData();
	if (timeZoomTo <= timeZoomFrom) {
		if (timeZoomFrom < maxSlider) {
			timeZoomTo = timeZoomFrom + 1;
			sliderTo->SetVal(timeZoomTo);
		}
		else {
			timeZoomFrom = timeZoomTo - 1;
			sliderFrom->SetVal(timeZoomFrom);
		}
	}

	calcMinMaxFromSlider();

	ftFrom->SetVal(tMin, ILWIS::Time::mDATETIME);
	ftTo->SetVal(tMax, ILWIS::Time::mDATETIME);

	SetNewValues();
	fInCallback = false;
	return 1;
}

int TimeZoomForm::sliderToCallBack(Event *ev) {
	if (fInCallback)
		return 1;
	fInCallback = true;
	sliderTo->StoreData();
	if (timeZoomTo <= timeZoomFrom) {
		if (timeZoomTo > 0) {
			timeZoomFrom = timeZoomTo - 1;
			sliderFrom->SetVal(timeZoomFrom);
		}
		else {
			timeZoomTo = timeZoomFrom + 1;
			sliderTo->SetVal(timeZoomTo);
		}
	}

	calcMinMaxFromSlider();

	ftFrom->SetVal(tMin, ILWIS::Time::mDATETIME);
	ftTo->SetVal(tMax, ILWIS::Time::mDATETIME);

	SetNewValues();
	fInCallback = false;
	return 1;
}

void TimeZoomForm::apply() {
	if (initial || fInCallback)
		return;
	fInCallback = true;
	ftFrom->StoreData();
	ftTo->StoreData();

	ILWIS::Time tMinFull = stc->getTimeBoundsFullExtent()->tMin();
	ILWIS::Time tMaxFull = stc->getTimeBoundsFullExtent()->tMax();

	if (tMin < tMinFull) {
		tMin = tMinFull;
		ftFrom->SetVal(tMin, ILWIS::Time::mDATETIME);
	}
	if (tMax > tMaxFull) {
		tMax = tMaxFull;
		ftTo->SetVal(tMax, ILWIS::Time::mDATETIME);
	}

	if (tMax <= tMin) {
		if (tMin < tMaxFull) {
			tMax = tMin + (ILWIS::Time)(1.0/86400.0); // + 1 sec
			ftTo->SetVal(tMax, ILWIS::Time::mDATETIME);
		}
		else {
			tMin = tMax - (ILWIS::Time)(1.0/86400.0); // - 1 sec
			ftFrom->SetVal(tMin, ILWIS::Time::mDATETIME);
		}
	}

	calcSliderFromMinMax();
	sliderFrom->SetVal(timeZoomFrom);
	sliderTo->SetVal(timeZoomTo);

	SetNewValues();
	fInCallback = false;
}

void TimeZoomForm::SetNewValues()
{
	*(stc->getTimeBoundsZoom()) = TimeBounds(tMin, tMax);

	String text("TimeZoom (%S,%S)", tMin.toString(), tMax.toString());
	TreeItem titem;
	view->getItem(htiTimeZoom,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

	strcpy(titem.item.pszText,text.c_str());
	view->GetTreeCtrl().SetItem(&titem.item);

	//LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(cdrw);
	PreparationParameters pp(NewDrawer::pt3D, 0);
	drw->getRootDrawer()->prepare(&pp);

	updateMapView();
}

void TimeZoomForm::OnOK() {
	timeZoomTool.setFormAutoDeleted();
	DisplayOptionsForm::OnOK();
}

void TimeZoomForm::OnCancel() {
	timeZoomTool.setFormAutoDeleted();
	DisplayOptionsForm::OnCancel();
}
