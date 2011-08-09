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
/* $Log: /ILWIS 3.0/FormElements/fldval.cpp $
 * 
 * 6     5-04-01 15:45 Koolhoven
 * FieldVal::StoreData() protected against combobox and the user not
 * making a choice
 * 
 * 5     1-12-00 14:13 Koolhoven
 * Added FieldBool, FieldVal now uses a FieldBool in case of domain Bool
 * (was Check3Box)
 * 
 * 4     1-12-00 11:09 Hendrikse
 * startcomment corrected
 * 
 * 3     30-11-00 18:01 Koolhoven
 * in FieldVal::NewDomainItem() protect against no selection
// Revision 1.6  1998/10/13 16:15:43  Wim
// In NewDomainItem() in the catch() of code set the sCode on "".
//
// Revision 1.5  1998-10-08 13:04:39+01  Wim
// In FieldVal::NewDomainItem() ignore double codes of a new domain item.
//
// Revision 1.4  1998-10-07 10:48:27+01  Wim
// FieldVal::NewDomainItem() is now protected against throws of errors by DomainSort
// of double items etc.
//
// Revision 1.3  1998-09-16 18:37:53+01  Wim
// 22beta2
//
// Revision 1.2  1997/08/07 15:01:00  Wim
// Uses now DomainValueRangeStruct instead of Domain.
// Value domains use value range
//
/* FieldVal
   by Wim Koolhoven, may 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   13 Oct 98    5:13 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\dmident.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\FormElements\fldval.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldclass.h"
#include "Client\FormElements\fldbool.h"

char* far sNEW = "< new >"; // also defined in \src\dsp\editor.c

FieldVal::FieldVal(FormEntry* parent, const String& sQuestion,
	   const DomainValueRangeStruct& dvrs, String *sName, bool fEdit)
: FormEntry(parent, 0, true), dm(dvrs.dm()), sVal(sName), fEditor(fEdit), fld(0), st(0)
{
  psn->SetBound(0,0,0,0);
  psn->iMinHeight = psn->iHeight = 0;
  if (dm.fValid()) {
    iVal = dm->iRaw(*sVal);
    if (dm->pdbit()) {
      fVal = iVal!=0;
      fld = new CheckBox(this, sQuestion, &fVal);
    }
    else if (dm->pdbool()) {
      st = new StaticTextSimple(this, sQuestion);
      fld = new FieldBoolSimple(this, &iVal, dm->pdbool());
    }
    else if (dm->pdvi()) {
      iVal = dvrs.iValue((*sName).iVal());
      fld = new FieldInt(this, sQuestion, &iVal, dvrs.vr());
    }
    else if (dm->pdvr()) {
      rVal = dvrs.rValue(*sName);
      fld = new FieldReal(this, sQuestion, &rVal, dvrs.vr());
    }
    else if (dm->pdi()) {
      iVal = dvrs.iValue((*sName).iVal());
      fld = new FieldInt(this, sQuestion, &iVal, dm);
    }
    else if (dm->pdcol()) {
      iVal = dm->iRaw(*sName);
      fld = new FieldColor(this, sQuestion, (Color*)&iVal);
    }
  }
  if (0 == fld) {
    if (sQuestion.length() != 0)
      st = new StaticTextSimple(this, sQuestion);
    bool fOse = false;
    if (dm->pdc())
      fOse = true;
    else if (dm->pdid() && !fEditor)
      fOse = dm->pdid()->iNettoSize() < 5440;
    if (fOse)
      fld = new FieldClassSimple(this, &iVal, dm->pdsrt());
  }
  if (0 == fld) {
    dm = Domain();
    fld = new FieldStringSimple(this, sName, dm, false);
  }
  if (st)
    fld->Align(st, AL_AFTER);
}


void FieldVal::SetWidth(short iWidth) 
{ 
  fld->SetWidth(iWidth); 
}

void FieldVal::Fill()
{
  DomainSort* ds = dm->pdsrt();
	if (0 == ds)
		return; // could be domain bool 
  FieldOneSelect* fos = dynamic_cast<FieldOneSelect*>(fld);
  fos->ose->ResetContent();
  int i;
  for (i = 1; i <= ds->iNettoSize(); ++i) {
    long iKey = ds->iKey(i);
    String s = ds->sValueByRaw(iKey,0);
    fos->ose->AddString(s.c_str());
    fos->ose->SetItemData(i-1, iKey);
  }
  if (fEditor) {
    fos->ose->AddString("?");
    fos->ose->SetItemData(i-1, iUNDEF); 
    fos->ose->AddString(sNEW);
    fos->ose->SetItemData(i, iUNDEF);
  }  
  fos->ose->SelectString(-1, (*sVal).c_str());
}

void FieldVal::create()
{
  CreateChildren();
  FieldOneSelect* fos = dynamic_cast<FieldOneSelect*>(fld);
  if (fos) {
    Fill();
    fos->ose->setNotify(this, (NotifyProc)&FieldVal::NewDomainItem, Notify(CBN_SELCHANGE));
  }
  else {
    FieldStringSimple* fss = dynamic_cast<FieldStringSimple*>(fld);
    if (fss)
      fss->SelectAll();
  }
}

int FieldVal::NewDomainItem(Event*)
{
  class Form: public FormWithDest
  {
  public:
    Form(CWnd* wPar, String* sName, String* sCode, Color* clr)
    : FormWithDest(wPar, TR("Add item to Domain"))
    {
      new FieldBlank(root);
      new FieldString(root, TR("&Name"), sName, Domain(), false);
      new FieldString(root, TR("&Code"), sCode, Domain(), true);
      new FieldColor(root, TR("Co&lor"), clr);
  //    SetMenHelpTopic("ilwismen\\domain_class_id_editor_edit_item.htm");
      create();
    }
  };
  // check
  FieldOneSelect* fos = dynamic_cast<FieldOneSelect*>(fld);
  OneSelectEdit* ose = fos->ose;
  CString s;
	int iSel = ose->GetCurSel();
	if (iSel < 0)
		return 0;
  ose->GetLBText(iSel, s);
  if (s == sNEW) {
    String sName, sCode;
    Color clr;
ShowForm:
    Form form(frm()->wnd(), &sName, &sCode, &clr);
    if (form.fOkClicked()) {
      try {
        long iRaw = dm->pdsrt()->iAdd(sName);
        if (sCode.length() != 0) {
          try {
            dm->pdsrt()->SetCode(iRaw, sCode);
          }
          catch (const ErrorObject&) {
            // ignore code
            sCode = "";
          }
        }
        Representation rpr = dm->pdsrt()->rpr();
        if (rpr.fValid())
          rpr->prc()->PutColor(iRaw, clr);
        Fill();
        if (sCode.length() != 0)
          sName = String("%S: %S", sCode, sName);
        ose->SelectString(-1, sName.c_str());
      }
      catch (const ErrorObject& err) {
        err.Show();
        goto ShowForm;
      }
    }
  }  
  return 0;
}

void FieldVal::StoreData()
{
  fld->StoreData();
  FieldOneSelect* fos = dynamic_cast<FieldOneSelect*>(fld);
  FieldInt* fi = dynamic_cast<FieldInt*>(fld);
  FieldReal* fr = dynamic_cast<FieldReal*>(fld);
  if (fos) {
    OneSelectEdit* ose = fos->ose;
		int iCurSel = ose->GetCurSel();
		if (iCurSel < 0)
			*sVal = "?";
		else {
	    CString s;
	    ose->GetLBText(iCurSel, s);
		  *sVal = String(s);
		}
  }
  else if (fi)
    *sVal = String("%li", iVal);
  else if (fr)
    *sVal = String("%g", rVal);
  else if (dm->pdbit())
    *sVal = dm->sValueByRaw(fVal);
  else if (dm->pdcol())
    *sVal = dm->sValueByRaw(iVal);
}





