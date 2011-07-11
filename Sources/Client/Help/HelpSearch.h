#pragma once

class FieldListView;
class FieldStringList;

class HelpSearch : public FormWithDest{
public:
	HelpSearch(CWnd* parent);
private:
	int filterContent(Event *ev);
	int setTopics(Event *ev);
	int openHelp(Event *ev);
	FieldString *fsSearch;
	StringArrayLister *fsStrings;
	StringArrayLister *fTopics;
	String wrd;
	String selectedKey;
	vector<ILWIS::WordInfo> content;
	long currentContentIndex;
};