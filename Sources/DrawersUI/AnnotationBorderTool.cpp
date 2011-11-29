#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "drawers\linedrawer.h"
#include "drawers\AnnotationDrawers.h"
#include "Drawers\LayerDrawer.h"
#include "AnnotationBorderTool.h"
#include "DrawersUI\SetDrawerTool.h"


DrawerTool *createAnnotationBorderTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnnotationBorderTool(zv, view, drw);
}

AnnotationBorderTool::AnnotationBorderTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnnotationBorderTool",zv, view, drw),
	border(0)
{
}

AnnotationBorderTool::~AnnotationBorderTool() {
}

bool AnnotationBorderTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	return false;
}

HTREEITEM AnnotationBorderTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&AnnotationBorderTool::displayOptionAnnotationBorder);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&AnnotationBorderTool::makeActive);
	htiNode = insertItem(TR("Border"),"Border", item, border && border->isActive());

	return htiNode;
}

void AnnotationBorderTool::makeActive(void *v, HTREEITEM ) {
	bool act = *(bool *)v;
	if ( border) {
		border->setActive(act);
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
		border->prepare(&pp);
	}
	else {
		if ( act) {
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			ILWIS::DrawerParameters dp(drawer->getRootDrawer(), drawer);
			border = (AnnotationBorderDrawer *)NewDrawer::getDrawer("AnnotationBorderDrawer","ilwis38",&dp);
			if ( border) {
				border->prepare(&pp);
				ComplexDrawer *annotations = (ComplexDrawer *)(drawer->getRootDrawer()->getDrawer("AnnotationDrawers"));
				if ( annotations)
					annotations->addPostDrawer(380, border);
			}
		}
	}
	mpvGetView()->Invalidate();
}

void AnnotationBorderTool::displayOptionAnnotationBorder() {
	new AnnotationBorderForm(tree, border);
}

String AnnotationBorderTool::getMenuString() const {
	return TR("Border");
}

//---------------------------------------------------
AnnotationBorderForm::AnnotationBorderForm(CWnd *wPar, AnnotationBorderDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"Borders")
{
	step = dr->getStep();
	neatline = dr->hasNeatLine();
	new FieldInt(root,TR("Step"),&step);
	new CheckBox(root, TR("Neat line"),&neatline);
	create();
}

void  AnnotationBorderForm::apply() {
	if ( initial) return;
	root->StoreData();
	AnnotationBorderDrawer *borderDrw = (AnnotationBorderDrawer *)drw;
	borderDrw->setHasNeatLine(neatline);
	borderDrw->setStep(step);
	
	PreparationParameters pp(NewDrawer::ptRENDER | NewDrawer::ptGEOMETRY);
	borderDrw->prepare(&pp);

	updateMapView();

}
