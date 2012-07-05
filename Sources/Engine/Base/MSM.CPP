/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/*
// app/msm.c
// String Messages interface for ILWIS 2.0
// 4 october 1993, by Willem Nieuwenhuis
// (c) Computer Department ITC
	Last change:  WK    9 Sep 98    5:55 pm
*/
#define MSM_C

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

class LanguageStringError : public ErrorObject
{
public:
	LanguageStringError(const String& sErr) :
			ErrorObject(sErr, 666) {}
};

// Forward declaration
static FileName fnFindLng(const FileName& sName);

/*   retrieve the iTok'th item token from the string sLine (0 = first one)
     if iTok is negative get the tail of the string starting at the iTok'th
     token.
     This is a quick and dirty routine, slow.
     It is used only in the StringManager constructor to extract ID's and Strings
*/
String sToken(String& sLine, int iTok)
{
	if (sLine.length() == 0) return String();

	char pcDel[] = " \t\n";
	char *pc, *pcK, *pcTok;
	bool fRest = iTok < 0;
	if (fRest) iTok = -iTok;
	int iSize = sLine.length();
	iSize += 5;  // 27/1/97 - Wim: BoundsCheck message in strtok
	pc = new char[iSize];
	char* pcSave = pc;
	pcK = pc + sLine.length();
	strcpy(pc,sLine.sVal());
	int i = 0;
	pcTok = strtok(pc,pcDel);
	while (pcTok != NULL && i < iTok) 
	{
		pcTok = strtok(NULL, pcDel);
		i++;
	}
	if (fRest) 
	{
		if (pcTok != NULL)
		{
			pc = pcTok;
			while (*pc) pc++;
			if (pc < pcK) *pc = ' ';
		}
		else 
		{
		  delete [] pcSave;
		  return String();
		}   
	}
	String s;
	if (pcTok != 0)
		s = String(pcTok);

	delete [] pcSave;
	return s;
}

string TR(const string& text) {
	return getEngine()->getContext()->sm()->translate(text);
}

String ILWSF(const String& sMod, long id)
{
	try
	{
		return getEngine()->getContext()->sm()->sFormatted(sMod, id);
	}
	catch (LanguageStringError& err)
	{
		err.Show("Language Error");
	}
	return String();
}

StringManager::StringManager(const String& ilwDir, const String& lan)
{
	currentLanguage = lan;
	FileName fn(String("%SResources\\Strings\\string_table%S",ilwDir,lan));
	ifstream input(fn.sFullPath().c_str());
	if ( input.is_open()) {
		while(input.good()) {
			String txt;
			getline(input,txt);
			String eng = txt.sHead("|");
			String trans = txt.sTail("|");
			if ( trans == "??")
				continue;
			languages[lan][eng] = trans;
		}
		currentTexts = languages[lan];
	}
}

StringManager::~StringManager()
{
}

long StringManager::LoadLanguageStrings(const String& sModName)
{
	// Locate the language module file
	//FileName fn(sModName, String("%S", getEngine()->getContext()->getLanguageExtension()));
	FileName fn(String("%SResources\\Strings\\%S%S",getEngine()->getContext()->sIlwDir(),
													sModName, 
													getEngine()->getContext()->getLanguageExtension()));
	FileName fnLangFile = fnFindLng(fn);
	
	if (fnLangFile == FileName()) {
		if ( fn.sExt.toLower() != ".eng"){ // maybe language file in non default is missing; replace with default if possible
			fn.sExt = ".eng";
			fnLangFile = fnFindLng(fn);
		}
		if (fnLangFile == FileName()) {
			//throw LanguageStringError(String("Fatal: Language module not found"));
		}
	}

	try
	{
		ReadText(fnLangFile.sFullName(), m_mpMessages);
		if ( fnLangFile.sExt.toLower() != ".eng") {
			FileName fnDefault(fnLangFile,".eng");
			ReadText(fnDefault.sFullName(), m_mpMessages_default);
		}
	}
	catch (const FileErrorObject&)
	{
		throw LanguageStringError(String("Fatal: Language module could not be opened"));
	}
	// Add module name/number value in associative array
	long iMod = (long)m_mpModules.size();
	m_mpModules[sModName] = iMod;

	return iMod;
}

// Retrieve String for given ID; if the ID is not
// in the list then return an empty string
String StringManager::sFormatted(const String& sModName, const long iID)
{
	if (sModName.length() == 0)
		throw LanguageStringError(String("Fatal: No language module name specified"));

	ILWISSingleLock sl(&m_cs, TRUE);

	// Look for the module ID (and load the module in not already in memory)
	long iModID;
	map<String, long>::iterator itMod = m_mpModules.find(sModName);
	if (itMod != m_mpModules.end())
		iModID = (*itMod).second;  // Get the ID of the already loaded language module
	else
		iModID = LoadLanguageStrings(sModName);  // Load the module and return its ID

	// Calculate messageID as it is stored in memory
	long iMesID = (iModID << 24) + iID;

	// lookup the message string for the messageID
	map<long, String>::iterator itMes = m_mpMessages.find(iMesID);
	if (itMes != m_mpMessages.end())
		return (*itMes).second;
	else {
		map<long, String>::iterator itMes = m_mpMessages_default.find(iMesID);
		if (itMes != m_mpMessages_default.end())
			return (*itMes).second;
	}

	return String();  // unknown ID in this module
}

String ReplaceTabsAndNewlines(const String& sText)
{
	unsigned int i = 0;
	String s;
	while (i < sText.length() )
	{
		if (sText[i] == '\\') 
		{
			++i;
			if (sText[i] == 'n')
				s &= '\n';
			else if (sText[i] == 't')
				s &= '\t';
			else
			{
				s &= '\\';
				s &= sText[i];
			}  
		}
		else
			s &= sText[i];
		++i;
	}
	return s;
}

/* ReadText assumes the file to be in ASCII format with
   lines of message strings; each line contains:
	- an ID number
	- a string of characters.
   the ID and the string are delimited by whitespace

   String sFil              = the fully specified name of the file
*/
void StringManager::ReadText(const String& sFil,map<long, String>& messages)
{
	String sP, s;
	long id;
	File fil(sFil);
	// Calculate the new messageID offset for the messages string to load
	long iIdOffset = m_mpModules.size() << 24;
	while (!fil.fEof())
	{
		fil.ReadLnAscii(sP);

		if (sP.sTrimSpaces().length() == 0) // skip empty lines
			continue;
		if (sP.sLeft(2) == "//")  // skip comment lines
			continue;

		s = sToken(sP, 0);   // expect the message ID token
		if (s.length() == 0) // skip if something unexpected happens
			continue;

		String sMesID = s;
		id = iIdOffset + s.iVal();  // calculate the ID used in memory

		s = sToken(sP, -1).sTrimSpaces();
		String::size_type iBegQuote = s.find_first_of("\"");
		String::size_type iEndQuote = s.find_last_of("\"");
		if (iBegQuote != String::npos)
		{
			if (iEndQuote == iBegQuote)
			{
				if (iBegQuote == 0) // no closing quote
					iEndQuote = s.length() - iBegQuote;
				else // no opening quote
					iBegQuote = -1;  // not 0, because we need the first char also
			}
			else
				--iEndQuote;
			s = s.substr(iBegQuote + 1, iEndQuote);
		}
		s = ReplaceTabsAndNewlines(s);

		if (s.length() == 0)
		{
			// Something went wrong in the language file, so possibly no
			// strings are available, therefore the english
			String sErr("Language file %S corrupt at ID %S!", sFil, sMesID);
			throw LanguageStringError(sErr);
		}
		m_cs.Lock();
		messages[id] = s;
		m_cs.Unlock();
	}
}

static bool fValid(FileName& fn)
{
	if (!File::fExist(fn)) // added for efficiency
		return false;
	try
	{
		File fil(fn);
	}
	catch (const FileErrorObject&)
	{
		return false;
	}
	return true;
}

// Find the language module on disk
// Use fnFile as mask
// either try via the application path (fnProg is valid, sSysVar is empty)
//    or  try via the system variable (sSysVar is set, fnProg is empty)
//    or  only check validity of fnFile (fnProg is empty and sSysVar is empty)
static FileName fnFindFile(const FileName& fnProg, const String& sSysVar, const FileName& fnFile)
{
	FileName fn(fnFile);

	if (fnProg != FileName())
		fn.Dir(fnProg.sPath());  // check application path
	else if (sSysVar.length() > 0)
	{
		// check using path from system variable
		String sPath = getEngine()->getContext()->svl()->sGet(sSysVar);
		if (sPath.length() == 0)
			return FileName();
		fn.Dir(sPath);
	}
	
	if (fValid(fn))
		return fn;
	
	return FileName();
}

// fnName is a filename of a language module, f.e.: table.eng
// Locate it:
// 1. In the ILWIS application directory
// 2. In the user-defined directory
// Fallbacks in case modules not found; now for the english versions
// These must be there (as they have been installed)
// 3. In the ILWIS application directory
// 4. In the user-defined directory
FileName fnFindLng(const FileName& fnName)
{
	// check application dir
	FileName fnBase(getEngine()->getContext()->sProg());
	String sPath = fnBase.sPath() + "Resources\\Strings\\";
	FileName fnFile = fnFindFile(sPath, String(), fnName);
	if (fnFile != FileName())
		return fnFile;
	
	// check ilwis dir, this can only be .eng files
	fnFile = fnFindFile(FileName(), "IlwDir", fnName);
	if (fnFile != FileName())
		return fnFile;
	
	// check same dirs with language US
	// check application dir
	if (fCIStrEqual(fnName.sExt, String(".eng")))
		return FileName();
	
	FileName fn(fnName, String(".eng"));
	fn.Dir(FileName(getEngine()->getContext()->sProg()).sPath());
	fnFile = fnFindFile(FileName(), String(), fn);
	if (fnFile != FileName())
		return fnFile;
	
	// check ilwis dir
	return fnFindFile(FileName(), "IlwDir", fn);
}

string StringManager::translate(const string& text) {
	if ( currentLanguage == ".eng")
		return text;
	LanguageTexts::iterator cur = currentTexts.begin();
	if (( cur = currentTexts.find(text)) != currentTexts.end())
		return (*cur).second;
	return text;

}

void StringManager::setLanguage(const String& lan) {
	currentLanguage = lan;
	Languages::iterator cur = languages.begin();
	if ((cur = languages.find(lan)) != languages.end()) {
		currentTexts = (*cur).second;
	}
}
