#pragma once

namespace ILWIS {

class TimeBounds {
public:
	TimeBounds();
	TimeBounds(ILWIS::Time & tMin, ILWIS::Time & tMax);
	virtual ~TimeBounds();
	void AddMinMax(Time & tMin, Time & tMax);
	void Reset();
	bool fUndef() const;
	const Time & tMin() const {return m_tMin;};
	const Time & tMax() const {return m_tMax;};
protected:
	Time m_tMin;
	Time m_tMax;
};

}