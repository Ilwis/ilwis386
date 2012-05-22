#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
//#include "Engine\Drawers\SimpleDrawer.h" 

//#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
//#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "TimePositionBar.h"
#include "SpaceTimeCubeTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\MapPaneView.h"
#include "DrawersUI\ThreeDGlobalTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "DrawersUI\SetDrawerTool.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "TemporalDrawer.h"
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
#include "Client\Base\datawind.h"

LayerData::LayerData(NewDrawer *drw)
: drawer(drw)
, plotOption(false)
, sizeOption(false)
, fSelfTime(false)
, fSelfSize(false)
, fFeatureMap(false)
, fPointMap(false)
{
	if ( drw->getType() == "FeatureDataDrawer") {
		BaseMapPtr *bmp = ((FeatureDataDrawer *)drawer)->getBaseMap();
		fnBaseMap = bmp->fnObj;
		fFeatureMap = IOTYPEFEATUREMAP(fnBaseMap);
		fPointMap = IOTYPE(fnBaseMap) == IlwisObject::iotPOINTMAP;
		fSelfTime = bmp->dm()->pdtime() != 0;
		fSelfSize = bmp->dm()->pdv() != 0;
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
		if (fSelfSize)
			m_rrSizeMinMax = bmp->rrMinMax();
		else if (bmp->fTblAtt()) {
			attTable = bmp->tblAtt();
			for (int i = 0; i < attTable->iCols(); ++i) {
				Column col = attTable->col(i);
				if (col->dm()->pdv())
					sizeColumns.push_back(col);
			}
			if (sizeColumns.size() == 1) {
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

bool LayerData::getPlotOption() {
	return plotOption && (fSelfTime || temporalColumn.fValid());
}

bool LayerData::getSizeOption() {
	return sizeOption && (fSelfSize || sizeColumn.fValid());
}

void LayerData::setTimeColumn(String sColName) {
	temporalColumn = attTable->col(sColName);
	m_rrTimeMinMax = temporalColumn->rrMinMax();
}

void LayerData::setSizeColumn(String sColName) {
	sizeColumn = attTable->col(sColName);
	m_rrSizeMinMax = sizeColumn->rrMinMax();
}

DrawerTool *createSpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new SpaceTimeCubeTool(zv, view, drw);
}

SpaceTimeCubeTool::SpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("SpaceTimeCubeTool",zv, view, drw)
, timeBounds(new TimeBounds())
, useSpaceTimeCube(false)
, preTimeOffset(0)
, postTimeOffset(0)
, timePosBar(0)
, layerOptionsForm(0)
{
}

SpaceTimeCubeTool::~SpaceTimeCubeTool() {
	if (useSpaceTimeCube) {
		if (timePosBar)
			delete timePosBar;
		if (IsWindow(tree->GetTreeCtrl().m_hWnd)) { // No need to do this if the user closed the mapwindow.
			mpv->dwParent()->RecalcLayout();
			useSpaceTimeCube = false;
			refreshDrawerList();
		}
	}
	if (layerOptionsForm)
		delete layerOptionsForm;
	if (timeBounds)
		delete timeBounds;
}

bool SpaceTimeCubeTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	bool ok =  dynamic_cast<ThreeDGlobalTool *>(tool) != 0;
	if ( ok)
		parentTool = tool;
	return ok;
}

HTREEITEM SpaceTimeCubeTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0,(DTSetCheckFunc)&SpaceTimeCubeTool::setSpaceTimeCube);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&SpaceTimeCubeTool::startLayerOptionsForm);
	htiNode =  insertItem(TR("SpaceTimeCube"),"SpaceTimeCube",item,0);
	DrawerTool::configure(htiNode);
	return htiNode;
}

void SpaceTimeCubeTool::makeActive(void *v, HTREEITEM) {
	bool yesno = *(bool*)v;
}

String SpaceTimeCubeTool::getMenuString() const {
	return TR("SpaceTimeCube");
}

void SpaceTimeCubeTool::update() {

	ComplexDrawer *rootDrawer = (ComplexDrawer *)drawer->getRootDrawer();
	vector<LayerData> newLayerList;
	for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
		NewDrawer *drw = rootDrawer->getDrawer(i);
		if (find(ownDrawerIDs.begin(), ownDrawerIDs.end(), drw->getId()) == ownDrawerIDs.end()) {
			int j = 0;
			// if it is in the old list, copy it
			for (; j < layerList.size(); ++j) {
				if (layerList[j].getDrawer()->getId() == drw->getId()) {
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

void SpaceTimeCubeTool::setSpaceTimeCube(void *v, HTREEITEM) {
	useSpaceTimeCube = *(bool *)v;
	if (useSpaceTimeCube) {
		timePosBar = new TimePositionBar();
		timePosBar->Create(mpv->dwParent());
		if (preTimeOffset != 0)
			timePosBar->SetPreTimeOffsetDrawer(preTimeOffset);
		mpv->GetParentFrame()->DockControlBar(timePosBar, AFX_IDW_DOCKBAR_LEFT);
		refreshDrawerList();
	}
	else {
		refreshDrawerList();
		delete timePosBar;
		timePosBar = 0;
		mpv->dwParent()->RecalcLayout();
	}
	//drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void SpaceTimeCubeTool::refreshDrawerList() {
	RootDrawer *rootDrawer = drawer->getRootDrawer();
	CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom(); // backup values
	double rotX, rotY, rotZ, transX, transY, transZ;
	rootDrawer->getRotationAngles(rotX, rotY, rotZ);
	rootDrawer->getTranslate(transX, transY, transZ);
	double zoom3D = rootDrawer->getZoom3D();


	for (int i = 0; i < ownDrawerIDs.size(); ++i)
		rootDrawer->removeDrawer(ownDrawerIDs[i]);
	ownDrawerIDs.clear();

	if (preTimeOffset != 0 && postTimeOffset != 0) {
		for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
			ComplexDrawer *drw = dynamic_cast<ComplexDrawer*>(rootDrawer->getDrawer(i));
			if (drw != 0) {
				drw->removeDrawer(preTimeOffset->getId(), false);
				drw->removeDrawer(postTimeOffset->getId(), false);
			}
		}
	}

	update();		

	int n = rootDrawer->getDrawerCount();

	/*
	for(int i = 0 ; i < n; ++i) {
		if ( spaceTimePathList.size() > 0) {
			if ( !spaceTimePathList[i].status)
				continue;
		}

		SpatialDataDrawer *drw = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
		if ( !drw)
			continue;
		for(int j = 0 ; j < drw->getDrawerCount(); ++j) {
			ComplexDrawer *cdrw = (ComplexDrawer *)drw->getDrawer(j);
		}
	}

	for (int i = 0; i < spaceTimePathList.size(); ++i) {
		if (spaceTimePathList[i].status) { // change drawer to SpaceTimePathDrawer
			TreeItem titem;
			if ( tree->getItem(spaceTimePathList[i].item, TVIF_TEXT | TVIF_HANDLE, titem)) {
				String layerPath ("|%s", titem.item.pszText);
				HTREEITEM layerItem = tree->getItemHandle(layerPath);
				tree->DeleteAllItems(layerItem, true);
			}
		} else { // restore PointMapDrawer
		}
	}
	*/

	timeBounds->Reset();
	sizeStretch = RangeReal();

	for (int i = 0; i < layerList.size(); ++i) {
		//TreeItem titem;
		//if ( tree->getItem(spaceTimePathList[i].item, TVIF_TEXT | TVIF_HANDLE, titem)) {
			//String layerPath ("|%s", titem.item.pszText);
			//HTREEITEM layerItem = tree->getItemHandle(layerPath);
			HTREEITEM layerItem = getLayerHandle(layerList[i].getDrawer());

			if (layerItem != 0) { // change drawer depending on status
				SpatialDataDrawer * oldDrw = (SpatialDataDrawer*)(layerList[i].getDrawer());
				IlwisObjectPtr * object = oldDrw->getObject();
				//const String& type="FeatureDataDrawer";
				//const String& subtype="ilwis38";
				const String& subtype = (useSpaceTimeCube && layerList[i].getPlotOption()) ? "Cube" : "ilwis38";
				//DrawerParameters parms(rootDrawer, rootDrawer);
				//NewDrawer *newDrw = NewDrawer::getDrawer(type, subtype, &parms);
				BaseMap bm(object->fnObj);
				getDocument()->drAppend(bm, IlwisDocument::otUNKNOWN, IlwisWinApp::osNormal, subtype);
				int index = rootDrawer->getDrawerCount() - 1;
				NewDrawer * newDrw = rootDrawer->getDrawer(index);
				rootDrawer->removeDrawer(newDrw->getId(), false); // immediately remove it, because we don't intend to append it to the end
				HTREEITEM previous = tree->GetTreeCtrl().GetNextItem(layerItem, TVGN_PREVIOUS);
				tree->DeleteAllItems(layerItem, false);
				index = rootDrawer->getDrawerIndex(oldDrw);
				replaceDrawer(oldDrw, newDrw);
				if (rootDrawer->is3D()) {
					PreparationParameters pp(NewDrawer::pt3D);
					newDrw->prepare(&pp);
				}
				tree->addMapItem((SpatialDataDrawer*)newDrw, previous, index);
				layerList[i].setDrawer(newDrw);
				TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
				if (temporalDrawer) {
					temporalDrawer->SetTimeBounds(timeBounds);
					RangeReal rrMinMax (layerList[i].rrTimeMinMax());
					timeBounds->AddMinMax(Time(rrMinMax.rLo()), Time(rrMinMax.rHi()));
					if (layerList[i].isSelfTime())
						temporalDrawer->SetSelfTime();
					else
						temporalDrawer->SetTimeAttribute(layerList[i].getTimeColumn());
				}
				SizableDrawer * sizableDrawer = dynamic_cast<SizableDrawer*>(((ComplexDrawer*)newDrw)->getDrawer(0));
				if (sizableDrawer) {
					sizableDrawer->SetSizeStretch(&sizeStretch);
					RangeReal rrMinMax (layerList[i].rrSizeMinMax());
					sizeStretch += rrMinMax;
					if (layerList[i].isSelfSize())
						sizableDrawer->SetSelfSize();
					else
						sizableDrawer->SetSizeAttribute(layerList[i].getSizeColumn());
				}
			}
		//}
	}
	//getDocument()->UpdateAllViews(0);
	if (useSpaceTimeCube) {

		DrawerParameters dp(rootDrawer, rootDrawer);
		PreparationParameters pp(NewDrawer::ptALL);
		if (preTimeOffset == 0) {
			preTimeOffset = dynamic_cast<PreTimeOffsetDrawer*>(NewDrawer::getDrawer("PreTimeOffsetDrawer", "Cube", &dp));
			timePosBar->SetPreTimeOffsetDrawer(preTimeOffset);
		}
		//rootDrawer->insertDrawer(0, timeOffset);
		//ownDrawerIDs.push_back(timeOffset->getId());
		TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(preTimeOffset);
		temporalDrawer->SetTimeBounds(timeBounds);
		preTimeOffset->prepare(&pp);
		if (postTimeOffset == 0)
			postTimeOffset = dynamic_cast<PostTimeOffsetDrawer*>(NewDrawer::getDrawer("PostTimeOffsetDrawer", "Cube", &dp));
		//ownDrawerIDs.push_back(timeOffset->getId());
		temporalDrawer = dynamic_cast<TemporalDrawer*>(postTimeOffset);
		temporalDrawer->SetTimeBounds(timeBounds);
		postTimeOffset->prepare(&pp);

		for(int i = 0 ; i < rootDrawer->getDrawerCount(); ++i) {
			ComplexDrawer *drw = dynamic_cast<ComplexDrawer*>(rootDrawer->getDrawer(i));
			if (drw != 0) {
				if (find(ownDrawerIDs.begin(), ownDrawerIDs.end(), drw->getId()) == ownDrawerIDs.end()) {
					int j = 0;
					// if it is in the layerList, and it uses time, leave it
					for (; j < layerList.size(); ++j) {
						if (layerList[j].getDrawer()->getId() == drw->getId()) {
							if (!layerList[j].getPlotOption())
								j = layerList.size();
							else
								break; // keep j same
						}
					}
					// if not found in the layerList, or it does not use time, handle it
					if (j >= layerList.size()) {
						drw->addPreDrawer(0, preTimeOffset);
						drw->addPostDrawer(999, postTimeOffset);
					}
				}
			}
		}

		NewDrawer * cube = NewDrawer::getDrawer("CubeDrawer", "Cube", &dp);
		rootDrawer->insertDrawer(0, cube);
		ownDrawerIDs.push_back(cube->getId());
		temporalDrawer = dynamic_cast<TemporalDrawer*>(cube);
		temporalDrawer->SetTimeBounds(timeBounds);
		cube->prepare(&pp);
	}
	// restore (because the drAppend changed it all)
	rootDrawer->setCoordBoundsZoom(cbZoom);
	rootDrawer->setRotationAngles(rotX, rotY, rotZ);
	rootDrawer->setTranslate(transX, transY, transZ);
	rootDrawer->setZoom3D(zoom3D);
	
	mpv->Invalidate();
}

HTREEITEM SpaceTimeCubeTool::getLayerHandle(NewDrawer* drwFind)
{
	HTREEITEM hti= tree->GetTreeCtrl().GetNextItem(TVGN_ROOT, TVGN_CHILD);
	while ( hti) {
		LayerTreeItem *data = (LayerTreeItem*)tree->GetTreeCtrl().GetItemData(hti);
		if ( data) {
			DrawerLayerTreeItem *dlti = dynamic_cast<DrawerLayerTreeItem *>(data);
			if ( dlti) {
				SpatialDataDrawer *spdrw = dynamic_cast<SpatialDataDrawer *>(dlti->drw());
				if (spdrw == drwFind)
					return hti;
				/*
				if ( spdrw) {
					IlwisObjectPtr *obj = spdrw->getObject();
					objects.push_back(obj);
				}
				*/
			}
		}
		hti= tree->GetTreeCtrl().GetNextItem(hti, TVGN_NEXT);
	}
	return 0;
}

void SpaceTimeCubeTool::replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw)
{
	ComplexDrawer * rootDrawer = drawer->getRootDrawer();
	oldDrw->setActive(false);
	int index = rootDrawer->getDrawerIndex(oldDrw);
	if (iUNDEF != index) {
		//rootDrawer->removeDrawer(oldDrw->getId(), false);
		rootDrawer->removeDrawer(oldDrw->getId(), true);
		rootDrawer->insertDrawer(index, newDrw);
		newDrw->setActive(true);
	}
}

void SpaceTimeCubeTool::startLayerOptionsForm()
{
	update();
	if (layerOptionsForm)
		delete layerOptionsForm;
	layerOptionsForm = new LayerOptionsForm ((ComplexDrawer*)drawer, tree, *this, layerList);
}

void SpaceTimeCubeTool::setFormAutoDeleted()
{
	layerOptionsForm = 0;
}

//------------------------------------------------------
LayerOptionsForm::LayerOptionsForm(ComplexDrawer * drawer, CWnd *wPar, SpaceTimeCubeTool & _spaceTimeCubeTool, vector<LayerData> & layerList)
: DisplayOptionsForm(drawer, wPar, TR("Select Layer Options"))
, spaceTimeCubeTool(_spaceTimeCubeTool)
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
		vsSizeColumnNames.push_back("");
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
		FormEntry * feSize;
		if (layerData.isSelfSize()) {
			StaticText *stDummy = new StaticText(root, "");
			feSize = stDummy;
			fcSizeColumn.push_back(0);
		}
		else {
			FieldColumn *fcol = new FieldColumn(root, "", layerData.getAttTable(), &vsSizeColumnNames[vsSizeColumnNames.size() - 1], dmVALUE);
			fcSizeColumn.push_back(fcol);
			feSize = fcol;
		}
		feSize->Align(feTime, AL_AFTER);
	}

	create();
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
		if (!layerData.isSelfSize()) {
			fcSizeColumn[i]->StoreData();
			layerData.setSizeColumn(vsSizeColumnNames[i]);
		}
		fosPlotMethod[i]->StoreData();
		layerData.setPlotOption(vsPlotMethod[i] != "<regular>");
	}

	spaceTimeCubeTool.refreshDrawerList();

	//PreparationParameters pp(NewDrawer::ptGEOMETRY, 0);
	//drw->prepareChildDrawers(&pp);
	//updateMapView();

	
	//PreparationParameters pp(NewDrawer::ptRENDER, 0);
	//drw->prepareChildDrawers(&pp);
	//updateMapView();
}

int LayerOptionsForm::exec() {
	spaceTimeCubeTool.setFormAutoDeleted();
	return DisplayOptionsForm::exec();
}