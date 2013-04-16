#pragma once

#include <list>

using namespace std;

namespace ILWIS {

class TimeListener {
public:
	TimeListener();
	~TimeListener();
	static list<TimeListener*> & getTimeListeners();
	virtual void SetTime(double timePerc, long sender) = 0;
private:
	static list<TimeListener*> timeListeners;
};

class TimeProvider {
public:
	TimeProvider();
	void SendTimeMessage(double timePerc, long sender);
};

}