#include "Client\Headers\formelementspch.h"
#include "SpaceTimeCubeTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\MapPaneView.h"
#include "DrawersUI\ThreeDGlobalTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "TimePositionBar.h"
#include "TemporalDrawer.h"
#include "SortableDrawer.h"
#include "GroupableDrawer.h"
#include "SizableDrawer.h"
#include "PreTimeOffsetDrawer.h"
#include "PostTimeOffsetDrawer.h"
//#include "Drawers\LayerDrawer.h"
//#include "Drawers\FeatureLayerDrawer.h"
//#include "Drawers\PointLayerDrawer.h"
//#include "SpaceTimePathDrawer.h"
//#include "Drawers\RasterLayerDrawer.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "CubeElementsTool.h"
#include "Client\Base\datawind.h"

LayerData::LayerData(NewDrawer *drw)
: drawerId(drw->getId())
, plotOption(false)
, fSort(false)
, fGroup(false)
, fSize(false)
, fSelfTime(false)
, fFeatureMap(false)
, fPointMap(false)
{
	if ( drw->getType() == "FeatureDataDrawer") {
		BaseMapPtr *bmp = ((FeatureDataDrawer *)drw)->getBaseMap();
		fnBaseMap = bmp->fnObj;
		fFeatureMap = IOTYPEFEATUREMAP(fnBaseMap);
		fPointMap = IOTYPE(fnBaseMap) == IlwisObject::iotPOINTMAP;
		fSelfTime = bmp->dm()->pdtime() != 0;
		if (fSelfTime)
			m_rrTimeMinMax = bmp->rrMinMax();
		else if (bmp->fTblAtt()) {
			attTable = bmp->tblAtt();
			for (int i = 0; i < attTable->iCols(); ++i) {
				Column col = attTable->col(i);
				if (col->dm()->pdtime())
					temporalColumns.push_back(col);
			}
			if (temporalColumns.size() == 1) {
				temporalColumn = temporalColumns[0];
				m_rrTimeMinMax = temporalColumn->rrMinMax();
			}
		}
		if (bmp->fTblAtt()) {
			attTable = bmp->tblAtt();
			for (int i = 0; i < attTable->iCols(); ++i) {
				Column col = attTable->col(i);
				if (col->dm()->pdv()) // || col->dm()->pds())
					sortColumns.push_back(col);
				if (col->dm()->pdv() || col->dm()->pds() || col->dm()->pdc())
					groupColumns.push_back(col);
				if (col->dm()->pdv())
					sizeColumns.push_back(col);
			}
			if (sizeColumns.size() > 0) {
				sizeColumn = sizeColumns[0];
				m_rrSizeMinMax = sizeColumn->rrMinMax();
			}
		}
	} else
		fFeatureMap = false;
}

bool LayerData::hasTime() {
	return fFeatureMap && (fSelfTime || (temporalColumns.size() > 0));
}

bool LayerData::hasSort() {
	return fFeatureMap && (sortColumns.size() > 0);
}

bool LayerData::hasGroup() {
	return fFeatureMap && (groupColumns.size() > 0);
}

bool LayerData::hasSize() {
	return fFeatureMap && (sizeColumns.size() > 0);
}

bool LayerData::getPlotOption() {
	return plotOption && (fSelfTime || temporalColumn.fValid());
}

bool LayerData::fUseSort() {
	return fSort && sortColumn.fValid();
}

bool LayerData::fUseGroup() {
	return fGroup && groupColumn.fValid();
}

bool LayerData::fUseSize() {
	return fSize && sizeColumn.fValid();
}

void LayerData::setTimeColumn(String sColName) {
	temporalColumn = attTable->col(sColName);
	m_rrTimeMinMax = temporalColumn->rrMinMax();
}

void LayerData::setSortColumn(String sColName) {
	sortColumn = attTable->col(sColName);
}

void LayerData::setGroupColumn(String sColName) {
	groupColumn = attTable->col(sColName);
}

void LayerData::setSizeColumn(String sColName) {
	sizeColumn = attTable->col(sColName);
	m_rrSizeMinMax = sizeColumn->rrMinMax();
}

//------------------------------------------------------

map<ZoomableView*, SpaceTimeCube*> SpaceTimeCube::spaceTimeCubes;

SpaceTimeCube * SpaceTimeCube::getSpaceTimeCube(ZoomableView* mpv, LayerTreeView * tree, NewDrawer *drw)
{
	map<ZoomableView*, SpaceTimeCube*>::iterator it = spaceTimeCubes.find(mpv);
	if (it != spaceTimeCubes.end())
		return (*it).second;
	else {
		SpaceTimeCube* stc = new SpaceTimeCube(mpv, tree, drw);
		spaceTimeCubes[mpv] = stc;
		return stc;
	}
}

void SpaceTimeCube::deleteSpaceTimeCube(ZoomableView* mpv)
{
	map<ZoomableView*, SpaceTimeCube*>::iterator it = spaceTimeCubes.find(mpv);
	if (it != spaceTimeCubes.end()) {
		SpaceTimeCube * stc = (*it).second;
		spaceTimeCubes.erase(it);
		delete stc;
	}
}

SpaceTimeCube::SpaceTimeCube(ZoomableView * _mpv, LayerTreeView * _tree, NewDrawer *drw)
: mpv(_mpv)
, tree(_tree)
, rootDrawer((RootDrawer*)(drw->getRootDrawer()))
, useSpaceTimeCube(false)
, timePosBar(0)
, layerOptionsForm(0)
, timeBoundsZoom(new TimeBounds())
, timeBoundsFullExtent(new TimeBounds())
, timeOffset(0)
{
}

SpaceTimeCube::~SpaceTimeCube()
{
	if (timePosBar) {
		delete timePosBar;
		timePosBar = 0;
	}
	if (layerOptionsForm) {
		delete layerOptionsForm;
		layerOptionsForm = 0;
	}
	if (timeBoundsZoom) {
		delete timeBoundsZoom;
		timeBoundsZoom = 0;
	}
	if (timeBoundsFullExtent) {
		delete timeBoundsFullExtent;
		timeBoundsFullExtent = 0;
	}
}

void SpaceTimeCube::update() {
	vector<LayerData> newLayerList;
	for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
		NewDrawer *drw = rootDrawer->getDrawer(i);
		if (find(ownDrawerIDs.begin(), ownDrawerIDs.end(), drw->getId()) == ownDrawerIDs.end()) {
			int j = 0;
			// if it is in the old list, copy it
			for (; j < layerList.size(); ++j) {
				if (layerList[j].getDrawerId() == drw->getId()) {
					newLayerList.push_back(layerList[j]);
					break;
				}
			}
			// if not found in the old list, add a new one
			if (j >= layerList.size()) {
				LayerData layerData (drw);
				if (layerData.hasTime())
					newLayerList.push_back(layerData);
			}
		}
	}
	layerList = newLayerList;
}

void SpaceTimeCube::setUseSpaceTimeCube(bool yesno) {
	useSpaceTimeCube = yesno;
	if (useSpaceTimeCube) {
		timePosBar = new TimePositionBar();
		timePosBar->Create(mpv->dwParent());
		timePosBar->SetSpaceTimeCube(this);
		mpv->GetParentFrame()->DockControlBar(timePosBar, AFX_IDW_DOCKBAR_LEFT);
		refreshDrawerList();
	}
	else {
		refreshDrawerList();
		delete timePosBar;
		timePosBar = 0;
		mpv->dwParent()->RecalcLayout();
	}
}

bool SpaceTimeCube::fUseSpaceTimeCube()
{
	return useSpaceTimeCube;
}

MapCompositionDoc *SpaceTimeCube::getDocument() const {
	return (MapCompositionDoc *)mpv->GetDocument();
}

void SpaceTimeCube::refreshDrawerList() {
	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup values
	double rotX, rotY, rotZ, transX, transY, transZ;
	rootDrawer->getRotationAngles(rotX, rotY, rotZ);
	rootDrawer->getTranslate(transX, transY, transZ);
	double zoom3D = rootDrawer->getZoom3D();

	for (int i = 0; i < ownDrawerIDs.size(); ++i) {
		rootDrawer->removeDrawer(ownDrawerIDs[i]);
		for(int j = 0 ; j < rootDrawer->getDrawerCount(); ++j) {
			ComplexDrawer *drw = dynamic_cast<ComplexDrawer*>(rootDrawer->getDrawer(j));
			if (drw != 0) {
				drw->removeDrawer(ownDrawerIDs[i]);
			}
		}
	}

	ownDrawerIDs.clear();

	update();		

	timeBoundsFullExtent->Reset();
	sizeStretch = RangeReal();

	for (int i = 0; i < layerList.size(); ++i) {
		SpatialDataDrawer * oldDrw = (SpatialDataDrawer*)(rootDrawer->getDrawer(layerList[i].getDrawerId()));
		if (oldDrw == 0)
			continue; // skip it .. drawer was probably removed from the layers
		IlwisObjectPtr * object = oldDrw->getObject();
		const String& subtype = (useSpaceTimeCube && layerList[i].getPlotOption()) ? "Cube" : "ilwis38";
		BaseMap bm(object->fnObj);
		getDocument()->drAppend(bm, IlwisDocument::otUNKNOWN, IlwisWinApp::osNormal, subtype);
		int index = rootDrawer->getDrawerCount() - 1;
		NewDrawer * newDrw = rootDrawer->getDrawer(index);
		rootDrawer->removeDrawer(newDrw->getId(), false); // remove it, because we don't intend to append it to the end
		index = rootDrawer->getDrawerIndex(oldDrw);
		replaceTreeItem(oldDrw, (SpatialDataDrawer*)newDrw, index);
		replaceDrawer(oldDrw, newDrw);
		layerList[i].setDrawerId(newDrw->getId());
		TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
		if (temporalDrawer) {
			temporalDrawer->SetTimeBounds(timeBoundsZoom);
			RangeReal rrMinMax (layerList[i].rrTimeMinMax());
			timeBoundsFullExtent->AddMinMax(Time(rrMinMax.rLo()), Time(rrMinMax.rHi()));
			if (layerList[i].isSelfTime())
				temporalDrawer->SetSelfTime();
			else
				temporalDrawer->SetTimeAttribute(layerList[i].getTimeColumn());
		}
		SortableDrawer * sortableDrawer = dynamic_cast<SortableDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
		if (sortableDrawer) {
			if (layerList[i].fUseSort())
				sortableDrawer->SetSortAttribute(layerList[i].getSortColumn());
			else
				sortableDrawer->SetNoSort();
		}
		GroupableDrawer * groupableDrawer = dynamic_cast<GroupableDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
		if (groupableDrawer) {
			if (layerList[i].fUseGroup())
				groupableDrawer->SetGroupAttribute(layerList[i].getGroupColumn());
			else
				groupableDrawer->SetNoGroup();
		}
		SizableDrawer * sizableDrawer = dynamic_cast<SizableDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
		if (sizableDrawer) {
			sizableDrawer->SetSizeStretch(&sizeStretch);
			RangeReal rrMinMax (layerList[i].rrSizeMinMax());
			sizeStretch += rrMinMax;
			if (layerList[i].fUseSize())
				sizableDrawer->SetSizeAttribute(layerList[i].getSizeColumn());
			else
				sizableDrawer->SetNoSize();
		}
		if (rootDrawer->is3D()) {
			PreparationParameters pp(NewDrawer::pt3D);
			newDrw->prepare(&pp);
		}
	}

	if (useSpaceTimeCube) {

		DrawerParameters dp(rootDrawer, rootDrawer);
		PreparationParameters pp(NewDrawer::ptALL);

		for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
			ComplexDrawer *drw = dynamic_cast<ComplexDrawer*>(rootDrawer->getDrawer(i));
			if (drw != 0) {
				if (find(ownDrawerIDs.begin(), ownDrawerIDs.end(), drw->getId()) == ownDrawerIDs.end()) {
					int j = 0;
					// if it is in the layerList, and it uses time, leave it
					for (; j < layerList.size(); ++j) {
						if (layerList[j].getDrawerId() == drw->getId()) {
							if (!layerList[j].getPlotOption())
								j = layerList.size();
							else
								break; // keep j same
						}
					}
					// if not found in the layerList, or it does not use time, handle it
					if (j >= layerList.size()) {
						PreTimeOffsetDrawer* preTimeOffset = dynamic_cast<PreTimeOffsetDrawer*>(NewDrawer::getDrawer("PreTimeOffsetDrawer", "Cube", &dp));
						ownDrawerIDs.push_back(preTimeOffset->getId());
						TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(preTimeOffset);
						temporalDrawer->SetTimeBounds(timeBoundsZoom);
						preTimeOffset->prepare(&pp);
						preTimeOffset->SetTimeOffsetVariable(&timeOffset);
						PostTimeOffsetDrawer* postTimeOffset = dynamic_cast<PostTimeOffsetDrawer*>(NewDrawer::getDrawer("PostTimeOffsetDrawer", "Cube", &dp));
						ownDrawerIDs.push_back(postTimeOffset->getId());
						temporalDrawer = dynamic_cast<TemporalDrawer*>(postTimeOffset);
						temporalDrawer->SetTimeBounds(timeBoundsZoom);
						postTimeOffset->prepare(&pp);
						drw->addPreDrawer(0, preTimeOffset);
						drw->addPostDrawer(999, postTimeOffset);
					}
				}
			}
		}

		*timeBoundsZoom = *timeBoundsFullExtent;

		NewDrawer * cube = NewDrawer::getDrawer("CubeDrawer", "Cube", &dp);
		rootDrawer->insertDrawer(0, cube);
		ownDrawerIDs.push_back(cube->getId());
		TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(cube);
		temporalDrawer->SetTimeBounds(timeBoundsZoom);
		cube->prepare(&pp);
	}

	// disable the background drawer in Space Time Cube mode
	NewDrawer * backgroundDrawer = rootDrawer->getBackgroundDrawer();
	if (backgroundDrawer) {
		HTREEITEM hItemBackground = findTreeItem(backgroundDrawer);
		if (hItemBackground)
			tree->GetTreeCtrl().SetCheck(hItemBackground, !useSpaceTimeCube );
		backgroundDrawer->setActive(!useSpaceTimeCube);
	}
	
	// restore (because the drAppend changed it all)
	rootDrawer->setCoordBoundsZoom(cbZoom);
	rootDrawer->setRotationAngles(rotX, rotY, rotZ);
	rootDrawer->setTranslate(transX, transY, transZ);
	rootDrawer->setZoom3D(zoom3D);

	mpv->Invalidate();
}

void SpaceTimeCube::replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw)
{
	oldDrw->setActive(false);
	int index = rootDrawer->getDrawerIndex(oldDrw);
	if (iUNDEF != index) {
		rootDrawer->removeDrawer(oldDrw->getId(), true);
		rootDrawer->insertDrawer(index, newDrw);
		newDrw->setActive(true);
	}
}

bool SpaceTimeCube::replaceTreeItem(NewDrawer * oldDrw, SpatialDataDrawer * newDrw, int index)
{
	HTREEITEM layerItem = findTreeItem(oldDrw);
	if (layerItem != 0) { // change drawer depending on status
		HTREEITEM previous = tree->GetTreeCtrl().GetNextItem(layerItem, TVGN_PREVIOUS);
		tree->DeleteAllItems(layerItem, false);
		tree->addMapItem(newDrw, previous, index);
		return true;
	} else
		return false;
}

HTREEITEM SpaceTimeCube::findTreeItem(NewDrawer* drwFind)
{
	HTREEITEM hti= tree->GetTreeCtrl().GetNextItem(TVGN_ROOT, TVGN_CHILD);
	while ( hti) {
		LayerTreeItem *data = (LayerTreeItem*)tree->GetTreeCtrl().GetItemData(hti);
		if ( data) {
			DrawerLayerTreeItem *dlti = dynamic_cast<DrawerLayerTreeItem *>(data);
			if (dlti) {
				if (dlti->drw() == drwFind)
					return hti;
			} else {
				DisplayOptionTreeItem *doti = dynamic_cast<DisplayOptionTreeItem *>(data);
				if (doti) {
					if ( doti->drw() == drwFind)
						return hti;
				}
			}
		}
		hti= tree->GetTreeCtrl().GetNextItem(hti, TVGN_NEXT);
	}
	return 0;
}

void SpaceTimeCube::startLayerOptionsForm()
{
	update();
	CRect rect;
	bool fRestorePosition = false;
	if (layerOptionsForm) {
		fRestorePosition = true;
		layerOptionsForm->GetWindowRect(&rect);
		delete layerOptionsForm;
	}
	layerOptionsForm = new LayerOptionsForm(tree, *this, layerList);
	if (fRestorePosition)
		layerOptionsForm->SetWindowPos(tree, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOREPOSITION);
}

void SpaceTimeCube::setFormAutoDeleted()
{
	layerOptionsForm = 0;
}

bool SpaceTimeCube::showingLayerOptionsForm()
{
	return layerOptionsForm != 0;
}

void SpaceTimeCube::SetTime(double time) {
	timeOffset = time;
	mpv->Invalidate();
}

TimeBounds * SpaceTimeCube::getTimeBoundsZoom() const
{
	return timeBoundsZoom;
}

const TimeBounds * SpaceTimeCube::getTimeBoundsFullExtent() const
{
	return timeBoundsFullExtent;
}

double SpaceTimeCube::GetTime() {
	return timeOffset;
}

//------------------------------------------------------

LayerOptionsForm::LayerOptionsForm(CWnd *wPar, SpaceTimeCube & _spaceTimeCube, vector<LayerData> & layerList)
: DisplayOptionsForm(0, wPar, TR("Select Layer Options"))
, spaceTimeCube(_spaceTimeCube)
, m_layerList(layerList)
, fFirstTime(true)
{
	StaticText * stPrevious;
	for(int i=0; i < layerList.size(); ++i) {
		LayerData & layerData = layerList[i];
		StaticText * stLayerName = new StaticText(root, layerData.fnObj().sFile);
		if (i > 0)
			stLayerName->Align(stPrevious, AL_UNDER);
		stPrevious = stLayerName;
		vsPlotMethod.push_back("");
		FieldOneSelectTextOnly* fosPM = new FieldOneSelectTextOnly(root, &vsPlotMethod[vsPlotMethod.size() - 1]);
		//fosPM->SetWidth(100);
		fosPM->SetCallBack((NotifyProc)&LayerOptionsForm::ComboCallBackFunc);
		fosPM->Align(stLayerName, AL_AFTER);
		fosPlotMethod.push_back(fosPM);
		vsTimeColumnNames.push_back("");
		vsSortColumnNames.push_back("");
		vsGroupColumnNames.push_back("");
		vsSizeColumnNames.push_back("");
		vbSort.push_back(new bool);
		vbGroup.push_back(new bool);
		vbSize.push_back(new bool);
		*vbSort[vbSort.size() - 1] = false;
		*vbGroup[vbGroup.size() - 1] = false;
		*vbSize[vbSize.size() - 1] = false;
		FormEntry * feTime;
		if (layerData.isSelfTime()) {
			StaticText *stDummy = new StaticText(root, "");
			feTime = stDummy;
			fcTimeColumn.push_back(0);
		}
		else {
			FieldColumn *fcol = new FieldColumn(root, "", layerData.getAttTable(), &vsTimeColumnNames[vsTimeColumnNames.size() - 1], dmTIME);
			fcTimeColumn.push_back(fcol);
			feTime = fcol;
		}
		feTime->Align(fosPM, AL_AFTER);
		FormEntry * feSort1;
		FormEntry * feSort2;
		if (!layerData.hasSort()) {
			feSort1 = new StaticText(root, "");
			feSort2 = new StaticText(root, "");
			feSort2->Align(feSort1, AL_AFTER);
			cbSort.push_back(0);
			fcSortColumn.push_back(0);
		} else {
			CheckBox * cbsort = new CheckBox(root, "", vbSort[vbSort.size() - 1]);
			cbSort.push_back(cbsort);
			FieldColumn * fcol = new FieldColumn(cbsort, "", layerData.getAttTable(), &vsSortColumnNames[vsSortColumnNames.size() - 1], dmVALUE | dmCLASS | dmIDENT);
			fcSortColumn.push_back(fcol);
			fcol->Align(cbsort, AL_AFTER);
			feSort1 = cbsort;
			feSort2 = fcol;
		}
		feSort1->Align(feTime, AL_AFTER);
		FormEntry * feGroup1;
		FormEntry * feGroup2;
		if (!layerData.hasGroup()) {
			feGroup1 = new StaticText(root, "");
			feGroup2 = new StaticText(root, "");
			feGroup2->Align(feGroup1, AL_AFTER);
			cbGroup.push_back(0);
			fcGroupColumn.push_back(0);
		} else {
			CheckBox * cbgroup = new CheckBox(root, "", vbGroup[vbGroup.size() - 1]);
			cbGroup.push_back(cbgroup);
			FieldColumn * fcol = new FieldColumn(cbgroup, "", layerData.getAttTable(), &vsGroupColumnNames[vsGroupColumnNames.size() - 1], dmVALUE | dmCLASS | dmSTRING);
			fcGroupColumn.push_back(fcol);
			fcol->Align(cbgroup, AL_AFTER);
			feGroup1 = cbgroup;
			feGroup2 = fcol;
		}
		feGroup1->Align(feSort2, AL_AFTER);
		FormEntry * feSize1;
		FormEntry * feSize2;
		if (!layerData.hasSize()) {
			feSize1 = new StaticText(root, "");
			feSize2 = new StaticText(root, "");
			feSize2->Align(feSize1, AL_AFTER);
			cbSize.push_back(0);
			fcSizeColumn.push_back(0);
		}
		else {
			CheckBox * cbsize = new CheckBox(root, "", vbSize[vbSize.size() - 1]);
			cbSize.push_back(cbsize);
			FieldColumn *fcol = new FieldColumn(cbsize, "", layerData.getAttTable(), &vsSizeColumnNames[vsSizeColumnNames.size() - 1], dmVALUE);
			fcSizeColumn.push_back(fcol);
			fcol->Align(cbsize, AL_AFTER);
			feSize1 = cbsize;
			feSize2 = fcol;
		}
		feSize1->Align(feGroup2, AL_AFTER);
	}

	create();
}

LayerOptionsForm::~LayerOptionsForm()
{
	for (int i = 0; i < vbSort.size(); ++i)
		delete vbSort[i];
	for (int i = 0; i < vbGroup.size(); ++i)
		delete vbGroup[i];
	for (int i = 0; i < vbSize.size(); ++i)
		delete vbSize[i];
}

int LayerOptionsForm::ComboCallBackFunc(Event*)
{
	if (fFirstTime)
	{
		fFirstTime = false;
		for(int i=0; i < m_layerList.size(); ++i) {
			LayerData & layerData = m_layerList[i];
			fosPlotMethod[i]->AddString("<regular>");
			fosPlotMethod[i]->ose->SetCurSel(0);
			if (layerData.isPointMap())
				fosPlotMethod[i]->AddString("<stp>");
		}
	}

	//fisRelation->SetVal(fosRelation->ose->GetCurSel());
	return 0;
}

void LayerOptionsForm::apply() {

	for(int i=0; i < m_layerList.size(); ++i) {
		LayerData & layerData = m_layerList[i];
		if (!layerData.isSelfTime()) {
			fcTimeColumn[i]->StoreData();
			layerData.setTimeColumn(vsTimeColumnNames[i]);
		}
		cbSort[i]->StoreData();
		layerData.setUseSort(*vbSort[i]);
		if (*vbSort[i]) {
			fcSortColumn[i]->StoreData();
			layerData.setSortColumn(vsSortColumnNames[i]);
		}
		cbGroup[i]->StoreData();
		layerData.setUseGroup(*vbGroup[i]);
		if (*vbGroup[i]) {
			fcGroupColumn[i]->StoreData();
			layerData.setGroupColumn(vsGroupColumnNames[i]);
		}
		cbSize[i]->StoreData();
		layerData.setUseSize(*vbSize[i]);
		if (*vbSize[i]) {
			fcSizeColumn[i]->StoreData();
			layerData.setSizeColumn(vsSizeColumnNames[i]);
		}
		fosPlotMethod[i]->StoreData();
		layerData.setPlotOption(vsPlotMethod[i] != "<regular>");
	}

	spaceTimeCube.refreshDrawerList();

	//PreparationParameters pp(NewDrawer::ptGEOMETRY, 0);
	//drw->prepareChildDrawers(&pp);
	//updateMapView();

	
	//PreparationParameters pp(NewDrawer::ptRENDER, 0);
	//drw->prepareChildDrawers(&pp);
	//updateMapView();
}

void LayerOptionsForm::OnOK() {
	spaceTimeCube.setFormAutoDeleted();
	DisplayOptionsForm::OnOK();
}

void LayerOptionsForm::OnCancel() {
	spaceTimeCube.setFormAutoDeleted();
	DisplayOptionsForm::OnCancel();
}

//------------------------------------------------------

DrawerTool *createSpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SpaceTimeCubeTool(zv, view, drw);
}

SpaceTimeCubeTool::SpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("SpaceTimeCubeTool",zv, view, drw)
, stc(SpaceTimeCube::getSpaceTimeCube(zv, view, drw))
{
}

SpaceTimeCubeTool::~SpaceTimeCubeTool() {
	if (!IsWindow(tree->GetTreeCtrl().m_hWnd)) { // Clean-up if the user closed the mapwindow.
		SpaceTimeCube::deleteSpaceTimeCube(mpv);
	}
}

bool SpaceTimeCubeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	bool ok =  dynamic_cast<ThreeDGlobalTool *>(tool) != 0;
	if ( ok)
		parentTool = tool;
	return ok;
}

SpaceTimeCube * SpaceTimeCubeTool::getSpaceTimeCube() const
{
	return stc;
}

HTREEITEM SpaceTimeCubeTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&SpaceTimeCubeTool::setUseSpaceTimeCube);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimeCubeTool::startLayerOptionsForm);
	htiNode =  insertItem(TR("SpaceTimeCube"),"SpaceTimeCube",item,stc->fUseSpaceTimeCube());
	DrawerTool::configure(htiNode);
	if (stc->fUseSpaceTimeCube())
		stc->refreshDrawerList();
	if (stc->showingLayerOptionsForm())
		stc->startLayerOptionsForm();
	return htiNode;
}

void SpaceTimeCubeTool::makeActive(void *v, HTREEITEM) {
	bool yesno = *(bool*)v;
}

String SpaceTimeCubeTool::getMenuString() const {
	return TR("SpaceTimeCube");
}

void SpaceTimeCubeTool::startLayerOptionsForm() {
	stc->startLayerOptionsForm();
}

void SpaceTimeCubeTool::setUseSpaceTimeCube(void *v, HTREEITEM) {
	bool useSpaceTimeCube = *(bool *)v;
	stc->setUseSpaceTimeCube(useSpaceTimeCube);
	NewDrawer *drw = drawer->getRootDrawer()->getDrawer("CubeDrawer");
	drawer = drw;
	CubeElementsTool *ceTool = new CubeElementsTool(mpvGetView(), getDocument()->ltvGetView(),drawer);
	addTool(ceTool);
	ceTool->configure(htiNode);
}

