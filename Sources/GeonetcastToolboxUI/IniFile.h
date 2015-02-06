// IniFile.h: interface for the IniFile class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)

#if !defined(AFX_INIFILE_H__23694AC1_6859_11D7_B451_4854E890067A__INCLUDED_)
#define AFX_INIFILE_H__23694AC1_6859_11D7_B451_4854E890067A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>

using std::string;
using std::map;

class CompareAsNum
{
public:
	bool operator() (const string&, const string&) const;
};

typedef map<string, string>          SectionEntries;
typedef map<string, SectionEntries*> Sections;

class IniFile  
{
public:
	IniFile();
	virtual ~IniFile();

	void Open(const string& filename);
	void Close();

	void SetKeyValue(const string& section, const string& key, const string& value);
	string GetKeyValue(const string& section, const string& key);

	void RemoveKeyValue(const string& section, const string& key);
	void RemoveSection(const string& section);

private:
	string filename;
	Sections sections;

	void Flush();
	void Load();
};

#endif // !defined(AFX_INIFILE_H__23694AC1_6859_11D7_B451_4854E890067A__INCLUDED_)
