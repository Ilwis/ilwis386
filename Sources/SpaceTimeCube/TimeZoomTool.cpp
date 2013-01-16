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
#include "PreTimeOffsetDrawer.h"

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
	String text("Spatio-Temporal Zoom");
	htiNode = insertItem(text,"Valuerange", item);
	cbFullExtent = drawer->getRootDrawer()->getMapCoordBounds();

	return htiNode;
}

void TimeZoomTool::displayOptionTimeZoom() {
	if (drawer->getRootDrawer()->getMapCoordBounds().width() > cbFullExtent.width() || drawer->getRootDrawer()->getMapCoordBounds().height() > cbFullExtent.height())
		cbFullExtent = drawer->getRootDrawer()->getMapCoordBounds();
	CRect rect;
	bool fRestorePosition = false;
	if (timeZoomForm) {
		fRestorePosition = true;
		timeZoomForm->GetWindowRect(&rect);
		delete timeZoomForm;
		timeZoomForm = 0;
	}
	if (stc->fUseSpaceTimeCube() && stc->getTimeBoundsFullExtent()->fValid()) {
		timeZoomForm = new TimeZoomForm(tree, (ComplexDrawer *)drawer, htiNode, stc, cbFullExtent, *this);
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
TimeZoomForm::TimeZoomForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti, SpaceTimeCube * _stc, CoordBounds & _cbFullExtent, TimeZoomTool & _timeZoomTool)
: DisplayOptionsForm(dr,wPar,"Spatio-Temporal Zoom")
, maxSlider(1000)
, htiTimeZoom(hti)
, stc(_stc)
, cbFullExtent(_cbFullExtent)
, timeZoomTool(_timeZoomTool)
, fInCallback(false)
{
	tMin = stc->getTimeBoundsFullExtent()->tMin();
	tMax = 	stc->getTimeBoundsFullExtent()->tMax();
	zoom = 1.0;
	prevZoom = 1.0;
	xPos = cbFullExtent.MinX() + cbFullExtent.width() / 2.0;
	yPos = cbFullExtent.MinY() + cbFullExtent.height() / 2.0;
	calcSliderFromMinMax();
	calcSliderFromZoom();
	calcSliderFromPos();

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

	StaticText * stZoom = new StaticText(root, "Zoom");
	stZoom->Align(ftTo, AL_UNDER);
	sliderZoom = new FieldIntSlider(root, &iZoom, ValueRange(1, maxSlider), TBS_HORZ); // TBS_VERT|TBS_AUTOTICKS|TBS_RIGHT);
	sliderZoom->SetCallBack((NotifyProc)&TimeZoomForm::sliderZoomCallBack);
	sliderZoom->setContinuous(true);
	sliderZoom->Align(sliderTo, AL_UNDER);

	StaticText * stXPos = new StaticText(root, "X-Center");
	stXPos->Align(stZoom, AL_UNDER);
	sliderXPos = new FieldIntSlider(root, &iXPos, ValueRange(0, maxSlider), TBS_HORZ); // TBS_VERT|TBS_AUTOTICKS|TBS_RIGHT);
	sliderXPos->SetCallBack((NotifyProc)&TimeZoomForm::sliderPosCallBack);
	sliderXPos->setContinuous(true);
	sliderXPos->Align(sliderZoom, AL_UNDER);

	StaticText * stYPos = new StaticText(root, "Y-Center");
	stYPos->Align(stXPos, AL_UNDER);
	sliderYPos = new FieldIntSlider(root, &iYPos, ValueRange(0, maxSlider), TBS_HORZ); // TBS_VERT|TBS_AUTOTICKS|TBS_RIGHT);
	sliderYPos->SetCallBack((NotifyProc)&TimeZoomForm::sliderPosCallBack);
	sliderYPos->setContinuous(true);
	sliderYPos->Align(sliderXPos, AL_UNDER);
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

void TimeZoomForm::calcSliderFromZoom() {
	iZoom = zoom * maxSlider;
}

void TimeZoomForm::calcZoomFromSlider() {
	zoom = iZoom / maxSlider;
}

void TimeZoomForm::calcSliderFromPos() {
	iXPos = (xPos - cbFullExtent.MinX()) * maxSlider / cbFullExtent.width();
	iYPos = (yPos - cbFullExtent.MinY()) * maxSlider / cbFullExtent.height();
}

void TimeZoomForm::calcPosFromSlider() {
	xPos = cbFullExtent.MinX() + (double)(iXPos * cbFullExtent.width() / maxSlider);
	yPos = cbFullExtent.MinY() + (double)(iYPos * cbFullExtent.height() / maxSlider);
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

int TimeZoomForm::sliderZoomCallBack(Event *ev) {
	if (fInCallback)
		return 1;
	fInCallback = true;
	sliderZoom->StoreData();
	calcZoomFromSlider();

	SetNewValues();
	fInCallback = false;
	return 1;
}

int TimeZoomForm::sliderPosCallBack(Event *ev) {
	if (fInCallback)
		return 1;
	fInCallback = true;
	sliderXPos->StoreData();
	sliderYPos->StoreData();

	calcPosFromSlider();

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
	double xWidth = zoom * cbFullExtent.width() / 2.0;
	double yWidth = zoom * cbFullExtent.height() / 2.0;
	CoordBounds cbMap (Coord(xPos - xWidth, yPos - yWidth), Coord(xPos + xWidth, yPos + yWidth));
	drw->getRootDrawer()->setCoordBoundsMap(cbMap);
	double rotX, rotY, rotZ, transX, transY, transZ;
	drw->getRootDrawer()->getRotationAngles(rotX, rotY, rotZ);
	drw->getRootDrawer()->getTranslate(transX, transY, transZ);
	double zoom3D = drw->getRootDrawer()->getZoom3D();
	drw->getRootDrawer()->setCoordBoundsView(cbMap,true);
	// restore (because the setCoordBoundsView changed it all)
	drw->getRootDrawer()->setRotationAngles(rotX, rotY, rotZ);
	drw->getRootDrawer()->setTranslate(transX * zoom / prevZoom, transY * zoom / prevZoom, transZ * zoom / prevZoom);
	drw->getRootDrawer()->setZoom3D(zoom3D);
	prevZoom = zoom;

	String text("TimeZoom (%S,%S)", tMin.toString(), tMax.toString());
	TreeItem titem;
	view->getItem(htiTimeZoom,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);

	strcpy(titem.item.pszText,text.c_str());
	view->GetTreeCtrl().SetItem(&titem.item);

	PreparationParameters pp(NewDrawer::pt3D, 0);
	drw->getRootDrawer()->prepare(&pp);
	preparePreTimeOffsetDrawers(drw->getRootDrawer(), &pp);

	updateMapView();
}

// predrawers are never prepared again, only the root-drawer's pre/post drawers are prepared (flaw?) .. we are forced to re-prepare the PreTimeOffsetDrawers here
void TimeZoomForm::preparePreTimeOffsetDrawers(ComplexDrawer * drw, PreparationParameters * pp) {
	PreTimeOffsetDrawer *predrw = dynamic_cast<PreTimeOffsetDrawer*>(drw->getDrawer(0, ComplexDrawer::dtPRE));
	if (predrw != 0)
		predrw->prepare(pp);
	for (int i = 0; i < drw->getDrawerCount(); ++i) {
		ComplexDrawer *childDrw = dynamic_cast<ComplexDrawer*>(drw->getDrawer(i));
		if (childDrw != 0)
			preparePreTimeOffsetDrawers(childDrw, pp);
	}
}

void TimeZoomForm::OnOK() {
	timeZoomTool.setFormAutoDeleted();
	DisplayOptionsForm::OnOK();
}

void TimeZoomForm::OnCancel() {
	timeZoomTool.setFormAutoDeleted();
	DisplayOptionsForm::OnCancel();
}
