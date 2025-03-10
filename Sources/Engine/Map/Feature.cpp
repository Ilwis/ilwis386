#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Feature.h"
#include <geos/index/quadtree/Quadtree.h>
#include <geos/geom/Envelope.h>

FeatureID Feature::baseid = 0;

#undef max // max macro is interfering; disabled in this file

Feature::Feature(QuadTree *tree) : geom(0) {
	deleted = false;
	spatialIndex = tree;
	guid = ++baseid;
	if ( guid == std::numeric_limits<FeatureID>::max())
		baseid = 0;
}

void Feature::Delete(bool yesno) {
	 ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	deleted = yesno;
}

bool Feature::fDeleted() const {
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	return deleted;
}

FeatureID Feature::getGuid() const {
	return guid;
}

bool Feature::EnvelopeIntersectsWith(Geometry *g2, bool useMargine) {
	if ( geom == 0) {
		geom = dynamic_cast<geos::geom::Geometry *>(this);
	}
	const geos::geom::Envelope *env1 = geom->getEnvelopeInternal();
	if ( useMargine) {
		geos::geom::Envelope env2(g2->getEnvelopeInternal()->toString());
		env2.expandBy(0.05 * env2.getWidth());
		return env1->intersects(env2);
	}
	const geos::geom::Envelope *env2 = g2->getEnvelopeInternal();
	return env1->intersects(env2);
}

CoordBounds Feature::cbBounds() const // bounding rectangle
{
	if ( geom == 0) {
		Feature *f = const_cast<Feature *>(this);
		const_cast<Feature *>(this)->geom = dynamic_cast<geos::geom::Geometry *>(f);
	}
	const Envelope *env = (const Envelope *)geom->getEnvelopeInternal();
	return CoordBounds(env);	
}


