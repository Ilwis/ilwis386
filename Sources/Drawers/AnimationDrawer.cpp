#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\objlist.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

int AnimationDrawer::timerIdCounter=5000;

ILWIS::NewDrawer *createAnimationDrawer(DrawerParameters *parms) {
	return new AnimationDrawer(parms);
}

AnimationDrawer::AnimationDrawer(DrawerParameters *parms) : 
	ComplexDrawer(parms,"AnimationDrawer"),
	timerid(iUNDEF),
	interval(rUNDEF),
	datasource(0),
	sourceType(sotUNKNOWN),
	featurelayer(0),
	loop(true)
{
	setTransparency(1);
	timerid = timerIdCounter++;
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

void AnimationDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
	if ( sourceType == sotFEATURE ) {
		BaseMap basemap((*datasource)->fnObj);
		ILWIS::DrawerParameters parms(getDrawerContext(), getDrawerContext()->getRootDrawer());
		if ( drawers.size() > 0) {
			clear();
		}
		featurelayer = (FeatureLayerDrawer *)IlwWinApp()->getDrawer("FeatureLayerDrawer", "Ilwis38", &parms);
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
				fset->setActive(false);
				fset->getZMaker()->setTable(basemap->tblAtt(), names.at(i));
				fset->getZMaker()->setThreeDPossible(true);
			}
			//featurelayer->getZMaker()->setTable(basemap->tblAtt(),names);
			//featurelayer->getZMaker()->setThreeDPossible(true);
		}
		addDrawer(featurelayer);

	}

}

void AnimationDrawer::addDataSource(void *data, int options){
	IlwisObject *obj = dynamic_cast<IlwisObject *>((IlwisObject *)data);
	if ( obj) {
		datasource = new IlwisObject(*obj);
		IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType((*datasource)->fnObj);
		if  (type == IlwisObject::iotSEGMENTMAP || 
			 type == IlwisObject::iotPOINTMAP || 
			 type == IlwisObject::iotPOLYGONMAP) {
			 sourceType = sotFEATURE;
		}
		if ( type == IlwisObject::iotRASMAP) {
			sourceType = sotMAPLIST;
		}
	}

}

HTREEITEM AnimationDrawer::configure(LayerTreeView  *tv, HTREEITEM displayOptionsLastItem){
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,TVI_ROOT,this, (DisplayOptionItemFunc)&AnimationDrawer::animationControl);
				
	HTREEITEM after = tv->GetTreeCtrl().GetParentItem(displayOptionsLastItem);
	HTREEITEM htiAnimation = InsertItem(description(),"Animation",item, isActive(), after); 

	ComplexDrawer::configure(tv,htiAnimation);
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tv,htiAnimation,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationTiming);
	InsertItem("Timing","History",item2);
	item2 = new DisplayOptionTreeItem(tv,htiAnimation,this,
					0,
					(DisplayOptionItemFunc)&AnimationDrawer::animationSourceUsage);
	InsertItem("Source usage",".mpv",item2);
	return htiAnimation;

}

bool AnimationDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	ComplexDrawer::draw(norecursion, cbArea);
	/*if ( featurelayer)
		featurelayer->draw(norecursion, cbArea);*/
	return true;
}

void AnimationDrawer::animationTiming(CWnd *parent) {
	new AnimationTiming(parent, this);
}

void AnimationDrawer::animationControl(CWnd *parent) {
	new AnimationControl(parent, this);
}

void AnimationDrawer::animationSourceUsage(CWnd *parent) {
	new AnimationSourceUsage(parent, this);
}

void AnimationDrawer::timedEvent(UINT _timerid) {
	if ( timerid == _timerid) {
		if ( featurelayer){
			if ( names.size() > 0 && index < names.size()-1) {
				featurelayer->getDrawer(index)->setActive(false);
				featurelayer->getDrawer(++index)->setActive(true);
			} else {
				if (loop)
					index = 0;
			}
		}
		getDrawerContext()->getDocument()->mpvGetView()->Invalidate();
	}
}

//---------------------------------------------------------
AnimationTiming::AnimationTiming(CWnd *par, AnimationDrawer *ldr) 
	: DisplayOptionsForm(ldr, par, "Time")
{

  frSecondsPerFrame = new FieldReal(root, "Seconds per frame",&ldr->interval,ValueRangeReal(0.1,1000,0.1));
  create();
}

void  AnimationTiming::apply() {
	frSecondsPerFrame->StoreData();
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->interval = andr->interval * 1000.0;
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}

//----------------------------------------------------------
AnimationControl::AnimationControl(CWnd *par, AnimationDrawer *ldr) 
	: DisplayOptionsForm(ldr, par, "Time")
{
	
  create();
}

void  AnimationControl::apply() {
	AnimationDrawer *andr = (AnimationDrawer *)drw;
	andr->index = 0;
	drw->getDrawerContext()->getDocument()->mpvGetView()->SetTimer(andr->timerid,andr->interval,0);
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}

//----------------------------------------------------------
AnimationSourceUsage::AnimationSourceUsage(CWnd *par, AnimationDrawer *ldr) 
	: DisplayOptionsForm(ldr, par, "Time"), mcs(0), rg(0)
{
	if ( ldr->sourceType == AnimationDrawer::sotFEATURE) {
		BaseMap basemap((*(ldr->datasource))->fnObj);
		if ( basemap->fTblAtt()) {
			new StaticText(root, "Columns to be used");
			mcs = new MultiColumnSelector(root,basemap->tblAtt().ptr(), dmVALUE);
			RadioGroup *rg = new RadioGroup(root,"Type of use",&columnUsage);
			new RadioButton(rg, "As Z value");
			new RadioButton(rg, "As feature size");
			new RadioButton(rg, "As Coordinates");
				
		}
	}
  create();
}

void  AnimationSourceUsage::apply() {
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
	PreparationParameters pp(NewDrawer::ptRENDER);
	drw->prepare(&pp);
	updateMapView();
}
