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
//#include "Engine\Drawers\TextDrawer.h"
//#include "Engine\Drawers\MouseClickInfoDrawer.h"


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
//	bm.SetPointer(0);
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
	if ( mpl.fValid())
		return ".mpl";
	else
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

IlwisObjectPtr *SpatialDataDrawer::getObject() const {
	if ( bm.fValid())
		return bm.ptr();
	else if ( oc.fValid())
		return oc.pointer();
	else if ( mpl.fValid())
		return mpl.pointer();
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

RangeReal SpatialDataDrawer::getRange() const{
	if ( bm.fValid())
		return bm->rrMinMax();
	else if ( oc.fValid())
		return oc->getRange();
	else if ( mpl.fValid())
		return mpl->getRange();
	return RangeReal();
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
		if ( !oc->getStatusFor(ObjectCollection::csALLBASEMAP))
			throw ErrorObject(TR("Only collections with feature or raster maps only are allowed as layer"));
	}
	else if ( IOTYPE(fn) == IlwisObject::iotMAPLIST)
		mpl = MapList(fn);
	

	if ( obj.fValid()) {
		if ( getName() == "Unknown")
			setName(obj->sName());
		if ( bm.fValid() && bm->fTblAtt()) {
			try {
				attTable = bm->tblAtt();
			} catch (ErrorObject& err) {
				useAttTable = false;
				bm->SetAttributeTable(Table());
				err.Show();
			}
		}
		RootDrawer *rootdrawer = getRootDrawer();
		if ( bm.fValid())
			cbBounds = bm->cb();
		if ( oc.fValid())
			cbBounds = oc->cb(); // cbBounds for an ObjectCollection are the bounds that enclose all basemaps, in the CoordinateSystem of the first basemap
		if (bm.fValid())
			rootDrawer->addCoordBounds(bm->cs(), cbBounds, options & dsoEXTENDBOUNDS);
		if ( bm.fValid() && bm->fnObj == bm->dm()->fnObj)
			internalDomain = true;
		//MouseClickInfoDrawer *mid = (MouseClickInfoDrawer *)(rootdrawer)->getDrawer("MouseClickInfoDrawer");
		//if ( mid)
		//	mid->addDataSource(this);
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

// try to deprecate getAttributeTable(), bad api
Table SpatialDataDrawer::getAtttributeTable(int index) const{
	if ( !attTable.fValid()) {
		if( getBaseMap(index)->fTblAtt()) {
			(const_cast<SpatialDataDrawer *>(this))->attTable = getBaseMap(index)->tblAtt();
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
		attColumn = tbl->col(name);
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
		ObjectInfo::WriteElement(parentSection.c_str(),"AttributeTable",fnView, attTable);
	if ( attColumn.fValid())
		ObjectInfo::WriteElement(parentSection.c_str(),"AttributeColumn",fnView, attColumn->sName());
	ObjectInfo::WriteElement(parentSection.c_str(),"UseAttributes",fnView, useAttTable);
	FileName fn;
	if ( bm.fValid())
		fn = bm->fnObj;
	if ( oc.fValid())
		fn = oc->fnObj;
	if ( mpl.fValid())
		fn = mpl->fnObj;

	ObjectInfo::WriteElement(parentSection.c_str(),"Object",fnView, fn);

	if ( attTable.fValid()) {
		ObjectInfo::WriteElement(parentSection.c_str(),"AttributeTable",fnView, attTable);
		ObjectInfo::WriteElement(parentSection.c_str(),"AttributeColumn",fnView, attColumn);
	}

	return parentSection;

}

void SpatialDataDrawer::load(const FileName& fnView, const String& parentSection){
	ObjectInfo::ReadElement(parentSection.c_str(),"AttributeTable",fnView, attTable);
	if ( attTable.fValid()) {
		String colname;
		ObjectInfo::ReadElement(parentSection.c_str(),"AttributeColumn",fnView, colname);
		attColumn = attTable->col(colname);
	}
	ObjectInfo::ReadElement(parentSection.c_str(),"UseAttributes",fnView, useAttTable);
	FileName fn;
	ObjectInfo::ReadElement(parentSection.c_str(),"Object",fnView, fn);
	IlwisObject object = IlwisObject::obj(fn);
	addDataSource(&object);

	ObjectInfo::ReadElement(parentSection.c_str(),"AttributeTable",fnView, fn);
	if ( fn.fValid()) {
		attTable = Table(fn);
		if ( attTable.fValid()) {
			String name;
			ObjectInfo::ReadElement(parentSection.c_str(),"AttributeColumn",fnView, name);
			attColumn = attTable->col(name);
		}
	}
	ComplexDrawer::load(fnView, parentSection);

}

CoordBounds SpatialDataDrawer::cb() {
	return cbBounds;
}

IlwisObjectPtr *SpatialDataDrawer::getSourceSupportObject(IlwisObject::iotIlwisObjectType type){
	if ( type == IlwisObject::iotDOMAIN) {
		if ( getBaseMap() != 0)
			return getBaseMap()->dm().ptr();
		IlwisObjectPtr *ptr = getObject();
		MapListPtr *mptr = dynamic_cast<MapListPtr *>(ptr);
		if ( mptr && mptr->iSize() > 0)
			return mptr->map(0)->dm().ptr();
		ObjectCollectionPtr *optr = dynamic_cast<ObjectCollectionPtr *>(ptr);
		if ( optr) {
			if (optr->getStatusFor(ObjectCollection::csSAMEDOMAIN) && optr->iNrObjects() > 0) {
				IlwisObject obj = optr->ioObj(0);
				if (IOTYPEBASEMAP(obj->fnObj)) {
					BaseMap bm(obj->fnObj);
					bm->dm().ptr();
				}
			}
		}
	}
	if ( type == IlwisObject::iotREPRESENTATION) {
		if ( getBaseMap() != 0)
			getBaseMap()->dm()->rpr().ptr();
		IlwisObjectPtr *ptr = getObject();
		MapListPtr *mptr = dynamic_cast<MapListPtr *>(ptr);
		if ( mptr && mptr->iSize() > 0)
			return mptr->map(0)->dm()->rpr().ptr();
		ObjectCollectionPtr *optr = dynamic_cast<ObjectCollectionPtr *>(ptr);
		if ( optr) {
			if (optr->getStatusFor(ObjectCollection::csSAMEDOMAIN) && optr->iNrObjects() > 0) {
				IlwisObject obj = optr->ioObj(0);
				if (obj.fValid() && IOTYPEBASEMAP(obj->fnObj)) {
					BaseMap bm(obj->fnObj);
					bm->dm()->rpr().ptr();
				}
			}
		}
	}
	if ( type == IlwisObject::iotGEOREF) {
		if ( getBaseMap() != 0) {
			if ( IOTYPE(getBaseMap()->fnObj) == IlwisObject::iotRASMAP) {
				Map mp(getBaseMap()->fnObj);
				return mp->gr().ptr();
			}

		}
		IlwisObjectPtr *ptr = getObject();
		MapListPtr *mptr = dynamic_cast<MapListPtr *>(ptr);
		if ( mptr && mptr->iSize() > 0)
			return mptr->map(0)->gr().ptr();
		ObjectCollectionPtr *optr = dynamic_cast<ObjectCollectionPtr *>(ptr);
		if ( optr) {
			if (optr->getStatusFor(ObjectCollection::csSAMEGEOREF) && optr->iNrObjects() > 0) {
				IlwisObject obj = optr->ioObj(0);
				if (obj.fValid() && IOTYPE(getBaseMap()->fnObj) == IlwisObject::iotRASMAP) {
					Map bm(obj->fnObj);
					bm->gr().ptr();
				}
			}
		}
	}
	return 0;
}

vector<Feature *> SpatialDataDrawer::getSelectedFeatures() const {
	vector<Feature *> features;
	for(int i = 0; i < getDrawerCount(); ++i) {
		NewDrawer *drw = getDrawer(i, dtPOLYGONLAYER | dtSEGMENTLAYER | dtPOINTLAYER);
		ComplexDrawer *cdrw = (ComplexDrawer *)drw;
		for(int j = 0; j < cdrw->getDrawerCount(); ++j) {
			SimpleDrawer *sdrw = (SimpleDrawer *)cdrw->getDrawer(j);
			if ( !sdrw) 
				continue;
			if ( sdrw->getSpecialDrawingOption() & sdoSELECTED) {
				Feature *f = (Feature *)sdrw->getDataSource();
				features.push_back(f);
			}

		}
	}
	return features;
}
