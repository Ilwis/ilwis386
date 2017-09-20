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
#include "Drawers\FeatureLayerDrawer.h"
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
#include "DrawersUI\StretchTool.h"
#include "DrawersUI\AnnotationDrawerTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Drawers\SetDrawer.h"


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
	if (!layerDrawerTool && !setDrawerTool)
		return false;
	LayerDrawer *ldrw = ((ComplexDrawer*)drawer)->isSet() ? dynamic_cast<LayerDrawer*> (((SetDrawer*)drawer)->getDrawer(0)) : dynamic_cast<LayerDrawer*>(drawer);
	if ( !ldrw)
		return false;

	DrawerTool *tl = tool->getTool("AttributeTool");
	if (tl) // there is already such a tool
		return false;
	Table attTable = ((SpatialDataDrawer *)ldrw->getParentDrawer())->getAtttributeTable(); // deprecate getAtttributeTable()
	parentTool = tool;
	return attTable.fValid();
}

HTREEITEM AttributeTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	LayerDrawer *sdr = ((ComplexDrawer*)drawer)->isSet() ? (LayerDrawer*)((ComplexDrawer*)drawer)->getDrawer(0) : (LayerDrawer *)drawer;
	Column attColumn = sdr->getAtttributeColumn();
	htiNode = insertItem(TR("Display Attribute"),".tbt",item);
	BaseMap bmp (((ComplexDrawer*)drawer)->isSet() ? ((SpatialDataDrawer*)drawer)->getBaseMap()->fnObj : ((SpatialDataDrawer *)drawer->getParentDrawer())->getBaseMap()->fnObj);
	Table attTable = bmp->tblAtt();
	if ( attTable.fValid()) {
		attrCheck = new SetChecks(tree,this,(DTSetCheckFunc)&AttributeTool::setcheckattr);
		DisplayOptionRadioButtonItem *ritem = new DisplayOptionRadioButtonItem(bmp->sName(),tree, htiNode,drawer);
		ritem->setCheckAction(this,attrCheck, (DTSetCheckFunc)&AttributeTool::setcheckattr);
		ritem->setState(true);
		insertItem(bmp->sName(),bmp->fnObj.sExt,ritem);

		for(int i=0; i < attTable->iCols(); ++i) {
			Column col = attTable->col(i);
			if ( col->dm()->pdsrt() || col->dm()->pdv()) {
				ritem = new DisplayOptionRadioButtonItem(col->sName(),tree, htiNode,drawer);
				ritem->setCheckAction(this,attrCheck, (DTSetCheckFunc)&AttributeTool::setcheckattr);
				if ( col->dm()->pdv())
					insertItem(col->sName(),"integer",ritem);
				if ( col->dm()->pdsrt())
					insertItem(col->sName(),"Set",ritem);
			}
		}
	}
	lasthit = 0;
	return htiNode;
}

void AttributeTool::setcheckattr(void *value, HTREEITEM item) {
	if ( value == 0)
		return;
	int colNr = attrCheck->getState();
	HTREEITEM hit = attrCheck->getHTI(colNr);
	CString txt= tree->GetTreeCtrl().GetItemText(hit);
	DisplayOptionButtonItem *data = (DisplayOptionButtonItem  *)(tree->GetTreeCtrl().GetItemData(hit));
	data->setState(true);
	DrawerTool *parentTool = getParentTool();
	DrawerTool *colorTool = parentTool->findChildToolByType("ColorTool");
	DrawerTool *stretchTool = parentTool->findChildToolByType("StretchTool");
	DrawerTool *annotationDrawerTool = parentTool->findChildToolByType("AnnotationDrawerTool");
	PreparationParameters pp(NewDrawer::ptRENDER, 0,10);
	if ( hit != lasthit)
		pp.filteredRaws.push_back(iUNDEF); // enable all when switching
	ComplexDrawer * cdrw = (ComplexDrawer*)drawer;
	Column attColumn;
	if (cdrw->isSet()) {
		for(int i = 0; i < cdrw->getDrawerCount(); ++i) {
			FeatureLayerDrawer *featureLayerDrawer = (FeatureLayerDrawer *) (cdrw->getDrawer(i));
			BaseMap bmp (((BaseMap*)featureLayerDrawer->getDataSource())->ptr()->fnObj);
			Table attTable = bmp->tblAtt();
			attColumn = attTable->col(String(txt));
			if ( attColumn.fValid()) {
				featureLayerDrawer->setAttributeColumn(attColumn);
				featureLayerDrawer->setUseAttributeColumn(true);
				featureLayerDrawer->setRepresentation(attColumn->dm()->rpr());
				featureLayerDrawer->getDrawingColor()->setDataColumn(attColumn);
			} else {
				featureLayerDrawer->setUseAttributeColumn(false);
				featureLayerDrawer->getDrawingColor()->setDataColumn(Column());
				featureLayerDrawer->setRepresentation(bmp->dm()->rpr());
			}
			if (featureLayerDrawer->getRepresentation().fValid()) {
				featureLayerDrawer->setUseRpr(true);
				featureLayerDrawer->setDrawMethod(NewDrawer::drmRPR);
			} else {
				featureLayerDrawer->setUseRpr(false);
				featureLayerDrawer->setDrawMethod(NewDrawer::drmMULTIPLE);
			}
			featureLayerDrawer->prepareChildDrawers(&pp);
		}
		cdrw->prepare(&pp);
	} else {
		LayerDrawer *featureLayerDrawer = (LayerDrawer *)drawer;
		BaseMap bmp (((BaseMap*)featureLayerDrawer->getDataSource())->ptr()->fnObj);
		Table attTable = bmp->tblAtt();
		attColumn = attTable->col(String(txt));
		if ( attColumn.fValid() && !attColumn->dm()->pds()) {
			featureLayerDrawer->setAttributeColumn(attColumn);
			featureLayerDrawer->setUseAttributeColumn(true);
			featureLayerDrawer->setRepresentation(attColumn->dm()->rpr());
			featureLayerDrawer->getDrawingColor()->setDataColumn(attColumn);
		} else {
			featureLayerDrawer->setUseAttributeColumn(false);
			featureLayerDrawer->getDrawingColor()->setDataColumn(Column());
			featureLayerDrawer->setRepresentation(bmp->dm()->rpr());
		}
		if (featureLayerDrawer->getRepresentation().fValid()) {
			featureLayerDrawer->setUseRpr(true);
			featureLayerDrawer->setDrawMethod(NewDrawer::drmRPR);
		} else {
			featureLayerDrawer->setUseRpr(false);
			featureLayerDrawer->setDrawMethod(NewDrawer::drmMULTIPLE);
		}
		featureLayerDrawer->prepareChildDrawers(&pp);
	}

	if ( colorTool) {
		colorTool->removeTool(0); // all
		colorTool->clear();
		tree->DeleteAllItems(colorTool->getTreeItem(),true);
		colorTool->addChildTools(colorTool);
		DrawerTool *childTool = 0;
		int count = 0;
		while((childTool = colorTool->getTool(count++))) {
			childTool->configure(colorTool->getTreeItem());
		}
	}
	if ( hit != lasthit) {
		// refresh stretchTool
		if (stretchTool) {
			stretchTool->removeTool(0); // all
			stretchTool->clear();
			if (lasthit)
				tree->DeleteAllItems(lasthit,true);
		}
		if ( attColumn.fValid() && attColumn->dm()->pdv() && !attColumn->dm()->pdbool()) {
			if (!stretchTool)
				stretchTool = new StretchTool(mpvGetView(),tree,drawer);
			parentTool->addTool(stretchTool);
			stretchTool->configure(item);
		} else if (stretchTool)
			parentTool->removeTool(stretchTool); // will delete stretchTool
		// refresh annotationDrawerTool
		if (annotationDrawerTool) {
			annotationDrawerTool->clear(); // removes old legend drawer
			if (annotationDrawerTool->isActive()) {
				annotationDrawerTool->setActiveMode(false); // removes the item from the tree (it is re-added later on, if the selected attribute is class or value)
				annotationDrawerTool->setActive(true); // remembers the active state
			}
		}
		LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(drawer);
		SetDrawer *sdr = dynamic_cast<SetDrawer *>(drawer);
		Domain dm;
		if ( ldr) {
			SpatialDataDrawer *spdr = (SpatialDataDrawer *)(drawer->getParentDrawer());
			dm = ldr->useAttributeColumn() ? ldr->getAtttributeColumn()->dm() : spdr->getBaseMap()->dm();
		} else if ( sdr) {
			dm = sdr->useAttributeTable() ? sdr->getAtttributeColumn()->dm() :   sdr->getBaseMap()->dm();
		}
		if ((dm->pdv() && !dm->pdbool()) || dm->pdc()) {
			if (!annotationDrawerTool)
				annotationDrawerTool = new AnnotationDrawerTool(mpvGetView(),tree,drawer);
			parentTool->addTool(annotationDrawerTool);
			if (annotationDrawerTool->isActive()) // in the case of annotationDrawerTool isActive() and isActiveMode() would be the same (isActiveMode() is a function that does not exist, it would be symmetric to setActiveMode(bool)).
				annotationDrawerTool->setActiveMode(true); // add items to the tree, and force refresh of the legend drawer
		} else if (annotationDrawerTool) {
			annotationDrawerTool->setActive(false);
			parentTool->removeTool(annotationDrawerTool); // will delete annotationDrawerTool
		}
		lasthit = hit;
	}

	tree->collectStructure();
	tree->Invalidate();

	drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void AttributeTool::update() {
	LayerDrawer *sdr = ((ComplexDrawer*)drawer)->isSet() ? (LayerDrawer*)((ComplexDrawer*)drawer)->getDrawer(0) : (LayerDrawer *)drawer;
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

String AttributeTool::getMenuString() const {
	return TR("Display attribute");
}


