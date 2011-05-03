#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\LineLayerDrawer.h"
#include "DrawersUI\LineStyleTool.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "DrawersUI\PolygonSetTool.h"

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

int LineStyleTool::openGLLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case ldtDot:
			return 0xAAAA;
		case ldtDash:
			return 0xF0F0;
		case ldtDashDot:
			return 0x6B5A;
		case ldtDashDotDot:
			return 0x56B5;
		default:
			return 0xFFFF;
	}
	return 0xFFFF;
}

int LineStyleTool::ilwisLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case 0xAAAA:
			return ldtDot;
		case 0xF0F0:
			return ldtDash;
		case 0x6B5A:
			return ldtDashDot;
		case 0x56B5:
			return ldtDashDotDot;
		default:
			return ldtSingle;
	}
	return 0xFFFF;
}

//-----------------------------------------------
LineStyleForm::LineStyleForm(CWnd *par, ComplexDrawer *ldr) 
: DisplayOptionsForm(ldr, par, "Line Style"), fc(0)

{

	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(ldr);
	if ( setDrw) {
		SetDrawer *drw = (SetDrawer *)(ldr);
		for(int i=0; i < drw->getDrawerCount(); ++i) {
			int filter = (int)(ComplexDrawer::dtPOLYGONLAYER | ComplexDrawer::dtSEGMENTLAYER);
			NewDrawer *drwFeature = drw->getDrawer(i, filter);
			if ( drwFeature) {
				lprops = (LineProperties *)(drwFeature->getProperties());
				break;
			}
		}

	} else
	{
		lprops = (LineProperties *)ldr->getProperties();
	}
	style = (LineDspType)LineStyleTool::ilwisLineStyle(lprops->linestyle);
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
	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
	if ( setDrw) {
		for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
			NewDrawer *ndr = setDrw->getDrawer(i, ComplexDrawer::dtPOLYGONLAYER | ComplexDrawer::dtSEGMENTLAYER);
			if ( !ndr)
				continue;
			LineProperties *oldprops = (LineProperties *)ndr->getProperties();
			oldprops->drawColor = lprops->drawColor;
			oldprops->linestyle = lprops->linestyle;
			oldprops->thickness = lprops->thickness;
			ndr->prepare(&pp);
		}
	} else {
		if ( !lprops->ignoreColor)
			fc->StoreData();
		lprops->linestyle = LineStyleTool::openGLLineStyle(style);
		drw->prepare(&pp);
	}
	updateMapView();
}

