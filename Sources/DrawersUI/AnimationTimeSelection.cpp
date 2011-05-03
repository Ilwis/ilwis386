#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldList.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\AnimationTimeSelection.h"
#include "DrawersUI\SetDrawerTool.h"
#include "DrawersUI\AnimationTool.h"

using namespace ILWIS;

DrawerTool *createAnimationTimeSelectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationTimeSelectionTool(zv, view, drw);
}

AnimationTimeSelectionTool::AnimationTimeSelectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnimationTimeSelectionTool",zv, view, drw)
{
}

AnimationTimeSelectionTool::~AnimationTimeSelectionTool() {
}

bool AnimationTimeSelectionTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	bool ok = dynamic_cast<AnimationTool *>(tool) != 0;
	if ( ok)
		parentTool = tool;
	return ok;
}

HTREEITEM AnimationTimeSelectionTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem * itemFrameSelect = new DisplayOptionTreeItem(tree, parentItem,drawer);
	itemFrameSelect->setDoubleCickAction(this, (DTDoubleClickActionFunc)&AnimationTimeSelectionTool::timeSelection);
	htiNode = insertItem(TR("Time Selection"),"TimeSelection",itemFrameSelect);

	DrawerTool::configure(htiNode);

	return htiNode;
}

void AnimationTimeSelectionTool::timeSelection() {
	new TimeSelection(tree,(AnimationDrawer *)drawer );
}

String AnimationTimeSelectionTool::getMenuString() const {
	return TR("Frame Selection");
}
//---------------------------------------------------
TimeSelection::TimeSelection(CWnd *par, AnimationDrawer *animdrw) 
: DisplayOptionsForm2(animdrw, par, "Time selection"), activeMaps(animdrw->getActiveMaps())
{
	FillData();	
	fl = new FieldLister(root,data, cols);
	fl->setReadOnly(true);
	fl->SetWidth(100 + (cols.size() - 2) * 32 );
	fl->SetHeight(120 + min(160, data.size() * 16));
  create();
}

void TimeSelection::FillData() {
	IlwisObject *source = (IlwisObject *)((AnimationDrawer *)drw)->getDataSource();
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
		IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		Table tbl;
		cols.push_back("Name");
		cols.push_back("Index");
		if ( mpl->fTblAtt()) {
			tbl = mpl->tblAtt();
			for(int c=0; c < tbl->iCols(); ++c) {
				if ( tbl->col(c)->dm()->pdtime()) {
					cols.push_back(tbl->col(c)->sName());
				}
			}
		}
		for(int i = 0; i < mpl->iSize(); ++i) {
			int index = mpl->iLower() + i;
			String d("%S;%d", mpl[index]->sName(), index+1);
			if ( tbl.fValid()) {
				for(int c=0; c < tbl->iCols(); ++c) {
					if ( tbl->col(c)->dm()->pdtime()) {
						d += ";" + tbl->col(c)->sValue(i+1);
					}
				}
			}
			data.push_back(d);
		}
	}
}
int  TimeSelection::exec() {
	//vector<int> indexes;
	activeMaps.clear();
	fl->getSelectedIndexes(activeMaps);
	
	return 1;
}

