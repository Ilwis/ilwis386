#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\Ilwis.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\Rec.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
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
	ComplexDrawer(parms,"AbstractMapDrawer"),
	useAttTable(false),
	internalDomain(false)
{
}

AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	useAttTable(false),
	internalDomain(false)
{
}

AbstractMapDrawer::~AbstractMapDrawer() {
	bm.SetPointer(0);
}

void AbstractMapDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
}

String AbstractMapDrawer::description() const {
	String sName = getName();
	String sDescr = bm->sDescr();
	if ("" != sDescr) 
		sName = String("%S - %S", sName, sDescr);
	return sName;
}

String AbstractMapDrawer::iconName(const String& subtype) const 
{ 
	return getBaseMap()->fnObj.sExt;
}

BaseMapPtr* AbstractMapDrawer::getBaseMap() const { // we return the pointer to avoid copy constructors
	if ( bm.fValid())
		return bm.ptr();
	return 0;
}

void* AbstractMapDrawer::getDataSource() const {
	return (void *)&bm;
}

void AbstractMapDrawer::addDataSource(void *bmap,int options) 
{
	bm = BaseMap((*((BaseMap *)(bmap)))->fnObj);
	if ( bm.fValid()) {
		if ( getName() == "Unknown")
			setName(bm->sName());
		if ( bm->fTblAtt()) {
			attTable = bm->tblAtt();
			//attColumn = attTable->col(0);
		}
		RootDrawer *rootdrawer = getRootDrawer();
		CoordBounds cb = bm->cb();
		if ( bm->cs() != rootdrawer->getCoordinateSystem()) {
			cb.cMin = rootdrawer->getCoordinateSystem()->cConv(bm->cs(),cb.cMin);
			cb.cMax = rootdrawer->getCoordinateSystem()->cConv(bm->cs(),cb.cMax);
		}
		CoordBounds cbMap = rootdrawer->getMapCoordBounds();
		cbMap += cb;
		rootdrawer->setCoordBoundsMap(cbMap);
		if ( bm->fnObj == bm->dm()->fnObj)
			internalDomain = true;
		MouseClickInfoDrawer *mid = (MouseClickInfoDrawer *)(rootdrawer)->getDrawer("MouseClickInfoDrawer");
		if ( mid)
			mid->addDataSource(this);
	}
}

Representation AbstractMapDrawer::getRepresentation() const { // we return the pointer to avoid copy constructors
	BaseMapPtr *basemap = getBaseMap();
	if ( basemap != 0) {
		if ( useAttTable && attColumn.fValid()) {
			Column col = getAtttributeColumn();
			if ( col.fValid() && attColumn->dm()->rpr().fValid() )
				return attColumn->dm()->rpr();
		}
		else {
			if (basemap->dm()->rpr().fValid()) {
				return basemap->dm()->rpr();
			}
		}
	}
	return Representation();
}

RangeReal AbstractMapDrawer::getStretchRangeReal() const{
	BaseMapPtr *basemap = getBaseMap();
	if ( basemap->dm()->pdv()) {
		return basemap->rrMinMax(BaseMapPtr::mmmSAMPLED);
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
	if ( !attTable.fValid()) {
		if( getBaseMap()->fTblAtt()) {
			(const_cast<AbstractMapDrawer *>(this))->attTable = getBaseMap()->tblAtt();
		}
	}
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
	Table tbl = getAtttributeTable();
	if ( tbl.fValid()) {
		attColumn = getAtttributeTable()->col(name);
	}
}

bool AbstractMapDrawer::useAttributeTable() const{
	return useAttTable ;
}

void AbstractMapDrawer::setUseAttributeTable(bool yesno){
	useAttTable = yesno;
}

Ilwis::Record AbstractMapDrawer::rec(const Coord& crd)
{
  long iRec = getBaseMap()->iRaw(crd);
  if (iRec > 0) {
    Table tbl = getBaseMap()->tblAtt();
    if (tbl.fValid()) 
      return tbl->rec(iRec);
  }  
  return Ilwis::Record();
}

String AbstractMapDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);
	if ( attTable.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeTable",fnView, attTable);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.scVal(),"UseAttributes",fnView, useAttTable);
	ObjectInfo::WriteElement(parentSection.scVal(),"BaseMap",fnView, bm);

	return parentSection;

}

void AbstractMapDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.scVal(),"AttributeTable",fnView, attTable);
	if ( attTable.fValid()) {
		String colname;
		ObjectInfo::ReadElement(parentSection.scVal(),"AttributeColumn",fnView, colname);
		attColumn = attTable->col(colname);
	}
	ObjectInfo::ReadElement(parentSection.scVal(),"UseAttributes",fnView, useAttTable);
	FileName fn;
	ObjectInfo::ReadElement(parentSection.scVal(),"BaseMap",fnView, fn);
	bm = BaseMap(fn);

}

void AbstractMapDrawer::inactivateOtherPalettes(ILWIS::Palette * palette)
{

}


