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
/* FormGeneralMapCreate
   Copyright Ilwis System Development ITC
   march 1997, by Wim Koolhoven
	Last change:  WK   14 Jul 97   10:26 am
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Client\Forms\frmgencr.h"
#include "Client\FormElements\flddom.h"
#include "Engine\Domain\Dmvalue.h"
//#include "CommandLine\commline.h"

FormGeneralMapCreate::FormGeneralMapCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), 
  mw(mwin)
{}                    

int FormGeneralMapCreate::exec()
{
  FormGeneralApplication::exec();
  FileName fn(sOutMap);
  FileName fnMap(sInMap); 
  sInMap = fnMap.sRelative(true,fn.sPath());
  return 0;
}

void FormGeneralMapCreate::initMapIn(const String& asExt)
{
  fdt = new FieldDataType(root, TR("&Input Map"), &sInMap, asExt, true);
  fdt->SetCallBack((NotifyProc)&FormGeneralMapCreate::InMapCallBack);
}

void FormGeneralMapCreate::initMapOut(bool fAskDomain)
{  
  fmc = new FieldDataTypeCreate(root, TR("&Output Map"), &sOutMap, "", false);
  fmc->SetCallBack((NotifyProc)&FormGeneralMapCreate::OutMapCallBack);
  FormEntry* fe = fmc;
  if (fAskDomain) {
    fdc = new FieldDomainC(root, TR("&Domain"), &sDomain);
    fdc->Align(fe, AL_UNDER);
    fe = fdc;
  }  
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->Align(fe, AL_UNDER);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
} 
  
int FormGeneralMapCreate::InMapCallBack(Event*)
{
  fdt->StoreData();
  FileName fn(sInMap);
  sExt = fn.sExt;
  // OutMapCallBack(0); // in future useful ??
  return 0;
}

int FormGeneralMapCreate::OutMapCallBack(Event*)
{
  fmc->StoreData();
  // later check on existance etc.  
  return 0;
}

void FormGeneralMapCreate::execMapOut(const String& sExpr)
{
  FileName fn(sOutMap);
  fn.sExt = sExt;
  if (fn.fExist()) {
    String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
    int iRet=mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
    if (iRet=IDYES)
      return;
  }
  String sCmd = fn.sShortNameQuoted();
  sOutMap = sCmd;
  sCmd &= " = ";
  sCmd &= sExpr;
  SetCommand(sCmd);
  try {
    BaseMap mpOut;
    if (".mpr" == sExt)
      mpOut = Map(fn, sExpr);
    else if (".mpp" == sExt)
      mpOut = PointMap(fn, sExpr);
    else if (".mps" == sExt)
      mpOut = SegmentMap(fn, sExpr);
    else if (".mpa" == sExt)
      mpOut = PolygonMap(fn, sExpr);
      
    if (sDescr[0])
      mpOut->sDescription = sDescr;
    mpOut->fChanged = true;      
    if (fShow) 
		{
       String s = "show ";
       s &= mpOut->sNameQuoted(true);
       //winExec(s.c_str(), SW_SHOWNORMAL);
			 ILWISAPP->Execute(s);
    }
  }  
  catch (ErrorObject& err) {
    err.Show();
  }  
}






