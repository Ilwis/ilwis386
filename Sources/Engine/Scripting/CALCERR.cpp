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
/* $Log: /ILWIS 3.0/Calculator/CALCERR.cpp $
 * 
 * 4     8/01/01 4:16p Martin
 * Error typedef replaced with the int (it realy is), avoid name clashes
 * 
 * 3     3/05/99 15:31 Willem
 * - RCS Log message comment characters changed
 * 
 * 2     4/09/99 10:33a Martin
 * Font errors and a constructor errors
// Revision 1.3  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.2  1997/09/23 16:09:16  Wim
// Protect Show() against iColPos <= 0
//
/* calcerr.c
   Source for calculator errors
   june 1997, Jelle Wind
	Last change:  WK   23 Sep 97    5:31 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Scripting\CALCERR.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
/*
CalcError::CalcError(Error err, int iLineP, int iColP)
: ErrorObject(String(), err), iLinePos(iLineP), iColPos(iColP)
{
} */

CalcError::CalcError(const String& sExp, const String& sErr, const String& sTtl, int err, int iLineP, int iColP)
: ErrorObject(sErr, err), sExpr(sExp), iLinePos(iLineP), iColPos(iColP)
{
  if (iLinePos > 0) {
    Array<String> as;
    Split(sExpr, as, "\n");
    sExpr = as[iLinePos-1];
  }
  else
    sExpr = sExp;
  SetTitle(sTtl);
}
/*
void CalcError::SetExpression(const String& sExp)
{
  sExpr= sExp;
} */

void CalcError::Show(const String& sTtl) const
{
  String sErr = sShowError();
//  sErr &= String("\nin: '%S'", sExpr);
//  sErr &= String("\nchar pos: %i", iColPos);
  String sPos;
  if (iColPos > 0) {
    sPos = String(' ', iColPos);
    sPos[sPos.length()-1] = '^';
  }
  String sT = sTtl;
  if (sT.length() == 0)
    sT = where.sTitle();
  MessageBeep(MB_ICONHAND);
  String sCmd("showexpressionerror %S,%S,%S,%S,%S", sT, sErr,sExpr, sPos, where.sWhere());
  getEngine()->Execute(sCmd);
  sErr &= String("\nin: '%S'", sExpr);
  if (iColPos > 0)
    sErr &= String("\n     %S", sPos);
  getEngine()->getLogger()->LogLine(sErr);
}




