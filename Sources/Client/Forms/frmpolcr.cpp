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
/* $Log: /ILWIS Id Grid Map/ApplicationForms/frmpolcr.cpp $
 * 
 * 7     9/05/02 8:54a Lichun
 * == is replaced by != when OK is pressed in execPolygonMapOut function.
 * 
 * 6     5-07-00 9:36 Koolhoven
 * Application Forms now use Show and Define buttons instead of OK
 * 
 * 5     30-09-99 15:22 Koolhoven
 * Application Forms now use the correct icon
 * 
 * 4     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 3     24-03-99 11:15 Koolhoven
 * Header comment
 * 
 * 2     3/22/99 9:18a Martin
// Revision 1.4  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.3  1997/07/30 07:49:47  Wim
// Put winExec() before mpOut is destructed.
// Too fast reloading it seem to cause problems in Windows function
// GetPrivateProfileString or in Ilwis object management.
//
// Revision 1.2  1997-07-28 16:30:51+02  Wim
// call winExec after destructor of mpOut
//
/* FormPolygonMapCreate
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   30 Jul 97    9:49 am
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Client\Forms\frmpolcr.h"

FormPolygonMapCreate::FormPolygonMapCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), 
  mw(mwin)
{
	int iImg = IlwWinApp()->iImage("ExePol16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
}                    


void FormPolygonMapCreate::initPolygonMapOut(bool fAskDomain)
{  
  fmc = new FieldPolygonMapCreate(root, TR("&Output Polygon Map"), &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormPolygonMapCreate::OutPolygonMapCallBack);
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
  
int FormPolygonMapCreate::OutPolygonMapCallBack(Event*)
{
  fmc->StoreData();
  // later check on existance etc.  
  return 0;
}

void FormPolygonMapCreate::execPolygonMapOut(const String& sExpr)
{
  FileName fn(sOutMap);
  fn.sExt = ".mpa";
  if (fn.fExist()) {
    String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
    int iRet = mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
    if ( iRet != IDYES ) //== is replaced by !=
      return;
  }
  String sCmd = fn.sShortNameQuoted();
  sOutMap = sCmd;
  sCmd &= " = ";
  sCmd &= sExpr;
  SetCommand(sCmd);
  String sExec;
  try {
    PolygonMap mpOut(fn, sExpr);
    if (sDescr[0])
      mpOut->sDescription = sDescr;
    mpOut->Store(); // destructor of mpOut could be called later than opening map 
    // in new ILWIS instance using winExec
    if (fShow) 
		{
      String sExec = "show ";
      sExec &= mpOut->sNameQuoted(true);
      //winExec(sExec.c_str(), SW_SHOWNORMAL);
			ILWISAPP->Execute(sExec);
    }
  }  
  catch (ErrorObject& err) {
    err.Show();
    return;
  }
}
