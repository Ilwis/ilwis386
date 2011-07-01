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
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
// TrackerTool.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\Module.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Drawers\LineDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Base\datawind.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Engine\Domain\dmcoord.h"
#include "Tracking\TrackDrawer.h"
#include "Tracking\TrackerTool.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Tracking.H"
#include "Engine\Drawers\SpatialDataDrawer.h"


using namespace ILWIS;

DrawerTool *createTrackerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TrackerTool(zv, view, drw);
}

TrackerTool::TrackerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool("TrackerTool", zv, view, drw)
{
	active = false;
	tracking = false;
}

DrawerToolInfoVector *createDrawerTool() {
	DrawerToolInfoVector *infos = new DrawerToolInfoVector();

	infos->push_back(new DrawerToolInfo("TrackerTool",createTrackerTool));

	return infos;
}

extern "C" _export ILWIS::Module *getModuleInfo() {
	ILWIS::Module *module = new ILWIS::Module("Tracker", "IlwisTracking.dll",ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifDrawers, (void *)createDrawer); 
	module->addMethod(ILWIS::Module::ifgetCommandInfo, (void *)getApplicationInfo);
	module->addMethod(ILWIS::Module::ifDrawerTools, (void *)createDrawerTool);

	return module;
}

TrackerTool::~TrackerTool()
{
}

void TrackerTool::OnLButtonUp(UINT nFlags, CPoint point)
{
	vector<Geometry *> features;
	Coord crd = drawer->getRootDrawer()->screenToWorld(RowCol(point.y, point.x));
	int activeIndex = 0;
	ComplexDrawer *setDrawer = (ComplexDrawer *)drawer;
	for(int i=0; i < setDrawer->getDrawerCount(); ++i) {
		ComplexDrawer *poldrw = (ComplexDrawer *)setDrawer->getDrawer(i);
		if ( poldrw->isActive()) {
			activeIndex = i;
			BaseMap *bmp = (BaseMap *)poldrw->getDataSource();
			features = (*bmp)->getFeatures(crd);
			break;
		}
	}
	Feature *selectedFeature= features.size() > 0 ? CFEATURE(features.at(0)) : 0;
	if ( selectedFeature == 0) {
		reset();
		return;
	}
	vector<Coord> path;
	bool found = false;
	for(int i=activeIndex; i < setDrawer->getDrawerCount(); ++i) {
		ComplexDrawer *poldrw = (ComplexDrawer *)setDrawer->getDrawer(i);
		if ( poldrw->isActive() || found) {
			for(int j= 0; j < poldrw->getDrawerCount(); ++j) {
				Feature *feature = (Feature *)poldrw->getDrawer(j)->getDataSource();
				if ( selectedFeature->rValue() == feature->rValue()) {
					ILWIS::Polygon *pol = CPOLYGON(feature);
					Coordinate crd;
					pol->getCentroid(crd);
					path.push_back(Coord(crd));
					TrackSetDrawer *tdr = dynamic_cast<TrackSetDrawer *>(poldrw->getDrawer(TRACK_DRAWER_ID, ComplexDrawer::dtPOST));
					if ( tdr != 0) {
						poldrw->removeDrawer(tdr->getId());
					}
					DrawerParameters dp(drawer->getRootDrawer(), poldrw);
					tdr = (TrackSetDrawer *)NewDrawer::getDrawer("TrackSetDrawer", "ilwis38", &dp);
					poldrw->addPostDrawer(TRACK_DRAWER_ID, tdr);
					tdr->addDataSource(&path);
					PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
					tdr->prepare(&pp);
					found = true;
				}
			}
		}

	}
}

void TrackerTool::getMaxLengthTrack(vector<Coord>& crds) const{
	TrackSetDrawer *tdrMax = 0;
	long maxSize = iUNDEF;
	ComplexDrawer *setDrawer = (ComplexDrawer *)drawer;
	for(int i=0; i < setDrawer->getDrawerCount(); ++i) {
		ComplexDrawer *poldrw = (ComplexDrawer *)setDrawer->getDrawer(i);
		TrackSetDrawer *tdr = dynamic_cast<TrackSetDrawer *>(poldrw->getDrawer(TRACK_DRAWER_ID, ComplexDrawer::dtPOST));
		if ( tdr != 0) {
			long l = tdr->getTrackLength();
			if ( l > maxSize) {
				maxSize = l;
				tdrMax = tdr;
			}
		}
	}
	if ( tdrMax != 0) {
		tdrMax->getTrack(crds);	
	}
}

void TrackerTool::reset() {
	vector<Coord> path;
	ComplexDrawer *setDrawer = (ComplexDrawer *)drawer;
	for(int i=0; i < setDrawer->getDrawerCount(); ++i) {
		ComplexDrawer *poldrw = (ComplexDrawer *)setDrawer->getDrawer(i);
		for(int j= 0; j < poldrw->getDrawerCount(); ++j) {
			TrackSetDrawer *tdr = dynamic_cast<TrackSetDrawer *>(poldrw->getDrawer(TRACK_DRAWER_ID, ComplexDrawer::dtPOST));
			if ( !tdr)
				continue;
			tdr->addDataSource(&path);
			PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
			tdr->prepare(&pp);
		}
	}
}

void TrackerTool::Stop()
{
	//mpv->as = 0;
	//delete this;
}

bool TrackerTool::isToolUseableFor(ILWIS::DrawerTool *tool) {
	if ( tool->getType() == "AnimationTool") {
		Domain dm;
		IlwisObject *ds = (IlwisObject *)((ComplexDrawer *)drawer)->getDataSource();
		if ( ds == 0)
			return false;
		if ( IOTYPE((*ds)->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
			ObjectCollection objcol((*ds)->fnObj);
			return objcol->getStatusFor(ObjectCollection::csALLPOLYGON | ObjectCollection::csSAMEDOMAIN);
		}
	}
	return false;
}

void TrackerTool::toggleTracking(void *value, HTREEITEM) {
	bool use = *(bool *)value;
	if ( use) {
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
	} else {
		tree->GetDocument()->mpvGetView()->noTool(getId());
	}
}
String TrackerTool::cursorName() const {
	return "ARRMPACUR";
}

HTREEITEM TrackerTool::configure( HTREEITEM parentItem) {
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this, 0, (DTSetCheckFunc)&TrackerTool::toggleTracking);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&TrackerTool::options);
	htiNode =  insertItem("Tracker", "Track",item,tracking);

	return htiNode;
}

void TrackerTool::options(){
	new TrackingForm(tree, this);
}

String TrackerTool::getMenuString() const
{ 
	return TR("Feature Tracking");
}

//-----------------------------------------------
TrackingForm::TrackingForm(CWnd *par, TrackerTool *_tool) :
DisplayOptionsForm2((ComplexDrawer *)_tool->getDrawer(), par, TR("Tracking")),tool(_tool)
{
	new PushButton(root, TR("Open track as table"),  (NotifyProc )&TrackingForm::openAsTable);
	new FieldBlank(root);

	create();
}

class TableNameForm : public FormWithDest {
public:
	TableNameForm(CWnd *par,String *name) : FormWithDest(par,TR("Open as table"),fbsSHOWALWAYS | fbsMODAL) {
		new FieldString(root,TR("Table name"),name);
		//create();
	}

	int exec() {
		FormWithDest::exec();
		return 1;
	}
};

int TrackingForm::openAsTable(Event *) {
	String fname("Track");
	if ( TableNameForm(this, &fname).DoModal() == IDOK) {
		vector<Coord> crds;
		tool->getMaxLengthTrack(crds);
		FileName fnTable = FileName::fnUnique(FileName(fname,".tbt"));
		Table tbl(fnTable,Domain("none"));
		DomainValueRangeStruct dvInt(0 ,crds.size() );
		Column colIndex = tbl->colNew("Coordinate Index",dvInt);
		colIndex->SetOwnedByTable();
		Domain dmcrd;
		dmcrd.SetPointer(new DomainCoord(tool->getDrawer()->getRootDrawer()->getCoordinateSystem()->fnObj));
		Column colCrd = tbl->colNew("Coordinate", dmcrd, ValueRange());
		colCrd->SetOwnedByTable();
		tbl->iRecNew(crds.size());
		for(long m =0; m < crds.size(); ++m) {
			colIndex->PutVal(m,m);
			colCrd->PutVal(m, crds[m]);
		}
		tbl->Store();
		IlwWinApp()->Execute(String("Open %S",fnTable.sRelative())); 
	}
	return 1;
}

void TrackingForm::apply() {
}






