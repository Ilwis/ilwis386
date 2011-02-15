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
	ComplexDrawer(parms,"AbstractMapDrawer"),
	internalDomain(false)
{
}

AbstractMapDrawer::AbstractMapDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
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
		if (basemap->dm()->rpr().fValid()) {
			return basemap->dm()->rpr();
		}
	}
	return Representation();
}

RangeReal AbstractMapDrawer::getStretchRangeReal() const{
	BaseMapPtr *basemap = getBaseMap();
	if ( basemap->dm()->pdv()) {
		return basemap->rrMinMax(BaseMapPtr::mmmSAMPLED);
	} 
	return RangeReal();
}

bool AbstractMapDrawer::useInternalDomain() const {
	return internalDomain;
}

void AbstractMapDrawer::setRepresentation(const Representation& rp){
	throw ErrorObject("To be done");
}

void AbstractMapDrawer::setInfoMode(void *v,LayerTreeView *tv) {
	bool value = *(bool *)v;
	setInfo(value);
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
	ObjectInfo::WriteElement(parentSection.scVal(),"BaseMap",fnView, bm);

	return parentSection;

}

void AbstractMapDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
	FileName fn;
	ObjectInfo::ReadElement(parentSection.scVal(),"BaseMap",fnView, fn);
	bm = BaseMap(fn);

}

void AbstractMapDrawer::inactivateOtherPalettes(ILWIS::Palette * palette)
{

}

//------------------------------------UI--------------------------------

HTREEITEM AbstractMapDrawer:: configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = ComplexDrawer::configure(tv,parent);
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tv,parent,this,(SetCheckFunc)&AbstractMapDrawer::setInfoMode);
	InsertItem("Info","info", item, info);

	BaseMapPtr *basemap = getBaseMap();
	if (!basemap )
		return parent;
	bool singleSet = (drawers.size() + preDrawers.size() + postDrawers.size())  == 1;
	for(map<String, NewDrawer *>::iterator cur = preDrawers.begin(); cur != preDrawers.end(); ++cur) {
		NewDrawer *draw = (*cur).second;
		if ( !singleSet)
			
			hti = InsertItem(draw->getName(),draw->iconName(),
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&ComplexDrawer::setActiveMode,0,draw),
							 draw->isActive());
		draw->configure(tv,hti);
	}
	for(int i = 0; i < drawers.size(); ++i) {
		NewDrawer *draw = (NewDrawer *)drawers.at(i);
		if ( !singleSet)
			
			hti = InsertItem(draw->getName(),draw->iconName(), 
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&ComplexDrawer::setActiveMode, 0, draw)
							,draw->isActive());
			                
		draw->configure(tv,hti);
	}
	for(map<String, NewDrawer *>::iterator cur = postDrawers.begin(); cur != postDrawers.end(); ++cur) {
		NewDrawer *draw = (*cur).second;
		if ( !singleSet)
			hti = InsertItem(draw->getName(),draw->iconName(),
							 new DisplayOptionTreeItem(tv,parent, this,(SetCheckFunc)&ComplexDrawer::setActiveMode,0,draw),
							 draw->isActive());
		draw->configure(tv,hti);
	}

	return hti;
}



