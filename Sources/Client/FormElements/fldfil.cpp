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
/* FieldFilter
   by Wim Koolhoven, april 1996
   Copyright Ilwis System Development ITC
	Last change:  WK   14 Jul 97   10:21 am
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldfil.h"
#include "Engine\Function\FILTER.H"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\FILTER.hs"

int FieldFilterC::CreateFilter(void *)
{
  FormCreateFilter form(frm()->wnd(), &sNewName);
  if (form.fOkClicked()) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sFile);
    CallCallBacks();
    String s = "open ";
    s &= fn.sFullPathQuoted();
	IlwWinApp()->Execute(s); //    winExec(s.scVal(), SW_SHOWNORMAL);
  }
  return 0;
}

FormCreateFilter::FormCreateFilter(CWnd* wPar, String* sFil)
: FormWithDest(wPar, SFLTitleCreateFilter),
  sFilter(sFil), wParent(wPar)
{
	iImg = IlwWinApp()->iImage(".fil");

  sNewName = *sFilter;
  fdm = new FieldDataTypeCreate(root, SFLUiFilName, &sNewName, ".FIL", true);
  fdm->SetCallBack((NotifyProc)&FormCreateFilter::CallBackName);
  StaticText* st = new StaticText(root, SFLUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  
  iRgVal = 0;
  RadioGroup* rg = new RadioGroup(root, SFLUiType, &iRgVal);
  new RadioButton(rg, SFLUiLinearFilter);

  iRows = 3;
  iCols = 3;
  fReal = false;
  fiRows = new FieldInt(root, SFLUiRows, &iRows);
  fiCols = new FieldInt(root, SFLUiColumns, &iCols);
	fiRows->SetCallBack((NotifyProc)&FormCreateFilter::CallBackRowColSize);
	fiCols->SetCallBack((NotifyProc)&FormCreateFilter::CallBackRowColSize);

  String sFill('*', 60);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();

  SetMenHelpTopic(htpFilterCreate);
  create();
}

FormCreateFilter::~FormCreateFilter()
{
//  if (wParent)
//    SetActiveWindow(*wParent);
}

int FormCreateFilter::exec()
{
  FormWithDest::exec();
  *sFilter = sNewName;
  FileName fn(*sFilter, ".fil");
  String s("FilterLinear(%i,%i,%s)", iRows, iCols, fReal?"real":"int");
  try {
    Filter flt = Filter(fn, s);
    if (sDescr != "")
      flt->sDescription = sDescr;
	CWaitCursor wc;
    flt->Store();
  }
  catch (ErrorObject& err) {
    err.Show();
    *sFilter = "";
  }  
  return 0;
}

int FormCreateFilter::CallBackRowColSize(Event*)
{
  fiRows->StoreData();
  fiCols->StoreData();
	fRowColOk = ((iRows > 0) && (iCols > 0) && ((iRows % 2) != 0) && ((iCols % 2) != 0));

	String sRem = fRowColOk ? String("") : SFLRemWrongFilterSize;

	stRemark->SetVal(sRem);

  if (fRowColOk && fNameOK)
    EnableOK();
  else    
    DisableOK();
  return 0;
}

int FormCreateFilter::CallBackName(Event*)
{
  fdm->StoreData();
  FileName fn(sNewName, ".fil");
  fNameOK = false;
  if (!fn.fValid())
    stRemark->SetVal(SFLRemNotValidFilterName);
  else if(File::fExist(fn))   
    stRemark->SetVal(SFLRemFilExists);
  else {
    fNameOK = true;  
    stRemark->SetVal("");
  }

  if (fRowColOk && fNameOK)
    EnableOK();
  else    
    DisableOK();
  return 0;
}
