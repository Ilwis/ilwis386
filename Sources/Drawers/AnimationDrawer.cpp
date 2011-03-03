#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Domain\DomainTime.h" 
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\SelectionRectangle.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\RasterSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "drawers\Boxdrawer.h"
#include "Drawers\DrawingColor.h" 
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
	useTime(false),
	mapIndex(0)
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
				if (rrMinMaxMap.rLo() > rrMinMaxMap.rHi())
					rrMinMaxMap = mp->vr()->rrMinMax();
				if (i > 0)
					rrMinMax += rrMinMaxMap;
				else
					rrMinMax = rrMinMaxMap;
			}
		} else {
				SetDrawer *sdr = (SetDrawer *)const_cast<AnimationDrawer *>(this)->getDrawer(0);
				if ( !sdr)
					return RangeReal();
				if (sdr->useAttributeColumn() && sdr->getAtttributeColumn()->dm()->pdv()) {
				for (int i = 0; i < mlist->iSize(); ++i) {
					Map mp = mlist->map(i);
					if (i > 0)
						rrMinMax += sdr->getAtttributeColumn()->vr()->rrMinMax();
					else
						rrMinMax = sdr->getAtttributeColumn()->vr()->rrMinMax();
				}
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
	if ( sourceType == sotMAPLIST) {
		MapList mlist((*datasource)->fnObj);
		RangeReal rrMinMax = getMinMax(mlist);
		Palette * palette;
		removeSelectionDrawers();
		for(int i = 0; i < getDrawerCount(); ++i) {
			RasterSetDrawer *drw = (RasterSetDrawer*)getDrawer(i);
			ILWIS::DrawerParameters parms(getRootDrawer(), this);
			Map mp = mlist->map(i);
			RasterSetDrawer *rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
			rasterset->setUICode(0);
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
		ObjectCollection oc((*datasource)->fnObj);
		if ( pp->type & NewDrawer::ptGEOMETRY) {
			if ( getName() == "")
				setName(oc->sName());
			ILWIS::DrawerParameters parms(getRootDrawer(), getRootDrawer());
			if ( drawers.size() > 0) {
				clear();
			}
			Tranquilizer trq(TR("Adding maps"));
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				IlwisObject::iotIlwisObjectType type = IOTYPE(oc->fnObject(i));
				if (type == IlwisObject::iotPOINTMAP || type == IlwisObject::iotPOLYGONMAP || type ==IlwisObject::iotSEGMENTMAP) {
					BaseMap bmp(oc->fnObject(i));
					if ( bmp.fValid()) {
						if ( !rpr.fValid())
							rpr = bmp->dm()->rpr();
						
						SetDrawer *drw = createIndexDrawer(bmp, dp, pp);
						drw->setUICode(0);
						drw->setActive(i == 0 ? true : false);
						trq.fUpdate(i,oc->iNrObjects()); 
					}
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
		if ( pp->type && NewDrawer::ptRENDER) {
			for(int i = 0; i < oc->iNrObjects(); ++i) {
				SetDrawer *sdr = (SetDrawer *)getDrawer(i);
				sdr->prepare(pp);
			}
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
				if ( !rpr.fValid())
					rpr = mp->dm()->rpr();
				RasterSetDrawer *rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
				rasterset->setThreaded(false);
				if ( rrMinMax.fValid())
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
	setDrawMethod(getDrawer(0)->getDrawMethod());

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
	rsd->getZMaker()->setSpatialSource(basem, getRootDrawer()->getMapCoordBounds());
	rsd->getZMaker()->setDataSourceMap(basem);
	rsd->addDataSource(basem.ptr());
	rsd->prepare(&fp);
	if (!post)
		addDrawer(rsd);
}

void* AnimationDrawer::getDataSource() const {
	return (void *)datasource;
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

void AnimationDrawer::drawLegendItem(CDC *dc, const CRect& rct, double rVal) const{
	DomainValueRangeStruct dvs = getBaseMap()->dvrs();
	Color clr;
	SetDrawer *dr = (SetDrawer *)(const_cast<AnimationDrawer *>(this)->getDrawer(0));
	if ( dvs.dm()->pdv())
		clr = dr->getDrawingColor()->clrVal(rVal);
	else
		clr = dr->getDrawingColor()->clrRaw((long)rVal, dr->getDrawMethod());
	

	CBrush brushColor(clr);
	CBrush *br = dc->SelectObject(&brushColor);
	dc->Rectangle(rct);
	dc->SelectObject(br);
}


bool AnimationDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	AbstractMapDrawer::draw( cbArea);
	return true;
}

void AnimationDrawer::animationDefaultView() {
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
			//if ( animcontrol)
			//	animcontrol->PostMessage(ID_TIME_TICK,mapIndex, TRUE);
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
		ILWIS::Time ct(currentTime);

		if ( mapIndex < activeMaps.size() - 1 && col->rValue(activeMaps[mapIndex]) < currentTime){
			getDrawer(activeMaps[mapIndex])->setActive(false);
			getDrawer(activeMaps[++mapIndex])->setActive(true);
			//animBar.updateTime(String("index %d : %S", mapIndex, timeString(mpl, mapIndex)));
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

String AnimationDrawer::timeString(const MapList& mpl, int index) {
	double steps = 1000.0 / REAL_TIME_INTERVAL;
	double currentTime = mpl->tblAtt()->col(colTime)->rrMinMax().rLo() +  timestep * (double)index / steps;
	ILWIS::Time ct(currentTime);
	String timestring = ct.toString(true,mpl->tblAtt()->col(colTime)->dm()->pdtime()->getMode());
	return timestring;
}

String AnimationDrawer::iconName(const String& subtype) const {
	return "Animation";
}

void AnimationDrawer::setMapIndex(int ind) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	for(int i =0 ; i < drawers.size(); ++i)
		getDrawer(i)->setActive(false);

	getDrawer(ind)->setActive(true);
	mapIndex = activeMaps[ind];
}

void AnimationDrawer::updateLegendItem() {
	//if ( doLegend)
	//	doLegend->updateLegendItem();
}

//----------------------------------------------------------
//AnimationSlicing::AnimationSlicing(CWnd *par, AnimationDrawer *adr) 
//	: DisplayOptionsForm2(adr, par, TR("Slicing"))
//{
//	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
//	FieldGroup *fg = new FieldGroup(root);
//	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
//	fldSteps->SetCallBack((NotifyProc)&AnimationSlicing::createSteps);
//	fldSteps->Align(vs, AL_UNDER);
//	fldSteps->SetWidth(vs->psn->iWidth/3);
//	FlatIconButton *fb=new FlatIconButton(fg,"Save","",(NotifyProc)&AnimationSlicing::saveRpr,fnRpr);
//	fb->Align(fldSteps, AL_AFTER);
//
//
//	create();
//}
//
//int AnimationSlicing::saveRpr(Event *ev) {
//	CFileDialog filedlg (FALSE, "*.rpr", "*.rpr",OFN_HIDEREADONLY|OFN_NOREADONLYRETURN | OFN_LONGNAMES, "Ilwis Representation (*.rpr)|*.rpr||", NULL);
//	if ( filedlg.DoModal() == IDOK) {
//		String name(filedlg.GetPathName());
//		vs->setFileNameRpr(FileName(name));
//	}
//	return 1;
//}
//
//int AnimationSlicing::createSteps(Event*) {
//	if (fldSteps->ose->GetCount() == 0) {
//		for(int i = 2 ; i <= 10; ++i)
//			fldSteps->AddString(String("%d",i));
//		fldSteps->ose->SelectString(0,"2");
//	} else {
//		int mapIndex = fldSteps->ose->GetCurSel();
//		if ( mapIndex != -1) {
//			vs->setNumberOfBounds(mapIndex +2);
//		}
//		drw->getRootDrawer()->getDrawerContext()->doDraw();
//	}
//	return 1;
//}
//
//void AnimationSlicing::shutdown(int iReturn) {
//	AnimationDrawer *andr = (AnimationDrawer *)drw;
//	andr->animslicing = 0;
//	return DisplayOptionsForm2::shutdown();
//}

//----------------------------------------------------------
//AnimationSelection::AnimationSelection(CWnd *par, AnimationDrawer *adr) 
//	: DisplayOptionsForm2(adr, par, TR("Selection"))
//{
//	vs = new ValueSlicerSlider(root, ((SetDrawer *)adr->getDrawer(0)));
//	vs->setRprBase( ((AbstractMapDrawer *)adr)->getBaseMap()->dm()->rpr());
//	vs->setLowColor(colorUNDEF);
//	vs->setHighColor(colorUNDEF);
//	vs->setNumberOfBounds(3);
//	FieldGroup *fg = new FieldGroup(root);
//	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
//	fldSteps->SetCallBack((NotifyProc)&AnimationSelection::createSteps);
//	fldSteps->Align(vs, AL_UNDER);
//	fldSteps->SetWidth(vs->psn->iWidth/3);
//	adr->addSelectionDrawers(vs->getRpr());
//
//	create();
//
//	//vs->setBoundColor(1,Color(120,230,0));
//}
//int AnimationSelection::createSteps(Event*) {
//	if (fldSteps->ose->GetCount() == 0) {
//		for(int i = 2 ; i <= 10; ++i)
//			fldSteps->AddString(String("%d",i));
//		fldSteps->ose->SelectString(0,"3");
//	} else {
//		int mapIndex = fldSteps->ose->GetCurSel();
//		if ( mapIndex != -1) {
//			vs->setNumberOfBounds(mapIndex +2);
//			for(int i = 0; i < mapIndex + 2; ++i) {
//				if ( i % 2 == 1) {
//					vs->setBoundColor(i,Color(200,0,0));
//				}
//			}
//		}
//		drw->getRootDrawer()->getDrawerContext()->doDraw();
//	}
//	return 1;
//}
//
//void AnimationSelection::shutdown(int iReturn) {
//	AnimationDrawer *andr = (AnimationDrawer *)drw;
//	andr->animselection = 0;
//	return DisplayOptionsForm2::shutdown();
//}
////----------------------------------------------------------
//AnimationSourceUsage::AnimationSourceUsage(CWnd *par, AnimationDrawer *ldr) 
//	: DisplayOptionsForm2(ldr, par, "Time"), mcs(0), rg(0)
//{
//	if ( ldr->sourceType == AnimationDrawer::sotFEATURE) {
//		BaseMap basemap((*(ldr->datasource))->fnObj);
//		if ( basemap->fTblAtt()) {
//			new StaticText(root, TR("Columns to be used"));
//			mcs = new MultiColumnSelector(root,basemap->tblAtt().ptr(), dmVALUE);
//			RadioGroup *rg = new RadioGroup(root,TR("Type of use"),&columnUsage);
//			new RadioButton(rg, TR("As Z value"));
//			new RadioButton(rg, TR("As feature size"));
//			new RadioButton(rg, TR("As Coordinates"));
//				
//		}
//	}
//  create();
//}
//
//int  AnimationSourceUsage::exec() {
//	if ( rg) rg->StoreData();
//	if ( mcs) {
//		//mcs->StoreData();
//		IntBuf selectedIndexes;
//		mcs->iGetSelected(selectedIndexes);
//		AnimationDrawer *andr = (AnimationDrawer *)drw;
//		for(int  i =0; i < selectedIndexes.iSize(); ++i) {
//			andr->names.push_back(mcs->sName(i));
//		}
//
//	}
//	PreparationParameters pp(NewDrawer::ptGEOMETRY);
//	drw->prepare(&pp);
//	updateMapView();
//
//	return 1;
//}
//
////----------------------------------------------------------

//-------------------------------------------------

//-------------------------------------------------------
