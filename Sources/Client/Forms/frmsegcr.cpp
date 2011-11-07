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
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\base\system\engine.h"
#include "Client\ilwis.h"
#include "Client\Forms\frmsegcr.h"

FormSegmentMapCreate::FormSegmentMapCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), 
  mw(mwin)
{
	int iImg = IlwWinApp()->iImage("ExeSeg16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
}                    


void FormSegmentMapCreate::initSegmentMapOut(bool fAskDomain)
{  
  fmc = new FieldSegmentMapCreate(root, TR("&Output Segment Map"), &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormSegmentMapCreate::OutSegmentMapCallBack);
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
  
int FormSegmentMapCreate::OutSegmentMapCallBack(Event*)
{
  fmc->StoreData();
  // later check on existance etc.  
  return 0;
}

void FormSegmentMapCreate::execSegmentMapOut(const String& sExpr)
{
  FileName fn(sOutMap);
  fn.sExt = ".mps";
  if (fn.fExist()) {
    String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
    int iRet = mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
    if (iRet!=IDYES)
      return;
  }
  String sCmd = fn.sShortNameQuoted();
  sOutMap = sCmd;
  sCmd &= " = ";
  sCmd &= sExpr;
  String sExec;
  SetCommand(sCmd);
  try {
    SegmentMap mpOut(fn, sExpr);
    if (sDescr[0])
      mpOut->sDescription = sDescr;
    mpOut->Store(); // destructor of mpOut could be called later than opening map
	openMap(mpOut);
  }  
  catch (ErrorObject& err) {
    err.Show();
    return;
  }
}
