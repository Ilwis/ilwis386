#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\LineSetDrawer.h"
#include "Drawers\LineStyleTool.h"
#include "Drawers\PolygonSetDrawer.h"
#include "Drawers\PolygonSetTool.h"

//#include "drawers\linedrawer.h"



DrawerTool *createLineStyleTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new LineStyleTool(zv, view, drw);
}

LineStyleTool::LineStyleTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
DrawerTool("LineStyleTool", zv, view, drw)
{
}

LineStyleTool::~LineStyleTool() {
}

bool LineStyleTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	if ( dynamic_cast<LineStyleTool *>(tool) != 0) // prevent infinite adding the same tool to itself
		return false;

	return false;

}

HTREEITEM LineStyleTool::configure( HTREEITEM parentItem) {
	if ( isConfigured)
		return htiNode;

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&LineStyleTool::displayOptionSetLineStyle);
	String linestyle("Line style");
	htiNode = insertItem(linestyle,"LineStyle", item);
	DrawerTool::configure(htiNode);
	isConfigured = true;

	return htiNode;
}

void LineStyleTool::displayOptionSetLineStyle() {
	new LineStyleForm(tree, (ComplexDrawer *)drawer);
}

String LineStyleTool::getMenuString() const {
	return TR("Line properties");
}

//-----------------------------------------------
LineStyleForm::LineStyleForm(CWnd *par, ComplexDrawer *ldr) 
: DisplayOptionsForm(ldr, par, "Line Style")

{

	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(ldr);
	if ( animDrw) {
		lprops = (LineProperties *)(animDrw->getDrawer(0)->getProperties());
	} else
	{
		lprops = (LineProperties *)ldr->getProperties();
	}
	style = (LineDspType)LineDrawer::ilwisLineStyle(lprops->linestyle);
	fi = new FieldReal(root, "Line thickness", &lprops->thickness, ValueRange(1.0,100.0));
	flt = new FieldLineType(root, TR("Line Style"), &style);
	if ( !lprops->ignoreColor)
		fc = new FieldColor(root,TR("Line Color"), &lprops->drawColor);

	create();
}

void  LineStyleForm::apply() {
	fi->StoreData();
	flt->StoreData();
	if ( fc)
		fc->StoreData();
	PreparationParameters pp(NewDrawer::ptRENDER);
	AnimationDrawer *animDrw = dynamic_cast<AnimationDrawer *>(drw);
	if ( animDrw) {
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			NewDrawer *ndr = animDrw->getDrawer(i);
			LineProperties *oldprops = (LineProperties *)ndr->getProperties();
			oldprops->drawColor = lprops->drawColor;
			oldprops->linestyle = lprops->linestyle;
			oldprops->thickness = lprops->thickness;
		}
		animDrw->prepareChildDrawers(&pp);
	} else {
		if ( !lprops->ignoreColor)
			fc->StoreData();
		lprops->linestyle = LineDrawer::openGLLineStyle(style);
		drw->prepare(&pp);
	}
	updateMapView();
}

