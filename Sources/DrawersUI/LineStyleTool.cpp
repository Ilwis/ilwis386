#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Drawers\LineDrawer.h"
#include "drawers\polygondrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Editors\Utils\line.h"
#include "Drawers\LineLayerDrawer.h"
#include "DrawersUI\LineStyleTool.h"
#include "Drawers\PolygonLayerDrawer.h"
#include "DrawersUI\PolygonSetTool.h"
#include "DrawersUI\SetDrawerTool.h"

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
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	if ( sdrwt) {
		SetDrawer *sdrw = (SetDrawer *)sdrwt->getDrawer();
		LineLayerDrawer *ldrw = dynamic_cast<LineLayerDrawer *>(sdrw->getDrawer(0));
		if (!ldrw)
			return false;
	}
	parentTool = tool;

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
	ComplexDrawer::DrawerType dt = ComplexDrawer::dtMAIN;
	if ( parentTool) {
		if (parentTool->getType() == "LineSetTool") {
			dt = ComplexDrawer::dtSEGMENTLAYER;
		}
		if (parentTool->getType() == "PolygonSetTool") {
			dt = ComplexDrawer::dtPOLYGONLAYER;
		}
	}
	if ( drawer->isSimple()) {
		new LineStyleForm(tree, (LineDrawer *)drawer);
	} else
		new LineStyleForm(tree, (ComplexDrawer *)drawer, dt);
}

String LineStyleTool::getMenuString() const {
	return TR("Line properties");
}



//-----------------------------------------------
LineStyleForm::LineStyleForm(CWnd *par, LineDrawer *ldr) 
: DisplayOptionsForm((ComplexDrawer *)ldr->getParentDrawer(), par, "Line Style"), fc(0), drawerType(ComplexDrawer::dtDONTCARE), line(ldr)

{

	lprops = (LineProperties *)ldr->getProperties();
	style = (ILWIS::NewDrawer::LineDspType)LineDrawer::ilwisLineStyle(lprops->linestyle);
	fi = new FieldReal(root, "Line thickness", &lprops->thickness, ValueRange(1.0,100.0));
	flt = new FieldLineType(root, TR("Line Style"), &style);
	if ( !lprops->ignoreColor)
		fc = new FieldColor(root,TR("Line Color"), &lprops->drawColor);

	create();
}

LineStyleForm::LineStyleForm(CWnd *par, ComplexDrawer *ldr,ComplexDrawer::DrawerType dt) 
: DisplayOptionsForm(ldr, par, "Line Style"), fc(0), drawerType(dt), line(0)

{

	SetDrawer *setDrw = dynamic_cast<SetDrawer *>(ldr);
	if ( setDrw) {
		SetDrawer *drw = (SetDrawer *)(ldr);
		for(int i=0; i < drw->getDrawerCount(); ++i) {
			NewDrawer *drwFeature = drw->getDrawer(i, (int)drawerType);
			if ( drwFeature) {
				lprops = (LineProperties *)(drwFeature->getProperties());
				break;
			}
		}

	} else
	{
		lprops = (LineProperties *)ldr->getProperties();
	}
	style = (ILWIS::NewDrawer::LineDspType)LineDrawer::ilwisLineStyle(lprops->linestyle);
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
	if ( line != 0) {
		SetDrawer *setDrw = dynamic_cast<SetDrawer *>(drw);
		if ( setDrw) {
			for(int i = 0; i < setDrw->getDrawerCount(); ++i) {
				ComplexDrawer *ndr = (ComplexDrawer *)setDrw->getDrawer(i, (int)drawerType);
				if ( !ndr)
					continue;
				LineProperties *oldprops = (LineProperties *)ndr->getProperties();
				oldprops->drawColor = lprops->drawColor;
				oldprops->linestyle = LineDrawer::openGLLineStyle(style);
				oldprops->thickness = lprops->thickness;
				ndr->prepareChildDrawers(&pp);
			}
		} else {
			if ( !lprops->ignoreColor)
				fc->StoreData();
			lprops->linestyle = LineDrawer::openGLLineStyle(style);
			drw->prepareChildDrawers(&pp);
		}
	} else {
		lprops->linestyle = LineDrawer::openGLLineStyle(style);
		drw->prepare(&pp);
	}
	updateMapView();
}

