#pragma once

namespace ILWIS {

class TimeBounds {
public:
	TimeBounds();
	virtual ~TimeBounds();
	void AddMinMax(Time & _tMin, Time & _tMax);
	void Reset();
	const Time & tMin() const {return m_tMin;};
	const Time & tMax() const {return m_tMax;};
protected:
	Time m_tMin;
	Time m_tMax;
};

}