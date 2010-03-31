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
/* $Log: /ILWIS 3.0/Main/Ddeserv.cpp $
 * 
 * 13    31-10-01 15:28 Retsios
 * Added validity check before attempt using coordinate systems for
 * robuustness.
 * 
 * 12    30-10-01 18:05 Retsios
 * Preserve cwcs: we need it to check if coordinate conversion is
 * necessary to get map value at coordinate crd.
 * 
 * 11    1/10/01 17:29 Retsios
 * The sysitems match upon a request to the System topic of the ILWIS DDE
 * Server is now case insensitive.
 * 
 * 10    1/08/01 17:11 Retsios
 * Special case for Import and Export: popup corresponding dialog box when
 * not supplying any parameters.
 * 
 * 9     1/03/01 19:25 Retsios
 * Checked for memory leaks. Comment in code added (to show which part was
 * checked so that it is not checked again)
 * 
 * 8     8/29/00 12:41 Retsios
 * Bug 1153 repaired: ';' is now allowed at certain places (between quotes
 * and brackets) in DDE commands.
 * 
 * 7     7/07/00 17:52 Retsios
 * Code improvement
 * 
 * 6     7/05/00 18:41 Retsios
 * 1) Change working drive commands like c: or d: now translate to cd c:
 * or cd d: (this was a valid DDE command in Ilwis 2.23, and the only way
 * to change the current working drive)
 * 2) Cleanup of the code in TopicServerSystem::command
 * Still TODO: preserve path when current working drive changes
 * 
 * 5     6/27/00 17:58 Retsios
 * DDE: changed "ui command line execute" to "script execute":
 * "IlwWinApp()->Execute(str)" to "Script::Exec(str)"
 * 
 * 4     6/22/00 18:58 Retsios
 * Added implementation UpdateCoordinate()
 * Made StringFromCoord case-insensitive to the input (.x,.y,.XY)
 * 
 * 3     22-06-00 16:00 Koolhoven
 * Corrections to make ilwis linkable
 * 
 * 2     6/22/00 12:59 Retsios
 * DDE preliminary version .. compiles, but not tested yet.
 * 
 * 1     6/22/00 10:32 Retsios
// Revision 1.3  1998/04/22 17:32:52  Wim
// Report errors in command()
//
// Revision 1.2  1998-04-22 18:10:41+01  Wim
// Do not strip brackets ( ... ) anymore
// Allow ';' as delimiter and end character for commands which should be fed to the script.
//
/* ddeserv.c
// DdeServer
// by Wim Koolhoven
// (c) Computer Department ITC
	Last change:  WK   22 Apr 98    6:27 pm
*/
#include "Headers\toolspch.h"
#include "Client\ilwis.h"
#include "Engine\Map\basemap.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\Base\Ddeserv.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\Scripting\Script.h"
#include "Headers\messages.h"

DdeServer::DdeServer()
  : zDdeServer("ilwis")
{
  tsSystem = new TopicServerSystem(this);
  tsCalc = new TopicServerCalc(this);
  tsCoord = new TopicServerCoord(this);
}

DdeServer::~DdeServer()
{
  delete tsSystem;
  delete tsCalc;
  delete tsCoord;
}

void DdeServer::UpdateCoordinate(CoordWithCoordSystem* cwcs)
{
  tsCoord->Update(*cwcs);
}

TopicServerCalc::TopicServerCalc(zDdeServer* pDS)
  : zTopicServer(pDS, "Calc")
{
}

TopicServerCalc::~TopicServerCalc()
{
}

unsigned long TopicServerCalc::request(zRequestDservEvt* Evt)
{
  String str = (const char*)Evt->item();
  if (Evt->dataFormat() == CF_TEXT) {
    String sRes = Calculator::sSimpleCalc(str);
    return Evt->respondWith(sRes.sVal());
  }
  return zTopicServer::request(Evt);
}

TopicServerCoord::TopicServerCoord(zDdeServer* pDS)
  : zTopicServer(pDS, "Coord")
	, cwcs (CoordWithCoordSystem(crdUNDEF))
{
}

TopicServerCoord::~TopicServerCoord()
{
}

int TopicServerCoord::Update(CoordWithCoordSystem cw)
{
  cwcs = cw;
  if (links() != 0) {
    zAdviseLink *el;
	for (zAdviseLinkDlist::iterator trav = links()->begin();trav!=links()->end();++trav)
    {
		el = *trav;
		el->setDirty();
	}
  }
  return 0;
}

CoordWithCoordSystem TopicServerCoord::cwcsValue() const
{
  return cwcs;
}
  
unsigned long TopicServerCoord::request(zRequestDservEvt* Evt)
{
  if (Evt->dataFormat() == CF_TEXT) {
    String str = (const char*)(Evt->item());
    StringFromCoord sfc(str);
    return Evt->respondWith(sfc.sValue(cwcs).sVal());
  }
  return zDdeNAck;
}

unsigned long TopicServerCoord::adviseStart(zAdviseDservEvt* Evt)
{
  if (Evt->dataFormat() == CF_TEXT) {
    new AdviseLinkCoord(this,Evt); // This is correctly cleaned up by adviseStop or ~zTopicServer
    return zDdeAck;
  }
  return zDdeNAck;
}

StringFromCoord::StringFromCoord(const String& str)
{
  eOption = eNONE;
  String sInit = str;
  sInit.toLower();
  if (".x" == sInit)
    eOption = eX;
  else if (".y" == sInit)
    eOption = eY;
  else if (".xy" == sInit)  
    eOption = eXY;
  else {
    try {
      map = BaseMap(sInit);
      if (map.fValid()) 
        eOption = eMAP;
    }
    catch (...) {}
  }  
}

String StringFromCoord::sValue(const CoordWithCoordSystem& cwcs) const
{
  String sRes;
	Coord crd (cwcs);
	CoordSystem cs (cwcs);
  if (crd.fUndef())
    sRes = "?";
  else {  
    switch (eOption) {
      case eX: 
        sRes = String("%.1f", crd.x);
        break;
      case eY: 
        sRes = String("%.1f", crd.y);
        break;
      case eXY:  
        sRes = String("(%.1f,%.1f)", crd.x, crd.y);
        break;
      case eMAP:
        if (cs.fValid() && map->cs().fValid() && cs != map->cs())
          crd = map->cs()->cConv(cs, cwcs);
 				sRes = map->sValue(crd);  
        break;
    }
  }  
  return sRes;
}

AdviseLinkCoord::AdviseLinkCoord
  (TopicServerCoord* tsc, zAdviseDservEvt* Evt)
: zAdviseLink(tsc,Evt), 
  tsCoord(tsc), sfc(String(Evt->item()))
{
}

unsigned long AdviseLinkCoord::render
  (zRenderDservEvt* Evt)
{
  if (Evt->dataFormat() == CF_TEXT) {
    CoordWithCoordSystem cwcs = tsCoord->cwcsValue();
    String sRes = sfc.sValue(cwcs);
    return Evt->respondWith(sRes.sVal());
  }  
  return 0;
}

TopicServerSystem::TopicServerSystem(zDdeServer* pDS)
: zTopicServer(pDS, "System")
{
}

unsigned long TopicServerSystem::request(zRequestDservEvt* Evt)
{
  if (Evt->dataFormat() == CF_TEXT) {
    String sItem = ((String)(Evt->item())).toLower();
    if ("topics" == sItem)
      return Evt->respondWith("System\tCoord\tCalc");
    else if ("sysitems" == sItem)  
      return Evt->respondWith("SysItems\tTopics\tStatus\tFormat");
    else if ("status" == sItem)  
      return Evt->respondWith("Ready");
    else if ("format" == sItem)  
      return Evt->respondWith("Text");
  }
  return zDdeNAck;
}

unsigned long TopicServerSystem::execute(zExecuteDservEvt* Evt)
{
  String sCmd = Evt->cmd();
  char *str, *s;
  for (str = sCmd.sVal(); *str; ++str) {
    if (*str == '[') {
      ++str;
      for (s = str; *s; ++s) {
        if (*s == ']' && (s[1] == 0 || s[1] == '[')) {
          *s = 0;
          command(str);
          str = s;
          break;
        }
      }
    }
    else
      return command(sCmd.sVal());
  }
  return zDdeAck;
}

unsigned long TopicServerSystem::command(char* str)
{
	// str should contain a list of commands to execute, separated by ';''s (dotcomma's)
	// All commands are sent to the script command handler, except:
	// x: (change to drive x) where x in ('a'..'z'): this is translated to cd x: before sent to the script command handler
	// The commands 'import' and 'export' when no parameters are supplied. They're sent to the CommandHandler.
  try
	{
    char* tmp;
    while (str && *str)
		{
			tmp = parsedStrChr(str, ';');
      if (tmp)
        *tmp++ = 0;
			if (str[2]==0 && str[1]==':' && ((str[0]>='a' && str[0]<='z')||(str[0]>='A' && str[0]<='Z')))
			{
				// Follow this branch if the command is to change
				// a directory using something like 'c:' or 'd:'
				// This was (by accident) a valid DDE command in
				// Ilwis 2.23 because it was executed via the command line.
				// Now just append 'cd ' before the command and proceed
				char buf [10]; // Not bigger than needed
				sprintf(buf, "cd %s", str);
				Script::Exec(buf);
			}
			else if (fCIStrEqual(str,"import"))
			{
				// this should popup the import dialog box
		    String sCmd("*import"); // The * is for not putting the command in the history
				const char *s = sCmd.scVal();		
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)s);
			}
			else if (fCIStrEqual(str,"export"))
			{
				// this should popup the export dialog box
		    String sCmd("*export"); // The * is for not putting the command in the history
				const char *s = sCmd.scVal();		
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)s);
			}
			else
				// Normal branch
				Script::Exec(str);
			str = tmp;
    }
    return zDdeAck;
  }
  catch (ErrorObject& err) {
    err.Show();
    return zDdeNAck;
  }
}

char * TopicServerSystem::parsedStrChr(char * str, int c)
{
	// Should return something equivalent to strchr(str.c);
	// The difference is that in strchr the first occurrence of c is returned
	// while here we attempt to return the first occurrence outside any (), {}, [], "" and '' 's.
	// Note: we're only returning a pointer pointing somewhere in the original string. This means we're not allocating memory for a new string

	// Possibility for optimization: the walker = minPtr(...  lines call 'strchr' repeatedly. I dunno how "heavy" strchr is. Should be light though.
	const char singleQuote = '\'';
	const char doubleQuote = '"';
	const char * openBrackets = "([{";
	const char * closeBrackets = ")]}";
	int level = 0;
	char * walker = str;
	while (walker && *walker && ((walker[0] != c) || (level != 0)))
	{
		if (level != 0) // Then exclude c from search
			// Just search the nearest ({[)]}"'
			walker = minPtr(strchr(walker,'('),minPtr(strchr(walker,'{'),minPtr(strchr(walker,'['),minPtr(strchr(walker,doubleQuote),minPtr(strchr(walker,singleQuote),minPtr(strchr(walker,')'),minPtr(strchr(walker,']'),strchr(walker,'}'))))))));
		else // Include c in search
			// Search the nearest c({[)]}"'
			walker = minPtr(strchr(walker,c),minPtr(strchr(walker,'('),minPtr(strchr(walker,'{'),minPtr(strchr(walker,'['),minPtr(strchr(walker,doubleQuote),minPtr(strchr(walker,singleQuote),minPtr(strchr(walker,')'),minPtr(strchr(walker,']'),strchr(walker,'}')))))))));
		// Now walker is null or points to one of the following: c({[)]}"'
		if (walker && strchr(openBrackets, *walker))
		{
			++level;
			++walker;
		}
		if (walker && strchr(closeBrackets, *walker))
		{
			--level;
			++walker;
		}
		if (walker && (doubleQuote == *walker))
		{
			++walker;
			walker = strchr(walker, doubleQuote);
			if (walker) walker++;
		}
		if (walker && (singleQuote == *walker))
		{
			++walker;
			walker = strchr(walker, singleQuote);
			if (walker) walker++;
		}
		if (walker && !*walker) walker=0; // Pointing to the end of the string => character not found => return null pointer
	}

	return walker;
}

char * TopicServerSystem::minPtr(char * a, char * b)
{
	// We're returning a char *
	// We prefer not to return a null pointer, as far as possible
	if (0==a)
	{
		return b; // Whether b is null or not: no better solution
	}
	else // a<>0
	{
		if (0==b)
		{
			return a; // No other solution
		}
		else // Now the real "min" can be done
		{
			if (a<b) return a; else return b;
		}
	}
}
