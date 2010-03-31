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
/* $Log: /ILWIS 3.0/Script_Copier/Script2.cpp $
 * 
 * 14    19-07-00 3:46p Martin
 * stripped spaces to get endcomment working again
 * 
 * 13    21-02-00 16:36 Wind
 * moved more commands to command handler
 * 
 * 12    16-02-00 17:27 Wind
 * moved commands from script to commandhandler
 * 
 * 11    7-02-00 17:05 Wind
 * removed an ' update catalog'
 * 
 * 10    14-01-00 17:19 Koolhoven
 * Post reread Catalog as thread message
 * 
 * 9     2-12-99 12:02 Wind
 * deactivate delay in tranquilizer
 * 
 * 8     29-10-99 12:51 Wind
 * case sensitive stuff
 * 
 * 7     10/06/99 5:18p Wind
 * allow import of map lists
 * 
 * 6     9/29/99 10:34a Wind
 * added case insensitive string comparison
 * 
 * 5     9/08/99 12:56p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 4     3/12/99 3:05p Martin
 * Added support for case insensitive
 * 
 * 3     3/08/99 3:26p Martin
 * //->/*
 * 
 * 2     3/08/99 3:18p Martin
 * Tokenizerbas now correctly initialized
// Revision 1.10  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.9  1997/10/04 12:49:11  Wim
// fDefineObject() will now throw an InvalidLineError if the output object filename
// is invalid
//
// Revision 1.8  1997-09-15 21:50:58+02  Wim
// calculate minmax of columns also in script
//
// Revision 1.7  1997-09-11 12:30:13+02  Wim
// At the end of a script force the catalog to reread, do not nicely request
//
// Revision 1.6  1997-09-05 11:13:45+02  Wim
// Allow also for point, segment and polygon maps to set the domain and valuerange
// in fDefineObject()
//
// Revision 1.5  1997-09-03 18:12:49+02  Wim
// When define object fails stop the script
//
// Revision 1.4  1997-08-18 20:44:36+02  Wim
// Set fnDom default on map name when no domain specidifed
//
/* ScriptPtr  // part 2
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  WK   23 Jun 98    5:22 pm
*/

#include "Headers\constant.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\File\COPIER.H"
#include "Engine\Base\File\objinfo.h"
#include "Engine\Base\Tokbase.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Scripting\SCRIPTOK.H"
#include "Headers\Hs\DAT.hs"
#include "Engine\Map\Raster\MapList\maplist.h"
//#include "Client\ilwis.h"

static bool fDefineObject(bool fDataType, Token& tok, TokenizerBase& tkn, const FileName& fnObj)
{
  String sType = "map";
  if (fDataType) {
    sType = tok.sVal();
    tok = tkn.tokGet();
  }
  FileName fnObject(tok.sVal());
  if (!fnObject.fValid()) {
    ScriptPtr::InvalidLineError(tkn.inp->sLine(tok.iLine()), fnObj);
  }
  tok = tkn.tokGet();
  Domain dm;
  ValueRange vr;
  if (tok == "{")
    if (!ScriptPtr::fScanDomainValueRange(tok, tkn/**tkn.tknb*/, dm, vr))
      return false;
  bool fBreakDep = tok == ":=";
  String sRestOfLine = tkn.sCurrentLine();
  if (!fBreakDep && (tok != "="))
    ScriptPtr::InvalidLineError(tkn./*tknb->*/inp->sLine(tok.iLine()), fnObj);
  if (!fDataType) {
    sRestOfLine = sRestOfLine.sTrimSpaces();
    if (fCIStrEqual(sRestOfLine.sLeft(7) , "MapList"))
      sType = "mpl";
    else if(fCIStrEqual(sRestOfLine.sLeft(3) , "Map"))
      sType = "map";
    else if (fCIStrEqual(sRestOfLine.sLeft(5) , "Table"))
      sType = "tbl";
    else if (fCIStrEqual(sRestOfLine.sLeft(10) , "SegmentMap"))
      sType = "seg";
    else if (fCIStrEqual(sRestOfLine.sLeft(10) , "PolygonMap"))
      sType = "pol";
    else if (fCIStrEqual(sRestOfLine.sLeft(8) , "PointMap"))
      sType = "pnt";
    else if (fCIStrEqual(sRestOfLine.sLeft(6) , "Matrix"))
      sType = "mat";
  }
  if (fCIStrEqual(sType , "map")) {
    Map map(fnObject, sRestOfLine);
//  MapVirtual* mv = map->pmv();
    if (!map.fValid())
      return false;
    if (dm.fValid() && map->fDomainChangeable())
      map->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    else if (map->fValueRangeChangeable() && vr.fValid())
      map->SetValueRange(vr);
    if (fBreakDep)
      map->BreakDependency();
  }
  else if (fCIStrEqual(sType , "mpl")) {
    MapList mpl(fnObject, sRestOfLine);
    if (!mpl.fValid())
      return false;
  }
  else if (fCIStrEqual(sType , "tbl")) {
    Table tbl(fnObject, sRestOfLine);
    if (!tbl.fValid())
      return false;
    if (fBreakDep)
      tbl->BreakDependency();
//      tbl.VirtualToStore();
  }
  else if (fCIStrEqual(sType , "seg")) {
    fnObject.sExt = ".mps";
    SegmentMap map(fnObject, sRestOfLine);
    if (!map.fValid())
      return false;
    if (dm.fValid() && map->fDomainChangeable())
      map->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    else if (map->fValueRangeChangeable() && vr.fValid())
      map->SetValueRange(vr);
    if (fBreakDep)
      map->BreakDependency();
  }
  else if (fCIStrEqual(sType , "pol")) {
    fnObject.sExt = ".mpa";
    PolygonMap map(fnObject, sRestOfLine);
    if (!map.fValid())
      return false;
    if (dm.fValid() && map->fDomainChangeable())
      map->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    else if (map->fValueRangeChangeable() && vr.fValid())
      map->SetValueRange(vr);
    if (fBreakDep)
      map->BreakDependency();
  }
  else if (fCIStrEqual(sType , "pnt")) {
    fnObject.sExt = ".mpp";
    PointMap map(fnObject, sRestOfLine);
    if (!map.fValid())
      return false;
    if (dm.fValid() && map->fDomainChangeable())
      map->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
    else if (map->fValueRangeChangeable() && vr.fValid())
      map->SetValueRange(vr);
    if (fBreakDep)
      map->BreakDependency();
  }
  else if (fCIStrEqual(sType , "mat")) {
    fnObject.sExt = ".mat";
    MatrixObject mat(fnObject, sRestOfLine);
    if (!mat.fValid())
      return false;
  }
  else
    ScriptPtr::UnknownCommandError(fnObject.sFile, fnObj);
  return true;
}

void ScriptPtr::Exec(const Array<String>* asParms)
{
	bool fShowTranq = fnObj.fValid();
	if (fShowTranq) {
		trq.SetTitle(String(SDATTitleExecuteScript_S.scVal(), fnObj.sFile));
		trq.SetDelayShow(false);
		trq.Start();
	}
	ScriptTokenizer stkn(_sScript, asParms);
	bool fComment = false;
	while (!stkn.tknb->fEnd()) {
		String sLine = stkn.sCurrentLine().sTrimSpaces();
		if (fCIStrEqual(sLine, "begincomment"))
			fComment = true;
		if (fCIStrEqual(sLine, "endcomment") && fComment) {
			fComment = false;
			continue;
		}
		if (fComment)
			continue;
		if (fShowTranq) {
			String s = sLine;
			if (s.length() > 42) {
				s = sLine.sLeft(42);
				s &= "...";
			}
			TRACE2("%i trq=%s\n",GetCurrentThreadId(), s.scVal());
			if (trq.fText(s))
				return;
		}
		TextInput inp(sLine);
		TokenizerBase tkn(&inp);
		tkn.CreateListOfType(ttACTION, sActList);
		tkn.CreateListOfType(ttDATATYPE, sTypeList);
		tkn.SetComposed(sComposed);
		tkn.SetSpaceChars(" \t\r\n");
		Token tok = tkn.tokGet();
		if (tok == "")
			continue;
		try {
			if (tok.sVal() == "/*") {
				fComment = true;
				continue;
			}
			if (tok.sVal() == "*/" && fComment) {
				fComment = false;
				continue;
			}
			if ((tok.sVal() != "rem") && (tok.sVal() != "\\\\"))
				getEngine()->getLogger()->LogLine(sLine);
			//      if (tok.tt() == ttACTION)
			ExecAction(tok, tkn);
			/*      else if (fCIStrEqual(tok.sVal(), "tabcalc")) {
			getEngine()->Execute(sLine);
			}
			else
			if (!fDefineObject(tok.tt() == ttDATATYPE, tok, tkn, fnObj))
			break;*/
		}
		catch (const ErrorObject& err) {
			err.Show();
			break;
		}
	}
	if (fShowTranq) 
		trq.Stop();
	//	is also done in command handler:
	//  AfxGetApp()->PostThreadMessage(ILW_READCATALOG, 0, 0);
}

void Script::Exec(const String& sScriptLine)
{
  Script scr(sScriptLine);
  scr->Exec();
}

void ScriptPtr::Rename(const FileName& fnNew)
{
  IlwisObjectPtr::Rename(fnNew);
  // rename script data file
  FileName fn = fnScript;
  fn.sFile = fnNew.sFile;
  rename(fnScript.sFullName().scVal(), fn.sFullName().scVal());
  fnScript.sFile = fnNew.sFile;
}
