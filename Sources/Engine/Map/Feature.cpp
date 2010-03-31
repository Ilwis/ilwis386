#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Feature.h"

Feature::Feature() {
	deleted = false;
	GUID gd;
	CoCreateGuid(&gd);
	WCHAR buf[39];
	::StringFromGUID2(gd,buf,39);
	CString str(buf);
	guid = str;
}

void Feature::Delete(bool yesno) {
	 ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	deleted = yesno;
}

bool Feature::fDeleted() const {
    ILWISSingleLock sl(const_cast<CCriticalSection *>(&csAccess), TRUE, SOURCE_LOCATION);
	return deleted;
}

String Feature::getGuid() const {
	return guid;
}

bool Feature::EnvelopeIntersectsWith(Geometry *g2, bool useMargine) {
	Geometry *g1 = dynamic_cast<Geometry *>(this);
	const geos::geom::Envelope *env1 = g1->getEnvelopeInternal();
	if ( useMargine) {
		geos::geom::Envelope env2(g2->getEnvelopeInternal()->toString());
		env2.expandBy(0.05 * env2.getWidth());
		return env1->intersects(env2);
	}
	const geos::geom::Envelope *env2 = g2->getEnvelopeInternal();
	return env1->intersects(env2);
}
