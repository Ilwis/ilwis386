#include "Client\Headers\formelementspch.h"
#include "TimeBounds.h"

using namespace ILWIS;

TimeBounds::TimeBounds()
: m_tMin(rUNDEF)
, m_tMax(rUNDEF)
{
}

TimeBounds::TimeBounds(ILWIS::Time & tMin, ILWIS::Time & tMax)
: m_tMin(tMin)
, m_tMax(tMax)
{
}

TimeBounds::~TimeBounds()
{
}

void TimeBounds::Reset() {
	m_tMin = rUNDEF;
	m_tMax = rUNDEF;
}

bool TimeBounds::fValid() const {
	return m_tMin.isValid() && m_tMax.isValid();
}

void TimeBounds::AddMinMax(Time & tMin, Time & tMax)
{
	if (!m_tMin.isValid() || m_tMin > tMin)
		m_tMin = tMin;
	if (!m_tMax.isValid() || m_tMax < tMax)
		m_tMax = tMax;
}
