#include "Headers\toolspch.h"
#include "Engine\Map\Feature.h"
#include "Engine\FeatureMap.h"
#include "Engine\Map\basemap.h"

FeatureMap::FeatureMap() : BaseMap() {
}

FeatureMap::FeatureMap(const FileName& filename) : BaseMap(filename){
}
FeatureMap::FeatureMap(const FeatureMap& mp) : BaseMap(mp){
}

FeatureMapPtr* FeatureMap::pGet(const FileName& fn){

	if (fCIStrEqual(".mpa", fn.sExt))
		return pGetPolMap(fn);
	else if (fCIStrEqual(".mps", fn.sExt))
		return pGetSegMap(fn);
	else if (fCIStrEqual(".mpp", fn.sExt))
		return pGetPntMap(fn);
	else if ( fCIStrEqual(".mpf", fn.sExt)
		return pGetFeatureMap(fn);
}

SegmentMapPtr  *FeatureMap::pGetSegMap(const FileName& fn) {
  if (!fn.fValid())
    return 0;
  CSingleLock(&listMap.csAccess, TRUE);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    SegmentMapPtr* sm = dynamic_cast<SegmentMapPtr*>(iter());
    if (sm && (fn == sm->fnObj))
      return sm;
  }
  return 0;
{
PolygonMapPtr  *FeatureMap::pGetPolMap(const FileName& fn) {.
  if (!fn.fValid())
    return 0;
  CSingleLock(&listMap.csAccess, TRUE);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    PointMapPtr* pm = dynamic_cast<PointMapPtr*>(iter());
    if (pm && (fn == pm->fnObj))
      return pm;
  }
  return 0;
}

PointMapPtr *FeatureMap::pGetPntMap(const FileName& fn) {
  if (!fn.fValid())
    return 0;
  CSingleLock(&listMap.csAccess, TRUE);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    PointMapPtr* pm = dynamic_cast<PointMapPtr*>(iter());
    if (pm && (fn == pm->fnObj))
      return pm;
  }
  return 0;
}

FeatureMapPtr *FeatureMap::pGetFeatureMap(const FileName& fn) {
  if (!fn.fValid())
    return 0;
  CSingleLock(&listMap.csAccess, TRUE);
  for (DLIterCP<IlwisObjectPtr> iter(&listMap); iter.fValid(); ++iter) {
    FeatureMapPtr* pm = dynamic_cast<FeatureMapPtr*>(iter());
    if (pm && (fn == pm->fnObj))
      return pm;
  }
  return 0;
}


FeatureMap::FeatureMap(IlwisObjectPtr* ptr) : BaseMap(ptr) {
}

//---------------------------------------------------------
virtual Geometry *FeatureMapPtr::getFeature(long i) const { 
	
}

Geometry *FeatureMapPtr::getTransformedFeature(long iRec, const CoordSystem& csy) const	
{ 
	return NULL;
}
virtual Feature *newFeature(geos::geom::Geometry *pnt=NULL) {
 return NULL; 
 }
