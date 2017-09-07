#include "Client\Headers\formelementspch.h"
#include "Engine\Base\Round.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\FieldListView.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
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
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\AnnotationDrawers.h"
#include "DrawersUI\AnnotationLegendDrawerTool.h"
#include "DrawersUI\AnnotationDrawerTool.h"

using namespace ILWIS;

int AnnotationLegendDrawerTool::count = 0;

DrawerTool *createAnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationLegendDrawerTool(zv, view, drw);
}

AnnotationLegendDrawerTool::AnnotationLegendDrawerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool(TR("AnnotationLegendDrawerTool"), zv, view, drw), legend(0)
{
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(drawer);
	if ( ldrw) {
		SpatialDataDrawer *sddrw = dynamic_cast<SpatialDataDrawer *>(ldrw->getParentDrawer());
		if ( sddrw) {
			IlwisObjectPtr *obj = sddrw->getObject();
			if ( obj) {
				associatedFile = obj->fnObj;
			}
		}
	}
}

AnnotationLegendDrawerTool::~AnnotationLegendDrawerTool() {
}

void AnnotationLegendDrawerTool::clear() {
	ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if ( annotations && legend) {
		annotations->removeDrawer(legend->getId());
		legend = 0;
	}
}

bool AnnotationLegendDrawerTool::isToolUseableFor(ILWIS::DrawerTool *drw) { 

	return dynamic_cast<AnnotationDrawerTool *>(drw) != 0;
}

HTREEITEM AnnotationLegendDrawerTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree, parentItem, drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&AnnotationLegendDrawerTool::makeActive);
	htiNode = insertItem(TR("Legend"),"legend",item, legend && legend->isActive());
	item = new DisplayOptionTreeItem(tree, htiNode, drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&AnnotationLegendDrawerTool::setPosition);
	insertItem(TR("Size & Position"),"Position",item);

	item = new DisplayOptionTreeItem(tree, htiNode, drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&AnnotationLegendDrawerTool::setAppearance);
	insertItem(TR("Appearance"),"Appearance",item);

	DrawerTool::configure(htiNode);

	return htiNode;
}

String AnnotationLegendDrawerTool::getMenuString() const {
	return "Legend Annotation";
}

void AnnotationLegendDrawerTool::setPosition() {
	if ( legend)
		new LegendPosition(tree,legend);
}

void AnnotationLegendDrawerTool::setAppearance() {
	if ( legend)
		new LegendAppearance(tree, legend,(ComplexDrawer *)drawer);
}

AnnotationDrawer *AnnotationLegendDrawerTool::findAnnotation() const{
	AnnotationDrawers *annotations = (AnnotationDrawers *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
	if ( annotations) {
		vector<NewDrawer *> allDrawers;
		annotations->getDrawers(allDrawers);
		for(int i = 0; i < allDrawers.size(); ++i) {
			AnnotationDrawer *adrw = dynamic_cast<AnnotationDrawer *>(allDrawers[i]);
			if ( adrw && adrw->associaltedFile() == associatedFile)
				return adrw;
		}
	}
	return 0;
}
void AnnotationLegendDrawerTool::makeActive(void *v, HTREEITEM ) {
	bool act = *(bool *)v;
	if ( legend) {
		legend->setActive(act);
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
		legend->prepare(&pp);
	}
	else {
		if ( act) {
			legend = dynamic_cast<AnnotationLegendDrawer *>(findAnnotation());
			if ( legend)
				return;

			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(drawer);
			SetDrawer *sdr = dynamic_cast<SetDrawer *>(drawer);
			Domain dm;
			if ( ldr) {
				SpatialDataDrawer *spdr = (SpatialDataDrawer *)(drawer->getParentDrawer());
				dm = ldr->useAttributeColumn() ? ldr->getAtttributeColumn()->dm() : spdr->getBaseMap()->dm();
			} else if ( sdr) {
				dm = sdr->useAttributeTable() ? sdr->getAtttributeColumn()->dm() :   sdr->getBaseMap()->dm();
			}
			ILWIS::DrawerParameters dp(drawer->getRootDrawer(), drawer);
			if (  dm->pdv())
				legend = (AnnotationLegendDrawer *)NewDrawer::getDrawer("AnnotationValueLegendDrawer","ilwis38",&dp);
			else if (  dm->pdc()) {
				legend = (AnnotationLegendDrawer *)NewDrawer::getDrawer("AnnotationClassLegendDrawer","ilwis38",&dp);
			}
			if ( legend) {
				legend->prepare(&pp);
				ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
				if ( annotations) {
					annotations->addPostDrawer(400 + count, legend);
					++count;
					count = min(499,count);
				}
			}
		}
	}
	mpvGetView()->Invalidate();
}

//--------------------------------------------------------
LegendPosition::LegendPosition(CWnd *wPar, AnnotationLegendDrawer *dr) : 
	DisplayOptionsForm2(dr,wPar,TR("Position of Legend")), rg(0), fiColumns(0)
{
	orientation = dr->getOrientation() ? 1 : 0;
	CoordBounds cbZoom = drw->getRootDrawer()->getCoordBoundsZoom();
	CoordBounds cbMap = drw->getRootDrawer()->getMapCoordBounds();
	if (cbMap.MinX() > cbZoom.MinX())
		cbZoom.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cbZoom.MaxX())
		cbZoom.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cbZoom.MinY())
		cbZoom.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cbZoom.MaxY())
		cbZoom.MaxY() = cbMap.MaxY();
	CoordBounds cbBox = dr->getBox();
	cols = dr->noOfColumns();
	x = 100.0 * ( cbBox.MinX() - cbZoom.MinX() ) / cbZoom.width();
	y = 100.0 * ( cbBox.MinY() - cbZoom.MinY()) / cbZoom.height();
	sliderH = new FieldRealSliderEx(root,TR("X position"), &x,ValueRange(0,100),true);
	sliderV = new FieldRealSliderEx(root,TR("Y position"), &y,ValueRange(0,100),true);
	sliderV->Align(sliderH, AL_UNDER);
	sliderH->SetCallBack((NotifyProc)&LegendPosition::setPosition);
	sliderH->setContinuous(true);
	sliderV->SetCallBack((NotifyProc)&LegendPosition::setPosition);
	sliderV->setContinuous(true);
	if ( dr->getDomain()->pdv()) {
		rg = new RadioGroup(root,TR("Orientation"),&orientation,true);
		new RadioButton(rg,TR("Horizontal"));
		new RadioButton(rg,TR("Vertical"));
		rg->SetCallBack((NotifyProc)&LegendPosition::setOrientation);
	} else {
		fiColumns = new FieldInt(root,TR("Number of Columns"),&cols);
		fiColumns->SetCallBack((NotifyProc)&LegendPosition::setPosition);
	}
	create();
}

int LegendPosition::setOrientation(Event *ev) {
	if ( rg) {
		rg->StoreData();
		AnnotationLegendDrawer *ld = (AnnotationLegendDrawer *)drw;
		ld->setOrientation(orientation != 0);
		updateMapView();
	}

	return 1;
}

int LegendPosition::setPosition(Event *ev) {
	sliderV->StoreData();
	sliderH->StoreData();
	AnnotationLegendDrawer *ld = (AnnotationLegendDrawer *)drw;
	if ( fiColumns) {
		fiColumns->StoreData();
		if ( cols == 0 || cols > 5)
			cols = 1;
		ld->setNoOfColumns(cols);
		PreparationParameters pp(NewDrawer::ptRENDER);
		ld->prepare(&pp);

	}
	CoordBounds cbBox = ld->getBox();
	CoordBounds cbZoom = drw->getRootDrawer()->getCoordBoundsZoom();
	CoordBounds cbMap = drw->getRootDrawer()->getMapCoordBounds();
	if (cbMap.MinX() > cbZoom.MinX())
		cbZoom.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cbZoom.MaxX())
		cbZoom.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cbZoom.MinY())
		cbZoom.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cbZoom.MaxY())
		cbZoom.MaxY() = cbMap.MaxY();
	double newx = cbZoom.width() * x / 100.0 + cbZoom.MinX();
	double newy = cbZoom.height() * y / 100.0 + cbZoom.MinY();
	double w = cbBox.width();
	double h = cbBox.height();
	cbBox.MinX() = newx;
	cbBox.MinY() = newy;
	cbBox.MaxX() = cbBox.MinX() + w ;
	cbBox.MaxY() = cbBox.MinY() + h;
	ld->setBox(cbBox);
	updateMapView();

	return 1;
}

//-------------------------------------------------------------------------------
LegendAppearance::LegendAppearance(CWnd *wPar, AnnotationLegendDrawer *dr, ComplexDrawer *lyr) : 
DisplayOptionsForm(dr,wPar,TR("Appearance of Legend")) , layer(lyr), fview(0),fmin(0), fmax(0), fstep(0)
{
	useBgColor = dr->getUseBackBackground();
	bgColor = dr->getBackgroundColor();
	drawBoundary = dr->getDrawBorder();
	FieldGroup *fg1 = new FieldGroup(root);
	scale = dr->getScale();
	title = dr->getTitle();
	fscale = dr->getFontScale();
	cbColor = new CheckBox(fg1,TR("Background color"),&useBgColor);
	fc = new FieldColor(cbColor,"",&bgColor);
	fc->Align(cbColor, AL_AFTER);
	cbBoundary = new CheckBox(root,TR("Draw Boundary"),&drawBoundary);
	cbBoundary->Align(cbColor, AL_UNDER);
	fldScale =  new FieldReal(root,TR("Scale"),&scale,RangeReal(0.1,10.));
	fontScale =  new FieldReal(root,TR("Font Scale"),&fscale,RangeReal(0.1,10.));
	fldTitle = new FieldString(root,TR("Title"), &title);
	if ( dr->getDomain()->pdc()) {
		vector<FLVColumnInfo> cols;
		StaticText *st = new StaticText(root,TR("Active classes"));
		cols.push_back(FLVColumnInfo("Name", 150));
		fview = new FieldListView(root,cols,LVS_EX_GRIDLINES);
		fview->Align(st, AL_AFTER);
	} else {
		SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(layer); // case animation drawer
		if ( !mapDrawer)
			mapDrawer = dynamic_cast<SpatialDataDrawer *>(layer->getParentDrawer());
		dvrs = mapDrawer->getBaseMap()->dvrs();
		AnnotationValueLegendDrawer * vdr = (AnnotationValueLegendDrawer *)dr;
		RangeReal vrr = vdr->getRange();
		rstep = vdr->getStep();
		if ( rstep == rUNDEF) {
			vrr = roundRange(vrr.rLo(), vrr.rHi(), rstep);
		}
		rmin = vrr.rLo();
		rmax = vrr.rHi();

		fmin = new FieldReal(root,"Min",&rmin);
		fmax = new FieldReal(root,"Max", &rmax);
		fmax->Align(fmin, AL_UNDER);
		fstep = new FieldReal(root,"Step",&rstep);
	}
	create();
	if ( dr->getDomain()->pdc()) {
		for(int i=1; i <= dr->getDomain()->pdc()->iSize(); ++i) {
			long iRaw = dr->getDomain()->pdc()->iKey(i);
			if ( iRaw == iUNDEF)
				continue;
			//raws.push_back(iRaw);
			vector<String> values;
			String sV = dr->getDomain()->pdc()->sValueByRaw(iRaw);
			values.push_back(sV);
			fview->AddData(values);
		}
		vector<int> raws;
		((AnnotationClassLegendDrawer *)dr)->getActiveClasses(raws);
		fview->setSelectedRows(raws);

	}
}

void LegendAppearance::apply() {
	cbColor->StoreData();
	fc->StoreData();
	fldScale->StoreData();
	fontScale->StoreData();
	fldTitle->StoreData();
	vector<int> rows;
	if ( fview){
		fview->StoreData();
		fview->getSelectedRowNumbers(rows);
	} else {
		fstep->StoreData();
		fmax->StoreData();
		fmin->StoreData();
	}
	cbBoundary->StoreData();

	AnnotationLegendDrawer *andrw = (AnnotationLegendDrawer *)drw;
	andrw->setUseBackground(useBgColor);
	andrw->setBackgroundColor(bgColor);
	andrw->setDrawBorder(drawBoundary);
	andrw->setScale(scale);
	andrw->setTitle(title);
	andrw->setFontScale(fscale);
	if ( andrw->getDomain()->pdc()) {
		vector<int> rws;
		for(int i=0; i < rows.size(); ++i){
			long iRaw = andrw->getDomain()->pdc()->iKey(rows[i] + 1); // +1 because raws start at 1, not a 0;
			long ip = andrw->getDomain()->pdc()->iOrd(rows[i]);
			rws.push_back(iRaw);	
		}
		((AnnotationClassLegendDrawer *)andrw)->setActiveClasses(rws);
	} else {
		AnnotationValueLegendDrawer * vdr = (AnnotationValueLegendDrawer *)drw;
		vdr->setRange( RangeReal(rmin, rmax));
		vdr->setStep(rstep);
	}


	PreparationParameters pp(NewDrawer::ptRENDER);
	andrw->prepare(&pp);
	updateMapView();

}

