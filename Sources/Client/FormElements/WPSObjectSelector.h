#pragma once

#define WPS_DATA_HEADER_LOCAL "local files___________.directory"
#define WPS_DATA_HEADER_REMOTE "remote files___________.driveremote"

class _export WPSObjectSelector : public FieldOneSelectString{
public:
	WPSObjectSelector(FormEntry* parent, const String& sQuestion, String *txt, ObjectLister *ext);
	~WPSObjectSelector();
	void setLister(ObjectLister *ext);
	void setRemoteFiles(const vector<String>& remfiles);
	void StoreData();
private:
	void changeContent();
	long dummyvalue;
	vector<String> files;
	vector<String> remotefiles;
	ObjectLister *lister;
	String *text;

};