#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"


using namespace ILWIS;

void displayOptionSubRpr(NewDrawer *drw, CWnd *parent) {
	new RepresentationForm(parent, (AbstractMapDrawer *)drw);
}

void displayOptionAttColumn(NewDrawer *drw, CWnd *parent) {
	new ChooseAttributeColumnForm(parent, (AbstractMapDrawer *)drw);
}

void displayOptionStretch(NewDrawer *drw, CWnd *parent) {
	new SetStretchForm(parent, (AbstractMapDrawer *)drw);
}

void setcheckRpr(NewDrawer *drw, void *value) {
	drw->setDrawMethod(NewDrawer::drmRPR);
}

//--------------------------------------------------------------------
AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms) : 
	AbstractObjectDrawer(parms,"AbstractMapDrawer"),
	stretched(false),
	useAttTable(false),
	colorCheck(0)
{
}

AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms, const String& name) : 
	AbstractObjectDrawer(parms,name),
	stretched(false),
	useAttTable(false),
	colorCheck(0)
{
}

AbstractMapDrawer::~AbstractMapDrawer() {
	delete colorCheck;
}

void AbstractMapDrawer::prepare(PreparationParameters *pp){
	AbstractObjectDrawer::prepare(pp);
}


BaseMap AbstractMapDrawer::getBaseMap() const {
	BaseMapPtr *ptr = dynamic_cast<BaseMapPtr *>(obj);
	if ( ptr)
		return BaseMap(ptr->fnObj);
	return BaseMap();
}
void AbstractMapDrawer::setDataSource(void *bmap) 
{
	AbstractObjectDrawer::setDataSource(bmap);
	BaseMap bm = getBaseMap();
	if ( bm.fValid()) {
		rpr = bm->dm()->rpr();
		if ( bm->fTblAtt()) {
			attTable = bm->tblAtt();
			attColumn = attTable->col(0);
		}
		if ( bm->dm()->pdv()) {
			rrStretch = bm->vr()->rrMinMax();
		} else if (  bm->fTblAtt() && attColumn.fValid() && attColumn->dm()->pdv()) {
			rrStretch = attColumn->vr()->rrMinMax();
		}
	}
}

Representation AbstractMapDrawer::getRepresentation() const {
	return rpr;
}

void AbstractMapDrawer::setRepresentation(const Representation& rp){
	rpr = rp;
}

bool AbstractMapDrawer::isLegendUsefull() const {
	BaseMap bm = getBaseMap();
	if (bm.fValid() && bm->dm()->pdv() && stretched) 
		return true;
	return drm != drmSINGLE;
}

RangeReal AbstractMapDrawer::getStretchRange() const{
	return rrStretch;
}

void AbstractMapDrawer::setStretchRange(const RangeReal& rr){
	if ( rr != rrStretch && !rrStretch.fValid())
		stretched = true;
	rrStretch = rr;
}

RangeReal AbstractMapDrawer::getLegendRange() const{
	return rrLegendRange;
}

void AbstractMapDrawer::setLegendRange(const RangeReal& rr){
	rrLegendRange = rr;
}

Table AbstractMapDrawer::getAtttributeTable() const{
	return attTable;
}
void AbstractMapDrawer::setAttributeTable(const Table& tbl){
	attTable = tbl;
}

Column AbstractMapDrawer::getAtttributeColumn() const{
	return attColumn;
}
void AbstractMapDrawer::setAttributeColumn(const String& name){
	attColumn = attTable->col(name);
}

bool AbstractMapDrawer::useAttributeTable() const{
	return useAttTable ;
}

void AbstractMapDrawer::setUseAttributeTable(bool yesno){
	useAttTable = yesno;
}

bool AbstractMapDrawer::isStretched() const {
	return stretched;
}

HTREEITEM AbstractMapDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = AbstractObjectDrawer::configure(tv,parent);
	BaseMap bm = getBaseMap();
	if ( !bm.fValid())
		return parent;
	SetColors(tv,parent,bm);
	if ( bm->dm()->pdsrt()) {
		String sName = String("Attribute table");
		int iImg = IlwWinApp()->iImage(".tbt");
		HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, parent);
		tv->GetTreeCtrl().SetCheck(htiDisplayOptions, useAttTable);
		DisplayOptionAttTable *item = new DisplayOptionAttTable(tv, htiDisplayOptions, this, displayOptionAttColumn);
		item->SwitchCheckBox(useAttTable);
		tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);
	}
	if ( bm->dm()->pdv() || (attColumn.fValid() && attColumn->dm()->pdv())) {
		String sName = String("Stretch");
		int iImg = IlwWinApp()->iImage("Valuerange");
		HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, parent);
		tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTreeItem(tv, this, displayOptionStretch));
		iImg = IlwWinApp()->iImage("Calculationsingle");
		RangeReal rr =getStretchRange();
		tv->GetTreeCtrl().InsertItem(String("Lower : %f",rr.rLo()).scVal(), iImg, iImg, htiDisplayOptions);
		tv->GetTreeCtrl().InsertItem(String("Upper : %f",rr.rHi()).scVal(), iImg, iImg, htiDisplayOptions);
	}


	return hti;
}

HTREEITEM AbstractMapDrawer::SetColors(LayerTreeView  *tv, HTREEITEM parent,const BaseMap& bm) {
	Representation rpr = getRepresentation();
	NewDrawer::DrawMethod method = getDrawMethod();
	int iImg = IlwWinApp()->iImage("Colors");
	HTREEITEM colorItem = tv->GetTreeCtrl().InsertItem("Colors", iImg, iImg, parent);
	colorCheck = new SetChecks(tv,this);
	if ( rpr.fValid() ) {
		String sName = String("Representation");
		iImg = IlwWinApp()->iImage(".rpr");
		bool v =  method == NewDrawer::drmRPR;
		HTREEITEM htiDisplayOptions = tv->GetTreeCtrl().InsertItem(sName.scVal(), iImg, iImg, colorItem);
		tv->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)new DisplayOptionTreeItem(tv, this, displayOptionSubRpr,htiDisplayOptions,colorCheck,setcheckRpr));
		tv->GetTreeCtrl().SetCheck(htiDisplayOptions, v);
		//iImg = IlwWinApp()->iImage("CalculationMultiple");
		if ( v)
			tv->GetTreeCtrl().InsertItem(String("Value : %S",getRepresentation()->sName()).scVal(), iImg, iImg, htiDisplayOptions);
	}
	return colorItem;
}

//--------------------------------
RepresentationForm::RepresentationForm(CWnd *wPar, AbstractMapDrawer *dr) : 
	FormBaseDialog(wPar,String("Representation of %S",dr->getName()),fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS),
	rpr(dr->getRepresentation()->sName()),
	drw(dr),
	view((LayerTreeView *)wPar)
{
	fldRpr = new FieldRepresentation(root, "Representation", &rpr);
	create();
}

int RepresentationForm::exec() {
	return 1;
}

void  RepresentationForm::OnCancel() {
	fldRpr->StoreData();
	drw->setRepresentation(Representation(FileName(rpr)));
	PreparationParameters pp(NewDrawer::ptRENDER, 0, drw);
	drw->prepare(&pp);
	MapCompositionDoc* doc = view->GetDocument();
	doc->ChangeState();
	doc->UpdateAllViews(0,0);
}

//--------------------------------------
ChooseAttributeColumnForm::ChooseAttributeColumnForm(CWnd *wPar, AbstractMapDrawer *dr) : 
	FormBaseDialog(wPar,String("Attribute Column of %S",dr->getName()),fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS),
	attTable(dr->getAtttributeTable()),
	attColumn(dr->getAtttributeColumn()->sName()),
	drw(dr)

{
	new FieldColumn(root, "Column", attTable, &attColumn);
	create();
}

int ChooseAttributeColumnForm::exec() {
	return 1;
}

void  ChooseAttributeColumnForm::OnCancel() {
}

//-------------------------------------
SetStretchForm::SetStretchForm(CWnd *wPar, AbstractMapDrawer *dr) : 
	FormBaseDialog(wPar,String("Stretch Values of %S",dr->getName()),fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS),
	rr(dr->getStretchRange()),
	drw(dr)

{
	new FieldRangeReal(root,"Stretch",&rr);
	create();
}

int SetStretchForm::exec() {
	return 1;
}

void  SetStretchForm::OnCancel() {
}
//------------------------------------------------------


