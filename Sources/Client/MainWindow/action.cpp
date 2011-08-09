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
/* Action
// by Wim Koolhoven
// (c) ILWIS, ITC
	Last change:  WK   24 Jun 97   11:06 am
*/
#include "Headers\toolspch.h"	
#include "Client\Base\Res.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\Tokbase.h"

Action::Action(CommandInfoUI* info) {
	String s = info->menuString;
	Array<String> parts;
	sOption = s.sHead(".");
	s = s.sTail(".");
	sMiddleOption = s.sHead(".");
	s = s.sTail(".");
	sSubOption = s.sHead(".");
	sActionName = info->operationName;;
	sIco = info->iconName;
	String sExt = info->extension;
	Split(sExt, parts, ".");
	for(unsigned int i = 0; i < parts.size(); ++i) {
		lsExt.append("." + parts[i]);
	}
	sExt.toLower();
	sCmd = info->command;
	iHelpTopic = info->htopic;
	sDescr = info->description;
}


Action::Action(const String& s, const String& sObject, const String& sPathExternCommand)
{
  TextInput txtinp(s);
  TokenizerBase tokenizer(&txtinp);
  Token tok;

  tok = tokenizer.tokGet();
  sOption = tok.sVal();

  tok = tokenizer.tokGet();
  sMiddleOption = tok.sVal();
  
  tok = tokenizer.tokGet();
  sSubOption = tok.sVal();
  
  tok = tokenizer.tokGet();
  sActionName = tok.sVal();

  tok = tokenizer.tokGet();
  sIco = tok.sVal();

  tok = tokenizer.tokGet();
  String sExt = tok.sVal();
  sExt.toLower();
  for (int i = sExt.length()-1 ; i >= 0; --i) {
    if (sExt[i] == '.') {
      lsExt.append(&sExt[i]);
      sExt[i] = '\0';
    }
  }

  tok = tokenizer.tokGet();
  sCmd = tok.sVal();
	if ( sObject != "" )
		sCmd += " " + sObject;
	else if ((sPathExternCommand.length() > 0) && (sCmd.length() > 0) && ((sCmd[0] == '!') || (sCmd.substr(0, 4) == "run ")))
	{
		// substitute every occurrence of ".\" in sCmd with sPathExternCommand
		// use CString as its Replace works much better
		CString csCmd (sCmd.c_str());
		if (sPathExternCommand[sPathExternCommand.length() - 1] != '\\')
			csCmd.Replace(".\\", String(sPathExternCommand + "\\").c_str());
		else
			csCmd.Replace(".\\", sPathExternCommand.c_str());
		sCmd = csCmd;
	}
  tok = tokenizer.tokGet();
  iHelpTopic = tok.sVal().iVal();
  
  tok = tokenizer.tokGet();
  sDescr = tok.sVal();
	if ( sDescr[0] == '\'')
		sDescr = sDescr.substr(1, sDescr.length() - 2);
  tok = tokenizer.tokGet();
  sDescrS = tok.sVal();
  if (0 == sDescrS.length())
    sDescrS = sDescr;
}

bool Action::fExtension(const String& str) const
{
  String s = str;
  s.toLower();
  for (SLIterC<String> iter(&lsExt); iter.fValid(); ++iter) {
    String sExt = iter();
    if (s == sExt)
    //if (s == iter())
      return true;
  }
  return false;
}

String Action::sExec(const FileName& fn) const
{
  String s = fn.sRelativeQuoted();
  String sRet = sCmd;
  sRet &= " ";
  sRet &= s;
  return sRet;
}

String Action::sOperation()
{
	String sOp;
  if ( sOption != "" ) sOp =  sOption;
	if ( sSubOption != "") sOp =  sSubOption;
	if ( sActionName != "") sOp =  sActionName;
	if ( sMiddleOption != "") sOp = sMiddleOption;

	size_t iF = sOp.find('&');
	if ( iF != -1) sOp.erase(iF, 1);
	return sOp;
}

ActionList::ActionList()
: SListP<Action>(true)
{
  String s("action.def");
  FileName fn(s);
  addAppsFromList();
  AddExtraActions();
  AddScripts();
  AddTools();
  determineRootIcons();
  determineSecondlevelIcons();
}

void ActionList::AddScripts()
{
	IlwisSettings settings("DefaultSettings");
	String sPath = IlwWinApp()->Context()->sIlwDir() + "Scripts";
	sPath += ";" + settings.sValue("ScriptDirs", "");
	Array<String> sPaths;
	Split(sPath, sPaths, ";");
	for(unsigned int i= 0; i<sPaths.size(); ++i)
	{
		CFileFind finder;
		String sDirPath = sPaths[i];
		BOOL fFound = finder.FindFile(String("%S\\*.isl", sDirPath).c_str());
		while(fFound)
		{
			fFound = finder.FindNextFile();
			FileName fnObj(finder.GetFilePath());
			String sDescr = ObjectInfo::sDescr(fnObj);
			String sAct("Script\t\"\"\t\"%S\"\t\"%S\"\tScript\t\"\"\trun\t0\t\'%S\'", fnObj.sFile, fnObj.sFile, sDescr);
			Action *act = new Action(sAct, fnObj.sFullPathQuoted());
			append(act);
		}
	}
}

void ActionList::AddTools()
{
	IlwisSettings settings("DefaultSettings");
	String sPath = IlwWinApp()->Context()->sIlwDir() + "Tools";
	sPath += ";" + settings.sValue("ToolDir", "");
	Array<String> sPaths;
	Split(sPath, sPaths, ";");
	for(unsigned int i= 0; i<sPaths.size(); ++i)
	{
		CFileFind finder;
		String sDirPath = sPaths[i];
		BOOL fFound = finder.FindFile(String("%S\\*.*", sDirPath).c_str());
		while(fFound)
		{
			fFound = finder.FindNextFile();
			if ( finder.IsDirectory() || finder.IsDots() )
				continue;
			String s(finder.GetFilePath());
			String sDisp(finder.GetFileName());
			sDisp.toLower();
			if (sDisp.find(".dll") != -1)
				continue;
			if ( sDisp.find(".exe") != -1 || sDisp.find(".com") != -1)
				sDisp = finder.GetFileTitle();
			String sAct("Tools\t\"\"\t\"%S\"\t\"%S\"\tExe16Ico\t\"\"\t\"!!%S\"\t0\t", sDisp, sDisp, s.sQuote());
			Action *act = new Action(sAct);
			append(act);
		}
	}
}

void ActionList::AddExtraActions(const String& sStartDir)
{
	// Finds all action.def files in all subfolders of ilwis, and adds the actions.
	// Does not include the action.def file in the ilwis folder.
	// Files referred to by action.def and don't reside in the ilwis folder must either have an absolute path, or have .\ prepended.
	// The .\ characters are substituted by an absolute path pointing to the folder of the .def file so that ilwis can find the files

  String sSearchFile("action.def");
	CFileFind finder;
	CString strWildcard;
	bool fFirstCall = false;
	if (sStartDir.length() > 0)
	{
		if (sStartDir[sStartDir.length() - 1] != '\\')
			strWildcard = String(sStartDir + "\\*.*").c_str();
		else
			strWildcard = String(sStartDir + "*.*").c_str();
	}
	else
	{ // first call
		fFirstCall = true;
		IlwisAppContext *ilwapp = IlwWinApp()->Context();
		String sIlwisDir (ilwapp->svl()->sGet("IlwDir"));
		if ((sIlwisDir.length() > 0) && (sIlwisDir[sIlwisDir.length() - 1] != '\\'))
			strWildcard = String(sIlwisDir + "\\*.*").c_str();
		else
			strWildcard = String(sIlwisDir + "*.*").c_str();
	}

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			 continue;

		// if it's a directory, recursively search it

		if (finder.IsDirectory())
			AddExtraActions(finder.GetFilePath());
		else if (!fFirstCall && finder.GetFileName().CompareNoCase(sSearchFile.c_str()) == 0)
		{
			FileName fn(finder.GetFilePath());
			File fil(fn);
			add(fil, fn.sPath());
		}
	}

	finder.Close();
}

void ActionList::addAppsFromList()
{
	vector<Action *> actions = IlwWinApp()->CreateActions();
	for(vector<Action *>::iterator cur=actions.begin() ; cur != actions.end(); ++cur) {
		append(*cur);
	}
}
void ActionList::add(File& fil)
{
  String s;
  fil.KeepOpen(true);
  while (!fil.fEof()) {
		try {
			fil.ReadLnAscii(s);
			if (s.length() > 1 && s[0] != ';')
				append(new Action(s));
		}
		catch (ErrorObject& err) 
		{
			err.Show();
		}
  }
  fil.KeepOpen(false);
  addAppsFromList();
}

void ActionList::add(File& fil, const String& sPathExternCommand)
{
	// only called from "AddExtraActions"
  String s;
  fil.KeepOpen(true);
  while (!fil.fEof()) {
		try {
			fil.ReadLnAscii(s);
			if (s.length() > 1 && s[0] != ';')
			{
				if (sPathExternCommand.length() > 0)
					append(new Action(s, "", sPathExternCommand));
				else
					append(new Action(s));
			}
		}
		catch (ErrorObject& err) 
		{
			err.Show();
		}
  }
  fil.KeepOpen(false);
}

void ActionList::determineRootIcons() {
	//String prevTop = "";
	map<String, int> iconCounts;
	Action *oldAct = 0;
	for (SLIterCP<Action> iter(this); iter.fValid(); ++iter) {
		Action* act = iter();
		String sTop = act->sMenOpt();
		if ( sTop == "")
			continue;
		if ( oldAct == 0 || ( oldAct->sMenOpt() == act->sMenOpt())) {
			iconCounts[act->sIcon()] += 1;
		if (oldAct == 0)
			oldAct = act;
			continue;
		}
		String predominantName = "";
		int predominantCount = 0;
		for(map<String, int>::iterator cur = iconCounts.begin(); cur != iconCounts.end(); ++cur) {
			String name = (*cur).first;
			int count = (*cur).second;
			if ( count > predominantCount) {
				predominantCount = count;
				predominantName = name;
			}
		}
		iconCounts = map<String, int>();
		if ( predominantName != "") {
			topIcons[oldAct->sMenOpt()] = predominantName;
			iconCounts[act->sIcon()] += 1;
		}
		oldAct = act;
	}
	topIcons["Script"]=".isl";
}

void ActionList::GetIconCounts(Action *lastAction, map<String, int>& iconCounts)
{
	String predominantName = "";
	int predominantCount = 0;
	for(map<String, int>::iterator cur = iconCounts.begin(); cur != iconCounts.end(); ++cur) {
		String name = (*cur).first;
		int count = (*cur).second;
		if ( count > predominantCount) {
			predominantCount = count;
			predominantName = name;
		}
	}
	String entry = lastAction->sMenOpt() + lastAction->sMidOpt();
	middleIcons[entry] = predominantName;
	iconCounts = map<String, int>();
}

void ActionList::determineSecondlevelIcons() {
	Action *lastAction = 0;
	map<String, int> iconCounts;
	for (SLIterCP<Action> iter(this); iter.fValid(); ++iter) {
		Action* act = iter();
		if ( lastAction == NULL) 
			lastAction = act;
		
		String sMiddle = act->sMidOpt();
		if ( sMiddle == "" && lastAction->sMidOpt() == "") {
			continue;
		}
		if ( sMiddle != lastAction->sMidOpt() ) {
			GetIconCounts(lastAction, iconCounts);
		}
		iconCounts[act->sIcon()] += 1;
		lastAction = act;
	}
}

String ActionList::getTopIcon(String opt) {
	return topIcons[opt];
}

String ActionList::getMiddleIcon(String opt) {
	return middleIcons[opt];
}
