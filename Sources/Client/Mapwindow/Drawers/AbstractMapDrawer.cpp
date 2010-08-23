#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
//#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Mapwindow\Drawers\MouseClickInfoDrawer.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"


using namespace ILWIS;

//--------------------------------------------------------------------
AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms) : 
	AbstractObjectDrawer(parms,"AbstractMapDrawer"),
	useAttTable(false),
	internalDomain(false)
{
}

AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms, const String& name) : 
	AbstractObjectDrawer(parms,name),
	useAttTable(false),
	internalDomain(false)
{
}

AbstractMapDrawer::~AbstractMapDrawer() {
}

void AbstractMapDrawer::prepare(PreparationParameters *pp){
	AbstractObjectDrawer::prepare(pp);
}


BaseMap AbstractMapDrawer::getBaseMap() const {
	if ( bm.fValid())
		return bm;
	BaseMapPtr *ptr = dynamic_cast<BaseMapPtr *>(obj);
	if ( ptr){
		(const_cast<AbstractMapDrawer *>(this))->bm =  BaseMap(ptr->fnObj);
	}
	return bm;
}
void AbstractMapDrawer::addDataSource(void *bmap,int options) 
{
	AbstractObjectDrawer::addDataSource(bmap, options);
	BaseMap bm = getBaseMap();
	if ( bm.fValid()) {
		if ( bm->fTblAtt()) {
			attTable = bm->tblAtt();
			attColumn = attTable->col(0);
		}
		DrawerContext *context = getDrawerContext();
		CoordBounds cb = bm->cb();
		if ( bm->cs() != context->getCoordinateSystem()) {
			cb.cMin = context->getCoordinateSystem()->cConv(bm->cs(),cb.cMin);
			cb.cMax = context->getCoordinateSystem()->cConv(bm->cs(),cb.cMax);
		}
		CoordBounds cbMap = context->getMapCoordBounds();
		cbMap += cb;
		context->setCoordBoundsMap(cbMap);
		if ( bm->fnObj == bm->dm()->fnObj)
			internalDomain = true;
		MouseClickInfoDrawer *mid = (MouseClickInfoDrawer *)((ComplexDrawer *)getRootDrawer())->getDrawer("MouseClickInfoDrawer");
		if ( mid)
			mid->addDataSource(&bm);
	}
}

Representation AbstractMapDrawer::getRepresentation() const {
	BaseMap basemap = getBaseMap();
	if ( basemap.fValid()) {
		if ( useAttTable && attColumn.fValid())
			return attColumn->dm()->rpr();
		else
			return basemap->dm()->rpr();
	}
	return Representation();
}

RangeReal AbstractMapDrawer::getStretchRangeReal() const{
	BaseMap basemap = getBaseMap();
	if ( basemap->dm()->pdv()) {
		return basemap->vr()->rrMinMax();
	} else if (  basemap->fTblAtt() && attColumn.fValid() && attColumn->dm()->pdv()) {
		return attColumn->vr()->rrMinMax();
	}
	return RangeReal();
}

bool AbstractMapDrawer::useInternalDomain() const {
	return internalDomain;
}

void AbstractMapDrawer::setRepresentation(const Representation& rp){
	throw ErrorObject("To be done");
}

Table AbstractMapDrawer::getAtttributeTable() const{
	return attTable;
}
void AbstractMapDrawer::setAttributeTable(const Table& tbl){
	attTable = tbl;
}

Column AbstractMapDrawer::getAtttributeColumn() const{
	if ( useAttributeTable())
		return attColumn;
	return "";
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

void AbstractMapDrawer::setColumnCheckumn(void *w, LayerTreeView *view) {
	bool yesno = *(bool *)w;
	setUseAttributeTable(yesno);
}


void AbstractMapDrawer::setInfoMode(void *v,LayerTreeView *tv) {
	bool value = *(bool *)v;
	setInfo(value);
}

//------------------------------------UI--------------------------------

HTREEITEM AbstractMapDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = AbstractObjectDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(SetCheckFunc)&AbstractMapDrawer::setInfoMode);
	InsertItem("Info","info", item, info);

	BaseMap bm = getBaseMap();
	if ( !bm.fValid())
		return parent;
	if ( bm->dm()->pdsrt()) {
		item = new DisplayOptionTreeItem(tv,parent,this,
					(SetCheckFunc)&AbstractMapDrawer::setColumnCheckumn,
					(DisplayOptionItemFunc)&AbstractMapDrawer::displayOptionAttColumn);

		HTREEITEM itemColumn = InsertItem("Attribute table",".tbt",item,(int)useAttTable);
		if ( useAttributeTable() && attColumn.fValid())
			InsertItem(tv,itemColumn,String("Column : %S",attColumn->sName()),"column");
	}
	return hti;
}

void AbstractMapDrawer::displayOptionAttColumn(CWnd *w) {
	if (useAttributeTable())
		new ChooseAttributeColumnForm(w, this);
}



//--------------------------------------
ChooseAttributeColumnForm::ChooseAttributeColumnForm(CWnd *wPar, AbstractMapDrawer *dr) : 
	DisplayOptionsForm(dr,wPar,"Choose attribute column"),
	attTable(((AbstractMapDrawer *)dr)->getAtttributeTable()),
	attColumn(((AbstractMapDrawer *)dr)->getAtttributeColumn()->sName())
{
	fc = new FieldColumn(root, "Column", attTable, &attColumn);
	create();
}


void  ChooseAttributeColumnForm::apply() {
	fc->StoreData();
	if ( attColumn != "")
		((AbstractMapDrawer *)drw)->setAttributeColumn(attColumn);
	updateMapView();

}


