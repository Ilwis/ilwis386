#pragma once

#include <list>

using namespace std;

namespace ILWIS {

class TimeListener {
public:
	_export TimeListener();
	_export ~TimeListener();
	static list<TimeListener*> & getTimeListeners();
	_export virtual void SetTime(ILWIS::Time time, long sender) = 0;
private:
	static list<TimeListener*> timeListeners;
};

class TimeProvider {
public:
	_export TimeProvider();
	_export void SendTimeMessage(ILWIS::Time time, long sender);
};

}