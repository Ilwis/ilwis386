#include "Client\Headers\formelementspch.h"
#include "TimeBounds.h"

using namespace ILWIS;

TimeBounds::TimeBounds()
: m_tMin(rUNDEF)
, m_tMax(rUNDEF)
{
}

TimeBounds::~TimeBounds()
{
}

void TimeBounds::Reset() {
	m_tMin = rUNDEF;
	m_tMax = rUNDEF;
}

void TimeBounds::AddMinMax(Time & _tMin, Time & _tMax)
{
	if (!m_tMin.isValid() || m_tMin > _tMin)
		m_tMin = _tMin;
	if (!m_tMax.isValid() || m_tMax < _tMax)
		m_tMax = _tMax;
}
