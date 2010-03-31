#pragma once

class _export ILWISSingleLock : public CSingleLock {
public:
	ILWISSingleLock(CSyncObject* pObject, BOOL bInitialLock = FALSE, const char *id=NULL );
	BOOL Lock(DWORD dwTimeOut = INFINITE, const char *id=NULL);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	BOOL IsLocked();
	~ILWISSingleLock();
	void setIdentity(const char *id);
protected:
	char identity[250];


};