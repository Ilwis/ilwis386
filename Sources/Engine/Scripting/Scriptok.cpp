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
/* ScriptTokenizer
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  WK    7 Oct 98    5:17 pm
*/

#include "Engine\Scripting\Script.h"
#include "Engine\Scripting\SCRIPTOK.H"
#include "Engine\Base\System\Engine.h"

char* sComposed[]  = { ":=", "//", 0};
char* sActList[]   =
  { "cd", "mkdir", "rmdir", "md", "rd", "del", "delfile", "delcol", "copy", "copyfile", "open", "show", "run",
    "setdom", "setvr", "setgrf", "additemtodomain", "additemtodomaingroup",
    "setcsy", "cr2dim", "changedom", "domclasstoid", "domidtoclass", "dompictoclass",
    "setreadonly", "setreadwrite", "mergedom","setatttable", "rename", "opentbl", "closetbl", /*"uptodate", "tblinfo", "colinfo",*/
    "!", "calc", "calccol", "breakdep", "breakdepcol", "update", "updatecol", "reldisksp", "export14", "import14", "import", "export",
    "crdom", "crrpr", "crgrf", "crtbl",
    "crpntmap", "crsegmap", "crmap",
    "closeall",
    "rem", "//",
    "message", "pause", 0 };

char* sTypeList[]   =
  { "map", "seg", "pol", "pnt", "tbl", "mat", 0 };


ScriptTokenizer::ScriptTokenizer(const String& sScript, const Array<String>* asParms)
{
  tknb = new TokenizerBase(new TextInput(sScript));
  tknb->CreateListOfType(ttACTION, sActList);
  tknb->CreateListOfType(ttDATATYPE, sTypeList);
  tknb->SetComposed(sComposed);
  tknb->SetSpaceChars(" \t\r\n");
  bool fInQuotes = false;
  if (0 != asParms) {
    String sNew;
    char c = tknb->inp->cGet();
	String potentialSystemVar;
    while (c) {
      if (c == '\'')
        fInQuotes = !fInQuotes;
	  if (c != '%') {
        sNew &= c;
		if ( potentialSystemVar != "") {
			potentialSystemVar += c;
		}
	  }
      else {
        char cc = tknb->inp->cGet();
        int iParm = cc-'1';
        if ((iParm >= 0) && (iParm < (int)asParms->iSize())) {
          if (fInQuotes) 
            sNew &= (*asParms)[iParm].sUnQuote();
          else
            sNew &= (*asParms)[iParm];
        }
        else {
			bool isReplaced = false;
			if (potentialSystemVar == "") {
				potentialSystemVar += cc;
			} else {
				isReplaced = lookupAndReplace(sNew, potentialSystemVar);
				potentialSystemVar = "";
			}
			if ( isReplaced == false) {
				sNew &= c;
				sNew &= cc;
			}
        }
      }
      c = tknb->inp->cGet();
    }
    delete tknb->inp;
    tknb->inp = new TextInput(sNew);
  }  
} 

bool ScriptTokenizer::lookupAndReplace(String& sNew, const String& potentialSystemVar) {
	if ( potentialSystemVar == "ILWIS_LOCATION" ) {
		sNew  = sNew.sSub(0, sNew.size() - potentialSystemVar.size() - 1);
		sNew += getEngine()->getContext()->sIlwDir();
		//sNew = sNew.sSub(0,sNew.size() - 1);
		//sNew = "\"" + sNew + "\"";
		return true;
	} else if ( potentialSystemVar == "ILWIS_CURRENTDIR") {
	  	sNew  = sNew.sSub(0, sNew.size() - potentialSystemVar.size() - 1);
		sNew += getEngine()->sGetCurDir();
	}
	return false;
}

ScriptTokenizer::~ScriptTokenizer()
{
	if (tknb)
	{
		if (tknb->inp)
		{
			delete tknb->inp;
			tknb->inp = 0;
		}
		delete tknb;
		tknb = 0;
	}
}

Token ScriptTokenizer::tokGet()
{
  return tknb->tokGet();
}

String ScriptTokenizer::sCurrentLine()
{ 
  return tknb->sCurrentLine(); 
}





