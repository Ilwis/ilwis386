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
/* FieldTable2Dim
   by Wim Koolhoven, march 1997
   Copyright Ilwis System Development ITC
	Last change:  WK   12 Mar 97    6:43 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fld2dtbl.h"
#include "Client\FormElements\fentvalr.h"
#include "Engine\Table\tbl2dim.h"
#include "Client\FormElements\objlist.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Table.hs"
#include "Client\ilwis.h"

FormCreateTable2Dim::FormCreateTable2Dim(CWnd* wPar, String* sTbl2Dim, const String& sDom)
: FormWithDest(wPar, STBTitleCreateTable2Dim),
  sTable2Dim(sTbl2Dim), sDom1(sDom), wParent(wPar)
{
	iImg = IlwWinApp()->iImage(".ta2");

  sDomFill = "value.dom";
  sNewName = *sTable2Dim;
  fdt = new FieldTable2DimCreate(root, STBUiTblName, &sNewName);
  fdt->SetCallBack((NotifyProc)&FormCreateTable2Dim::CallBackName);
  StaticText* st = new StaticText(root, STBUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  new FieldDomainC(root, STB2dPrimDom, &sDom1, dmCLASS|dmIDENT);
  new FieldDomainC(root, STB2dSecDom, &sDom2, dmCLASS|dmIDENT);

  new StaticText(root, STB2dContents);
  FieldDomainC* fdc = new FieldDomainC(root, STBUiDom, &sDomFill);
  FieldValueRange* fvr = new FieldValueRange(root, STBUiRange, &vr, fdc);
  fdc->SetCallBack((NotifyProc)&FieldValueRange::DomainCallBack, fvr);

  SetMenHelpTopic(htpCreateTable2Dim);
  String sFill('*', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
  create();
}  

FormCreateTable2Dim::~FormCreateTable2Dim()
{
  if (wParent)
    ::SetActiveWindow(wParent->m_hWnd);
}

int FormCreateTable2Dim::CallBackName(Event *)
{
  fdt->StoreData();
  FileName fn(sNewName, ".ta2");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(STBRemNotValidTblName);
  else if(File::fExist(fn))   
    stRemark->SetVal(STBRemTable2DimExists);
  else {
    fOk = true;  
    stRemark->SetVal("");
  }
  if (fOk)
    EnableOK();
  else    
    DisableOK();
  return 0;
}

int FormCreateTable2Dim::exec()
{
  FormWithDest::exec();
  FileName fn(sNewName, ".ta2");
  sNewName = fn.sFullPath(true);
  *sTable2Dim = sNewName;
  Domain dm1(sDom1);
  Domain dm2(sDom2);
  Domain dm(sDomFill);
  if (!dm1.fValid() || !dm2.fValid() || !dm.fValid())
    return 0;
  DomainValueRangeStruct dvrs(dm, vr);
  Table2Dim tbl(fn, dm1, dm2, dvrs);
  tbl->sDescription = sDescr;
  tbl->Store();
  return 0;
}






