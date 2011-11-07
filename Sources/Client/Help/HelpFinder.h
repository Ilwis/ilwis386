#pragma once

namespace ILWIS {
	struct WordEntry{
		WordEntry() { count = 0; }
		long count;
		String title;
	};
	struct WordInfo {
		WordInfo() { count = 0; }
		WordInfo(const WordInfo& wi) {
			count = wi.count;
			for(map<String, WordEntry>::const_iterator cur = wi.entries.begin(); cur != wi.entries.end(); ++cur) {
				entries[(*cur).first] = (*cur).second;
			}
			word = wi.word;
		}
		void add(const String& fn, const String& _t) { 
			entries[fn].count++; 
			entries[fn].title = _t; 
			++count; 
		}
		map<String, WordEntry> entries;
		long count;
		String word;
	};


class HelpFinder {
public:
	HelpFinder();
	void indexHTMLFiles();
	FileName find(const String& query);
	void startIndexing();
	void find(const String& wrd, vector<WordInfo>& result) ;
	bool isIndexed() const { return indexed; }

private:
	static UINT startIndexing(LPVOID _hf);
	void findFolder(const String& folder) ;
	void indexFile(const FileName& fn);
	bool skipFile(const String& fn) const;
	bool skipWord(const String& word) const;
	void getWord(char buffer [], int& index, String& outWord, int mx);
	bool isWhiteSpace(char c);
	bool noSpecialSymbol(char c);
	String getTitle(int index, const String& s);
	bool isAccepted(const map<String, WordInfo>::iterator& cur) const;
	bool loadIndexFile(const String& folder);

	map<String, WordInfo> indexedWords;
	map<String, WordInfo> tempIndexedWords;
	map<char, map<String, WordInfo>::iterator> startPoints;
	set<String> exceptionsWords;
	set<String> includedWords;
	set<String> exceptionsFiles;
	CCriticalSection csAccess;
	String oldWord;
	vector<WordInfo> oldContent;
	bool indexed;
};
}