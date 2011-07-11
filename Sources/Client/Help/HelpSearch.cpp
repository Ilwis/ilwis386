#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FieldStringList.h"
#include "Client\FormElements\FieldListView.h"
#include "Client\Help\HelpFinder.h"
#include "Client\Help\HelpSearch.h"

HelpSearch::HelpSearch(CWnd* parent) : FormWithDest(parent, TR("Search Help") ), currentContentIndex(iUNDEF){
	fsSearch = new FieldString(root,TR("Find"),&wrd);
	fsSearch->SetIndependentPos();
	fsSearch->SetCallBack((NotifyProc )&HelpSearch::filterContent);
	Array<String> strings;
	fsStrings = new StringArrayLister(root, strings);
	fsStrings->SetCallBack((NotifyProc )&HelpSearch::setTopics);
	fsStrings->SetWidth(200);
	fTopics = new StringArrayLister(root,strings);
	fTopics->SetCallBack((NotifyProc )&HelpSearch::openHelp);
	fTopics->SetWidth(200);
	create();

}

int HelpSearch::openHelp(Event *ev) {
	vector<int> lines;
	int sel = fTopics->iGetSingleSelection();
	if ( sel >= 0 && currentContentIndex != iUNDEF) {
		ILWIS::WordInfo wi = content[currentContentIndex]; 
		long n = 0;
		for(map<String, ILWIS::WordEntry>::iterator cur = wi.entries.begin(); cur != wi.entries.end(); ++cur, ++n) {
			if ( sel == n) {
				String path = (*cur).first;
				IlwWinApp()->showHelp(path);
				break;
			}
		}
	}
	return 1;
}

int HelpSearch::filterContent(Event *ev) {
	fsSearch->StoreData();
	if ( wrd.size() == 0)
		return 1;
	content.clear();
	IlwWinApp()->getHelpFinder()->find(wrd, content);
	if ( content.size() == 0)
		return 0;
	Array<String> strings;
	strings.resize(content.size());
	for(int i=0; i < content.size(); ++i) {
#if _DEBUG
	strings[i] = String("%S(%d)",content[i].word, content[i].count); 
#else
	strings[i] = content[i].word;
#endif
	}
	fsStrings->resetContent(strings);


	return 1;
}

int HelpSearch::setTopics(Event *ev) {
	if ( fsStrings->iGetSingleSelection() == -1)
		return 0;
	String key = fsStrings->sGetSelectedString();
	#if _DEBUG
	key = key.sHead("(");
	#endif
	if ( key == sUNDEF)
		return 0;
	for(int i =0 ; i < content.size(); ++i) {
		if ( key == content[i].word) {
			currentContentIndex = i;
			Array<String> topics;
			for(map<String,ILWIS::WordEntry>::iterator cur = content[i].entries.begin(); cur != content[i].entries.end(); ++cur) {
				String s("%S(%d)",(*cur).second.title, (*cur).second.count);
				topics.push_back(s);

			}
			fTopics->resetContent(topics);
			break;
		}
	}


	return 1;
}