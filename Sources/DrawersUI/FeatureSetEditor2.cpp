#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
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
#include "FeatureSetEditor2.h"

#define sMen(ID) ILWSF("men",ID).c_str()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID));

BEGIN_MESSAGE_MAP(FeatureSetEditor2, BaseMapEditor)

END_MESSAGE_MAP()

using namespace ILWIS;

FeatureSetEditor2::FeatureSetEditor2(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	BaseMapEditor(tp,zv, view, drw),
	currentCoordIndex(iUNDEF),
	layerDrawer((ComplexDrawer *)drw),
	bmapptr(0),
	curInsert("EditPntCursor"),
	curEdit("EditCursor"),
	curMove("EditPntMoveCursor"), 
	curMoving("EditPntMovingCursor"),
	mdoc(0),
	curActive(curEdit),
	editorState(FeatureSetEditor2::msSELECT)
{ 
	SpatialDataDrawer *amdrw = dynamic_cast<SpatialDataDrawer *>(drw->getParentDrawer());
	if ( amdrw) {
		bmapptr = amdrw->getBaseMap();
		mdoc = (MapCompositionDoc *)zv->GetDocument();
	}
	active = false;
	editModeItems = new SetChecks(tree,this,(DTSetCheckFunc)&FeatureSetEditor2::setcheckEditMode);
	stay = true;
	needsMouseFocus = true;
}

FeatureSetEditor2::~FeatureSetEditor2(){
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

HTREEITEM FeatureSetEditor2::configure( HTREEITEM parentItem) {
	DisplayOptionRadioButtonItem *item = new DisplayOptionRadioButtonItem(TR("Select"), tree,parentItem,drawer);
	item->setState(true);
	item->setCheckAction(this,editModeItems,0); 
	item->setDoubleCickAction(this,(DTDoubleClickActionFunc ) &FeatureSetEditor2::setSelectionOptions);
	hitSelect = insertItem(TR("Select"),"Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Insert"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0);
	hitInsert = insertItem(TR("Insert"),"Bitmap", item);
	item = new DisplayOptionRadioButtonItem(TR("Move"), tree,parentItem,drawer);
	item->setState(false);
	item->setCheckAction(this,editModeItems,0); 
	hitMove = insertItem(TR("Move"),"Bitmap", item);
	DrawerTool::configure(htiNode);


	return parentItem;
}

void FeatureSetEditor2::setSelectionOptions() {
	new SelectionOptionsForm(tree, (ComplexDrawer *)drawer);
}

void FeatureSetEditor2::setcheckEditMode(void *value, HTREEITEM ) {
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

void FeatureSetEditor2::prepare() {
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

bool FeatureSetEditor2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){

	if (  nChar == VK_RETURN) {
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

void FeatureSetEditor2::clear() {
}


void FeatureSetEditor2::setMode(FeatureSetEditor2::States state) 
{
	int selectedIndex = editModeItems->getState();
	editorState |= state;
	if ( (editorState & msSELECT) != 0) {
		curActive = curEdit;
		if ( selectedIndex != 0) {
			editModeItems->checkItem(0);
		}
	}
	else if ( (editorState & msMOVE) != 0) {
		curActive = curMove;
		if ( selectedIndex != 2) {
			editModeItems->checkItem(2);
		}
	}
	else if ( (editorState & msINSERT) != 0) {
		curActive = curInsert;
		if ( selectedIndex != 1) {
			editModeItems->checkItem(1);
		}
	}
	OnSetCursor();
}

void FeatureSetEditor2::OnLButtonDown(UINT nFlags, CPoint point){

}

void FeatureSetEditor2::OnLButtonUp(UINT nFlags, CPoint point){

	return ;
}

void FeatureSetEditor2::OnMouseMove(UINT nFlags, CPoint point){
	return ;
}

void FeatureSetEditor2::OnSetCursor(FeatureSetEditor2::States m) {
	bool fSetCursor = (HCURSOR)0 != curActive;
	if (fSetCursor)
		SetCursor(curActive);
	return ;
}

void FeatureSetEditor2::OnContextMenu(CWnd* pWnd, CPoint point){
	return ;
}

LRESULT FeatureSetEditor2::OnUpdate(WPARAM, LPARAM) {
	return 0;
}

long FeatureSetEditor2::iCoordIndex(const vector<Coord *>& coords, const Coord& c) const
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

bool FeatureSetEditor2::fCopyOk(){
	return selectedFeatures.size() != 0;
	 
}

bool FeatureSetEditor2::fPasteOk()
{
  return false;
}


bool FeatureSetEditor2::hasSelection() const { 
	return selectedFeatures.size() > 0; 
}

void FeatureSetEditor2::OnInsertMode()
{
	setMode(editorState == FeatureSetEditor2::msINSERT ? FeatureSetEditor2::msNONE : FeatureSetEditor2::msINSERT);
}

void FeatureSetEditor2::OnMoveMode()
{
	setMode(editorState == FeatureSetEditor2::msMOVE ? FeatureSetEditor2::msNONE : FeatureSetEditor2::msMOVE);
}

void FeatureSetEditor2::OnSelectMode()
{
	setMode(editorState == FeatureSetEditor2::msSELECT ? FeatureSetEditor2::msNONE : FeatureSetEditor2::msSELECT);
}

void FeatureSetEditor2::OnSplitMode()
{
	setMode(editorState == FeatureSetEditor2::msSPLIT ? FeatureSetEditor2::msNONE : FeatureSetEditor2::msSPLIT);
}

void FeatureSetEditor2::OnMergeMode()
{
	//setMode(mode == BaseMapEditor::mMERGE ? BaseMapEditor::mUNKNOWN : BaseMapEditor::mMERGE);
}

void FeatureSetEditor2::setActive(bool yesno) {
	BaseMapEditor::setActive(yesno);
	editModeItems->setActive(yesno);
}

void FeatureSetEditor2::OnUpdateMode(CCmdUI* pCmdUI)
{
	BOOL fCheck;
	switch (pCmdUI->m_nID) {
		case ID_SELECTMODE:
			fCheck = (FeatureSetEditor2::msSELECT & editorState) != 0;
			break;
		case ID_MOVEMODE:
			fCheck = (FeatureSetEditor2::msMOVE & editorState) != 0;
			break;
		case ID_INSERTMODE:
			fCheck = (FeatureSetEditor2::msINSERT & editorState) != 0;
			break;
		//case ID_FINDUNDEFS:
		//	pCmdUI->SetCheck(fFindUndefs);
			return;
	}
	//if (0 != mpv->as)
	//	fCheck = false;
	pCmdUI->SetRadio(fCheck);
}

void FeatureSetEditor2::removeSelectedFeatures() {
	for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
		SelectedFeature *sfeature = (*cur).second;
		bmapptr->removeFeature(sfeature->feature->getGuid(), sfeature->selectedCoords);
		bool removed = false;
	/*	if ( sfeature->feature->getGuid() == currentGuid) {
			if ( sfeature->selectedCoords.size() == 0 || sfeature->selectedCoords.size() == sfeature->coords.size()) {
				((ComplexDrawer *)drawer)->removeDrawer(sfeature->drawers[0]->getId());
				currentGuid = sUNDEF;
				removed = true;
			}
		}
		if ( !removed) {
			PreparationParameters p(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER,bmapptr->cs());
			sfeature->drawers[0]->prepare(&p);
		}*/

	}
	selectedFeatures.clear();
	currentCoordIndex = iUNDEF;
}

bool FeatureSetEditor2::hasState(int state) {
	return (editorState & state) != 0;
}

//---------------------------------------------
SelectionOptionsForm::SelectionOptionsForm(CWnd *wPar, ComplexDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(dr->getSelectionColor())
{
	fc = new FieldColor(root, TR("Selection color"), &c);
	create();
}

void  SelectionOptionsForm::apply() {
	fc->StoreData();
	drw->setSelectionColor(c);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}




