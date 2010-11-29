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
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "FeatureSetEditor.h"

#define sMen(ID) ILWSF("men",ID).scVal()
#define addmen(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID));

using namespace ILWIS;

FeatureSetEditor::FeatureSetEditor(MapCompositionDoc *doc, const BaseMap& bm) : 
	mdoc(doc), 
	bmapptr(bm.ptr()),
	currentCoordIndex(iUNDEF),
	currentGuid(sUNDEF),
	mode(BaseMapEditor::mUNKNOWN),
	setdrawer(0)
{ 
}

FeatureSetEditor::~FeatureSetEditor(){
	if ( bmapptr && bmapptr->fChanged) {
		bmapptr->Store();
	}
	clear();
}

bool FeatureSetEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
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

void FeatureSetEditor::addToSelectedFeatures(Feature *f, const Coord& crd, const vector<NewDrawer *>& drawers) {
	if ( drawers.size() == 0)
		return;

	SFMIter cur = selectedFeatures.find(currentGuid);
	SelectedFeature *sf;
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
	currentCoordIndex = iCoordIndex(sf->coords,crd);
	if ( currentCoordIndex == iUNDEF)
		return;

	vector<int>::iterator loc = find(sf->selectedCoords.begin(), 
									 sf->selectedCoords.end(), 
									 currentCoordIndex);

	if (  loc == sf->selectedCoords.end()){
		sf->selectedCoords.push_back(currentCoordIndex);
	} else {
		sf->selectedCoords.erase(loc);
	}

}

bool FeatureSetEditor::select(UINT nFlags, CPoint point) {
	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	vector<Geometry *> geometries;
	if ( mode == mSELECT || mode == mMOVE) {
		bool fCtrl = nFlags & MK_CONTROL ? true : false;
		if ( !fCtrl) {
			clear();
		}
	}

	geometries = bmapptr->getFeatures(crd);
	if ( geometries.size() > 0) {
		Feature *f = CFEATURE(geometries[0]);
		currentGuid = f->getGuid();
		vector<NewDrawer *> drawers;
		mdoc->rootDrawer->getDrawerFor(f, drawers);
		addToSelectedFeatures(f, crd, drawers);

	} else {
		clear();
		mdoc->mpvGetView()->Invalidate();
		return false;
	}
	return true;

}

bool FeatureSetEditor::OnLButtonDown(UINT nFlags, CPoint point){
	if ( mode == mUNKNOWN)
		return false;

	if ( mode == mSELECT || mode == mMOVE) {
		select(nFlags, point);
	}
	else if (mode == mINSERT) {
		insert(nFlags, point);
	}
	if ( mode == mMOVE) {
		mode = mMOVING;
	}
	return true;

}

bool FeatureSetEditor::OnLButtonUp(UINT nFlags, CPoint point){
	if ( mode == mUNKNOWN)
		return false;
	if ( mode == mMOVING) {
		CoordSystem csyMap = bmapptr->cs();
		CoordSystem csyPane = setdrawer->getRootDrawer()->getCoordinateSystem();
		if ( csyMap == csyPane)  {
			for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
				SelectedFeature *f = (*cur).second;
				updateFeature(f);
				bmapptr->fChanged = true;
			}
		}
		mode = mMOVE;
	}
	return true;
}

bool FeatureSetEditor::OnLButtonDblClk(UINT nFlags, CPoint point){
	if ( mode == mUNKNOWN)
		return false;
	return true;
}
bool FeatureSetEditor::OnMouseMove(UINT nFlags, CPoint point){
	if ( mode != mMOVING)
		return false;

	Coord crd = mdoc->rootDrawer->screenToWorld(RowCol(point.y, point.x));
	if ( currentCoordIndex != iUNDEF && currentGuid != sUNDEF) {
		Coord cDelta(*(selectedFeatures[currentGuid]->coords[currentCoordIndex]));
		cDelta -= crd;
		for(SFMIter cur = selectedFeatures.begin(); cur != selectedFeatures.end(); ++cur) {
			vector<Coord *> &coords = (*cur).second->coords;
			for(int i = 0; i < (*cur).second->selectedCoords.size(); ++i){
				*(coords[(*cur).second->selectedCoords[i]]) -= cDelta;
			}
			//for(int i=0; i < coords.size(); ++i)
			//	*(coords[i]) -= cDelta;
		}
		mdoc->mpvGetView()->Invalidate();
	}
	return true;
}

bool FeatureSetEditor::OnSetCursor(BaseMapEditor::Mode m) {
	bool fSetCursor = (HCURSOR)0 != curActive;
	if (fSetCursor)
		SetCursor(curActive);
	return fSetCursor;
}

bool FeatureSetEditor::OnContextMenu(CWnd* pWnd, CPoint point){
	return false;
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

void FeatureSetEditor::init(ILWIS::ComplexDrawer *d,PixelInfoDoc *pdoc) {
	setdrawer = d;
	pixdoc = pdoc;
}

bool FeatureSetEditor::hasSelection() const { 
	return selectedFeatures.size() > 0; 
}