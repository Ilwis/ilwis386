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
	Last change:  WK   10 Feb 97    2:07 pm
*/
#ifndef ILWACTION_H
#define ILWACTION_H

struct ApplicationInfoUI;

#ifndef ILWRES_H
#include "Client\Base\Res.h"
#endif

class Action
{
public:
  Action(ApplicationInfoUI* info);
  Action(const String&, const String& sObject="", const String& sPathExternCommand="");
	String sIcon() const { return sIco; }
  bool fExtension(const String&) const;
  String _export sExec(const FileName&) const;
  String sAction() const { return sCmd; }
  String sDescription() const { return sDescr; }
  String sDescriptionS() const { return sDescrS; }
  String sMenOpt() const { return sOption; }
  String sSubOpt() const { return sSubOption; }
  String sMidOpt() const { return sMiddleOption; }
  String sName() const { return sActionName; }
  long iHelp() const { return iHelpTopic; }
	String sOperation();
private:
  String sOption;
  String sSubOption;
  String sMiddleOption;
	String sIco;
  SList<String> lsExt;
  String sCmd;
  String sDescr;
  String sActionName;
  String sDescrS;
  long iHelpTopic;
};

class ActionList: public SListP<Action>
{
public:
  ActionList();
  void determineRootIcons();
  void determineSecondlevelIcons();
  map<String, int> GetIconCounts(Action *lastAction);

  String getTopIcon(String opt);
  String getMiddleIcon(String opt);
private:
	void AddScripts();
	void AddTools();
	void AddExtraActions(const String& sStartDir = "");
  void add(File&);
  void addAppsFromList();
  void ActionList::GetIconCounts(Action *lastAction, map<String, int>& iconCounts);

	void add(File&, const String& sPathExternCommand);

	map<String, String> topIcons;
	map<String, String> middleIcons;
};

#endif // ILWACTION_H
