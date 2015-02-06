// IniFile.cpp: implementation of the IniFile class.
//
//////////////////////////////////////////////////////////////////////


//#include "stdafx.h"

#include "IniFile.h"

bool CompareAsNum::operator() (const string& s1, const string& s2) const
{
	long Num1 = atoi(s1.c_str());
	long Num2 = atoi(s2.c_str());
	return Num1 < Num2;
}

string TrimSpaces(const string& input)
{
	// find first non space
	if ( input.empty()) 
		return string();

	int iFirstNonSpace = input.find_first_not_of(' ');
	int iFindLastSpace = input.find_last_not_of(' ');
	if (iFirstNonSpace == string::npos || iFindLastSpace == string::npos)
		return string();

	return input.substr(iFirstNonSpace, iFindLastSpace - iFirstNonSpace + 1);
}

char line[1024];

string GetLine(FILE* fil)
{
	char *p = fgets(line, 1024, fil);
	if (p == NULL)
		return string();

	p = line + strlen(line) - 1; // move to last char in buffer
	while ((p >= line) && isspace(*p))
		--p;         // isspace is succesful at least once, because of the "\n"
	*(p + 1) = '\0';   // therefore this will not fail

	return string(line);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IniFile::IniFile()
{

}

IniFile::~IniFile()
{

}

void IniFile::Open(const string& filenam)
{
	filename = filenam;

	Load();
}

void IniFile::Close()
{
	Flush();

	for (Sections::iterator iter = sections.begin(); iter != sections.end(); ++iter)
	{
		(*(*iter).second).clear();
		delete (*iter).second;
	}

	sections.clear();
}

void IniFile::SetKeyValue(const string& section, const string& key, const string& value)
{
	Sections::iterator iterSect = sections.find(section);
	if (iterSect == sections.end())
	{
		// Add a new section, with one new key/value entry
		SectionEntries *entries = new SectionEntries;
		(*entries)[key] = value;
		sections[section] = entries;
	}
	else
	{
		// Add one new key/value entry in an existing section
		SectionEntries *entries = (*iterSect).second;
		(*entries)[key] = value;
	}
}
string IniFile::GetKeyValue(const string& section, const string& key)
{
	Sections::iterator iterSect = sections.find(section);
	if (iterSect != sections.end())
	{
		SectionEntries *entries = (*iterSect).second;
		SectionEntries::iterator iterEntry = (*entries).find(key);
		if (iterEntry != (*entries).end())
			return (*iterEntry).second;
	}

	return string();
}

void IniFile::RemoveKeyValue(const string& section, const string& key)
{
	Sections::iterator iterSect = sections.find(section);
	if (iterSect != sections.end())
	{
		// The section exists, now erase entry "key"
		SectionEntries *entries = (*iterSect).second;
		(*entries).erase(key);
	}
}

void IniFile::RemoveSection(const string& section)
{
	Sections::iterator iterSect = sections.find(section);
	if (iterSect != sections.end())
	{
		// The section exists, so remove it and all its entries.
		SectionEntries *entries = (*iterSect).second;
		(*entries).clear();
		sections.erase(iterSect);
	}
}

void IniFile::Load()
{
	enum ParseState { FindSection, FindKey, ReadFindKey, StoreKey, None } state;
	FILE *filIni = fopen(filename.c_str(), "r");
	if (filIni == NULL)
		return;

	string section, key, value;
	state = FindSection;
	string s;
	while (!feof(filIni))
	{
		switch (state)
		{
			case FindSection:
				s = GetLine(filIni);
				if (s.empty())
					continue;

				if (s[0] == '[')
				{
					int iLast = s.find_first_of(']');
					if (iLast != string::npos)
					{
						section = s.substr(1, iLast - 1);
						state = ReadFindKey;
					}
				}
				else
					state = FindKey;
				break;
			case ReadFindKey:
				s = GetLine(filIni); // fall through (no break)
			case FindKey:
				{
					int iEqu = s.find_first_of('=');
					if (iEqu != string::npos)
					{
						key = s.substr(0, iEqu);
						value = s.substr(iEqu + 1);
						state = StoreKey;
					}
				}
				break;
			case StoreKey:
				SetKeyValue(section, key, value);
				state = FindSection;
				break;
		}
	}

	fclose(filIni);
}

void IniFile::Flush()
{
	FILE *filIni = fopen(filename.c_str(), "w+");
	if (filIni == NULL)
		return;

	Sections::iterator iterSect;
	for (iterSect = sections.begin(); iterSect != sections.end(); ++iterSect)
	{
		// write the section name
		fprintf(filIni, "[%s]\n", (*iterSect).first.c_str());
		SectionEntries *entries = (*iterSect).second;
		SectionEntries::iterator iterEntry;
		for (iterEntry = (*entries).begin(); iterEntry != (*entries).end(); ++iterEntry)
		{
			string key = (*iterEntry).first;
			fprintf(filIni, "%s=%s\n", TrimSpaces(key).c_str(), (*iterEntry).second.c_str());
		}

		fprintf(filIni, "\n");
	}

	fclose(filIni);
}
