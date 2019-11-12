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
#include "DrawersUI\AnnotationNorthArrowTool.h"
#include "DrawersUI\GlobalAnnotationTool.h"

using namespace ILWIS;

DrawerTool *createAnnotationNorthArrowTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationNorthArrowTool(zv, view, drw);
}

AnnotationNorthArrowTool::AnnotationNorthArrowTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationNorthArrowTool"), zv, view, drw), northDrawer(0)
{
}

AnnotationNorthArrowTool::~AnnotationNorthArrowTool() {
}

bool AnnotationNorthArrowTool::isToolUseableFor(ILWIS::DrawerTool *tl) { 

	bool ok = dynamic_cast<GlobalAnnotationTool *>(tl) != 0;
	if (ok)
		return true;
	return false;
}

HTREEITEM AnnotationNorthArrowTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if (annotations)
		northDrawer = (AnnotationNorthArrowDrawer *)annotations->getDrawer("AnnotationNorthArrowDrawer");
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&AnnotationNorthArrowTool::makeActive);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&AnnotationNorthArrowTool::setPosition);
	htiNode = insertItem(TR("North Arrow"),"NorthArrow",item, northDrawer && northDrawer->isActive());

	DrawerTool::configure(htiNode);
	return htiNode;
}

String AnnotationNorthArrowTool::getMenuString() const {
	return "North Arrow Annotation";
}

void AnnotationNorthArrowTool::setPosition() {
	if ( northDrawer)
		new NorthArrowPosition(tree,northDrawer);
}

void AnnotationNorthArrowTool::setAppearance() {
	if ( northDrawer)
		new NorthArrowAppearance(tree, northDrawer);
}


void AnnotationNorthArrowTool::makeActive(void *v, HTREEITEM ) {
	bool act = *(bool *)v;
	if ( northDrawer) {
		northDrawer->setActive(act);
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
		northDrawer->prepare(&pp);
	}
	else {
		if ( act) {
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			ILWIS::DrawerParameters dp((RootDrawer *)drawer, drawer);
			northDrawer = (AnnotationNorthArrowDrawer *)NewDrawer::getDrawer("AnnotationNorthArrowDrawer","ilwis38",&dp);
			if ( northDrawer) {
				northDrawer->prepare(&pp);
				ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
				if ( annotations)
					annotations->addPostDrawer(409, northDrawer);
			}
		}
	}
	mpvGetView()->Invalidate();
}

//--------------------------------------------------------
NorthArrowPosition::NorthArrowPosition(CWnd *wPar, AnnotationNorthArrowDrawer *dr) : 
DisplayOptionsForm(dr,wPar,TR("Properties of the North Arrow")), scale(40 * dr->getScale()),selection(0)
{
	ILWIS::SVGLoader *loader = NewDrawer::getSvgLoader();
	int i = 0;
	for(map<String, IVGElement *>::iterator cur = loader->begin(); cur != loader->end(); ++cur) {
		if ( (*cur).second->getType() == IVGElement::ivgNORTHARROW) {
			String name = (*cur).first;
			name = name.sHead("|");
			names.push_back(name);
			if (name == dr->getArrowType())
				selection = i;
			++i;
		}
	}
	arrowType = new FieldOneSelectString(root,TR("North Arrows"),&selection, names);
	arrowType->SetCallBack((NotifyProc)&NorthArrowPosition::setArrow);

	Coord begin = dr->getBegin();
	if (begin.fUndef()) {
		begin.x = 0.9;
		begin.y = 0.9;
		dr->setBegin(begin);
	}
	x = round(100.0 * begin.x);
	y = round(100.0 * begin.y);
	sliderScale = new FieldIntSliderEx(root,TR("Scale(%)"), &scale,ValueRange(10,500),true);
	sliderH = new FieldIntSliderEx(root,TR("X position"), &x,ValueRange(0,100),true);
	sliderV = new FieldIntSliderEx(root,TR("Y position"), &y,ValueRange(0,100),true);
	sliderV->Align(sliderH, AL_UNDER);
	sliderH->SetCallBack((NotifyProc)&NorthArrowPosition::setPosition);
	sliderH->setContinuous(true);
	sliderV->SetCallBack((NotifyProc)&NorthArrowPosition::setPosition);
	sliderV->setContinuous(true);
	sliderScale->SetCallBack((NotifyProc)&NorthArrowPosition::setScale);
	sliderScale->setContinuous(true);
	create();
}

int NorthArrowPosition::setArrow(Event *ev) {
	arrowType->StoreData();
	if ( selection >= 0) {
		AnnotationNorthArrowDrawer *northDrw = (AnnotationNorthArrowDrawer *)drw;
		northDrw->setArrowType(names[selection]);
		northDrw->setScale(scale * 0.025);
		updateMapView();
	}
	return 1;
}

int NorthArrowPosition::setPosition(Event *ev) {
	sliderV->StoreData();
	sliderH->StoreData();
	AnnotationNorthArrowDrawer *northDrw = (AnnotationNorthArrowDrawer *)drw;

	double newx = x / 100.0;
	double newy = y / 100.0;
	northDrw->setBegin(Coord(newx,newy));
	updateMapView();

	return 1;
}

int NorthArrowPosition::setScale(Event *ev) {
	sliderScale->StoreData();
	AnnotationNorthArrowDrawer *northDrw = (AnnotationNorthArrowDrawer *)drw;
	northDrw->setScale(scale * 0.025);
	updateMapView();

	return 1;
}

void NorthArrowPosition::apply() {
	setArrow(0);
	setPosition(0);
	setScale(0);
	updateMapView();
}

//-------------------------------------------------------------------------------
NorthArrowAppearance::NorthArrowAppearance(CWnd *wPar, AnnotationNorthArrowDrawer *dr) : DisplayOptionsForm(dr,wPar,TR("Appearance of North Arrow"))
{
}

void NorthArrowAppearance::apply() {
	updateMapView();
}

