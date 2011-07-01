#include "Headers\toolspch.h"
#include <sys/stat.h>
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Ilwis.h"
#include "Engine\Base\System\ILWSingleLock.h"
#include "Client\Help\HelpFinder.h"

using namespace ILWIS;

HelpFinder::HelpFinder() {
}

UINT HelpFinder::startIndexing(LPVOID _hf) {
	HelpFinder *hf = (HelpFinder *) _hf;
	hf->startIndexing();
	return 1;
}

void HelpFinder::findFolder(const String& folder) {
	String pattern = folder + "help\\*.*";
	CFileFind finder;
	BOOL fFound = finder.FindFile(CString(pattern.c_str()));
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			String file(finder.GetFilePath());
			FileName fn(file);
			if ( skipFile(file))
				indexFile(fn);
		} else {
			bool dir = folder.find_last_of(".") == folder.size() - 1;
			dir = dir || folder.find_last_of("..") == folder.size() - 2;
			if ( !dir) {
				String folder2(finder.GetFilePath());
				findFolder(folder2);
			}
		}
	}
}

bool HelpFinder::skipFile(const String& fn) const {
	FileName f(fn);
	if ( f.sExt != ".html" || f.sExt != ".htm")
		return false;
	set<String>::const_iterator cur = exceptionsFiles.find(fn);
	return  cur != exceptionsFiles.end();
}

bool HelpFinder::skipWord(const String& word) const {
	if ( word.size() < 2)
		return true;
	if ( word[0] == '<' && word[word.size() - 1] == '>')
		return false;


	set<String>::const_iterator cur = exceptionsWords.find(word);
	return cur != exceptionsWords.end();
}

void HelpFinder::indexFile(const FileName& fn) {
	struct __stat64 fileStat; 
	int err = _stat64( fn.sFullPath().scVal(), &fileStat );
	char *buffer =  new char(fileStat.st_size + 1);
	ifstream htmlfile( fn.sFullPath().scVal(), ios::in|ios::binary);
	htmlfile.read(buffer, fileStat.st_size);
	buffer[fileStat.st_size] = 0;
	set<String> words;
	String s(buffer);
	s.toLower();
	int index = s.find("<body>");
	if ( index == string::npos)
		return;

	while ( index < fileStat.st_size) {
		String word1;
		getWord(buffer, index, word1);
		if ( !skipWord(word1))
			indexedWords[word1] = fn;
	}

	delete [] buffer;
}

void HelpFinder::getWord(char buffer [], int& index, String& word1) {
	word1 = "";
	char c = buffer[index];
	bool isTag = false;
	while( !isWhiteSpace(c) || isTag) {
		if ( c == '<' && ! noSpecialSymbol(buffer[index+1]) ) {
			isTag = true;
		}
		if ( c == '>' && isTag) {
			isTag = false;
		}
		if ( !isTag && noSpecialSymbol(c))
			word1 += c;
		c = buffer[++index];
	}
}

bool HelpFinder::noSpecialSymbol(char c) {
	if ( (c >= 48 && c <= 57) || ( c >= 65 && c <= 90) || ( c >= 97 && c <= 122) || (c >= 192 && c <= 255)) {
		return true;
	}
	return false;
}

bool HelpFinder::isWhiteSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void HelpFinder::startIndexing() {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	String folder = getEngine()->getContext()->sIlwDir();
	findFolder(folder);
}


void HelpFinder::indexHTMLFiles() {
	AfxBeginThread(startIndexing,  this);
}

