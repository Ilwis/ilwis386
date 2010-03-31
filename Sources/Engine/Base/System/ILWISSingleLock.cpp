#include "Headers\toolspch.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\ILWSingleLock.h"

ILWISSingleLock::ILWISSingleLock(CSyncObject* pObject, BOOL bInitialLock , const char *id) : 
CSingleLock(pObject, bInitialLock) {
	if ( id != NULL)
		strcpy(identity,id);
	else
		identity[0] = 0;
}

void ILWISSingleLock::setIdentity(const char *s) {
	if ( s != NULL)
		strcpy(identity,s);
	else
		identity[0]=0;
}

BOOL ILWISSingleLock::Lock(DWORD dwTimeOut, const char *s){
	if ( s != NULL)
		strcpy(identity,s);
	if ( identity[0] != 0 && getEngine()->getDebugMode()){
		DWORD tid = AfxGetThread()->m_nThreadID;
		getEngine()->getLogger()->LogLine(String("thread=%d,Entering lock for %s",tid,identity), Logger::lmDEBUG);
	}

	return CSingleLock::Lock(dwTimeOut);
}

BOOL ILWISSingleLock::Unlock(){
	if ( identity[0] != 0 && getEngine()->getDebugMode()) {
		DWORD tid = AfxGetThread()->m_nThreadID;
		getEngine()->getLogger()->LogLine(String("thread=%d, leaving lock for %s",tid, identity), Logger::lmDEBUG );
	}
	return CSingleLock::Unlock();
}

BOOL ILWISSingleLock::Unlock(LONG lCount, LPLONG lPrevCount){
	if ( identity[0] != 0 && getEngine()->getDebugMode()){
		DWORD tid = AfxGetThread()->m_nThreadID;
		getEngine()->getLogger()->LogLine(String("thread=%d, leaving lock for %s",tid, identity), Logger::lmDEBUG);
	}
	return CSingleLock::Unlock(lCount, lPrevCount);
}

ILWISSingleLock::~ILWISSingleLock() {
	if ( identity[0] != 0 && getEngine()->getDebugMode()) {
		DWORD tid = AfxGetThread()->m_nThreadID;
		getEngine()->getLogger()->LogLine(String("thread=%d, leaving lock(destructor) for %s",tid, identity), Logger::lmDEBUG);
	}
}