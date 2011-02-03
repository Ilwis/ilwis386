#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\DisplayOptionsLegend.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Hs\Drwforms.hs"

DisplayOptionsLegend::DisplayOptionsLegend(LayerTreeView  *tv, HTREEITEM displayOptionsLastItem) : 
	ltv(tv), parent(displayOptionsLastItem),htiLeg(0),setdr(0),animDrw(0)
{
}

void DisplayOptionsLegend::createForAnimation(AnimationDrawer *animdr) {
	animDrw = animdr;
	createForSet((SetDrawer *)animDrw->getDrawer(0));
}

void DisplayOptionsLegend::createForSet(SetDrawer *setdrw) {
	setdr = setdrw;
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)setdr->getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();
	if ( animDrw) {
		colorCheck = new SetChecks(ltv,animDrw,(SetCheckFunc)&AnimationDrawer::setcheckRpr);
	}
	else {
		colorCheck = new SetChecks(ltv,setdr,(SetCheckFunc)&SetDrawer::setcheckRpr);
	}
	rpr = setdr->getRepresentation();
	if ( rpr.fValid() ) {
		DisplayOptionItemFunc func = animDrw == 0 ? (DisplayOptionItemFunc)&SetDrawer::displayOptionSubRpr : (DisplayOptionItemFunc)&AnimationDrawer::displayOptionSubRpr;
		NewDrawer *dr = animDrw ? (NewDrawer *)animDrw : (NewDrawer *)setdr;
		bool usesRpr = setdr->getDrawMethod() == NewDrawer::drmRPR;
		DisplayOptionTreeItem *item = new DisplayOptionTreeItem(ltv,parent,dr,func,0,colorCheck);
		rprItem = setdr->InsertItem("Representation", ".rpr", item, (int)usesRpr);
		insertLegendItems(dvs);
		if ( usesRpr)
			setdr->InsertItem(ltv, rprItem,String("Value : %S",rpr->sName()),".rpr");
	}
}

void DisplayOptionsLegend::insertLegendItems(const DomainValueRangeStruct& dvs) {
	String sName = SDCRemLegend;
	int iImgLeg = IlwWinApp()->iImage("legend");
	htiLeg = ltv->GetTreeCtrl().InsertItem(sName.scVal(), iImgLeg, iImgLeg, rprItem);
	if (0 == htiLeg)
		return;

	if ( rpr->prg() || rpr->prv())
		insertLegendItemsValue(dvs);
	else if ( rpr->prc()) {
		insertLegendItemsClass();
	}
	ltv->GetTreeCtrl().Expand(htiLeg, TVE_EXPAND);

}

void DisplayOptionsLegend::insertLegendItemsValue(const DomainValueRangeStruct& dvs){
	if ( htiLeg) {
		ltv->DeleteAllItems(htiLeg, true);
	}
	ltv->GetTreeCtrl().SetItemData(htiLeg, (DWORD_PTR)new ObjectLayerTreeItem(ltv, rpr.pointer()));
	int iItems = 5;
	double rStep = dvs.rStep();
	RangeReal rr = setdr->getStretchRangeReal();
	if (rStep > 1e-6) {
		int iSteps = 1 + round(rr.rWidth() / rStep);
		if (iSteps < 2)
			iSteps = 2;
		if (iSteps <= 11)
			iItems = iSteps;
	}
	for (int i = 0; i < iItems; ++i) {
		double rMaxItem = iItems - 1;
		double rVal = rr.rLo() + i / rMaxItem * rr.rWidth();
		String sName = dvs.sValue(rVal, 0);
		HTREEITEM hti = ltv->GetTreeCtrl().InsertItem(sName.scVal(), htiLeg);
		ltv->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendValueLayerTreeItem(ltv, setdr, dvs, rVal));		
	}
}

void DisplayOptionsLegend::insertLegendItemsClass(){
	if ( htiLeg) {
		ltv->DeleteAllItems(htiLeg, true);
	}
	ltv->GetTreeCtrl().SetItemData(htiLeg, (DWORD_PTR)new LegendLayerTreeItem(ltv, setdr));		
	DomainClass* dc = rpr->dm()->pdc();
	int iItems = dc->iNettoSize();
	for (int i = 1; i <= iItems; ++i) {
		int iRaw = dc->iKey(i);
		String sName = dc->sValueByRaw(iRaw, 0);
		HTREEITEM hti = ltv->GetTreeCtrl().InsertItem(sName.scVal(), htiLeg);
		ltv->GetTreeCtrl().SetCheck(hti);
		NewDrawer *dr = animDrw ? (NewDrawer *)animDrw : (NewDrawer *)setdr;
		ltv->GetTreeCtrl().SetItemData(hti, (DWORD_PTR)new LegendClassLayerTreeItem(ltv, dr, rpr->dm(), iRaw));		
	}
}

void DisplayOptionsLegend::updateLegendItem() {
	AbstractMapDrawer *mapDrawer = (AbstractMapDrawer *)setdr->getParentDrawer();
	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();
	DomainClass* dc = rpr->dm()->pdc();
	if ( dc)
		insertLegendItemsClass();
	else
		insertLegendItemsValue(dvs);
}

void DisplayOptionsLegend::setcheckRpr(void *value, LayerTreeView *tree) {
	if ( value == 0)
		return;
	HTREEITEM hItem = *((HTREEITEM *)value);
	String name = tree->getItemName(hItem);
	if ( name == sUNDEF)
		return;
	int index = name.find_last_of("|");

	if ( index == string::npos)
		return;

	String method = name.substr(index + 1);
	if ( method == "Representation")
		setdr->setDrawMethod(NewDrawer::drmRPR);
	else if ( method == "Single Color")
		setdr->setDrawMethod(NewDrawer::drmSINGLE);
	else if ( method == "Multiple Colors"){
		setdr->setDrawMethod(NewDrawer::drmMULTIPLE);
	}
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			SetDrawer *sdr = (SetDrawer *)animDrw->getDrawer(i);
			sdr->modifyLineStyleItem(tree, (setdr->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		setdr->modifyLineStyleItem(tree, (setdr->getDrawMethod() == NewDrawer::drmRPR && rpr.fValid() && rpr->prc()));
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdr->prepareChildDrawers(&pp);
	}
	setdr->getRootDrawer()->getDrawerContext()->doDraw();
}


//---------------------------------------------------
RepresentationFormL::RepresentationFormL(CWnd *wPar, SetDrawer *dr,AnimationDrawer *adr) : 
	DisplayOptionsForm(dr,wPar,"Set Representation"),
	rpr(dr->getRepresentation()->sName()),
	setDrawer(dr),
	animDrw(adr)
{
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

void  RepresentationFormL::apply() {
	fldRpr->StoreData();
	if ( animDrw) {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		for(int i = 0; i < animDrw->getDrawerCount(); ++i) {
			SetDrawer *sdr = (SetDrawer *)animDrw->getDrawer(i);
			sdr->setRepresentation(rpr);
			sdr->prepareChildDrawers(&pp);
		}
	}
	else {
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setDrawer->setRepresentation(rpr);
		setDrawer->prepareChildDrawers(&pp);
	}
	/*SetDrawer *setDrawer = (SetDrawer *)drw;
	setDrawer->setRepresentation(rpr);
	PreparationParameters pp(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&pp);*/
	updateMapView();

	HTREEITEM child = view->GetTreeCtrl().GetNextItem(setDrawer->getRprItem(), TVGN_CHILD);
	if ( child) {
		FileName fn(rpr);
		String name("Value : %S",fn.sFile);
		TreeItem titem;
		view->getItem(child,TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIS_SELECTED,titem);
	
		strcpy(titem.item.pszText,name.scVal());
		view->GetTreeCtrl().SetItem(&titem.item);
	}
	Representation setRpr = Representation(FileName(rpr));
	HTREEITEM parent = view->getAncestor(setDrawer->getRprItem(), 2);
	if ( parent) {
		HTREEITEM stretchItem = setDrawer->findTreeItemByName(view,parent,"Stretch");
		if ( !stretchItem && setRpr->prg()) {
			setDrawer->insertStretchItem(view,parent);
		}
		if ( stretchItem && setRpr->prv()) {
			view->GetTreeCtrl().DeleteItem(stretchItem);
		}
		view->collectStructure();
	}
	if ( animDrw) {
		animDrw->updateLegendItem();
	} else {
		setDrawer->updateLegendItem();
	}

}







