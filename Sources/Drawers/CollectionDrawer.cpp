#include "Headers\toolspch.h"
#include "Engine\Table\tbl.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Domain\DomainTime.h" 
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\SelectionRectangle.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\CollectionDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createCollectionDrawer(DrawerParameters *parms) {
	return new CollectionDrawer(parms, "CollectionDrawer");
}

CollectionDrawer::CollectionDrawer(DrawerParameters *parms, const String& title) : 
	SetDrawer(parms,title)
{
	setTransparency(1);
}

CollectionDrawer::~CollectionDrawer(){
}

String CollectionDrawer::description() const {
	String sName = getName();
	//String sDescr = (*datasource)->sDescr();
	//if ("" != sDescr) 
	//	sName = String("%S Animated", sName);
	return sName;
}

void CollectionDrawer::addDataSource(void *col, int options){
	ObjectCollection *oc = (ObjectCollection *)col;
	if ((*oc)->getStatusFor(ObjectCollection::csALLBASEMAP | ObjectCollection::csSAMEDOMAIN) == false)
		throw ErrorObject(TR("Not valid as a drawing layer. There might be incompatible objects in the collection"));
	SetDrawer::addDataSource(col, options);
}

void CollectionDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);


}


bool CollectionDrawer::draw(int drawerIndex , const CoordBounds& cbArea) const{
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE,SOURCE_LOCATION);
	SetDrawer::draw( drawerIndex, cbArea);
	return true;
}


String CollectionDrawer::iconName(const String& subtype) const {
	return ".ioc";
}



