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
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "FeatureSetEditor.h"

#define sMen(ID) ILWSF("men",ID).c_str()
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
	//ON_UPDATE_COMMAND_UI(ID_SELECTMODE, OnUpdateMode)
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
	currentGuid(0),
	mode(BaseMapEditor::mUNKNOWN),
	LayerDrawer((ComplexDrawer *)drw),
	bmapptr(0),
	curInsert("EditEditCursor"),
	curEdit("EditPntCursor"),
	curMove("EditPntMoveCursor"), 
	curMoving("EditPntMovingCursor"),
	mdoc(0)
{ 
	SpatialDataDrawer *amdrw = dynamic_cast<SpatialDataDrawer *>(drw->getParentDrawer());
	if ( amdrw) {
		bmapptr = amdrw->getBaseMap();
		mdoc = (MapCompositionDoc *)zv->GetDocument();
	}
	active = false;
	editModeItems = new SetChecks(tree,this,(DTSetCheckFunc)&FeatureSetEditor::setcheckEditMode);
	stay = true;
	needsMouseFocus = true;
}

FeatureSetEditor::~FeatureSetEditor(){
	if ( bmapptr) {
		if (  bmapptr->fChanged )
			bmapptr->Store();
		MapCompositionDoc *mdoc = tree->GetDocument();
		if ( mdoc->pixInfoDoc) {
			mdoc->pixInfoDoc->setAssociatedDrawerTool(0,bmapptr->fnObj.sRelative()); 
		}
		mdoc->mpvGetView()->noTool(getId());
	}
	clear();
}

HTREEITEM FeatureSetEditor::configure( HTREEITEM parentItem) {
	DisplayOptionRadioButtonItem *item = new DisplayOptionRadioButtonItem(TR("Select"), tree,parentItem,drawer);
	item->setState(true);
	item->setCheckAction(this,editModeItems,0); 
	insertItem(TR("Select"),"Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Insert"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0);
	insertItem(TR("Insert"),"Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Move"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0); 
	insertItem(TR("Move"),"Bitmap", item);
	DrawerTool::configure(htiNode);


	return parentItem;
}

void FeatureSetEditor::setcheckEditMode(void *value, HTREEITEM  it) {
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
	} else if ( choice == 3) {
		OnSplitMode();
	} else if ( choice == 4) {
		OnMergeMode();
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
		mode = mode & ~mMOVING;
		for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
			SelectedFeature *f = (*cur).second;
			updateFeature(f);
		}
		return true;
	}
	if ( nChar == VK_DELETE && hasSelection()) {
		if ( MessageBox(tree->m_hWnd, TR("Remove selected point(s)?").c_str(),TR("Delete").c_str(), MB_YESNO) == IDYES  ) {
			removeSelectedFeatures();
	
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
	currentGuid = 0;
}

// adds a vertex and the feature it belongs to to a set of selected features(maybe empty).
SelectedFeature * FeatureSetEditor::addToSelectedFeatures(Feature *f, const Coord& crd, const vector<NewDrawer *>& drawers, int coordIndex) {
	if ( drawers.size() == 0)
		return 0;

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
		return 0;
	currentGuid = sf->feature->getGuid();

	vector<int>::iterator loc = find(sf->selectedCoords.begin(), 
									 sf->selectedCoords.end(), 
									 currentCoordIndex);

	if (  loc == sf->selectedCoords.end()){
		sf->selectedCoords.push_back(currentCoordIndex);
		for(int i=0; i < sf->drawers.size(); ++i) {
			vector<int> coords;
			for(int c=0; c < sf->selectedCoords.size(); ++c) {
				int ind = sf->selectedCoords.at(c);
				if ( ind < sf->coords.size()) {
					coords.push_back(ind);
				}
			}
			sf->drawers[i]->setSpecialDrawingOptions(NewDrawer::sdoSELECTED,true, coords);
		}
	} else {
		sf->selectedCoords.erase(loc);
	}

	return sf; 
}

bool FeatureSetEditor::selectMove(UINT nFlags, CPoint point) {
	if ( selectedFeatures.size() == 0)
		return select(nFlags, point);

	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	vector<Geometry *> geometries = bmapptr->getFeatures(crd);
	if ( geometries.size() > 0 || currentGuid != 0) {
		if ( geometries.size() > 0) {
			Feature *f = CFEATURE(geometries[0]);
			currentGuid = f->getGuid();
		}
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
	Coord crd = LayerDrawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
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
		LayerDrawer->getRootDrawer()->getDrawerFor(f, drawers);
		// add the feature to the list of selected features
		addToSelectedFeatures(f, crd, drawers);

		LayerDrawer->getRootDrawer()->getDrawerContext()->doDraw();

	} else {
		// empty selection remove all previous selections
		clear();
		LayerDrawer->getRootDrawer()->getDrawerContext()->doDraw();
		return false;
	}
	return true;

}

void FeatureSetEditor::setMode(BaseMapEditor::Mode m) 
{
	int selectedIndex = editModeItems->getState();
	switch (m) {
	case mSELECT:
		curActive = curEdit;
		if ( selectedIndex != 0) {
			editModeItems->checkItem(0);
		}
		break;
	case mMOVE:
		curActive = curMove;
		if ( selectedIndex != 2) {
			editModeItems->checkItem(2);
		}
		break;
	case mMOVING:
		curActive = curMoving;
		break;
	case mINSERT:
		curActive = curInsert;
		if ( selectedIndex != 1) {
			editModeItems->checkItem(1);
		}
		break;
	}
	mode = m;
	OnSetCursor();
}

void FeatureSetEditor::OnLButtonDown(UINT nFlags, CPoint point){

	// nothing to do
	if ( mode == mUNKNOWN)
		return ;

	// select vertex or feature at current location
	if ( mode & mSELECT || mode & mMERGE) {
		select(nFlags, point);
	} // insert a new feature if we are not in the moving mode or add a vertex to an feature being inserted
	else if (mode & mINSERT) {
		if ( (mode & mMOVING) == 0)
			insertFeature(nFlags, point);
		else {
			// insert a new vertex in the feature; if not possible, the editing has ended for this feature.
			if ( !insertVertex(nFlags, point)) {
				mode = mode & ~mMOVING;
				clear();
			}
		}
	}
	if ( (mode & mMOVING) && ((mode & mINSERT) == 0)) {
		CoordSystem csyMap = bmapptr->cs();
		CoordSystem csyPane = LayerDrawer->getRootDrawer()->getCoordinateSystem();
		if ( csyMap == csyPane)  {
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				SelectedFeature *f = (*cur).second;
				updateFeature(f);
			}
			bmapptr->fChanged = true;
			mode = mode & ~mMOVING;
		}
	}
	else if ( mode & mMOVE) {
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
		CoordSystem csyPane = LayerDrawer->getRootDrawer()->getCoordinateSystem();
		if ( csyMap == csyPane)  {
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				SelectedFeature *f = (*cur).second;
				updateFeature(f);
			}
			bmapptr->fChanged = true;
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
	if ( currentCoordIndex != iUNDEF && currentGuid != 0) {
		SelectedFeature *sf = selectedFeatures[currentGuid];
		if ( sf && currentCoordIndex < sf->coords.size()) {
			Coord cDelta(*(sf->coords[currentCoordIndex]));
			cDelta -= crd;
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				vector<Coord *> &coords = (*cur).second->coords;
				for(int i = 0; i < (*cur).second->selectedCoords.size(); ++i){
					*(coords[(*cur).second->selectedCoords[i]]) -= cDelta;
				}
			}
		}
		bmapptr->fChanged = true;
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
			if (r2 == 0) {
				return i;
			}
			r2Res = r2;
			iRes = i;
		}
	}
	if ( iRes != iUNDEF) {
		TRACE(String("crdIndex %d\n",iRes).c_str());
		return iRes;
	}
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

void FeatureSetEditor::OnSplitMode()
{
	setMode(mode == BaseMapEditor::mSPLIT ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mSPLIT);
}

void FeatureSetEditor::OnMergeMode()
{
	setMode(mode == BaseMapEditor::mMERGE ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mMERGE);
}

void FeatureSetEditor::setActive(bool yesno) {
	BaseMapEditor::setActive(yesno);
	editModeItems->setActive(yesno);
	LayerDrawer->setSpecialDrawingOptions(NewDrawer::sdoSELECTED | NewDrawer::sdoTOCHILDEREN,false);

	if ( bmapptr->fChanged)
		bmapptr->Store();
	mdoc->mpvGetView()->Invalidate();
}

void FeatureSetEditor::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck;
	switch (pCmdUI->m_nID) {
		case ID_SELECTMODE:
			fCheck = (BaseMapEditor::mSELECT & mode) != 0;
			break;
		case ID_MOVEMODE:
			fCheck = (BaseMapEditor::mMOVE & mode) != 0;
			break;
		case ID_INSERTMODE:
			fCheck = (BaseMapEditor::mINSERT & mode) != 0;
			break;
		//case ID_FINDUNDEFS:
		//	pCmdUI->SetCheck(fFindUndefs);
			return;
	}
	//if (0 != mpv->as)
	//	fCheck = false;
	pCmdUI->SetRadio(fCheck);
}

void FeatureSetEditor::removeSelectedFeatures() {
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sfeature = (*cur).second;
		FeatureID oldGuid = sfeature->feature->getGuid();
		bmapptr->removeFeature(oldGuid, sfeature->selectedCoords);
		sfeature->feature = 0;
		bool removed = false;
		if ( oldGuid == currentGuid) {
			if ( sfeature->selectedCoords.size() == 0 || sfeature->selectedCoords.size() == sfeature->coords.size()) {
				((ComplexDrawer *)drawer)->removeDrawer(sfeature->drawers[0]->getId());
				currentGuid = 0;
				removed = true;
			}
		}
		if ( !removed) {
			PreparationParameters p(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
			sfeature->drawers[0]->prepare(&p);
		}
		bmapptr->fChanged = true;

	}
	selectedFeatures.clear();
	currentCoordIndex = iUNDEF;
}





