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
// sysvar.c
// Implementation of SysVarList Interface for ILWIS 2.0
// 8 sept 1993, by Jelle Wind  (as BehaviourModifierManager)
// (c) Computer Department ITC
// changed for template lists, jan. 94
// changed for parameter implementation, jan. 94
// changed name to SysVarList, april. 94

#include "Headers\toolspch.h"
#include "Engine\Base\System\SYSVAR.H"

static inline const char * GetEnv(const String& sVar)
// same as getenv, but with environment var in upper case
{
  return (const char*)getenv(String(sVar).toUpper().c_str());
}

SysVarList::SysVarList()
  : ParmList()
{
}

void SysVarList::Fill(ParmList *pl)
{
  for (DLIterP<Parm> iter(&pl->parms); iter.fValid(); )
    if (fExist(*iter())) {
      Replace(iter());
      delete iter.remove();
      iter.first();
     }
     else
       ++iter;
}

void SysVarList::Add(Parm *prm)
{
  // check environment for value
  const char *ps = GetEnv(prm->sOpt());
  if (ps != 0)
    if (prm->pmt() == pmtOpt)
      prm->Replace(String(ps));
    else // flag
      prm->Replace((bool) !( (strcmp(ps, "false") == 0) ||
                             (strcmp(ps, "off") == 0) ));
                             
  // check configuration files
  char sBuf[100];
  String sIni = "ilwis.ini";
  String sEntry = prm->sOpt();
  GetPrivateProfileString("ilwis", sEntry.c_str(), "", sBuf, 100, sIni.c_str());
  String s = sBuf;
  if (s != "") 
    if (prm->pmt() == pmtOpt)
      prm->Replace(String(s));
    else // flag
      prm->Replace((bool) !( (strcmp(s, "false") == 0) ||
                             (strcmp(s, "off") == 0) ));
  
  // add to behviour modifier list
  ParmList::Add(prm);
}








