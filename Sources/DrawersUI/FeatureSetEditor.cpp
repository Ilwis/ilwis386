#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Client\Ilwis.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\PixelInfoDoc.h"
#include "Client\Mapwindow\PixelInfoBar.h"
#include "Client\Mapwindow\PixelInfoView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "FeatureSetEditor.h"

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID));

BEGIN_MESSAGE_MAP(FeatureSetEditor, BaseMapEditor)
	//{{AFX_MSG_MAP(LineSetEditor)
	// ON_COMMAND(ID_COPY, OnCopy)
	// ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateCopy)
	//ON_COMMAND(ID_PASTE, OnPaste)
	// ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	//ON_COMMAND(ID_EDIT, OnEdit)
	// ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	//ON_COMMAND(ID_CLEAR, OnClear)
	// ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateClear)
	//ON_COMMAND(ID_CUT, OnCut)
	// ON_UPDATE_COMMAND_UI(ID_CUT, OnUpdateCopy)
	//ON_COMMAND(ID_SELECTMODE, OnSelectMode)
//	ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
	//ON_COMMAND(ID_MOVEMODE, OnMoveMode)
	//ON_UPDATE_COMMAND_UI(ID_MOVEMODE, OnUpdateMode)
	//ON_COMMAND(ID_INSERTMODE, OnInsertMode)
	//ON_UPDATE_COMMAND_UI(ID_INSERTMODE, OnUpdateMode)
	//ON_COMMAND(ID_SPLITMODE, OnSplitMode)
	//ON_UPDATE_COMMAND_UI(ID_SPLITMODE, OnUpdateMode)
	//ON_COMMAND(ID_SEGCHECKSELF, OnCheckSelf)
	//ON_COMMAND(ID_UNDOALL, OnUndoAllChanges)
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//ON_UPDATE_COMMAND_UI(ID_UNDOALL, OnUpdateFileSave)
	//ON_COMMAND(ID_SEGPACK, OnSegPack)
	//ON_UPDATE_COMMAND_UI(ID_SEGPACK, OnUpdateSegPack)
	//ON_COMMAND(ID_SEGSETBOUNDS, OnSetBoundaries)
	//ON_COMMAND(ID_SELALL, OnSelectAll)*/
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

using namespace ILWIS;

FeatureSetEditor::FeatureSetEditor(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	BaseMapEditor(tp,zv, view, drw),
	currentCoordIndex(iUNDEF),
	currentGuid(sUNDEF),
	mode(BaseMapEditor::mUNKNOWN),
	setdrawer((ComplexDrawer *)drw),
	bmapptr(0)
{ 
	AbstractMapDrawer *amdrw = dynamic_cast<AbstractMapDrawer *>(drw->getParentDrawer());
	if ( amdrw) {
		bmapptr = amdrw->getBaseMap();
		mdoc = (MapCompositionDoc *)zv->GetDocument();
	}
	active = false;
	editModeItems = new SetChecks(tree,this,(DTSetCheckFunc)&FeatureSetEditor::setcheckEditMode);
	stay = true;
}

FeatureSetEditor::~FeatureSetEditor(){
	if ( bmapptr) {
		if (  bmapptr->fChanged )
			bmapptr->Store();
		MapCompositionDoc *mdoc = tree->GetDocument();
		mdoc->pixInfoDoc->setAssociatedDrawerTool(0,bmapptr->fnObj.sRelative()); 
		mdoc->mpvGetView()->noTool(getId());
	}
	clear();
}

HTREEITEM FeatureSetEditor::configure( HTREEITEM parentItem) {
	DisplayOptionRadioButtonItem *item = new DisplayOptionRadioButtonItem(TR("Select"), tree,parentItem,drawer);
	item->setState(true);
	item->setCheckAction(this,editModeItems,0); 
	insertItem("Select","Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Insert"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0);
	insertItem("Insert","Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Move"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0); 
	insertItem("Move","Bitmap", item);
	DrawerTool::configure(htiNode);


	return parentItem;
}

void FeatureSetEditor::setcheckEditMode(void *value) {
	if ( value == 0)
		return;
	HTREEITEM hItem = *((HTREEITEM *)value);

	DisplayOptionRadioButtonItem *item = dynamic_cast<DisplayOptionRadioButtonItem * >((LayerTreeItem *)(tree->GetTreeCtrl().GetItemData(hItem)));
	int choice = item->getChecks()->getState();
	if ( choice == 0) {
		OnSelectMode();
	} else if ( choice == 1) {
		OnInsertMode();
	} else if ( choice == 2) {
		OnMoveMode();
	}
	mdoc->mpvGetView()->iActiveTool = getId();
	setActive(true);
}

void FeatureSetEditor::prepare() {
	bool editMode = drawer->inEditMode();
	String reason = bmapptr->fnObj.sRelative();
	if ( editMode) { // toggle
		mdoc->pixInfoDoc->setAssociatedDrawerTool(0,reason); 
		mdoc->mpvGetView()->noTool(getId());
	}
	else {
		mdoc->pixInfoDoc->setAssociatedDrawerTool(this, reason );
		mdoc->mpvGetView()->addTool(this, getId());
	}
	getDrawer()->setEditable(!getDrawer()->isEditable());
	((ComplexDrawer *)getDrawer())->setEditMode(!editMode);
	OnSelectMode();


}

bool FeatureSetEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){

	if (  nChar == VK_RETURN) {
		mode -= mMOVING;
		clear();
		return true;
	}
	if ( nChar == VK_DELETE && hasSelection()) {
		if ( MessageBox(tree->m_hWnd, TR("Remove selected point(s)?").scVal(),TR("Delete").scVal(), MB_YESNO) == IDYES  ) {
			removeSelectedFeatures();
		/*	PreparationParameters p(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
			drawer->prepare(&p);*/
			mpvGetView()->Invalidate();
		}
		return true;
	}
	return false;
}

void FeatureSetEditor::clear() {
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		for(vector<NewDrawer *>::iterator cur2 = (*cur).second->drawers.begin(); cur2 != (*cur).second->drawers.end(); ++cur2) {
			NewDrawer *drws = (*cur2);
			drws->setSpecialDrawingOptions(NewDrawer::sdoSELECTED, false);
		}
		delete (*cur).second;
	}
	selectedFeatures.clear();
	currentCoordIndex = iUNDEF;
	currentGuid = sUNDEF;
}

// adds a vertex and the feature it belongs to to a set of selected features(maybe empty).
void FeatureSetEditor::addToSelectedFeatures(Feature *f, const Coord& crd, const vector<NewDrawer *>& drawers, int coordIndex) {
	if ( drawers.size() == 0)
		return;

	SelectedFeature *sf;
	// see if the feature already is in the 
	SFMIter cur = selectedFeatures.find(currentGuid);
	if ( cur == selectedFeatures.end()) {
		sf = new SelectedFeature(f);
		sf->drawers.push_back(drawers[0]);
		selectedFeatures[currentGuid] = sf;
		drawers[0]->shareVertices(sf->coords);
		drawers[0]->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true); 
		mdoc->mpvGetView()->Invalidate();
	} else {
		sf = (*cur).second;
	}
	if (coordIndex == iUNDEF)
		currentCoordIndex = iCoordIndex(sf->coords,crd);
	else 
		currentCoordIndex = coordIndex;

	if ( currentCoordIndex == iUNDEF)
		return;

	vector<int>::iterator loc = find(sf->selectedCoords.begin(), 
									 sf->selectedCoords.end(), 
									 currentCoordIndex);

	if (  loc == sf->selectedCoords.end()){
		sf->selectedCoords.push_back(currentCoordIndex);
		for(int i=0; i < sf->drawers.size(); ++i) {
			vector<Coord> coords;
			for(int c=0; c < sf->selectedCoords.size(); ++c) {
				Coord crd = *(sf->coords.at(sf->selectedCoords.at(c)));
				coords.push_back(crd);
			}
			sf->drawers[i]->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true, &coords);
		}
	} else {
		sf->selectedCoords.erase(loc);
	}

}

bool FeatureSetEditor::selectMove(UINT nFlags, CPoint point) {
	if ( selectedFeatures.size() == 0)
		return select(nFlags, point);

	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	vector<Geometry *> geometries = bmapptr->getFeatures(crd);
	if ( geometries.size() > 0) {
		Feature *f = CFEATURE(geometries[0]);
		currentGuid = f->getGuid();
		SFMIter cur = selectedFeatures.find(currentGuid);
		if ( cur == selectedFeatures.end()) { // other feature selected, clear all selections and select new feature
			clear();
			return select(nFlags, point);
		}
		SelectedFeature *sf = (*cur).second;
		currentCoordIndex = iCoordIndex(sf->coords,crd);
		vector<int>::iterator cur2 = find(sf->selectedCoords.begin(), sf->selectedCoords.end(), currentCoordIndex);
		if ( currentCoordIndex == iUNDEF || cur2 == sf->selectedCoords.end()) { // selected other point, not in the selection, clear selection and select a new one
			sf->selectedCoords.clear();
			return select(nFlags, point);
		}
	}
	return true;
}

// adds a point and the feature it belongs to the current selection. If no valid point(belonging to no feature)
// is choosen the current selection is removed.
bool FeatureSetEditor::select(UINT nFlags, CPoint point) {
	// calc world coordinates from the point
	Coord crd = setdrawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	// if no control key is pressed previous selection has to be removed
	// note that the mMOVE mode is a hidden selection as needing to select anything 
	// before being able to move things would be a bit cumbersome.
	if ( mode & mSELECT || mode & mMOVE) {
		bool fCtrl = nFlags & MK_CONTROL ? true : false;
		if ( !fCtrl) {
			clear();
		}
	}

	// check which geometries/ features belong to the location
	vector<Geometry *> geometries = bmapptr->getFeatures(crd);
	if ( geometries.size() > 0) {
		Feature *f = CFEATURE(geometries[0]);
		currentGuid = f->getGuid();
		vector<NewDrawer *> drawers;
		// find the lowest drawer that belongs to this feature
		setdrawer->getRootDrawer()->getDrawerFor(f, drawers);
		// add the feature to the list of selected features
		addToSelectedFeatures(f, crd, drawers);

		setdrawer->getRootDrawer()->getDrawerContext()->doDraw();

	} else {
		// empty selection remove all previous selections
		clear();
		setdrawer->getRootDrawer()->getDrawerContext()->doDraw();
		return false;
	}
	return true;

}

void FeatureSetEditor::OnLButtonDown(UINT nFlags, CPoint point){

	// nothing to do
	if ( mode & mUNKNOWN)
		return ;

	// select vertex or feature at current location
	if ( mode & mSELECT) {
		select(nFlags, point);
	} // insert a new feature if we are not in the moving mode or add a vertex to an feature being inserted
	else if (mode & mINSERT) {
		if ( (mode & mMOVING) == 0)
			insertFeature(nFlags, point);
		else {
			// insert a new vertex in the feature; if not possible, the editing has ended for this feature.
			if ( !insertVertex(nFlags, point)) {
				mode -= mMOVING;
				clear();
			}
		}
	}
	if ( mode & mMOVE) {
		selectMove(nFlags, point);
		mode |= mMOVING;
	}
	return ;

}

void FeatureSetEditor::OnLButtonUp(UINT nFlags, CPoint point){
	if ( mode & mUNKNOWN)
		return ;
	if ( mode & mMOVING) {
		CoordSystem csyMap = bmapptr->cs();
		CoordSystem csyPane = setdrawer->getRootDrawer()->getCoordinateSystem();
		if ( csyMap == csyPane)  {
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				SelectedFeature *f = (*cur).second;
				updateFeature(f);
			}
			bmapptr->fChanged = true;
			mode -= mMOVING;
		}
	} else
		return ;

	return ;
}

void FeatureSetEditor::OnLButtonDblClk(UINT nFlags, CPoint point){
	if ( mode & mUNKNOWN)
		return ;
	return ;
}
void FeatureSetEditor::OnMouseMove(UINT nFlags, CPoint point){
	if ((mode & mMOVING) == 0)
		return ;

	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	if ( currentCoordIndex != iUNDEF && currentGuid != sUNDEF) {
		Coord cDelta(*(selectedFeatures[currentGuid]->coords[currentCoordIndex]));
		cDelta -= crd;
		for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
			vector<Coord *> &coords = (*cur).second->coords;
			for(int i = 0; i < (*cur).second->selectedCoords.size(); ++i){
				*(coords[(*cur).second->selectedCoords[i]]) -= cDelta;
			}
		}
		mdoc->mpvGetView()->Invalidate();
	}
	return ;
}

void FeatureSetEditor::OnSetCursor(BaseMapEditor::Mode m) {
	bool fSetCursor = (HCURSOR)0 != curActive;
	if (fSetCursor)
		SetCursor(curActive);
	return ;
}

void FeatureSetEditor::OnContextMenu(CWnd* pWnd, CPoint point){
	return ;
}

LRESULT FeatureSetEditor::OnUpdate(WPARAM, LPARAM) {
	return 0;
}

long FeatureSetEditor::iCoordIndex(const vector<Coord *>& coords, const Coord& c) const
{
	long iRes = iUNDEF;
	double r2Res = bmapptr->rProximity();
	r2Res *= r2Res;
	double r2;
	Coord crd;
	for (long i = 0; i < coords.size(); ++i) {
		crd = *coords.at(i);
		if (crd.fUndef())
			continue;
		r2 = rDist2(c, crd);
		if (r2 <= r2Res) {
			if (r2 == 0)
				return i;
			r2Res = r2;
			iRes = i;
		}
	}
	if ( iRes != iUNDEF)
		return iRes;
	return iUNDEF;
}

bool FeatureSetEditor::fCopyOk(){
	return selectedFeatures.size() != 0;
	 
}

bool FeatureSetEditor::fPasteOk()
{
  return false;
}


bool FeatureSetEditor::hasSelection() const { 
	return selectedFeatures.size() > 0; 
}

void FeatureSetEditor::OnInsertMode()
{
	setMode(mode == BaseMapEditor::mINSERT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mINSERT);
}

void FeatureSetEditor::OnMoveMode()
{
	setMode(mode == BaseMapEditor::mMOVE ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mMOVE);
}

void FeatureSetEditor::OnSelectMode()
{
	setMode(mode == BaseMapEditor::mSELECT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mSELECT);
}

void FeatureSetEditor::setActive(bool yesno) {
	BaseMapEditor::setActive(yesno);
	editModeItems->setActive(yesno);
}




