#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "WhiteSpaceTool.h"
#include "drawers\linedrawer.h"
#include "Drawers\GraticuleDrawer.h"
#include "DrawersUI\GraticuleTool.h"
#include "drawers\GridDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "GridTool.h"
#include "drawers\CanvasBackgroundDrawer.h"
#include "DrawersUI\GlobalAnnotationTool.h"

DrawerTool *createwhitespacetool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new WhiteSpaceTool(zv, view, drw);
}

WhiteSpaceTool::WhiteSpaceTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("WhiteSpaceTool",zv, view, drw)
{
}

WhiteSpaceTool::~WhiteSpaceTool() {
}

bool WhiteSpaceTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	bool ok = dynamic_cast<GlobalAnnotationTool *>(tool) != 0;
	if (!ok)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM WhiteSpaceTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&WhiteSpaceTool::displayOptionWhiteSpace);
	htiNode = insertItem("White space","whitespace", item);

	return htiNode;
}

void WhiteSpaceTool::displayOptionWhiteSpace() {
	new WhiteSpaceForm(tree, (ComplexDrawer *)drawer);
}

String WhiteSpaceTool::getMenuString() const {
	return TR("White Space");
}

//---------------------------------------------------
WhiteSpaceForm::WhiteSpaceForm(CWnd *wPar, ComplexDrawer *dr) : 
DisplayOptionsForm(dr,wPar,"White Space")
{
	ext = dr->getRootDrawer()->extension();
	ValueRange vrr(-99, 100, 1);
	frTop = new FieldReal(root, TR("Percentage at &Top"), &ext.top, vrr);
	frBot = new FieldReal(root, TR("Percentage at &Bottom"), &ext.bottom, vrr);
	frLeft = new FieldReal(root, TR("Percentage at &Left"), &ext.left  , vrr);
	frRight = new FieldReal(root, TR("Percentage at &Right"), &ext.right , vrr);

	create();
}



void  WhiteSpaceForm::apply() {
	
	frTop->StoreData();
	frBot->StoreData();
	frLeft->StoreData();
	frRight->StoreData();

	drw->getRootDrawer()->setExtension(ext);

	updateMapView();

}
