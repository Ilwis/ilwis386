#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\SetDrawer.h"
#include "Client\Ilwis.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Client\FormElements\FieldListView.h"
#include "CrossSectionGraph.h"
#include "DrawersUI\CrossSectionTool.h"
#include "headers\constant.h"

using namespace ILWIS;

DrawerTool *createCrossSectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new CrossSectionTool(zv, view, drw);
}


CrossSectionTool::CrossSectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("CrossSectionTool",zv, view, drw)
{
	active = false;
	graphForm = false;
	stay = true;
	graphForm = 0;
	working = false;
}

CrossSectionTool::~CrossSectionTool() {
}

void CrossSectionTool::clear() {
}

bool CrossSectionTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *layerDrawerTool = dynamic_cast<LayerDrawerTool *>(tool);
	SetDrawerTool *setDrawerTool = dynamic_cast<SetDrawerTool *>(tool);
	if (!layerDrawerTool)
		return false;
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(layerDrawerTool->getDrawer());
	if ( !ldrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM CrossSectionTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc)&CrossSectionTool::displayOptionAddList);
	item->setCheckAction(this,0, (DTSetCheckFunc)&CrossSectionTool::setcheckTool);
	htiNode = insertItem(TR("Cross section"),"CrossSection",item,0);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void CrossSectionTool::setcheckTool(void *w, HTREEITEM item) {
	working = *(bool *)w;
	if ( working) {
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
		if (!graphForm) {
			graphForm = new CrossSectionGraphFrom(tree, (LayerDrawer *)drawer);
		} else {
			graphForm->ShowWindow(SW_SHOW);
		}
		for(int i = 0; i < sources.size(); ++i) {
			graphForm->addSourceSet(sources[i]);
		}
	}
	else {
		tree->GetDocument()->mpvGetView()->noTool(getId());
		if ( graphForm)
			graphForm->ShowWindow(SW_HIDE);
	}
}

String CrossSectionTool::getMenuString() const {
	return TR("Cross Section");
}

void CrossSectionTool::displayOptionAddList() {
	new ChooseCrossSectionForm(tree, (LayerDrawer *)drawer, this);
}

bool CrossSectionTool::isUnique(const FileName& fn) {
	
	for(int i=0; i < sources.size(); ++i) {
		if ( sources.at(i)->fnObj == fn)
			return false;
	}
	return true;
}

void CrossSectionTool::addSource(const FileName& fn) {
	IlwisObject obj;
	if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
		MapList mpl(fn);
		obj = mpl;
	} else if (IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(fn);
		obj = oc;
	}
	if( obj.fValid()) {
		if ( isUnique(obj->fnObj)) {
			sources.push_back(obj);
			if (graphForm)
				graphForm->addSourceSet(obj);
		}
	}
}

void CrossSectionTool::OnLButtonUp(UINT nFlags, CPoint point) {
	if ( mpvGetView()->iActiveTool == ID_ZOOMIN) // during zooming, no message handling
		return;

	Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	if ( graphForm && working) {
		if ( sources.size() > 0 && working) {
			graphForm->ShowWindow(SW_SHOW);
			graphForm->setSelectCoord(c1);
		}
	}
}

//-------------------------------------------------------------------
ChooseCrossSectionForm::ChooseCrossSectionForm(CWnd *wPar, LayerDrawer *dr, CrossSectionTool *t) : 
	DisplayOptionsForm2(dr,wPar,TR("Add data source"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsOKHASCLOSETEXT),
	tool(t)
{
	fm = new FieldDataType(root,TR("Data source"),&name, ".mpl.ioc",true);
	new PushButton(root, TR("Add"),(NotifyProc) &ChooseCrossSectionForm::addSource);
	create();
}


int ChooseCrossSectionForm::addSource(Event *ev) {
	fm->StoreData();
	if ( name != "") {
		tool->addSource(FileName(name));
	}
	return 1;
}

//========================================================================
CrossSectionGraphFrom::CrossSectionGraphFrom(CWnd *wPar, LayerDrawer *dr) :
DisplayOptionsForm2(dr,wPar,TR("Cross section Graph"),fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON|fbsHIDEONCLOSE)
{
	vector<FLVColumnInfo> v;
	v.push_back(FLVColumnInfo("Source", 220));
	v.push_back(FLVColumnInfo("Index range", 40));
	v.push_back(FLVColumnInfo("Value range", 80));
	v.push_back(FLVColumnInfo("Selected index", 40));
	v.push_back(FLVColumnInfo("Value", 60));
	graph = new CrossSectionGraphEntry(root);
	graph->setListView(new FieldListView(root,v));
	create();
	ShowWindow(SW_HIDE);
}

void CrossSectionGraphFrom::addSourceSet(const IlwisObject& obj) {
	graph->addSourceSet(obj);
}

void CrossSectionGraphFrom::setSelectCoord(const Coord& crd) {
	graph->setCoord(crd);
}
