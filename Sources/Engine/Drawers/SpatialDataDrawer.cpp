#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Table\tbl.h"
#include "Engine\Table\Rec.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\MouseClickInfoDrawer.h"


using namespace ILWIS;

//--------------------------------------------------------------------
SpatialDataDrawer::SpatialDataDrawer(DrawerParameters *parms) : 
	ComplexDrawer(parms,"SpatialDataDrawer"),
	useAttTable(false),
	internalDomain(false)
{
}

SpatialDataDrawer::SpatialDataDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	useAttTable(false),
	internalDomain(false)
{
}

SpatialDataDrawer::~SpatialDataDrawer() {
	bm.SetPointer(0);
}

void SpatialDataDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
}

String SpatialDataDrawer::description() const {
	String sName = getName();
	String sDescr = obj->sDescr();
	if ("" != sDescr) 
		sName = String("%S - %S", sName, sDescr);
	return sName;
}

String SpatialDataDrawer::iconName(const String& subtype) const 
{ 
	return getBaseMap()->fnObj.sExt;
}

BaseMapPtr* SpatialDataDrawer::getBaseMap(int index) const { // we return the pointer to avoid copy constructors
	if ( bm.fValid())
		return bm.ptr();
	else if ( oc.fValid())
		return (BaseMapPtr*)oc->ioObj(index).pointer();
	else if ( mpl.fValid())
		return (BaseMapPtr*)mpl[index].pointer();
	return 0;
}

void* SpatialDataDrawer::getDataSource() const {
	if ( bm.fValid())
		return (void *)&bm;
	else if ( oc.fValid())
		return (void *)&oc;
	else if ( mpl.fValid())
		return (void *)&mpl;
	return 0;
}

void SpatialDataDrawer::addDataSource(void *bmap,int options) 
{
	IlwisObject *optr = (IlwisObject *)bmap;
	FileName fn((*optr)->fnObj);
	obj = IlwisObject::obj(fn);
	if ( IOTYPEBASEMAP(fn))
		bm = BaseMap(fn);
	else if (IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
		oc = ObjectCollection(fn);
		if ( !oc->isBaseMapOnly())
			throw ErrorObject(TR("Only collections with feature or raster maps only are allowed as layer"));
	}
	else if ( IOTYPE(fn) == IlwisObject::iotMAPLIST)
		mpl = MapList(fn);
	

	if ( obj.fValid()) {
		if ( getName() == "Unknown")
			setName(obj->sName());
		if ( bm.fValid() && bm->fTblAtt()) {
			attTable = bm->tblAtt();
			//attColumn = attTable->col(0);
		}
		RootDrawer *rootdrawer = getRootDrawer();
		if ( bm.fValid())
			cbBounds = bm->cb();
		if ( oc.fValid())
			cbBounds = oc->cb();
		if ( bm.fValid() && bm->cs() != rootdrawer->getCoordinateSystem()) {
			cbBounds.cMin = rootdrawer->getCoordinateSystem()->cConv(bm->cs(),cbBounds.cMin);
			cbBounds.cMax = rootdrawer->getCoordinateSystem()->cConv(bm->cs(),cbBounds.cMax);
		}
		CoordBounds cbMap = rootdrawer->getMapCoordBounds();
		cbMap += cbBounds;
		rootdrawer->setCoordBoundsMap(cbMap);
		if ( bm.fValid() && bm->fnObj == bm->dm()->fnObj)
			internalDomain = true;
		MouseClickInfoDrawer *mid = (MouseClickInfoDrawer *)(rootdrawer)->getDrawer("MouseClickInfoDrawer");
		if ( mid)
			mid->addDataSource(this);
	}
}

Representation SpatialDataDrawer::getRepresentation(int index) const { // we return the pointer to avoid copy constructors
	BaseMapPtr *basemap = getBaseMap(index);
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

RangeReal SpatialDataDrawer::getStretchRangeReal(int index) const{
	BaseMapPtr *basemap = getBaseMap(index);
	if ( basemap->dm()->pdv()) {
		return basemap->rrMinMax(BaseMapPtr::mmmSAMPLED);
	} else if (  basemap->fTblAtt() && attColumn.fValid() && attColumn->dm()->pdv()) {
		return attColumn->vr()->rrMinMax();
	}
	return RangeReal();
}

bool SpatialDataDrawer::useInternalDomain() const {
	return internalDomain;
}

void SpatialDataDrawer::setRepresentation(const Representation& rp){
	throw ErrorObject("To be done");
}

Table SpatialDataDrawer::getAtttributeTable(int index) const{
	if ( !attTable.fValid()) {
		if( getBaseMap(index)->fTblAtt()) {
			(const_cast<SpatialDataDrawer *>(this))->attTable = getBaseMap()->tblAtt();
		}
	}
	return attTable;
}
void SpatialDataDrawer::setAttributeTable(const Table& tbl){
	attTable = tbl;
}

Column SpatialDataDrawer::getAtttributeColumn() const{
	if ( useAttributeTable())
		return attColumn;
	return "";
}
void SpatialDataDrawer::setAttributeColumn(const String& name){
	Table tbl = getAtttributeTable();
	if ( tbl.fValid()) {
		attColumn = getAtttributeTable()->col(name);
	}
}

bool SpatialDataDrawer::useAttributeTable() const{
	return useAttTable ;
}

void SpatialDataDrawer::setUseAttributeTable(bool yesno){
	useAttTable = yesno;
}

String SpatialDataDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);
	if ( attTable.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeTable",fnView, attTable);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.scVal(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.scVal(),"UseAttributes",fnView, useAttTable);
	ObjectInfo::WriteElement(parentSection.scVal(),"BaseMap",fnView, bm);

	return parentSection;

}

void SpatialDataDrawer::load(const FileName& fnView, const String& parentSection){
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

CoordBounds SpatialDataDrawer::cb() {
	return cbBounds;
}
