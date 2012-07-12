#pragma once

class _export RemoteLister {
public:
	RemoteLister(const URL& rurl)
		: remoteURL(rurl) {}
	virtual ~RemoteLister();
	virtual void getFiles(vector<String>& files);
private:
	void handleGNCCase(const String& txt,vector<String>& files);
	void handleIlwisCase(const String& txt, vector<String>& files);
	URL remoteURL;


};