#include "Headers\toolspch.h"
#include <sys/stat.h>
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Ilwis.h"
#include "Engine\Base\System\ILWSingleLock.h"
#include "Client\Help\HelpFinder.h"

#define LIMIT_COUNT 650

ostream& operator <<(ostream &os, const ILWIS::WordInfo& inf)
{
	os << inf.count << "|";
	for(map<String, ILWIS::WordEntry>::const_iterator cur = inf.entries.begin(); cur != inf.entries.end(); ++cur) {
		os << (*cur).first << "|" << (*cur).second.count << "|" << (*cur).second.title << "@";  
	}
	 return os;
}

using namespace ILWIS;

HelpFinder::HelpFinder() : indexed(false){
	String exc("segment,script,semivariogram,stereo,table,tiepoints,undefined");
	
	Array<String> parts;
	Split(exc,parts,",");
	for(int i=0; i < parts.size(); ++i)
		exceptionsWords.insert(parts[i]);

	String inc("");
	inc += "";
	parts.clear();
	Split(exc,parts,",");
	for(int i=0; i < parts.size(); ++i)
		includedWords.insert(parts[i]);

}

UINT HelpFinder::startIndexing(LPVOID _hf) {
	HelpFinder *hf = (HelpFinder *) _hf;
	hf->startIndexing();
	return 1;
}

void HelpFinder::findFolder(const String& folder) {
	String pattern = folder + "\\*.*";
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
			String folder2(finder.GetFilePath());
			bool dir = folder2.find_last_of(".") == folder2.size() - 1;
			dir = dir || folder2.find_last_of("..") == folder2.size() - 2;
			if ( !dir) {
				findFolder(folder2);
			}
		}
	}
}

bool HelpFinder::skipFile(const String& fn) const {
	FileName f(fn);
	if ( f.sExt != ".html" && f.sExt != ".htm")
		return false;
	set<String>::const_iterator cur = exceptionsFiles.find(fn);
	return  cur == exceptionsFiles.end();
}

bool HelpFinder::skipWord(const String& word) const {
	if ( word.size() <= 2)
		return true;
	if ( word[0] == '<' && word[word.size() - 1] == '>')
		return false;

	return false;


}

String HelpFinder::getTitle(int index, const String& s) {
	String title;
	for(int i=index; i < s.size(); ++i) {
		if ( s[i] != '<')
			title += s[i];
		else
			break;
	}
	return title;
}

void HelpFinder::indexFile(const FileName& fn) {
	struct __stat64 fileStat; 
	int err = _stat64( fn.sFullPath().c_str(), &fileStat );
	FILE *fp = fopen(fn.sFullPath().c_str(), "rb");
	char * buffer = new char[fileStat.st_size + 1];
	fread(buffer, 1, fileStat.st_size,fp);
	fclose(fp);
	set<String> words;
	String s(buffer);
	s.toLower();
	String title;
	int index = s.find("<title");
	if ( index != string::npos) {
		title = getTitle(index + 7,s);
	}
	index = s.find("<body");
	if ( index == string::npos)
		return;

	while ( index < fileStat.st_size - 1) {
		String word1;
		getWord(buffer, index, word1,fileStat.st_size);
		if ( !skipWord(word1)) {
			tempIndexedWords[word1].add(fn.sFullPath(), title);
		}
	}
	delete [] buffer;
}

void HelpFinder::getWord(char buffer [], int& index, String& word1, int maxs) {
	word1 = "";
	char c = buffer[index];
	bool isTag = false;
	while( (!isWhiteSpace(c) || isTag) && index < maxs - 1) {
		char c2 = buffer[index+1];
		if ( (c == '<' && noSpecialSymbol(c2)) || (c == '<' && c2 == '/') ) {
			isTag = true;
		}
		if ( c == '>' && isTag) {
			isTag = false;
		}
		if ( !isTag && noSpecialSymbol(c))
			if ( !(c >= 48 && c <= 57 && word1.size() == 0)) 
				word1 += c;
		c = buffer[++index];
	}
	if ( isWhiteSpace(c))
		++index;
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

bool HelpFinder::isAccepted(const map<String, WordInfo>::iterator& cur) const{
	int sz = (*cur).second.count;
	if ( sz > 2 && sz < LIMIT_COUNT) 
		return true;
	if ( sz >= LIMIT_COUNT) {
		set<String>::const_iterator fnd = exceptionsWords.find((*cur).first);
		if ( fnd != exceptionsWords.end())
				return true;
	}
	return false;
}

void HelpFinder::startIndexing() {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	String folder = getEngine()->getContext()->sIlwDir() + "help";
	if ( !loadIndexFile(folder)) {
		findFolder(folder);
		for(map<String, WordInfo>::iterator cur = tempIndexedWords.begin(); cur != tempIndexedWords.end(); ++cur) {

			if ( isAccepted(cur)) {
				String word = (*cur).first;
				word.toLower();

				indexedWords[word] = (*cur).second;
				indexedWords[word].word = word;
			} 		
		}
		tempIndexedWords.clear();
		ofstream outfile(String("%S\\index_eng.idx", folder).c_str());
		for(map<String, WordInfo>::const_iterator cur = indexedWords.begin(); cur != indexedWords.end(); ++cur) {
			outfile << (*cur).first << "|" << (*cur).second << "\n";
		}
		outfile.close();
	}
	char c = 0;
	for(map<String, WordInfo>::iterator cur = indexedWords.begin(); cur != indexedWords.end(); ++cur) {
		String word = (*cur).first;
		char c1 = word[0];
		if ( c1 != c) {
			startPoints[c1] = cur;
			c = c1;
		}
	}

	indexed = true;
}

bool HelpFinder::loadIndexFile(const String& folder) {
	FileName fn(String("%S\\index_eng.idx", folder));
	if ( !fn.fExist())
		return false;

	ifstream infile(fn.sFullPath().c_str());
	char cline[100000];
	while(!infile.bad() && !infile.eof()) {
		infile.getline(cline,100000);
		String line(cline);
		if ( line == "")
			break;
		WordInfo inf;
		inf.word = line.sHead("|");
		String rest = line.sTail("|");
		inf.count = rest.sHead("|").iVal();
		rest = rest.sTail("|");
		Array<String> parts;
		Split(rest,parts,"@");
		for(int i=0; i < parts.size(); ++i) {
			WordEntry entry;
			Array<String> eparts;
			Split(parts[i], eparts,"|");
			String key = eparts[0];
			if ( eparts.size() == 3) {
				entry.title = eparts[2];
				entry.count = eparts[1].iVal();
			} else {
				entry.count = eparts[1].iVal();
			}
			inf.entries[key] = entry;
		}
		indexedWords[inf.word] = inf;


	}
	return true;

}

void HelpFinder::find(const String& wrd, vector<WordInfo>& result) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	if ( !indexed)
		startIndexing();

	if ( wrd == oldWord) {
		result.resize(oldContent.size());
		copy(oldContent.begin(), oldContent.end(), result.begin());
		return;
	}
	oldWord = wrd;
	oldContent.clear();
	if ( wrd.size() == 0) {
		result.resize(indexedWords.size());
		for(map<String, WordInfo>::const_iterator cur = indexedWords.begin(); cur != indexedWords.end(); ++cur) {
			result.push_back((*cur).second);
		}
	} else {
		char c = wrd[0];
		if ( startPoints.size() == 0) {
			return;
		}

		map<String, WordInfo>::iterator start = startPoints[c];
		if ( (*start).second.count > 0) {
			for(map<String, WordInfo>::const_iterator cur = start; cur != indexedWords.end(); ++cur) {
				WordInfo inf = (*cur).second;
				if ( inf.word[0] != c)
					break;
				else {
					if ( inf.word.find(wrd) != string::npos)
						result.push_back(inf);
				}

			}
		}

	}
	oldContent.resize(result.size());
	copy(result.begin(), result.end(), oldContent.begin());
}

void HelpFinder::indexHTMLFiles() {
	AfxBeginThread(startIndexing,  this);
}

