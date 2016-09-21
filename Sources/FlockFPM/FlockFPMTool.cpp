/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52°North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52°North Initiative for Geospatial
Open Source Software GmbH

Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
tel +31-534874371

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see gnu-gpl v2.txt); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
// FlockFPMTool.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "FlockFPMTool.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "SpaceTimeCube\SpaceTimePathDrawer.h"
#include "LCMFlockViz.h"
#include "FlockFPMDrawer.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Table\tblview.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\AreaSelector.h"

DrawerTool *createFlockFPMTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new FlockFPMTool(zv, view, drw);
}

FlockFPMTool::FlockFPMTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("FlockFPMTool",zv, view, drw)
{
}

FlockFPMTool::~FlockFPMTool() {
}

bool FlockFPMTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	SpaceTimePathDrawer *pdrw = dynamic_cast<SpaceTimePathDrawer *>(drawer);
	if ( !pdrw)
		return false;
	parentTool = tool;
	return true;
}

HTREEITEM FlockFPMTool::configure( HTREEITEM parentItem) {
	if ( !active)
		return parentItem;

	//htiNode = insertItem(parentItem, TR("FlockFPM"),"Transparent");

	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&FlockFPMTool::changeFlockFPM);
	//SpaceTimeDrawer *pdrw = dynamic_cast<SpaceTimeDrawer *>(drawer);
	//String transp("Edges (%d)", pdrw->iNrSteps());
	htiNode = insertItem("FlockFPM","Circle", item);

	//item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	//item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&FlockFPMTool::changeFlockFPM);
	//item->setCheckAction(this,0,(DTSetCheckFunc)&FlockFPMTool::setFlockFPMVisibility);
	//insertItem(TR("Minimum members"),"Axis",item,1);

	//item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	//item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&FlockFPMTool::changeFlockFPM);
	//item->setCheckAction(this,0,(DTSetCheckFunc)&FlockFPMTool::setFlockFPMVisibility);
	//insertItem(TR("Minimum timeframes"),"Axis",item,1);

	//DrawerTool::configure(htiNode);

	return htiNode;
}

String FlockFPMTool::getMenuString() const {
	return TR("FlockFPMTool");
}

void FlockFPMTool::changeFlockFPM() {
	new FlockFPMForm(tree, (ComplexDrawer *)drawer, htiNode);
}

class FieldFlockColumn: public FieldColumn
{
public:
	FieldFlockColumn(FormEntry* fe, const String& sQuestion, const Table& tbl, String *psName, long types = 0)
	: FieldColumn(fe, sQuestion, tbl, psName, types)
	{
	}
	void FillWithFlockColumns(TablePtr *t)
	{
		String sCol;
  
		tbl = t;
		if (0 == dynamic_cast<TableView*>(t))
			table.SetPointer(t);
		if (0 == fld->ose)
			return;
		fld->ose->ResetContent();
		if (tbl) {
			for (int i = 0; i < tbl->iCols(); ++i) {
				Column col = tbl->col(i);
				if (!col.fValid())
					continue;
				if (dmTypes) {
					Domain dm = col->dm();
					bool fAdd = false;
					if ((dmTypes & dmCLASS) && 0 != dm->pdc()) fAdd = true;
					else if ((dmTypes & dmIDENT) && 0 != dm->pdid()) fAdd = true;
					else if ((dmTypes & dmVALUE) && 0 != dm->pdv() && 0 == dm->pdbool()) fAdd = true;
					else if ((dmTypes & dmGROUP) && 0 != dm->pdgrp()) fAdd = true;
					else if ((dmTypes & dmIMAGE) && 0 != dm->pdi()) fAdd = true;
					else if ((dmTypes & dmPICT)  && 0 != dm->pdp()) fAdd = true;
					else if ((dmTypes & dmCOLOR) && 0 != dm->pdcol()) fAdd = true;
					else if ((dmTypes & dmBOOL)  && 0 != dm->pdbool()) fAdd = true;
					else if ((dmTypes & dmBIT)   && 0 != dm->pdbit()) fAdd = true;
					else if ((dmTypes & dmSTRING)&& 0 != dm->pds()) fAdd = true;
					else if ((dmTypes & dmCOORD) && 0 != dm->pdcrd()) fAdd = true;
					else if ((dmTypes & dmUNIQUEID)&& 0 != dm->pdUniqueID()) fAdd = true;				
					if (!fAdd)
						continue;
				}
				RangeInt ri (col->riMinMax());
				if ((ri.fValid() && RangeInt(0,1).fContains(ri)) || (!ri.fValid()))
				{
					sCol = col->sName();
					sCol &= ".clm";
					fld->ose->AddString(sCol.sVal());
				}
			}
		}
		sCol = _sName.sUnQuote();
		if ( sCol != "")
		{
			sCol &= ".clm";
			fld->ose->SelectString(-1, sCol.c_str());
		}
		else
			fld->ose->SetCurSel(0);
	}
};

FlockFPMForm::FlockFPMForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti)
: DisplayOptionsForm(dr,wPar,"FlockFPM")
, transparency(0)
, color(Color(255, 0, 0))
, visible(true)
, selectable(false)
//, rSampleInterval((1.0/24.0)/60.0)
, fInit(true)
, ffpmSelect(0)
{
	flockFPMDrawer = (FlockFPMDrawer*)(drw->getRootDrawer()->getDrawer("FlockFPMDrawer"));
	if (flockFPMDrawer) {
		mu = flockFPMDrawer->getMu();
		epsilon = flockFPMDrawer->getEpsilon();
		delta = flockFPMDrawer->getDelta();
		iSampleInterval = round(flockFPMDrawer->getSampleInterval() * 24.0 * 60.0 * 60.0);
	} else {
		mu = 3;
		epsilon = 200;
		delta = 4;
		SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;
		long numberOfFeatures = stpdrw->getFeatures().size();
		const Column & colTime = stpdrw->getTimeAttribute();
		if (numberOfFeatures > 1) {
			iSampleInterval = iUNDEF;
			for (int i = 2; i <= min(20, numberOfFeatures); ++i) { // use first 20 samples to guess the sample rate
				double rSampleInterval = colTime->rValue(i) - colTime->rValue(i - 1);
				if (rSampleInterval > 0)
					iSampleInterval = (iSampleInterval != iUNDEF) ? min(iSampleInterval, round(rSampleInterval * 24.0 * 60.0 * 60.0)) : round(rSampleInterval * 24.0 * 60.0 * 60.0);
			}
			iSampleInterval = max(iSampleInterval, 1);
		} else
			iSampleInterval = 300; // user has supplied bad data; this is just to have something there
	}
	new FieldInt(root, TR("Minumim members per flock"), &mu);
	new FieldReal(root, TR("Search radius"), &epsilon);
	new FieldInt(root, TR("Time sampling interval (seconds)"), &iSampleInterval);
	FieldInt * fiDelta = new FieldInt(root, TR("Minimum flock duration (nr samples)"), &delta, ValueRange(1, 200), true);
	fiDelta->SetCallBack((NotifyProc)&FlockFPMForm::setDelta);

	cbVisible = new CheckBox(root, TR("Visible"), &visible);
	cbVisible->SetCallBack((NotifyProc)&FlockFPMForm::setVisible);

	cbSelectable = new CheckBox(root, TR("Selectable"), &selectable);
	cbSelectable->SetCallBack((NotifyProc)&FlockFPMForm::setSelectable);

	slider = new FieldIntSliderEx(root,"Transparency(0-100)", &transparency,ValueRange(0,100),true);
	slider->SetCallBack((NotifyProc)&FlockFPMForm::setTransparency);
	slider->setContinuous(true);

	colorPicker = new FieldColor(root, TR("Color"), &color);
	colorPicker->SetCallBackForAll((NotifyProc)&FlockFPMForm::setColor);
	//for (int i = 0; i < colorPicker->childlist().size(); ++i)
	//	colorPicker->childlist()[i]->SetCallBack((NotifyProc)&FlockFPMForm::setColor); // it is no use to add this to the FieldColor, it will not respond
	fgStoreColumn = new FieldGroup(root);

	fldCol = new FieldFlockColumn(fgStoreColumn, "", Table(), &sFlockCol, dmVALUE);
	fldCol->SetCallBack((NotifyProc)&FlockFPMForm::ColCallBackFunc);
	//fldCol->Align(colorPicker, AL_UNDER);
	//fldCol->psn->SetBound(0,-1,0,0); // to "connect" to button
	fldCol->psn->iBndRight = -1;
	OwnButtonSimple* pbCreateColumn = new OwnButtonSimple(fgStoreColumn, "CreateBut", (NotifyProc)&FlockFPMForm::CallBackAddColumn, false, false);
	pbCreateColumn->Align(fldCol, AL_AFTER);
	pbCreateColumn->SetIndependentPos();
	PushButton* pb = new PushButton(fgStoreColumn, TR("Store Flocks in Column"), (NotifyProc)&FlockFPMForm::StoreFlocks);
	pb->Align(pbCreateColumn, AL_AFTER);
	ftcFlockTable = new FieldTableCreate(fgStoreColumn, "", &sFlockTable);
	ftcFlockTable->Align(fldCol, AL_UNDER);
	pb = new PushButton(fgStoreColumn, TR("Store Flocks in Table"), (NotifyProc)&FlockFPMForm::WriteFlockTable);
	pb->Align(ftcFlockTable, AL_AFTER);

	SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;
	if (flockFPMDrawer)
		ffpmSelect = new FlockFPMSelectTool(view->GetDocument()->mpvGetView(), stpdrw->getBasemap(), flockFPMDrawer->getSelectedRaws(), dynamic_cast<LayerDrawer *>(flockFPMDrawer), cbSelectable);

	create();

	fInit = false;

	if (!flockFPMDrawer) {
		cbVisible->Hide();
		cbSelectable->Hide();
		slider->Hide();
		colorPicker->Hide();
		fgStoreColumn->Hide();
	}

	Table tbl = stpdrw->getBasemap()->tblAtt();
	if (tbl.fValid())
		FillWithSuitableColumns(tbl);
}

FlockFPMForm::~FlockFPMForm()
{
	delete ffpmSelect;
}

int FlockFPMForm::StoreFlocks(Event *ev) {
	fldCol->StoreData();
	if (flockFPMDrawer) {
		map<int, vector<int>> & flockIDs = flockFPMDrawer->getFlockIDs();
		SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;
		const vector<Feature *> & features = stpdrw->getFeatures();
		const bool fUseGroup = stpdrw->fGetUseGroup();
		long numberOfFeatures = features.size();
		String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? stpdrw->getGroupValue(features[0]) : "";
		bool fUseIDColumn = (sLastGroupValue.iVal() != iUNDEF);
		Table tbl = stpdrw->getBasemap()->tblAtt();
		if (tbl.fValid()) {
			Column col (tbl->col(sFlockCol));
			int trajectoryId = 0;
			map<int, vector<int>>::iterator found = flockIDs.find(fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId);
			for(long i = 0; i < numberOfFeatures; ++i) {
				Feature *feature = features[i];
				if (fUseGroup && sLastGroupValue != stpdrw->getGroupValue(feature)) {
					sLastGroupValue = stpdrw->getGroupValue(feature);
					++trajectoryId;
					found = flockIDs.find(fUseIDColumn ? sLastGroupValue.iVal() : trajectoryId);
				}
				if (found != flockIDs.end())
					col->PutVal(i + 1, (long)1); // TODO: follow sort-by order!! .. and also the flockID does 1 + the original flockID
				else
					col->PutVal(i + 1, (long)0);
			}
			tbl->Updated();
			updateMapView();
		}
	}
	return 0;
}

std::string FlockFPMForm::getPointsString(vector<int> & pointIDs) {
	char buf [256];
	std::string ret = "[";
	for (unsigned int i = 0; i < pointIDs.size() - 1; ++i) {
		sprintf(buf, "%d, ", pointIDs[i]);
		ret += buf;
	}
	sprintf(buf, "%d]", pointIDs[pointIDs.size() - 1]);
	ret += buf;
	return ret;
}

int FlockFPMForm::WriteFlockTable(Event *ev) {
	ftcFlockTable->StoreData();
	FileName fnFlockTable(sFlockTable, ".tbt", true);
	double rSampleInterval = ((((double)iSampleInterval) / 24.0) / 60.0) / 60.0;
	if (flockFPMDrawer && sFlockTable.length() > 0 && fnFlockTable.fValid()) {
		SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;
		const Column & colTime = stpdrw->getTimeAttribute();
		double timeOffset = colTime->rrMinMax().rLo();
		vector<vector<Coord>> & flocks = flockFPMDrawer->getFlocks();
		vector<vector<int>> & trajectoryIDs = flockFPMDrawer->getTrajectoryIDs();
		long size = flocks.size();
		long iTotalRows = 0;
		for (long i = 0; i < size; ++i)
			iTotalRows += flocks[i].size();
		Domain flockDom (FileName(fnFlockTable, ".dom", true), iTotalRows, dmtUNIQUEID);
		Table flockTbl (fnFlockTable, flockDom);
		Column colFlockID (flockTbl, "FlockID", DomainValueRangeStruct(0, size - 1, 1));
		Column colX (flockTbl, "X", DomainValueRangeStruct(-1e8,1e8, 0));
		Column colY (flockTbl, "Y", DomainValueRangeStruct(-1e8,1e8, 0));
		Column colT (flockTbl, "T", colTime->dvrs());
		Column colMembers (flockTbl, "Members", Domain("String"));
		Column colNrMembers (flockTbl, "NrMembers", DomainValueRangeStruct(0, 10000, 1));

		long row = 1;

		for (long i = 0; i < size; ++i) {
			vector<Coord> & flock = flocks[i];
			vector<int> & finalPoints = trajectoryIDs[i];
			std::string sMembers = getPointsString(finalPoints);
			int flockSize = flock.size();
			for (int j = 0; j < flockSize; ++j) {
				Coord & crd = flock[j];
				colFlockID->PutVal(row, i);
				colX->PutVal(row, crd.x);
				colY->PutVal(row, crd.y);
				double time = timeOffset + crd.z * rSampleInterval;
				colT->PutVal(row, time);
				colMembers->PutVal(row, sMembers);
				colNrMembers->PutVal(row, (long)(finalPoints.size()));
				++row;
			}
		}
	}

	return 0;
}

int FlockFPMForm::ColCallBackFunc(Event *ev) {
	return 1;
}

int FlockFPMForm::CallBackAddColumn(Event *ev) {
	class NewColumnForm: public FormWithDest
	{
	public:
		NewColumnForm(CWnd* parent)
		: FormWithDest(parent, TR("Add Attribute"))
		,	vr(0, 1) // 0 = no-flock, 1 = yes-flock
		{
			sDomName = "value.dom";
			sDescr = "flock ID";
			new FieldString(root, TR("&Attribute Name"), &sColName, Domain(), false);
			create();
		}
		String sColName;
		String sDomName;
		String sDescr;
		ValueRange vr;
	};

	if (!fInit && flockFPMDrawer) {
		SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;
		Table tbl = stpdrw->getBasemap()->tblAtt();
		if (!tbl.fValid())
			return 0;
		if (tbl->fDataReadOnly()) 
			return 0;
		try
		{
			NewColumnForm frm(wnd());
			if (frm.fOkClicked())
			{
				if ("" == frm.sColName)
					return 0;

				Domain dm(frm.sDomName);
				if (!dm.fValid())
					return 0; // strange if this happens

				DomainValueRangeStruct dvrs(dm, frm.vr);
			
				Column col;
				try
				{
					col = Column(tbl, frm.sColName, dvrs);
				}
				catch (CMemoryException* err)
				{
					AfxMessageBox(TR("Could not create column (not enough memory)").c_str());
					err->Delete();
				}
				if (!col.fValid()) // failed to create .. also if it already exists
				{
					fldCol->SetVal(frm.sColName); // if exists, try to display anyway
					return 0;
				}
				col->SetDescription(frm.sDescr);  
				col->Fill();
				FillWithSuitableColumns(tbl);
				fldCol->SetVal(frm.sColName);
			}    
		}
		catch (ErrorObject& err)
		{
			err.Show();
		}
	}

	return 0;
}

void FlockFPMForm::FillWithSuitableColumns(Table & tbl)
{
	try
	{
		if (tbl.fValid())
			fldCol->FillWithFlockColumns(tbl.ptr());
		else
			fldCol->FillWithColumns("");
	}
	catch (ErrorObject&)
	{
		// -- just ignore it
	}
}

int FlockFPMForm::setDelta(Event *ev) {
	if (!fInit && flockFPMDrawer) {
		root->StoreData();
		double rSampleInterval = ((((double)iSampleInterval) / 24.0) / 60.0) / 60.0;
		if (flockFPMDrawer->getSampleInterval() != rSampleInterval) {
			flockFPMDrawer->setSampleInterval(rSampleInterval);
			PreparationParameters pp(NewDrawer::ptALL);
			flockFPMDrawer->prepare(&pp);
		}
		Tranquilizer trq(TR("Computing FPM Flocks"));
		flockFPMDrawer->recomputeFlocks(mu, epsilon, delta, trq);
		updateMapView();
	}
	return 1;
}

int FlockFPMForm::setVisible(Event *ev) {
	if (flockFPMDrawer) {
		cbVisible->StoreData();
		flockFPMDrawer->setActive(visible);
		updateMapView();
	}
	return 1;
}

int FlockFPMForm::setSelectable(Event *ev) {
	if (flockFPMDrawer) {
		cbSelectable->StoreData();

		flockFPMDrawer->setSelectable(selectable);
		ffpmSelect->setSelectable(selectable);
	}
	return 1;
}

int FlockFPMForm::setTransparency(Event *ev) {
	if (flockFPMDrawer) {
		slider->StoreData();
		flockFPMDrawer->setAlpha(1.0 - (double)transparency/100.0);
		updateMapView();
	}
	return 1;
}

int FlockFPMForm::setColor(Event *ev) {
	if (flockFPMDrawer) {
		colorPicker->StoreData();
		flockFPMDrawer->setSingleColor(color);
		PreparationParameters pp(NewDrawer::ptRENDER);
		flockFPMDrawer->prepare(&pp);
		updateMapView();
	}
	return 1;
}

void FlockFPMForm::apply() {
	root->StoreData();
	SpaceTimePathDrawer *stpdrw = (SpaceTimePathDrawer*)drw;

	//PreparationParameters pp(NewDrawer::ptGEOMETRY, 0);
	//cdrw->prepare(&pp);

	RootDrawer * rootDrawer = drw->getRootDrawer();

	bool fCreate = true;
	if (fCreate) {
		if (flockFPMDrawer == 0) {
			flockFPMDrawer = (FlockFPMDrawer*)(rootDrawer->getDrawer("FlockFPMDrawer"));
			if (flockFPMDrawer == 0) {
				//DrawerParameters dp(rootDrawer, rootDrawer);
				DrawerParameters dp(rootDrawer, stpdrw->getParentDrawer());
				flockFPMDrawer = (FlockFPMDrawer*)(NewDrawer::getDrawer("FlockFPMDrawer", "FlockFPM", &dp));
				int index = rootDrawer->getDrawerIndex(stpdrw->getParentDrawer());
				if (index != iUNDEF)
					rootDrawer->insertDrawer(index + 1, flockFPMDrawer);
				else
					rootDrawer->insertDrawer(0, flockFPMDrawer);
			}
			ffpmSelect = new FlockFPMSelectTool(view->GetDocument()->mpvGetView(), stpdrw->getBasemap(), flockFPMDrawer->getSelectedRaws(), dynamic_cast<LayerDrawer *>(flockFPMDrawer), cbSelectable);
			TemporalDrawer * temporalDrawer = dynamic_cast<TemporalDrawer*>(flockFPMDrawer);
			temporalDrawer->SetTimeBounds(const_cast<TimeBounds*>(stpdrw->getTimeBounds()));
			flockFPMDrawer->setSpaceTimePathDrawer(stpdrw);
			cbVisible->Show();
			cbSelectable->Show();
			slider->Show();
			colorPicker->Show();
			fgStoreColumn->Show();
		}
		double rSampleInterval = ((((double)iSampleInterval) / 24.0) / 60.0) / 60.0;
		if (flockFPMDrawer->getSampleInterval() != rSampleInterval) {
			flockFPMDrawer->setSampleInterval(rSampleInterval);
			PreparationParameters pp(NewDrawer::ptALL);
			flockFPMDrawer->prepare(&pp);
		}
		Tranquilizer trq(TR("Computing FPM Flocks"));
		flockFPMDrawer->recomputeFlocks(mu, epsilon, delta, trq);
	} else {
		if (flockFPMDrawer != 0)
			rootDrawer->removeDrawer(flockFPMDrawer->getId());
	}

	updateMapView();
}

FlockFPMSelectTool::FlockFPMSelectTool(ZoomableView* zv, BaseMapPtr *bmpp, vector<long> * selRaws, LayerDrawer *ldrw, CheckBox * cbSel)
: MapPaneViewTool(zv)
, fCtrl(false)
, fShift(false)
, as(0)
, selectedRaws(selRaws)
, bmapptr(bmpp)
, layerDrawer(ldrw)
, cbSelectable(cbSel)
{
	stay = true;
}

FlockFPMSelectTool::~FlockFPMSelectTool() {
	mpv->changeStateTool(ID_SELECTFEATURES, false);
}

void FlockFPMSelectTool::setSelectable(bool selectable)
{
	if ( selectable) {
		mpv->noTool();
		if (!mpv->addTool(this, ID_SELECTFEATURES)) // addTool(this, getId());
			mpv->changeStateTool(ID_SELECTFEATURES, true);		

		if (mpv->fAdjustSize)
			as = new AreaSelector(mpv, this, (NotifyRectProc)&FlockFPMSelectTool::FeatureAreaSelected, Color(0,255,0,200));
		else 
			as = new AreaSelector(mpv, this, (NotifyRectProc)&FlockFPMSelectTool::FeatureAreaSelected, mpv->dim, Color(0,255,0,200));
		as->SetCursor(zCursor("Edit"));
		as->setActive(true);
	} else {
		mpv->changeStateTool(ID_SELECTFEATURES, false);
		if (as) {
			as->Stop();
			if ( as->stayResident() == false) {
				delete as;
				as = 0;
			}
		}

		//layerDrawer->select(false); // deselect all points
		selectedRaws->clear();
		mpv->Invalidate();
	}
}

void FlockFPMSelectTool::Stop()
{
	layerDrawer->setSelectable(false);
	if (as) {
		as->Stop();
		if (!as->stayResident()) {
			delete as;
			as = 0;
		}
	}
	//if (tree->m_hWnd)
	//	tree->GetTreeCtrl().SetCheck(htiNode, false);
	cbSelectable->SetVal(false);
}

void FlockFPMSelectTool::OnMouseMove(UINT nFlags, CPoint point)
{
	if (as)
		as->OnMouseMove(nFlags, point);
}

void FlockFPMSelectTool::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (as)
		as->OnLButtonDown(nFlags, point);
}

void FlockFPMSelectTool::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (as)
		as->OnLButtonUp(nFlags, point);
}

bool FlockFPMSelectTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	return fCtrl || fShift;
}

bool FlockFPMSelectTool::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){
	fCtrl = GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	fShift = GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	return fCtrl || fShift;
}

void FlockFPMSelectTool::OnEscape() {
	//layerDrawer->select(false); // deselect all points
	selectedRaws->clear();
	IlwWinApp()->SendUpdateTableSelection(*selectedRaws, bmapptr->dm()->fnObj, (long)this);
	mpv->Invalidate();
}

void FlockFPMSelectTool::FeatureAreaSelected(CRect rect)
{
	if (!layerDrawer)
		return;
	if (!isActive())
		return;
	layerDrawer->select(rect, *selectedRaws, fCtrl ? LayerDrawer::SELECTION_ADD : (fShift ? LayerDrawer::SELECTION_REMOVE : LayerDrawer::SELECTION_NEW));
	IlwWinApp()->SendUpdateTableSelection(*selectedRaws, bmapptr->dm()->fnObj, (long)this);
	mpv->Invalidate();
}
