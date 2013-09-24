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
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "CoordSystemTool.h"
#include "LayerDrawerTool.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "DrawersUI\GeometryTool.h"


DrawerTool *createCoordSystemTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CoordSystemTool(zv, view, drw);
}

CoordSystemTool::CoordSystemTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("CoordSystemTool",zv, view, drw)
{
}

CoordSystemTool::~CoordSystemTool() {
}

bool CoordSystemTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	return dynamic_cast<GeometryTool *>(tool) != 0;
}

HTREEITEM CoordSystemTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&CoordSystemTool::displayOptionCoordSystem);
	htiNode = insertItem(TR("Coordinate System"),".csy", item);

	return htiNode;
}

void CoordSystemTool::displayOptionCoordSystem() {
	new CoordSystemForm(tree, (ComplexDrawer *)drawer);
}

String CoordSystemTool::getMenuString() const {
	return TR("CoordSystem");
}

//---------------------------------------------------
CoordSystemForm::CoordSystemForm(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"CoordinateSystem")
{
	name = dr->getRootDrawer()->getCoordinateSystem()->fnObj.sFullPath();
	fldCsy = new FieldCoordSystem(root,TR("Coordinate system"),&name);
	create();
}

int CoordSystemForm::setCoordSystem(Event *ev) {
	apply();
	return 1;
}

FormEntry *CoordSystemForm::CheckData() {
	fldCsy->StoreData();
	FileName fn(name);
	if ( !fn.fExist())
		return fldCsy;
	CoordSystem csy(fn);
	bool fConvertPossible = csy->fConvertFrom(drw->getRootDrawer()->getCoordinateSystem());
	if ( !fConvertPossible)
		return fldCsy;
	return 0;
}
void  CoordSystemForm::apply() {
	fldCsy->StoreData();
	FileName fn(name);
	CoordSystem csy(fn);
	bool fConvertPossible = csy->fConvertFrom(drw->getRootDrawer()->getCoordinateSystem());
	if ( fConvertPossible) {
		drw->getRootDrawer()->setCoordinateSystem(csy,true);
		view->ClearTree();
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER | NewDrawer::ptNEWCSY);
		drw->getRootDrawer()->prepare(&pp);
		view->OnUpdate(NULL, 2, NULL); // for now limit the change to the LayerTreeView; if it appears that "drawer" is remembered in more places, call UpdateAllViews(0, 2)
		view->GetDocument()->mpvGetView()->ResetStatusBar(); // let the statusbar grow from scratch, to show whatever is needed (eliminate the rowcol)
	}

	updateMapView();

}
