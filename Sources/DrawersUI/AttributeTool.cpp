#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "DrawersUI\SetDrawerTool.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\AttributeTool.h"
#include "DrawersUI\LayerDrawerTool.h"


DrawerTool *createAttributeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AttributeTool(zv, view, drw);
}

AttributeTool::AttributeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
DrawerTool("AttributeTool",zv, view, drw)
{
}

AttributeTool::~AttributeTool() {
}

bool AttributeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool)
		return false;
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(layerDrawerTool->getDrawer());
	if ( !ldrw)
		return false;

	DrawerTool *tl = tool->getTool("AttributeTool");
	if (tl) // there is already such a tool
		return false;
	Table attTable = ((SpatialDataDrawer *)ldrw->getParentDrawer())->getAtttributeTable();
	parentTool = tool;
	return attTable.fValid();
}

HTREEITEM AttributeTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&AttributeTool::displayOptionAttColumn);
	item->setCheckAction(this,0, (DTSetCheckFunc)&AttributeTool::setcheckAttributeTable);
	LayerDrawer *sdr = (LayerDrawer *)drawer;
	Column attColumn = sdr->getAtttributeColumn();
	htiNode = insertItem(TR("Attribute table"),".tbt",item,sdr->useAttributeColumn());
	if ( sdr->useAttributeColumn() && attColumn.fValid())
		insertItem(htiNode,String("Column : %S",attColumn->sName()),"column");
	return htiNode;
}

void AttributeTool::setcheckAttributeTable(void *w, HTREEITEM ) {
	bool yesno = *(bool *)w;
	LayerDrawer *sdr = (LayerDrawer *)drawer;
	sdr->setUseAttributeColumn(yesno);
	if ( !yesno){
		sdr->getDrawingColor()->setDataColumn(Column());
		sdr->setAttributeColumn(Column());
	}

	update();

	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	sdr->prepareChildDrawers(&pp);
	sdr->getRootDrawer()->getDrawerContext()->doDraw();
}

void AttributeTool::update() {
	LayerDrawer *sdr = (LayerDrawer *)drawer;
	Column attColumn = sdr->getAtttributeColumn();
	if ( attColumn.fValid()) {
		DrawerTool *parentTool = getParentTool();
		int n = 0;
		DrawerTool *current = parentTool->getTool(n);
		while( current) {
			if ( current->getId() != getId())
				parentTool->removeTool(current);
			else
				++n;
			current = parentTool->getTool(n);
		}
		tree->DeleteAllItems(getParentTool()->getTreeItem(),true);
		addChildTools(parentTool);
		for(int i =0; i < parentTool->getToolCount(); ++i) {
			DrawerTool *current = parentTool->getTool(i);
			if ( current && current->isActive())
				current->configure(getParentTool()->getTreeItem());
		}

	}
}


void AttributeTool::displayOptionAttColumn() {
	LayerDrawer *sdr = (LayerDrawer *)drawer;
	if ( sdr->useAttributeColumn())
		new ChooseAttributeColumnForm(tree, (LayerDrawer *)drawer, this);
}

String AttributeTool::getMenuString() const {
	return TR("Attribute selection");
}

//-------------------------------------
ChooseAttributeColumnForm::ChooseAttributeColumnForm(CWnd *wPar, LayerDrawer *dr, DrawerTool *t) : 
	DisplayOptionsForm(dr,wPar,"Choose attribute column"),
	tool(t)
{
	attTable = ((SpatialDataDrawer *)dr->getParentDrawer())->getBaseMap()->tblAtt();
	attColumn = dr->getAtttributeColumn().fValid() ? dr->getAtttributeColumn()->sName() : "";
	fc = new FieldColumn(root, "Column", attTable, &attColumn,dmVALUE | dmIMAGE | dmBOOL | dmCLASS | dmIDENT | dmUNIQUEID);
	create();
}


void  ChooseAttributeColumnForm::apply() {
	fc->StoreData();
	if ( attColumn != "") {
		Column col = attTable->col(attColumn);
		((LayerDrawer *)drw)->setUseAttributeColumn(true);
		((LayerDrawer *)drw)->setAttributeColumn(col);
		((LayerDrawer *)drw)->setRepresentation(col->dm()->rpr());
		((LayerDrawer *)drw)->getDrawingColor()->setDataColumn(col);
		
		tool->update();
	
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		drw->prepareChildDrawers(&pp);
		updateMapView();
	}

}


