#pragma once

#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"

class _export DomainTime : public DomainValueReal {
public:
	DomainTime(const FileName&, const ILWIS::TimeInterval& v, ILWIS::Time::Mode m= ILWIS::Time::mDATETIME, bool fCreate = false);
	DomainTime(const FileName& fn);
	~DomainTime();
	DomainTime* create(const FileName& fn);
	void Store();
  bool fEqual(const IlwisObjectPtr& ptr) const;
  StoreType stNeeded() const;
  bool fValid(const String&) const;
  String sType() const;
  ILWIS::TimeInterval getInterval() const;
  ILWIS::Time tValue(const String& v) const;
  String sValue(double rValue, short w=-1, short dec=-1) const;
  double rValue(const String& sValue) const;
  ILWIS::Time::Mode getMode() const;
  void setMode(ILWIS::Time::Mode m);
  bool isLocalTime() const;
  void setHasLocalTime(bool yesno);
private:
	ILWIS::Time::Mode mode;
	bool localtime;
};