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
/* $Log: /ILWIS 3.0/FormElements/fldtbl.cpp $
 * 
 * 12    10/11/00 11:47 Willem
 * The command string "Open ..." now uses the quoted file name to execute.
 * 
 * 11    19/09/00 12:45 Willem
 * Create table now also allows domain bool. Domain Image is now
 * disallowed
 * 
 * 10    31-08-00 9:24a Martin
 * mapaatribute works now with histograms and rpr's  (?!)
 * 
 * 9     17-05-00 14:35 Koolhoven
 * Added icon in create forms
 * 
 * 8     15-02-00 11:24 Wind
 * domain bool allowed for table creation
 * 
 * 7     8/12/99 17:37 Willem
 * Add the extension to the domain name in FieldTableC constructor
 * 
 * 6     8/12/99 10:29 Willem
 * Changed FileName asignments (added extension in constructor)
 * 
 * 5     2/12/99 16:18 Willem
 * changed "ilwis" to "open" or "edit" for the execute function
 * 
 * 4     30-11-99 10:16 Koolhoven
 * Header comment
 * 
 * 3     30/11/99 9:53 Willem
 * Removed all references to winExec(); replaced by IlwWinApp->Execute()
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/07/28 17:08:51  Wim
// Use FieldDomainC with all extensions
//
/* FieldTable
   by Wim Koolhoven, november 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   28 Jul 97    7:08 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fldtbl.h"
#include "Engine\Table\tbl.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Table.hs"

void FieldTable::SetDomain(const Domain& dm) 
{ 
  SetObjLister(new TableLister(dm, sGetExtentsions()));
}        

FieldTableC::FieldTableC(FormEntry* fe, const String& sQuestion, 
               String* sTable)
: FieldDataTypeC(fe, sQuestion, sTable, ".TBT", true, 
   (NotifyProc)&FieldTableC::CreateTable)
  { sNewName = *sTable; }

FieldTableC::FieldTableC(FormEntry* fe, const String& sQuestion, 
               String* sTable, const Domain& dm)
: FieldDataTypeC(fe, sQuestion, sTable, new TableLister(dm), true, 
   (NotifyProc)&FieldTableC::CreateTable), sDom(dm->sName(true))
  { sNewName = *sTable; }

void FieldTableC::SetDomain(const Domain& dm) 
{ 
  SetObjLister(new TableLister(dm));
  sDom = dm->sName(); 
}        

int FieldTableC::CreateTable(Event*)
{
  FormCreateTable form(frm()->wnd(), &sNewName, sDom);
  if (form.fOkClicked()) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sFile);
    CallCallBacks();
    String sCmd = "open ";
    sCmd &= fn.sFullPathQuoted(true);
	IlwWinApp()->Execute(sCmd); //        winExec(s.c_str(), SW_SHOWNORMAL);
  }  
  return 0;
}

FormCreateTable::FormCreateTable(CWnd* wPar, String* sTable, const String& sDom)
: FormWithDest(wPar, TR("Create Table")),
  sTable(sTable), sDomain(sDom), wParent(wPar), iRecs(100)
{
	iImg = IlwWinApp()->iImage(".tbt");

  FileName fn(*sTable);
  sNewName = fn.sFile;
  fdt = new FieldTableCreate(root, TR("&Table Name"), &sNewName);
  fdt->SetCallBack((NotifyProc)&FormCreateTable::CallBackName);
  StaticText* st = new StaticText(root, TR("&Description:"));
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  fdc = new FieldDomainC(root, TR("&Domain"), &sDomain, dmCLASS|dmIDENT|dmNONE|dmBOOL, true);
  fdc->SetCallBack((NotifyProc)&FormCreateTable::CallBackDomain);
  fiRecs = new FieldInt(root, TR("&Records"), &iRecs);
  SetMenHelpTopic("ilwismen\\create_a_table.htm");
  String sFill('*', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
  create();
}  

FormCreateTable::~FormCreateTable()
{
  if (wParent)
    ::SetActiveWindow(wParent->m_hWnd);
}

int FormCreateTable::CallBackName(Event*)
{
  fdt->StoreData();
  FileName fn(sNewName, ".tbt");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(TR("Not a valid table name"));
  else if(File::fExist(fn))   
    stRemark->SetVal(TR("Table already exists"));
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

int FormCreateTable::CallBackDomain(Event*)
{
  fdc->StoreData();
  try {
    Domain dm(sDomain);
    if (dm->pdnone())
      fiRecs->Show();
    else
      fiRecs->Hide();  
  }    
  catch (...) {
    fiRecs->Hide();  
  }    
  return 0;
}

int FormCreateTable::exec()
{
  FormWithDest::exec();
  FileName fn(sNewName, ".tbt");
  sNewName = fn.sFullPath(true);
  *sTable = sNewName;
  Domain dm(sDomain);
  if (!dm.fValid() || (0 == dm->pdsrt() && 0 == dm->pdnone() && 0 == dm->pdbool()))
    dm = Domain("none");
  Table tbl(fn, dm);
  if (dm->pdnone())
    tbl->iRecNew(iRecs);
  tbl->sDescription = sDescr;
  tbl->Store();
  return 0;
}






