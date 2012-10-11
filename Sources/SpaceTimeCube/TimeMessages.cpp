#include "TimeMessages.h"

using namespace ILWIS;

list<TimeListener*> TimeListener::timeListeners;

list<TimeListener*> & TimeListener::getTimeListeners()
{
	return timeListeners;
}

TimeListener::TimeListener()
{
	timeListeners.push_back(this);
}

TimeListener::~TimeListener()
{
	timeListeners.remove(this);
}

TimeProvider::TimeProvider()
{
}

void TimeProvider::SendTimeMessage(double timePerc, bool fShiftDown, long sender)
{
	list<TimeListener*> & timeListeners = TimeListener::getTimeListeners();
	for (list<TimeListener*>::iterator it = timeListeners.begin(); it != timeListeners.end(); ++it)
		(*it)->SetTime(timePerc, fShiftDown, sender);
}