#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\FieldListView.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Engine\Domain\dmclass.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\AnnotationDrawers.h"
#include "DrawersUI\AnnotationScaleBarDrawerTool.h"
#include "DrawersUI\GlobalAnnotationTool.h"

using namespace ILWIS;

DrawerTool *createAnnotationScaleBarDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationScaleBarDrawerTool(zv, view, drw);
}

AnnotationScaleBarDrawerTool::AnnotationScaleBarDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationScaleBarDrawerTool"), zv, view, drw), scaleDrawer(0)
{
}

AnnotationScaleBarDrawerTool::~AnnotationScaleBarDrawerTool() {
	clear();
}

void AnnotationScaleBarDrawerTool::clear() {
	if ( drawer) {
		ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
		if ( annotations && scaleDrawer)
			annotations->removeDrawer(scaleDrawer->getId());
	}
}

bool AnnotationScaleBarDrawerTool::isToolUseableFor(ILWIS::DrawerTool *tl) { 

	bool ok = dynamic_cast<GlobalAnnotationTool *>(tl) != 0;
	if (ok)
		return true;
	return false;
}

HTREEITEM AnnotationScaleBarDrawerTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if (annotations)
		scaleDrawer = (AnnotationScaleBarDrawer *)annotations->getDrawer("AnnotationScaleBarDrawer");
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&AnnotationScaleBarDrawerTool::makeActive);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&AnnotationScaleBarDrawerTool::setPosition);
	htiNode = insertItem(TR("ScaleBar"),"ScaleBar",item, scaleDrawer && scaleDrawer->isActive());

	DrawerTool::configure(htiNode);
	return htiNode;
}

String AnnotationScaleBarDrawerTool::getMenuString() const {
	return "ScaleBar Annotation";
}

void AnnotationScaleBarDrawerTool::setPosition() {
	if ( scaleDrawer)
		new ScaleBarPosition(tree,scaleDrawer);
}

void AnnotationScaleBarDrawerTool::makeActive(void *v, HTREEITEM ) {
	bool act = *(bool *)v;
	if ( scaleDrawer) {
		scaleDrawer->setActive(act);
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
		scaleDrawer->prepare(&pp);
	}
	else {
		if ( act) {
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			ILWIS::DrawerParameters dp((RootDrawer *)drawer, drawer);
			scaleDrawer = (AnnotationScaleBarDrawer *)NewDrawer::getDrawer("AnnotationScaleBarDrawer","ilwis38",&dp);
			if ( scaleDrawer) {
				scaleDrawer->prepare(&pp);
				ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
				if ( annotations)
					annotations->addPostDrawer(405, scaleDrawer);
			}
		}
	}
	mpvGetView()->Invalidate();
}

//--------------------------------------------------------
ScaleBarPosition::ScaleBarPosition(CWnd *wPar, AnnotationScaleBarDrawer *dr)
: DisplayOptionsForm(dr,wPar,TR("Properties of the ScaleBar"))
, cbUseKilometers(0)
, fldNumbers(0)
{
	Coord begin = dr->getBegin();
	line = dr->getLine();
	divideFirstInterval = dr->getDivideFirstInterval();
	multiLabels = dr->getMultiLabels();
	unit = dr->getUnit();
	km = dr->getKm();
	ticks = dr->getTicks();
	sz = dr->getSize();
	num = dr->getNumberOfDigits();
	x = round(100.0 * begin.x);
	y = round(100.0 * begin.y);
	sliderH = new FieldIntSliderEx(root,TR("X position"), &x,ValueRange(0,100),true);
	sliderV = new FieldIntSliderEx(root,TR("Y position"), &y,ValueRange(0,100),true);
	sliderV->Align(sliderH, AL_UNDER);
	sliderH->SetCallBack((NotifyProc)&ScaleBarPosition::setPosition);
	sliderH->setContinuous(true);
	sliderV->SetCallBack((NotifyProc)&ScaleBarPosition::setPosition);
	sliderV->setContinuous(true);
	cbLine = new CheckBox(root, TR("Line"),&line);
	fldTicks = new FieldInt(root, TR("Nr of Intervals"),&ticks);
	fldSize = new FieldReal(root, TR("Interval Length"),&sz);
	cbDivide = new CheckBox(root, TR("1st interval divided into smaller parts"),&divideFirstInterval);
	cbMultiLabels = new CheckBox(root, TR("Labels for every interval"),&multiLabels);
	fldUnit = new FieldString(root, TR("Unit"),&unit);
	CoordSystem csy = dr->getRootDrawer()->getCoordinateSystem();
	if (csy.fValid()) {
		if (csy->pcsLatLon()) {
			fldNumbers = new FieldInt(root,TR("Decimal digits"), &num);
		} else {
			cbUseKilometers = new CheckBox(root, TR("Use Kilometers"), &km);
			cbUseKilometers->SetCallBack((NotifyProc)&ScaleBarPosition::UseKilometersChanged);
			fldNumbers = new FieldInt(cbUseKilometers,TR("Decimal digits"), &num);
		}
	}
	create();
}

int ScaleBarPosition::setPosition(Event *ev) {
	sliderV->StoreData();
	sliderH->StoreData();
	AnnotationScaleBarDrawer *scaleDrw = (AnnotationScaleBarDrawer *)drw;
	double newx = x / 100.0;
	double newy = y / 100.0;
	scaleDrw->setBegin(Coord(newx,newy));
	updateMapView();

	return 1;
}

int ScaleBarPosition::UseKilometersChanged(Event *)
{
	if (cbUseKilometers->fVal())
		fldUnit->SetVal("km");
	else
		fldUnit->SetVal("meters");
  return 0;  
}

void ScaleBarPosition::apply() {
	setPosition(0);
	fldUnit->StoreData();
	cbLine->StoreData();
	cbDivide->StoreData();
	cbMultiLabels->StoreData();
	fldSize->StoreData();
	fldTicks->StoreData();
	if (cbUseKilometers)
		cbUseKilometers->StoreData();
	if (fldNumbers)
		fldNumbers->StoreData();
	AnnotationScaleBarDrawer *scaleDrw = (AnnotationScaleBarDrawer *)drw;
	scaleDrw->setUnit(unit);
	scaleDrw->setLine(line);
	scaleDrw->setDivideFirstInterval(divideFirstInterval);
	scaleDrw->setMultiLabels(multiLabels);
	scaleDrw->setSize(sz);
	scaleDrw->setTicks(ticks);
	scaleDrw->setKm(km);
	scaleDrw->setNumberOfDigits(num);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	scaleDrw->prepare(&pp);
	updateMapView();
}
