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
/* ActionPair
// by Wim Koolhoven
// (c) ILWIS System Development, ITC
	Last change:  WK   24 Jun 97   11:07 am
*/
#include "Headers\toolspch.h"	
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Mainwind.hs"

ActionPair::ActionPair(const String& s)
{
  TextInput txtinp(s);
  TokenizerBase tokenizer(&txtinp);
  Token tok;

  tok = tokenizer.tokGet();
  s1 = tok.sVal();
  tok = tokenizer.tokGet();
  s2 = tok.sVal();

  tok = tokenizer.tokGet();
  sCmd = tok.sVal();
  tok = tokenizer.tokGet();
  sDescr = tok.sVal();
}

bool ActionPair::fExtension(const String& str1, const String& str2) const
{
  if (s2 == "")
    return (s1 == str1);
  else  
    return (s1 == str1 && s2 == str2) ||
           (s1 == str2 && s2 == str1);
}

String ActionPair::sExec(const FileName& fn) const
{
  String sRet = sCmd;
  sRet &= " ";
  sRet &= fn.sFullPathQuoted();
  return sRet;  
}

String ActionPair::sExec(const FileName& fn1, const FileName& fn2) const
{
  String str1 = fn1.sFullPathQuoted();
  String str2 = fn2.sFullPathQuoted();
  String sRet = sCmd;
  sRet &= " ";
  if (s1 == fn1.sExt)
    sRet &= str1 & " " & str2;
  else  
    sRet &= str2 & " " & str1;
  return sRet;  
}

String ActionPair::sDescription(const FileName& fn) const
{
  const char* s = sDescr.c_str();
  const char* p = strchr(s, '%');
  if (p == 0)
    return sDescr;
  if (p[1] != 'S')
    return TR("Invalid Description");  
  s = &p[1];
  const char *p2 = strchr(s, '%');
  if (p2 != 0) 
    return TR("Invalid Description");  
  if (s2 == "") { 
    String sFile = fn.sFile;
    sFile &= fn.sExt;
     return String(sDescr.c_str(), sFile);  
  }   
  return String(sDescr.c_str(), fn.sFile);  
}

String ActionPair::sDescription(const FileName& fn1, const FileName& fn2) const
{
  const char *str1, *str2;
  String sTmp = sDescr;
  char* s = sTmp.sVal();
  char* p = strchr(s, '%');
  if (p == 0)
    return sTmp;
  if (p[1] == '1')
    if (fn1.sExt == s1)
      str1 = fn1.sFile.c_str();
    else  
      str1 = fn2.sFile.c_str();
  else if (p[1] == '2')  
    if (fn1.sExt == s2)
      str1 = fn1.sFile.c_str();
    else  
      str1 = fn2.sFile.c_str();
  else  
    return TR("Invalid Description");  
  p[1] = 's';  
  s = &p[1];
  p = strchr(s, '%');
  if (p == 0)
    return String(sTmp.c_str(), str1);  
  if (p[1] == '1')
    if (fn1.sExt == s1)
      str2 = fn1.sFile.c_str();
    else  
      str2 = fn2.sFile.c_str();
  else if (p[1] == '2')  
    if (fn1.sExt == s2)
      str2 = fn1.sFile.c_str();
    else  
      str2 = fn2.sFile.c_str();
  else  
    return TR("Invalid Description");  
  s = &p[1];
  p[1] = 's';  
  p = strchr(s, '%');
  if (p != 0) 
    return TR("Invalid Description");  
  return String(sTmp.c_str(), str1, str2);  
}

bool ActionPair::fAction(const Action* act) const
{
	return sCmd == act->sAction();
}

void ActionPair::SetOperation(const String& sOp)
{
	s1 = sOp;
}

void ActionPair::SetCommand(const String& sCom)
{
	sCmd = sCom;
}

String ActionPair::sGetOperation() const
{
	return s1;
}

String ActionPair::sGetExt() const
{
	return s2;
}

void ActionPair::SetDescription(const String& sDesc)
{
	sDescr = sDesc;
}


ActionPairList::ActionPairList()
: SListP<ActionPair>(true)
{
  String path = IlwWinApp()->Context()->sIlwDir();
  String s("Resources\\Def\\actpair.def");
  s = path + s;
  FileName fn(s);
  if (fn.fExist())
	{
    File fil(fn);
    add(fil);
  }
  else
	  return;
	//else
	//{
	//	IlwisAppContext *ilwapp = IlwWinApp()->Context();
	//	String sLang = ilwapp->svl()->sGet("Language");
	//	fn.sExt = ".";
	//	fn.sExt &= sLang;
	//	fn.Dir(ilwapp->svl()->sGet("IlwDir"));
	//	if (fn.fExist())
	//	{
	//		File fil(fn);
	//		add(fil);
	//	}
	//	else
	//	{
	//		fn.sExt = ".def";
	//		File fil(fn);
	//		add(fil);
	//	}
	//}

	// overule from the registry
	IlwisSettings settings("DefaultSettings");
	String sAction = settings.sValue("MapListDblClkAction", "");
	if ( sAction != "" )
		ChangeActionPair(".mpl", "", "", sAction, "");
	sAction = settings.sValue("FilterDblClkAction", "");
	if ( sAction != "" )
		ChangeActionPair(".fil", "", "", sAction, "");
	sAction = settings.sValue("StereoPairDblClkAction", "");
	if ( sAction != "" )
		ChangeActionPair(".stp", "", "", sAction, "");
}

void ActionPairList::add(File& fil)
{
  String s;
  fil.KeepOpen(true);
  while (!fil.fEof()) {
    fil.ReadLnAscii(s);
    if (s.length() > 1 && s[0] != ';')
      append(new ActionPair(s));
  }
  fil.KeepOpen(false);
}

ActionPair* ActionPairList::ap(const String& s, const FileName& fn) const
{
  for (SLIterCP<ActionPair> iter(this); iter.fValid(); ++iter)
    if (iter()->fExtension(s, fn.sExt))
      return iter();
  return 0;
}

ActionPair* ActionPairList::ap(const FileName& fn1, const FileName& fn2) const
{
  for (SLIterCP<ActionPair> iter(this); iter.fValid(); ++iter)
    if (iter()->fExtension(fn1.sExt, fn2.sExt))
      return iter();
  return 0;
}

void ActionPairList::ChangeActionPair(const String& sExt, const String& sOldOp, const String& sNewOp, const String& sNewCom, const String& sNewDesc)
{
  for (SLIterCP<ActionPair> iter(this); iter.fValid(); ++iter)
	{
		ActionPair *ap = iter();
		if (ap->fExtension(sOldOp, sExt))
		{
			if (sNewOp != "")
				ap->SetOperation(sNewOp);
			if (sNewCom != "")
				ap->SetCommand(sNewCom);
			if (sNewDesc != "")
				ap->SetDescription(sNewDesc);
		}
	}
}

