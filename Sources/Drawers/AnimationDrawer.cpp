#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldList.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\TimeGraphSlider.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\objlist.h"
#include "Engine\Domain\DomainTime.h" 
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SelectionRectangle.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\ValueSlicer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

int AnimationDrawer::timerIdCounter=5000;
int mycount = 0;
#define REAL_TIME_INTERVAL 100
#define BOX_DRAWER_ID 497

ILWIS::NewDrawer *createAnimationDrawer(DrawerParameters *parms) {
	return new AnimationDrawer(parms);
}

AnimationDrawer::AnimationDrawer(DrawerParameters *parms) : 
	AbstractMapDrawer(parms,"AnimationDrawer"),
	timerid(iUNDEF),
	interval(1.0),
	datasource(0),
	sourceType(sotUNKNOWN),
	featurelayer(0),
	loop(true),
	index(0),
	animcontrol(0), 
	useTime(false),
	mapIndex(0),
	animselection(0),
	animslicing(0)
	//animselectionFeature(0)
{
	setTransparency(1);
	last = 0;
}

AnimationDrawer::~AnimationDrawer(){
	delete datasource;
	delete featurelayer;
}

String AnimationDrawer::description() const {
	String sName = getName();
	String sDescr = (*datasource)->sDescr();
	if ("" != sDescr) 
		sName = String("%S Animated", sName);
	return sName;
}

RangeReal AnimationDrawer::getMinMax(const MapList& mlist) const{
	RangeReal rrMinMax (0, 255);
	if (mlist->iSize() > 0) {
		if (mlist->map(0)->dm()->pdv()) {
			for (int i = 0; i < mlist->iSize(); ++i) {
				Map mp = mlist->map(i);
				RangeReal rrMinMaxMap = mp->rrMinMax(BaseMapPtr::mmmSAMPLED);
				if (rrMinMaxMap.rLo() >= rrMinMaxMap.rHi())
					rrMinMaxMap = mp->vr()->rrMinMax();
				if (i > 0)
					rrMinMax += rrMinMaxMap;
				else
					rrMinMax = rrMinMaxMap;
			}
		} else if (mlist->map(0)->fTblAtt() && attColumn.fValid() && attColumn->dm()->pdv()) {
			for (int i = 0; i < mlist->iSize(); ++i) {
				Map mp = mlist->map(i);
				if (i > 0)
					rrMinMax += attColumn->vr()->rrMinMax();
				else
					rrMinMax = attColumn->vr()->rrMinMax();
			}
		}
	}
	return rrMinMax;
}

void AnimationDrawer::removeSelectionDrawers() {
	for(int i = 0; i < getDrawerCount(); ++i) {
		SetDrawer *sdrw = (SetDrawer *) getDrawer(i);
		NewDrawer *drwPost = sdrw->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
		if ( drwPost)
			sdrw->removeDrawer(drwPost->getId());
	}
}

void AnimationDrawer::addSelectionDrawers(const Representation& rpr) {
	MapList mlist((*datasource)->fnObj);
	RangeReal rrMinMax = getMinMax(mlist);
	Palette * palette;
	removeSelectionDrawers();
	for(int i = 0; i < getDrawerCount(); ++i) {
		RasterSetDrawer *drw = (RasterSetDrawer*)getDrawer(i);
		ILWIS::DrawerParameters parms(getRootDrawer(), this);
		Map mp = mlist->map(i);
		RasterSetDrawer *rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
		rasterset->setThreaded(false);
		rasterset->setRepresentation(rpr);
		rasterset->setMinMax(rrMinMax);
		if (i == 0)
		{
			palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
			paletteList.push_back(palette);
		}
		else
			rasterset->SetPalette(palette);
		PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
		addSetDrawer(mp,&pp,rasterset,String("overlay %d",i), true);
		drw->addPostDrawer(RSELECTDRAWER,rasterset);
		rasterset->setActive(i == 0 ? true : false);

	}
}

SetDrawer *AnimationDrawer::createIndexDrawer(const BaseMap& basem,ILWIS::DrawerParameters& dp, PreparationParameters* pp) {
	FeatureSetDrawer *fsd;
	IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(basem->fnObj);
	switch ( otype) {
		case IlwisObject::iotPOINTMAP:
			fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("PointSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd);
			break;
		case IlwisObject::iotSEGMENTMAP:
			fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("LineSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd);
			break;
		case IlwisObject::iotPOLYGONMAP:
			fsd = (FeatureSetDrawer *)IlwWinApp()->getDrawer("PolygonSetDrawer", pp, &dp); 
			addSetDrawer(basem,pp,fsd, "Areas");
			break;
	}
	return fsd;
}

void AnimationDrawer::prepare(PreparationParameters *pp){
	AbstractMapDrawer::prepare(pp);
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	if ( sourceType == sotFEATURE ) {
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			ObjectCollection oc((*datasource)->fnObj);
			if ( getName() == "")
				setName(oc->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}

			for(int i = 0; i < oc->iNrObjects(); ++i) {
				BaseMap bmp(oc->fnObject(i));
				if ( bmp.fValid()) {
					SetDrawer *drw = createIndexDrawer(bmp, dp, pp);
					//addSetDrawer(bmp,pp,drw,String("band %d",i));
					drw->setActive(i == 0 ? true : false);
				}
			}
	/*		featurelayer = (FeatureLayerDrawer *)IlwWinApp()->getDrawer("FeatureLayerDrawer", "Ilwis38", &parms);
			featurelayer->setActive(true);
			featurelayer->addDataSource(&basemap);
			if ( basemap->fTblAtt()) {
				for(int i = 0; i< names.size(); ++i) {
					PreparationParameters parms(NewDrawer::ptGEOMETRY | NewDrawer::ptANIMATION);
					featurelayer->prepare(&parms);
				}
				vector<NewDrawer *> allDrawers;
				featurelayer->getDrawers(allDrawers);
				for(int i=0; i < allDrawers.size(); ++i) {
					FeatureSetDrawer *fset = (FeatureSetDrawer *)allDrawers.at(i);
					fset->setActive(i == 0 ? true : false);
					fset->getZMaker()->setTable(basemap->tblAtt(), names.at(i));
					fset->getZMaker()->setThreeDPossible(true);
				}
			}*/

		} 
	}
	if ( sourceType == sotMAPLIST) {
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			MapList mlist((*datasource)->fnObj);
			if ( getName() == "Unknown")
				setName(mlist->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			// Calculate the min/max over the whole maplist. This is used for palette and texture generation.
			RangeReal rrMinMax = getMinMax(mlist);
			Palette * palette;
			for(int i = 0; i < mlist->iSize(); ++i) {
				ILWIS::DrawerParameters parms(getRootDrawer(), this);
				Map mp = mlist->map(i);
				RasterSetDrawer *rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
				rasterset->setThreaded(false);
				rasterset->setMinMax(rrMinMax);
				if (i == 0)
				{
					palette = rasterset->SetPaletteOwner(); // create only the palette of the first rasterset, and share it with the other rastersets
					paletteList.push_back(palette);
				}
				else
					rasterset->SetPalette(palette);
				addSetDrawer(mp,pp,rasterset,String("band %d",i));
				rasterset->setActive(i == 0 ? true : false);
			}
		}
	}
	if ( pp->type & NewDrawer::pt3D) {
		for(int i=0; i < drawers.size(); ++i)
			drawers.at(i)->prepare(pp);
	}

}
void AnimationDrawer::setTransparency(double v) {
	for(int i = 0; i < getDrawerCount(); ++i) {
		NewDrawer *drw = getDrawer(i);
		drw->setTransparency(v);
	}
}

void AnimationDrawer::addSetDrawer(const BaseMap& basem,PreparationParameters *pp,SetDrawer *rsd, const String& name, bool post) {
	//MessageBox(0,"Add set drawer","", MB_OK);
	PreparationParameters fp((int)pp->type | NewDrawer::ptANIMATION, 0);
	fp.rootDrawer = getRootDrawer();
	fp.parentDrawer = this;
	fp.csy = basem->cs();
	rsd->setName(name);
	rsd->setRepresentation(basem->dm()->rpr()); //  default choice
	rsd->getZMaker()->setSpatialSourceMap(basem);
	rsd->getZMaker()->setDataSourceMap(basem);
	rsd->addDataSource(basem.ptr());
	rsd->prepare(&fp);
	if (!post)
		addDrawer(rsd);
}

void AnimationDrawer::addDataSource(void *data, int options){
	IlwisObject *obj = dynamic_cast<IlwisObject *>((IlwisObject *)data);
	if ( obj) {
		datasource = new IlwisObject(*obj);
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
		if  (type == IlwisObject::iotSEGMENTMAP || 
			 type == IlwisObject::iotPOINTMAP || 
			 type == IlwisObject::iotPOLYGONMAP ) {
			 sourceType = sotFEATURE;
			 AbstractMapDrawer::addDataSource(data);
		}
		if ( type == IlwisObject::iotOBJECTCOLLECTION) {
			sourceType = sotFEATURE;
			ObjectCollection oc((*datasource)->fnObj);
			AbstractMapDrawer::addDataSource((void *)&(oc->ioObj(0)));
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				activeMaps.push_back(i);
			}
		}
		if ( type == IlwisObject::iotMAPLIST) {
			sourceType = sotMAPLIST;
			MapList mlist((*datasource)->fnObj);
			AbstractMapDrawer::addDataSource((void *)&(mlist->map(0)));
			for(int i = 0; i < mlist->iSize(); ++i) {
				activeMaps.push_back(i);
			}
		}
	}
}

void AnimationDrawer::inactivateOtherPalettes(ILWIS::Palette * palette)
{
	for (int i=0; i < paletteList.size(); ++i)
	{
		Palette * pal = paletteList.at(i);
		if (pal != palette)
			pal->SetNotCurrent();
	}
}

HTREEITEM AnimationDrawer::configure(LayerTreeView  *tv, HTREEITEM displayOptionsLastItem){
	ComplexDrawer::configure(tv,displayOptionsLastItem);
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationControl);
	InsertItem(TR("Run"),"History",item2);

	item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationDefaultView);
	InsertItem(TR("Restore default view"),".isl",item2);

	//item2 = new DisplayOptionTreeItem(tv,displayOptionsLastItem,this,0);
	HTREEITEM portrayalItem = InsertItem(tv, displayOptionsLastItem, TR("Selections"),"Select");

	if (IOTYPE((*datasource)->fnObj) == IlwisObject::iotMAPLIST) {

		DisplayOptionTreeItem * itemSlicing = new DisplayOptionTreeItem(tv, portrayalItem,this,
			0,(DisplayOptionItemFunc)&AnimationDrawer::animationSlicing);
		InsertItem(TR("Interactive Slicing"),"Slicing",itemSlicing);
	}
	DisplayOptionTreeItem * itemSelect = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&AnimationDrawer::animationSelection);
	InsertItem(TR("Attribute thresholds"),"SelectArea",itemSelect);


	DisplayOptionTreeItem * itemFrameSelect = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&AnimationDrawer::timeSelection);
	InsertItem(TR("Time Selection"),"TimeSelection",itemFrameSelect);

	DisplayOptionTreeItem * itemAOI = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&AnimationDrawer::areaOfInterest);
	InsertItem(TR("Area of Interest"),"SelectAoi",itemAOI);
	MapWindow *parent = (MapWindow *)getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->GetParent();

	animBar.Create(parent);
	CRect rect;
	parent->barScale.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	parent->DockControlBar(&animBar,AFX_IDW_DOCKBAR_TOP, rect);

	return displayOptionsLastItem;

}
void AnimationDrawer::animationSlicing(CWnd *parent) {
	animslicing = new AnimationSlicing(parent,this);
}

void AnimationDrawer::areaOfInterest(CWnd *parent) {
	new AnimationAreaOfInterest(parent,this);
}

void AnimationDrawer::timeSelection(CWnd *parent) {
	new TimeSelection(parent,this, activeMaps);
}

void AnimationDrawer::animationSelection(CWnd *parent) {
	if (IOTYPE((*datasource)->fnObj) == IlwisObject::iotMAPLIST)
		animselection = new AnimationSelection(parent,this);
	else {
	}
}

bool AnimationDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	AbstractMapDrawer::draw(norecursion, cbArea);
	return true;
}

void AnimationDrawer::animationControl(CWnd *parent) {
	animcontrol = new AnimationControl(parent, this);
}

void AnimationDrawer::animationDefaultView(CWnd *parent) {
	MapList mlist;
	mlist.SetPointer(datasource->pointer());
	for(int i =0 ; i < drawers.size(); ++i) {
		SetDrawer *sdr = (SetDrawer *)drawers.at(i);
		NewDrawer *drPost;
		if ( (drPost = sdr->getDrawer(RSELECTDRAWER,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);
		if ( (drPost = sdr->getDrawer(BOX_DRAWER_ID,dtPOST)) != 0)
			sdr->removeDrawer(drPost->getId(), true);

		if ( mlist[i]->dm()->pdi()) {
			sdr->setDrawMethod(NewDrawer::drmIMAGE);
		} else {
			sdr->setRepresentation(mlist[i]->dm()->rpr());
			sdr->setStretchRangeReal(mlist[i]->rrPerc1());
		}
		PreparationParameters pp(NewDrawer::ptRENDER);
		sdr->prepare(&pp);
		paletteList.clear();
	}
	if ( animselection)
		animselection->OnOK();
	if ( animslicing)
		animslicing->OnOK();
	getRootDrawer()->getDrawerContext()->doDraw();


}

void AnimationDrawer::timedEvent(UINT _timerid) {
    ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	bool redraw = false;
	if ( timerid == _timerid) {
		if ( useTime) {
			redraw = timerPerTime();
		} else {
			redraw = timerPerIndex();
		}
		if ( redraw) {
			if ( animcontrol)
				animcontrol->PostMessage(ID_TIME_TICK,mapIndex, TRUE);
			getRootDrawer()->getDrawerContext()->doDraw();
		}
	}
}

bool AnimationDrawer::timerPerIndex() {
	if ( sourceType == sotFEATURE){
		ObjectCollection oc((*datasource)->fnObj);
		if ( oc->iNrObjects() > 0 && mapIndex < activeMaps.size() - 1) {
			getDrawer(activeMaps[mapIndex])->setActive(false);
			getDrawer(activeMaps[++mapIndex])->setActive(true);
		} else {
			if (loop) {
				getDrawer(activeMaps[mapIndex])->setActive(false);
				mapIndex = 0;
				getDrawer(activeMaps[0])->setActive(true);
			}
		}
	}
	if ( sourceType == sotMAPLIST) {
		MapList mlist;
		mlist.SetPointer(datasource->pointer());
		if ( mlist->iSize() > 0 && mapIndex < activeMaps.size() - 1) {
			getDrawer(activeMaps[mapIndex])->setActive(false);
			getDrawer(activeMaps[++mapIndex])->setActive(true);
		} else {
			if (loop) {
				getDrawer(activeMaps[mapIndex])->setActive(false);
				mapIndex = 0;
				getDrawer(activeMaps[0])->setActive(true);
			}
		}
	}
	return true;
}

bool AnimationDrawer::timerPerTime() {
	if ( (double)timestep == rUNDEF || (double)timestep == 0.0)
		return false;

	bool redraw = false;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION ) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl;
		mpl.SetPointer(datasource->pointer());
		Column col = mpl->tblAtt()->col(colTime);
		ILWIS::Duration duration = (col->rrMinMax().rHi() - col->rrMinMax().rLo());
		double steps = 1000.0 / REAL_TIME_INTERVAL;
		double currentTime = col->rrMinMax().rLo() +  timestep * (double)index / steps;
		//TRACE(String("%f %f\n",currentTime,col->rValue(activeMaps[mapIndex])).scVal());
		if ( mapIndex < activeMaps.size() - 1 && col->rValue(activeMaps[mapIndex]) < currentTime){
			getDrawer(activeMaps[mapIndex])->setActive(false);
			getDrawer(activeMaps[++mapIndex])->setActive(true);
			//TRACE(String("%d %d\n",mapIndex,activeMaps[mapIndex]).scVal());
			redraw = true;
		} else {
			if (loop && mapIndex >= activeMaps.size() -1 && currentTime >= col->rValue(col->iRecs() - 1)) {
				getDrawer(activeMaps[mapIndex])->setActive(false);
				mapIndex = 0;
				index = 0;
				getDrawer(activeMaps[0])->setActive(true);
				redraw = true;
			}
		}
	}
	++index;

	return redraw;

}

String AnimationDrawer::iconName(const String& subtype) const {
	return "Animation";
}

void AnimationDrawer::setMapIndex(int ind) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	for(int i =0 ; i < drawers.size(); ++i)
		getDrawer(i)->setActive(false);

	mapIndex = activeMaps[ind];
}

//---------------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationControl, DisplayOptionsForm2)
	ON_MESSAGE(ID_TIME_TICK, OnTimeTick)
END_MESSAGE_MAP()

LRESULT AnimationControl::OnTimeTick( WPARAM wParam, LPARAM lParam ) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->getDrawerCount() - 1 == (int)wParam )
		st->Hide();

	if ( lParam == TRUE)
		graphSlider->setIndex(adrw->activeMaps[wParam]);
	else
		adrw->setMapIndex(wParam);
	return 1;
}

AnimationControl::AnimationControl(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, "Time"), fgTime(0)
{
	initial = true;
	IlwisObject *source = adr->datasource;
	fps = 1.0 / adr->interval;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
		IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			fcol = new FieldColumn(root,TR("Reference Attribute"),mpl->tblAtt(),&colName,dmVALUE);
			fcol->SetCallBack((NotifyProc)&AnimationControl::changeColum);
		}
	}
	//if ( type ==IlwisObject::iotOBJECTCOLLECTION) {
	//	ObjectCollection oc((*datasource)->fnObj);
	//	if ( mpl->fTblAtt()) {
	//		fcol = new FieldColumn(root,TR("Reference Attribute"),mpl->tblAtt(),&colName,dmVALUE);
	//		fcol->SetCallBack((NotifyProc)&AnimationControl::changeColum);
	//	}
	//}

	cbTime = new CheckBox(root,TR("Use Time Attribute"), &adr->useTime);
	cbTime->SetCallBack((NotifyProc)&AnimationControl::setTimingMode);

	
	/*frtime = new FieldReal(root,TR("Interval"), &adr->interval,ValueRangeReal(0.1,1000,0.1));
	frtime->SetCallBack((NotifyProc)&AnimationControl::speed(Event *ev));
	frtime->Align(cbTime, AL_UNDER);*/
	setTimeElements(cbTime);
	setSlider(cbTime);
	sliderFps = new FieldRealSliderEx(root,"Frame rate(fps)", &fps,ValueRange(RangeReal(0,5),0.1),false);
	sliderFps->Align(graphSlider, AL_UNDER,-25);
	sliderFps->SetCallBack((NotifyProc)&AnimationControl::speed);
	new FieldBlank(root);

	FieldGroup *fg = new FieldGroup(root, true);

	fg->SetBevelStyle(FormEntry::bsRAISED);
	FlatIconButton *fi1 = new FlatIconButton(fg,"Begin","",(NotifyProc)&AnimationControl::begin, FileName());
	fbBegin = fi1;
	FlatIconButton *fi2 = new FlatIconButton(fg,"Pause","",(NotifyProc)&AnimationControl::pause, FileName());
	fi2->Align(fi1,AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"Run","",(NotifyProc)&AnimationControl::run, FileName());
	fi1->Align(fi2,AL_AFTER,-10);
	fi2 = new FlatIconButton(fg,"Stop","",(NotifyProc)&AnimationControl::stop, FileName());
	fi2->Align(fi1, AL_AFTER,-10);
	fi1 = new FlatIconButton(fg,"End","",(NotifyProc)&AnimationControl::end, FileName());
	fi1->Align(fi2, AL_AFTER,-10);
	st = new StaticText(root,"Preparing . . .");	
	st->Align(fg, AL_AFTER);

	create();

}

int AnimationControl::speed(Event *ev) {
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	if ( fps == 0 || fps == rUNDEF)
		return 1;
	andr->interval = 1.0 / fps;
	run(0);
	return 1;
}

int AnimationControl::setTimingMode(Event *ev) {
	cbTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	sliderFps->Hide();
	if ( fgTime)
		fgTime->Hide();
	if ( adrw->useTime) {
		sliderFps->Hide();
		if ( fgTime)
			fgTime->Show();
		IlwisObject *source = adrw->datasource;
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			Column col = mpl->tblAtt()->col(adrw->colTime);
			TimeInterval interval(col->rrMinMax().rLo(),col->rrMinMax().rHi());
			graphSlider->setTimeInterval(interval);
			graphSlider->setTimes(col);
		}
	} else {
		sliderFps->Show();
		graphSlider->setTimeInterval(TimeInterval());
	}
	stop(0);
	run(0);
	return 1;
}

void AnimationControl::setTimeElements(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	IlwisObject *source = adrw->datasource;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc((*source)->fnObj);
	}
	if ( type ==IlwisObject::iotMAPLIST) {
		MapList mpl((*source)->fnObj);
		if ( mpl->fTblAtt()) {
			fgTime = new FieldGroup(entry);
			fcolTime = new FieldColumn(fgTime,"",mpl->tblAtt(),&adrw->colTime,dmTIME);
			fcolTime->SetCallBack((NotifyProc)&AnimationControl::changeTimeColumn);
			fgTime->Align(entry, AL_AFTER);
			FieldGroup *fg2 = new FieldGroup(fgTime);
			fiYr = new FieldInt(fg2,TR("Period(YMDHm)/ sec"),&year);
			fiYr->Align(fg2,AL_AFTER);
			fiMonth = new FieldInt(fg2,"",&month);
			fiMonth->Align(fiYr,AL_AFTER,-5);
			fiMonth->SetWidth(8);
			fiDay = new FieldInt(fg2,"",&day);
			fiDay->Align(fiMonth,AL_AFTER,-5);
			fiDay->SetWidth(8);
			fiHour = new FieldInt(fg2,"",&hour);
			fiHour->Align(fiDay,AL_AFTER,-5);
			fiHour->SetWidth(8);
			fiMinute = new FieldInt(fg2,"",&minute);
			fiMinute->Align(fiHour,AL_AFTER,-5);
			fiMinute->SetWidth(8);
			fg2->Align(entry, AL_UNDER);
			fg2->SetIndependentPos();
			fiYr->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMinute->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMonth->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiDay->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
			fiMinute->SetCallBack((NotifyProc)&AnimationControl::changeDuration);
		}
	} 
}

int AnimationControl::changeDuration(Event *ev) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->colTime != "" && adrw->useTime) {
		fiYr->StoreData();
		fiMinute->StoreData();
		fiMonth->StoreData();
		fiDay->StoreData();
		fiMinute->StoreData();
		
		Duration dur(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		adrw->timestep = dur;
	}
	return 1;
}
int AnimationControl::changeTimeColumn(Event *e) {
	fcolTime->StoreData();
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	if ( adrw->colTime != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = adrw->datasource;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		if ( type == IlwisObject::iotRASMAP ||  IlwisObject::iotSEGMENTMAP || 
			IlwisObject::iotPOINTMAP || IlwisObject::iotPOLYGONMAP) {
				ObjectCollection oc((*source)->fnObj);
		}
		if ( type ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			if ( mpl->fTblAtt()) {
				Column col = mpl->tblAtt()->col(adrw->colTime);
				adrw->timestep = calcNiceStep((col->rrMinMax().rHi() - col->rrMinMax().rLo()) / mpl->iSize());
				//sliderFps->SetVal(adrw->timestep);

			}
		} 
		
	}
	return 1;
}

double AnimationControl::calcNiceStep(Duration time) {
	year = 4712 + time.get(Time::tpYEAR);
	month = time.get(Time::tpMONTH);
	day = time.get(Time::tpDAYOFMONTH);
	hour = time.get(Time::tpHOUR);
	minute = time.get(Time::tpMINUTE);

	if ( year > 10) {
		int l10 = log10((double)year);
		int p10 = pow(10.0, l10);
		int y = (int)(year / p10) * p10;
		int rest = year - y;
		if ( rest * 2 > p10 )
			year = y + 5 * p10 / 10;
		else
			year = y;
	}
	if ( year > 0) {
		day = hour = minute = 0;
		if ( month > 6)
			month = 6;
		else 
			month = 0;
	} else if ( month > 0) {
		hour = minute = 0;
	} else if ( day > 0) {
		minute = 0;
	}
	if ( year > 5) {
		month = 0;
	}
	fiYr->SetVal(year);
	fiMonth->SetVal(month);
	fiDay->SetVal(day);
	fiHour->SetVal(hour);
	fiMinute->SetVal(minute);

	return Duration(String("P%04dY%02dM%02dDT%02dH%02dM00",year,month,day,hour,minute));
	
}
void AnimationControl::setSlider(FormEntry *entry) {
	AnimationDrawer *adrw = (AnimationDrawer *)drw;
	IlwisObject *source = adrw->datasource;
	int number=iUNDEF;
	IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
	if ( type ==IlwisObject::iotMAPLIST ) {
		MapList mpl((*source)->fnObj);
		number = mpl->iSize();
	
	}
	if ( type == IlwisObject::iotOBJECTCOLLECTION){
		ObjectCollection oc((*source)->fnObj);
		number = oc->iNrObjects();
	}
	RangeInt setRange = RangeInt(0, number);
	graphSlider = new TimeGraphSlider(root, setRange);
	graphSlider->SetWidth(180);
	graphSlider->Align(entry, AL_UNDER, 23);

}

int AnimationControl::changeColum(Event *) {
	fcol->StoreData();
	if ( colName != "") {
		AnimationDrawer *adrw = (AnimationDrawer *)drw;
		IlwisObject *source = adrw->datasource;
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*source)->fnObj);
		Table tbl;
		if (  type ==IlwisObject::iotMAPLIST) {
			MapList mpl((*source)->fnObj);
			tbl = mpl->tblAtt();
		}
		graphSlider->setSourceTable(tbl);
		graphSlider->setSourceColumn(colName);
	}
	return 1;

}
int AnimationControl::setTiming(Event *ev) {
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	
	run(0);
	return 1;
}

int AnimationControl::stop(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	andr->setMapIndex(0);
	andr->index = 0;
	andr->timerid = iUNDEF;
	return 1;
}

int AnimationControl::pause(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	return 1;
}

int AnimationControl::end(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->mapIndex = andr->drawers.size();
	andr->index = 0;
	return 1;
}

int AnimationControl::run(Event  *ev) {
	sliderFps->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	if ( andr->timerid != iUNDEF) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->KillTimer(andr->timerid);
	}else{
		andr->timerid = AnimationDrawer::timerIdCounter++;
	}
	if ( andr->useTime) {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->timerid,REAL_TIME_INTERVAL,0);
	} else {
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->timerid,andr->interval * 1000.0,0);
	}
	updateMapView();
	return 1;
}

int AnimationControl::begin(Event  *ev) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->setMapIndex(0);
	andr->index = 0;
	return 1;
}

void AnimationControl::shutdown(int iReturn) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->animcontrol = 0;
	return DisplayOptionsForm2::shutdown();
}
//----------------------------------------------------------
AnimationSlicing::AnimationSlicing(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Slicing"))
{
	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&AnimationSlicing::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/3);
	FlatIconButton *fb=new FlatIconButton(fg,"Save","",(NotifyProc)&AnimationSlicing::saveRpr,fnRpr);
	fb->Align(fldSteps, AL_AFTER);


	create();
}

int AnimationSlicing::saveRpr(Event *ev) {
	CFileDialog filedlg (FALSE, "*.rpr", "*.rpr",OFN_HIDEREADONLY|OFN_NOREADONLYRETURN | OFN_LONGNAMES, "Ilwis Representation (*.rpr)|*.rpr||", NULL);
	if ( filedlg.DoModal() == IDOK) {
		String name(filedlg.GetPathName());
		vs->setFileNameRpr(FileName(name));
	}
	return 1;
}

int AnimationSlicing::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"2");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
		}
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;
}

void AnimationSlicing::shutdown(int iReturn) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->animslicing = 0;
	return DisplayOptionsForm2::shutdown();
}

//----------------------------------------------------------
AnimationAreaOfInterest::AnimationAreaOfInterest(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm(adr, par, TR("Area of Interest")), boxdrw(0)
{
	clr = Color(60,60,60, 120);
	fb = new FlatIconButton(root, "SelectAoi", TR("Select Area"),(NotifyProc)&AnimationAreaOfInterest::createROIDrawer,FileName());
	fb->SetIndependentPos();
	fc = new FieldColor(root, TR("Out area color"), &clr, true);
	fb->SetWidth(80);
	create();
}

AnimationAreaOfInterest::~AnimationAreaOfInterest() {
	if ( boxId != "") {
		drw->getRootDrawer()->removeDrawer(boxId);
		drw->getRootDrawer()->getDrawerContext()->doDraw();
		boxdrw = 0;
	}
	::SetCursor(zCursor(Arrow));
}

void AnimationAreaOfInterest::areaOfInterest(CRect rect) {
	if ( boxId != "") {
		drw->getRootDrawer()->removeDrawer(boxId);
		boxId = "";
		boxdrw = 0;
	}
	if ( rect.Width() != 0 && rect.Height() != 0) { // case of clicking on the map in zoom mode
		Coord c1 = drw->getRootDrawer()->screenToWorld(RowCol(rect.top, rect.left));
		Coord c2 = drw->getRootDrawer()->screenToWorld(RowCol(rect.bottom, rect.right));
		CoordBounds cbInner(c1,c2);
		CoordBounds cbOuter = drw->getRootDrawer()->getCoordBoundsZoom();
		ILWIS::DrawerParameters sp(drw->getRootDrawer(), drw->getRootDrawer());
		boxdrw = (ILWIS::BoxDrawer *)IlwWinApp()->getDrawer("BoxDrawer", "Ilwis38", &sp);
		boxdrw->setBox(cbOuter, cbInner);
		boxId = String("%d", BOX_DRAWER_ID);
		drw->getRootDrawer()->addPostDrawer(BOX_DRAWER_ID,boxdrw); 
		::SetCursor(zCursor(Arrow));
	} 
}

void AnimationAreaOfInterest::apply() {
	fc->StoreData();
	if ( boxdrw) {
		boxdrw->setDrawColor(clr);
		updateMapView();
	}
	

}

int AnimationAreaOfInterest::createROIDrawer(Event*) {

	if ( boxId == "") {
		fb->SetText(TR("Unselect Area"));
		drw->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->selectArea(this,
			(NotifyRectProc)&AnimationAreaOfInterest::areaOfInterest,"DRAGOK",Color(255,0,0,0.2)); 
	} else {
		drw->getRootDrawer()->removeDrawer(boxId);
		fb->SetText(TR("Select Area"));
		boxId= "";
		boxdrw = 0;
		::SetCursor(zCursor(Arrow));
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;

}

//----------------------------------------------------------
AnimationSelection::AnimationSelection(CWnd *par, AnimationDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Selection"))
{
	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
	vs->setRprBase( ((AbstractMapDrawer *)adr)->getBaseMap()->dm()->rpr());
	vs->setLowColor(colorUNDEF);
	vs->setHighColor(colorUNDEF);
	vs->setNumberOfBounds(3);
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&AnimationSelection::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/3);
	adr->addSelectionDrawers(vs->getRpr());

	create();

	//vs->setBoundColor(1,Color(120,230,0));
}
int AnimationSelection::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"3");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
			for(int i = 0; i < mapIndex + 2; ++i) {
				if ( i % 2 == 1) {
					vs->setBoundColor(i,Color(200,0,0));
				}
			}
		}
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;
}

void AnimationSelection::shutdown(int iReturn) {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->animselection = 0;
	return DisplayOptionsForm2::shutdown();
}
//----------------------------------------------------------
AnimationSourceUsage::AnimationSourceUsage(CWnd *par, AnimationDrawer *ldr) 
	: DisplayOptionsForm2(ldr, par, "Time"), mcs(0), rg(0)
{
	if ( ldr->sourceType == AnimationDrawer::sotFEATURE) {
		BaseMap basemap((*(ldr->datasource))->fnObj);
		if ( basemap->fTblAtt()) {
			new StaticText(root, TR("Columns to be used"));
			mcs = new MultiColumnSelector(root,basemap->tblAtt().ptr(), dmVALUE);
			RadioGroup *rg = new RadioGroup(root,TR("Type of use"),&columnUsage);
			new RadioButton(rg, TR("As Z value"));
			new RadioButton(rg, TR("As feature size"));
			new RadioButton(rg, TR("As Coordinates"));
				
		}
	}
  create();
}

int  AnimationSourceUsage::exec() {
	if ( rg) rg->StoreData();
	if ( mcs) {
		//mcs->StoreData();
		IntBuf selectedIndexes;
		mcs->iGetSelected(selectedIndexes);
		AnimationDrawer *andr = (AnimationDrawer *)drw;
		for(int  i =0; i < selectedIndexes.iSize(); ++i) {
			andr->names.push_back(mcs->sName(i));
		}

	}
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	drw->prepare(&pp);
	updateMapView();

	return 1;
}

//----------------------------------------------------------
TimeSelection::TimeSelection(CWnd *par, AnimationDrawer *ldr, vector<int>& _activeMaps) 
	: DisplayOptionsForm2(ldr, par, "Time selection"), activeMaps(_activeMaps)
{
	FillData();	
	fl = new FieldLister(root,data, cols);
	fl->setReadOnly(true);
	fl->SetWidth(100 + (cols.size() - 2) * 32 );
	fl->SetHeight(120 + min(160, data.size() * 16));
  create();
}

void TimeSelection::FillData() {
	IlwisObject *source = ((AnimationDrawer *)drw)->datasource;
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

//-------------------------------------------------
BEGIN_MESSAGE_MAP(AnimationBar, CToolBar)
	ON_EN_SETFOCUS(ID_AnimationBar,OnSetFocus)
	ON_EN_KILLFOCUS(ID_AnimationBar,OnKillFocus)
END_MESSAGE_MAP()

AnimationBar::AnimationBar()
{
	fActive = false;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// Since design guide says toolbars are fixed height so is the font.
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");
	fnt.CreateFontIndirect(&logFont);
}

AnimationBar::~AnimationBar()
{
}

void AnimationBar::Create(CWnd* pParent)
{
	int iWidth = 80;

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		              CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	CToolBar::CreateEx(pParent, dwCtrlStyle, dwStyle, rectBB, ID_AnimationBar);
	UINT ai[2];
	ai[0] = ID_AnimationBar;
	ai[1] = ID_SEPARATOR;
	SetButtons(ai,2);
	CRect rect;
	GetItemRect(0, &rect);
	SetButtonInfo(1, ID_AnimationBar,	TBBS_SEPARATOR, iWidth - rect.Width());

	rect.top = 3;
	rect.bottom -= 2;
	rect.right = rect.left + iWidth;
	ed.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rect,this,ID_AnimationBar);
	ed.SetFont(&fnt);
	SendMessage(DM_SETDEFID,IDOK);

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	SetBarStyle(GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);

	SetWindowText(TR("Animation").scVal());
}

void AnimationBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	if (fActive)
		return;
//	CView* vw = pParent->GetActiveView();
//	MapPaneView* mpv = dynamic_cast<MapPaneView*>(vw);
//	if (0 == mpv) {
//		ed.EnableWindow(FALSE);
//		return;
//	}
//	double rScale = mpv->rScaleShow();
//	if (rUNDEF == rScale)	{
////		ed.ShowWindow(SW_HIDE);
//		ed.SetWindowText(SMWRemNoScale.scVal());
//		ed.EnableWindow(FALSE);
//	}
//	else {
//		String sTxt("1:%.f", rScale);
//		ed.SetWindowText(sTxt.scVal());
////		ed.ShowWindow(SW_SHOW);
//		ed.EnableWindow(TRUE);
//	}
}

void AnimationBar::OnSetFocus()
{
	fActive = true;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(false);
}

void AnimationBar::OnKillFocus()
{
	fActive = false;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(true);
}


void AnimationBar::updateTime(const String& ) // called by AnimationBarEdit
{
	//CString str;
	//ed.GetWindowText(str);
	//if (str.GetLength() > 2 && str[0] == '1' && str[1] == ':')
	//	str = str.Mid(2);
	//bool fOk = true;
	//double rScale;
	//if (-1 != str.Find(':'))
	//	fOk = false;
	//if (fOk) {
	//	rScale = atof(str);
	//	if (rScale < 1)
	//		fOk = false;
	//	if (rScale > 1e10)
	//		fOk = false;
	//}
	//if (fOk) {
	//	CWnd* wnd = GetOwner();
	//	CFrameWnd* fw = dynamic_cast<CFrameWnd*>(wnd);
	//	CView* vw = fw->GetActiveView();
	//	MapPaneView* mpv = dynamic_cast<MapPaneView*>(vw);
	//	if (0 == mpv)
	//		return;
	//	mpv->UseScale(rScale);
	//}
	//else
	//	MessageBeep(-1);
}

