#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\AttributeTool.h"
#include "Drawers\SetDrawerTool.h"


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
	SetDrawerTool *sdrwt = dynamic_cast<SetDrawerTool *>(tool);
	if (!sdrwt)
		return false;
	SetDrawer *sdrw = dynamic_cast<SetDrawer *>(sdrwt->getDrawer());
	Table attTable = ((AbstractMapDrawer *)sdrw->getParentDrawer())->getAtttributeTable();
	parentTool = tool;
	return attTable.fValid();
}

HTREEITEM AttributeTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&AttributeTool::displayOptionAttColumn);
	item->setCheckAction(this,0, (DTSetCheckFunc)&AttributeTool::setcheckAttributeTable);
	SetDrawer *sdr = (SetDrawer *)drawer;
	Column attColumn = sdr->getAtttributeColumn();
	htiNode = insertItem("Attribute table",".tbt",item,sdr->useAttributeColumn());
	if ( sdr->useAttributeColumn() && attColumn.fValid())
		insertItem(htiNode,String("Column : %S",attColumn->sName()),"column");
	return htiNode;
}

void AttributeTool::setcheckAttributeTable(void *w) {
	bool yesno = *(bool *)w;
	SetDrawer *sdr = (SetDrawer *)drawer;
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
	SetDrawer *sdr = (SetDrawer *)drawer;
	Column attColumn = sdr->getAtttributeColumn();
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)sdr->getParentDrawer();
	Domain dm = attColumn.fValid() ? attColumn->dm() : mapDrawer->getBaseMap()->dm();
	sdr->setRepresentation(dm->rpr());
	DrawerTool *stretchTool = getParentTool()->getTool("StretchTool");

	if ((dm->pdv() || dm->pdi()) && stretchTool == 0)  {
		stretchTool = DrawerTool::createTool("StretchTool",
			drawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView(),tree, drawer);
		getParentTool()->addTool(stretchTool);
		stretchTool->setParentTool(getParentTool());
	}
	bool makeActive = (dm->pdv() || dm->pdi()) && stretchTool;
	if ( stretchTool)
		stretchTool->setActiveMode(makeActive);

	DrawerTool *legendTool = getTool("LegendTool");
	if ( legendTool)
		legendTool->update();
}


void AttributeTool::displayOptionAttColumn() {
	SetDrawer *sdr = (SetDrawer *)drawer;
	if ( sdr->useAttributeColumn())
		new ChooseAttributeColumnForm(tree, (SetDrawer *)drawer, this);
}

String AttributeTool::getMenuString() const {
	return TR("Attribute selection");
}

//-------------------------------------
ChooseAttributeColumnForm::ChooseAttributeColumnForm(CWnd *wPar, SetDrawer *dr, DrawerTool *t) : 
	DisplayOptionsForm(dr,wPar,"Choose attribute column"),
	tool(t)
{
	attTable = ((AbstractMapDrawer *)dr->getParentDrawer())->getBaseMap()->tblAtt();
	attColumn = dr->getAtttributeColumn().fValid() ? dr->getAtttributeColumn()->sName() : "";
	fc = new FieldColumn(root, "Column", attTable, &attColumn,dmVALUE | dmIMAGE | dmBOOL | dmCLASS | dmIDENT | dmUNIQUEID);
	create();
}


void  ChooseAttributeColumnForm::apply() {
	fc->StoreData();
	if ( attColumn != "") {
		Column col = attTable->col(attColumn);
		((SetDrawer *)drw)->setUseAttributeColumn(true);
		((SetDrawer *)drw)->setAttributeColumn(col);
		((SetDrawer *)drw)->setRepresentation(col->dm()->rpr());
		((SetDrawer *)drw)->getDrawingColor()->setDataColumn(col);
		
		tool->update();
	
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		drw->prepareChildDrawers(&pp);
		updateMapView();
	}

}


