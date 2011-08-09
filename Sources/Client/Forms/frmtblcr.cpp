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
/* FormTableCreate
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK    9 Jun 98   10:45 am
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Client\Forms\frmtblcr.h"

FormTableCreate::FormTableCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), 
  mw(mwin)
{
	int iImg = IlwWinApp()->iImage("ExeTbl16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
}                    


void FormTableCreate::initTableOut(bool fAskDomain)
{  
  ftc = new FieldTableCreate(root, TR("&Output Table"), &sOutTable);
  FormEntry* fe = ftc;
  if (fAskDomain) {
    fdc = new FieldDomainC(root, TR("&Domain"), &sDomain);
    fdc->Align(fe, AL_UNDER);
  }  
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->Align(fe, AL_UNDER);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
} 
  
void FormTableCreate::execTableOut(const String& sExpr)
{
  FileName fn(sOutTable);
  if (fn.sExt == "") // take care of histograms
    fn.sExt = ".tbt";
  if (".tbt" == fn.sExt && fn.fExist()) {
    String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
    int iRet= mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
    if (iRet != IDYES)
      return;
  }
  String sCmd = fn.sShortNameQuoted();
  sCmd &= " = ";
  sCmd &= sExpr;
  SetCommand(sCmd);
  try {
    Table tbOut(fn, sExpr);
    if ( tbOut.fValid() )
    {
        if (sDescr[0])
            tbOut->sDescription = sDescr;
        tbOut->Store(); // destructor of tbOut could be called later than opening table
        // in new ILWIS instance using winExec
        if (fShow) 
				{
            String s = "show ";
            s &= tbOut->sNameQuoted(true);
            //winExec(s.c_str(), SW_SHOWNORMAL);
						ILWISAPP->Execute(s);
        }
    }
  }  
  catch (ErrorObject& err) {
    err.Show();
  }  
}
