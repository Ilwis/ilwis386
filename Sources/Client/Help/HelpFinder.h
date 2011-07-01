#pragma once

namespace ILWIS {
class HelpFinder {
public:
	HelpFinder();
	void indexHTMLFiles();
	FileName find(const String& query);
	void startIndexing();

private:
	static UINT startIndexing(LPVOID _hf);
	void findFolder(const String& folder) ;
	void indexFile(const FileName& fn);
	bool skipFile(const String& fn) const;
	bool skipWord(const String& word) const;
	void getWord(char buffer [], int& index, String& outWord);
	bool isWhiteSpace(char c);
	bool noSpecialSymbol(char c);

	map<String, FileName> indexedWords;
	set<String> exceptionsWords;
	set<String> exceptionsFiles;
	CCriticalSection csAccess;
};
}