#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\DomainTime.h"


using namespace ILWIS ;

DomainTime::DomainTime(const FileName& fn, const ILWIS::TimeInterval& interv, ILWIS::Time::Mode m, bool fCreate) : 
DomainValueReal(fn,interv.getBegin(), interv.getEnd(),interv.getStep()), mode(m)
{
	_fRawAvail = false;
	localtime = true;
}

DomainTime::DomainTime(const FileName& fn)
: DomainValueReal(fn)
{ 

   _fRawAvail = false;
   int m;
   ReadElement("DomainTime","HasDate", m);
   ReadElement("DomainTime","LocalTime", localtime);
   mode = (ILWIS::Time::Mode)m;
   if ( mode < 0) // not possible or acceptable
	   mode = ILWIS::Time::mDATETIME;
}

DomainTime::~DomainTime() {
}

DomainTime* DomainTime::create(const FileName& fn)
{
  return new DomainTime(fn);
}

void DomainTime::Store()
{
  DomainValueReal::Store();
  WriteElement("DomainValueReal", "Type", "DomainTime");
  WriteElement("DomainTime","HasDate", (int)mode);
  WriteElement("DomainTime","LocalTime", localtime);
}

bool DomainTime::fEqual(const IlwisObjectPtr& ptr) const {
	const DomainTime *dt = dynamic_cast<const DomainTime*>(&ptr);
	if ( !dt)
		return false;
	ILWIS::TimeInterval interv1 = dt->getInterval();
	ILWIS::TimeInterval interv2 = getInterval();
	return interv1 == interv2;

	
}
StoreType DomainTime::stNeeded() const {
	return stREAL;
}

bool DomainTime::fValid(const String& v) const{
	Time time(v);
	return time.isValid();
}

String DomainTime::sType() const
{
  return "Domain Time";
}

ILWIS::TimeInterval DomainTime::getInterval() const {
	RangeReal rr = rrMinMax();
	ILWIS::Time begin(rr.rLo());
	ILWIS::Time end(rr.rHi());
	ILWIS::Duration step(rStep());
	ILWIS::TimeInterval interval(begin, end ,step) ;
	return interval;
}

Time DomainTime::tValue(const String& sVal) const{
	Time t(sVal);
	double v = t;
	if ( v >= rrMinMax().rLo() && v <= rrMinMax().rHi())
		return t;
	return tUNDEF;
}

String DomainTime::sValue(double rValue, short w, short dec) const {

	if ( rValue >= rrMinMax().rLo() && rValue <= rrMinMax().rHi())
		return Time(rValue).toString(true, mode);
	return "?";
}

double DomainTime::rValue(const String& sValue) const {
	Time t(sValue);
	double v = t;
	if ( v >= rrMinMax().rLo() && v <= rrMinMax().rHi())
		return t;
	return rUNDEF;
}

void DomainTime::setMode(ILWIS::Time::Mode m) {
	mode = m;
}

ILWIS::Time::Mode DomainTime::getMode() const {
	return mode;
}

bool DomainTime::isLocalTime() const {
	return localtime;
}
void DomainTime::setHasLocalTime(bool yesno) {
	localtime = yesno;
}


//----------------------------------------------------------
