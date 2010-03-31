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
/* $Log: /ILWIS 3.0/ApplicationForms/imp14.cpp $
 * 
 * 6     17-07-00 8:15a Martin
 * changed sCurDir to sGetCurDir from IlwisWinApp
 * 
 * 5     28/04/00 9:57 Willem
 * File mask is now preserved (it was overwritten by a wrong extension
 * mask)
 * 
 * 4     29-11-99 10:25 Wind
 * adapted for UNC (use now SetCurrentDirectory and GetCurrentDirectory)
 * 
 * 3     9/10/99 1:19p Wind
 * adaption to changes in FileName constructors
 * 
 * 2     18-06-99 9:43a Martin
 * ported the import forms
 * 
 * 1     17-06-99 4:09p Martin
// Revision 1.3  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.2  1997/08/28 15:45:40  Wim
// Removed path from sOutMap in all map import forms and the table import form
//
/* 
  Import14
  by Wim Koolhoven
  (c) ILWIS System Development, ITC
	Last change:  WK   28 Aug 97    5:30 pm
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\IMP14.H"
#include "Engine\DataExchange\MAPIMP.H"
#include "Engine\DataExchange\SEGIMP.H"
#include "Engine\DataExchange\POLIMP.H"
#include "Engine\DataExchange\PNTIMP.H"
#include "Engine\DataExchange\Tblimp.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Representation\Rprclass.h"
#include "Client\ilwis.h"
#include "Headers\hourglss.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Client\FormElements\fldtbl.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\ilwis.h"

Import14Form::Import14Form(CWnd* wPar, const String& sTitle)
  : FormWithDest(wPar, sTitle)
{
}

Import14Form::~Import14Form()
{
}

Import14MaskForm::Import14MaskForm(CWnd* wPar, String* sMask)
  : Import14Form(wPar, SIETitleImp14)
{
  FormEntry* fe = new FieldString(root, SIEUiMask, sMask);
  fe->SetWidth(90);
  SetMenHelpTopic(htpImport14);
  create();
}

int Import14(CWnd* wPar)
{
  String sMask = "*";
  bool fOk;
  {
    Import14MaskForm frm(wPar, &sMask);
    fOk = frm.fOkClicked();
  }
  if (fOk) 
    Import14(wPar, sMask);  
  return 0;  
}


int Import14(CWnd* wPar, const String& sMask)
{
  String sOutDir = IlwWinApp()->sGetCurDir();
  try {
    FileName fn(sMask);

    String sDir = fn.sPath(); 
    if (sDir[sDir.length()-2] != ':' && sDir[sDir.length()-1] == '\\')
      sDir[sDir.length()-1] = 0;
    SetCurrentDirectory(sDir.scVal());

    char sTemp[MAX_PATH];
    if (0 == GetCurrentDirectory(MAX_PATH, sTemp))
      return 0;

    bool fDoneSomething = false;

    fn.sExt = ".mpd";
    String sSearch = fn.sFullName();
		for(FilenameIter files(sSearch); files != files.Invalid(); ++files)
		{
      FileName fn = *files;
      fn.Dir(sTemp);
      FileName fnOut = fn;
      fnOut.Dir(sOutDir);
      if (!File::fExist(FileName(fnOut, ".MPR"))) 
			{
        fDoneSomething = true;      
        try {
          Import14MapForm frm(wPar, fn, fnOut.sRelative(false, fn.sPath()));
          if (frm.fOkClicked())
            frm.import();
          else  
            return 0;
        }    
        catch (ErrorObject& err) { err.Show(); }
      }
    }
    fn.sExt = ".seg";
    sSearch = fn.sFullName();
    for(FilenameIter files2(sSearch); files2 != files2.Invalid(); ++files2)
    {
      FileName fn = *files2;
      fn.Dir(sTemp);
      FileName fnOut = fn;
      fnOut.Dir(sOutDir);
      if (!File::fExist(FileName(fnOut, ".MPS")) &&
          !File::fExist(FileName(fnOut, ".MPA")) &&
          !File::fExist(FileName(fnOut, ".POL"))) {
        try {
          fDoneSomething = true;      
          Import14SegmentMapForm frm(wPar, fn, fnOut.sRelative(false, fn.sPath())); 
          if (!frm.fOkClicked())
            return 0;
        }    
        catch (ErrorObject& err) { err.Show(); }
      }
    }
    fn.sExt = "*.pol";
    sSearch = fn.sFullName();
    for(FilenameIter files3(sSearch); files3 != files3.Invalid(); ++files3)
    {
      FileName fn = *files3;
      fn.Dir(sTemp);
      FileName fnOut = fn;
      fnOut.Dir(sOutDir);
      if (!File::fExist(FileName(fnOut, ".MPA"))) {
        try {
          fDoneSomething = true;      
          Import14PolygonMapForm frm(wPar, fn, fnOut.sRelative(false, fn.sPath()));
          if (!frm.fOkClicked())
            return 0;
        }    
        catch (ErrorObject& err) { err.Show(); }
      }
    }
    fn.sExt = "*.pnt";
    sSearch = fn.sFullName();
    for(FilenameIter files4(sSearch); files4 != files4.Invalid(); ++files4)
    {
      FileName fn = *files4;
      fn.Dir(sTemp);
      FileName fnOut = fn;
      fnOut.Dir(sOutDir);
      if (!File::fExist(FileName(fnOut, ".MPP"))) {
        try {
          fDoneSomething = true;      
          Import14PointMapForm frm(wPar, fn, fnOut.sRelative(false, fn.sPath()));
          if (!frm.fOkClicked())
            return 0;
        }    
        catch (ErrorObject& err) { err.Show(); }
      }
    }
    fn.sExt = ".tbl";
    sSearch = fn.sFullName();
    for(FilenameIter files5(sSearch); files5 != files5.Invalid(); ++files5)
    {
      FileName fn = *files5;
      fn.Dir(sTemp);
      FileName fnOut = fn;
      fnOut.Dir(sOutDir);
      if (!File::fExist(FileName(fnOut, ".TBT"))) {
        try {
          fDoneSomething = true;      
          Import14TableForm frm(wPar, fn, fnOut.sRelative(false, fn.sPath()));
          if (!frm.fOkClicked())
            return 0;
        }    
        catch (ErrorObject& err) { err.Show(); }
      }
    }
//
//  IlwisApp()->mh()->Show();
    if (fDoneSomething)
      MessageBox(0, SIEMsgFinished.scVal(), SIEMsgImp14.scVal(), MB_ICONINFORMATION|MB_OK);
    else
      MessageBox(0, SIEMsgNothingToDo.scVal(), SIEMsgImp14.scVal(), MB_ICONINFORMATION|MB_OK);
//  IlwisApp()->mh()->Message(SDFinished);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
  catch (...) {
    MessageBox(0, SIEErrImpError.scVal(), SIEErrError.scVal(), MB_ICONSTOP|MB_OK);
  }
  String sCurDir = sOutDir;
//  _chdrive(1 + tolower(sCurDir[0]) - 'a');
//  if (sCurDir[sCurDir.length()-2] != ':')
//    sCurDir[sCurDir.length()-1] = 0;
  SetCurrentDirectory(sCurDir.scVal());
  ILWISAPP->DirChanged();
  return 0;
}

Import14MapForm::Import14MapForm(CWnd* wPar, const FileName& fn, const String& sDef)
: Import14Form(wPar, SIETitleImpRas14),
  fnMap(fn),
  fClass(false), fId(false), fValue(false), 
  fPicture(false), fImage(false), fBit(false),
  sOutMap(sDef)
{
  if ("" == sOutMap)
    sOutMap = fn.sFile;
  int iMapType, iScale;
  bool fInf, fCol, fGrfNone;
  FileName fnGrf;
  MapImport::GetImportInfo(fn, iMapType, iScale, fInf, fCol, fGrfNone, fnGrf, FileName(sDef).sPath());
  iDom = 0;
  if (iMapType == 0) {
    iDom = 0; // dmt = dmtBIT;
    fBit = true;
    SetMenHelpTopic(htpImport14MapBit);
  }
  else if (iMapType == 1) {  // byte maps
    if (fInf) {              // has class info
      iDom = 0;              //   dmtCLASS
      fClass = fId = true;   //   possible selections
      SetMenHelpTopic(htpImport14MapInf);
    }
    else if (fCol) {         // no class info, but has LUT
      iDom = 2;              //   dmtPICTURE
      fImage = fClass = fPicture = true;
      SetMenHelpTopic(htpImport14MapCol);
    }
    else {                   // no class info, no LUT
      iDom = 2;              //   dmtIMAGE
      fClass = fId = fValue = fImage = true;
      SetMenHelpTopic(htpImport14MapByte);
    }
  }
  else {  // integer maps; ignore LUT
    if (fInf) {              // class info found
      iDom = 1;              //   dmtID; 
      fClass = fId = true;
      SetMenHelpTopic(htpImport14MapInf);
    }
    else {                   // no class info found
      iDom = 2;              // dmtVALUE;
      fClass = fId = fValue = true;
      SetMenHelpTopic(htpImport14MapInt);
    }
  }
  
  sDescr = String(SIEDscRasMap_s.scVal(), fn.sFile);
  new StaticText(root, sDescr);
  rg = new RadioGroup(root, SIEUiDom, &iDom);
  if (fClass) 
    new RadioButton(rg, SIEUiClass);
  if (fId) 
    new RadioButton(rg, SIEUiIdentifier);
  if (fImage) 
    new RadioButton(rg, SIEUiImage);
  if (fValue) 
    new RadioButton(rg, SIEUiValue);
  if (fPicture) 
    new RadioButton(rg, SIEUiPicture);
  if (fBit) 
    new RadioButton(rg, SIEUiBit);
  if (fClass || fId) {
    sDom = FileName(sOutMap).sRelative(false, fn.sPath());
    rg->SetCallBack((NotifyProc)&Import14MapForm::CallBack);
    fldDom = new FieldDataTypeCreate(root, SIEUiDomName, &sDom, ".dom", true);
    fldDom->SetCallBack((NotifyProc)&Import14Form::DomCallBack);
  }
  if (fGrfNone) {
    StaticText* st = new StaticText(root, SIERemNoGeoRef);
    st->SetIndependentPos();
  }
  else if (fnGrf.fValid()) {
    sGrf = fnGrf.sRelative(false, fn.sPath());
    String s(SIERemUsingGeoRef_s.scVal(), sGrf);
    StaticText* st = new StaticText(root, s);
    st->SetIndependentPos();
  }
  else {
//  sGrf = fn.sFile;
    sGrf = FileName(sOutMap).sRelative(false, fn.sPath());
    new FieldDataTypeCreate(root, SIEUiGrf, &sGrf, ".grf", true);
  }
  sOutMap = FileName(sOutMap).sRelative(false, fn.sPath());
  new FieldMapCreate(root, SAFUiOutRasMap, &sOutMap);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  
  HideOnOk(true);
  create();
}

int Import14MapForm::CallBack(Event*)
{
  rg->StoreData();
  if ((iDom == 0) && (fClass || fId) || 
      (iDom == 1) && (fClass && fId)) {
    fldDom->Show();
    return DomCallBack(0);
  }  
  else { 
    fldDom->Hide();
    EnableOK();
  }  
  return 0;  
}

int Import14Form::DomCallBack(Event*)
{
  fldDom->StoreData();
  String sExt(".dom");
  FileName fn(sDom, sExt);
  if (!File::fExist(fn)) {
    rg->Enable();
    EnableOK();
    return 0;
  }
  rg->Disable();  
  DisableOK();
  try {
    Domain dm(sDom);
    if (0 != dm->pdsrt()) {
      EnableOK();
      if (0 != dm->pdc())
        rg->SetVal(0);
      else  
        rg->SetVal(1);
    }
  }
  catch (...) {}
  return 0;  
}

int Import14MapForm::import()
{
  HourglassCursor cur(this);
//  Import14Form::exec();
  DomainType dmt;
  if (fClass)
    if (0 == iDom--) { dmt = dmtCLASS; goto ready; }
  if (fId)  
    if (0 == iDom--) { dmt = dmtID; goto ready; }
  if (fImage)  
    if (0 == iDom--) { dmt = dmtIMAGE; goto ready; }
  if (fValue)  
    if (0 == iDom--) { dmt = dmtVALUE; goto ready; }
  if (fPicture)  
    if (0 == iDom--) { dmt = dmtPICTURE; goto ready; }
  if (fBit)  
    if (0 == iDom--) { dmt = dmtBIT; goto ready; }
  assert (0 == "impossible domain");  
ready:
  FileName fnDom;
  if (dmt == dmtCLASS || dmt == dmtID) 
    fnDom = sDom;
    
  FileName fnGrf(sGrf);
  FileName fnOut(sOutMap);
  MapImport::import(fnMap, fnOut, dmt, fnDom, fnGrf, sDescr);
  return 0;
}

Import14SegmentMapForm::Import14SegmentMapForm(CWnd* wPar, const FileName& fn, const String& sDef)
: Import14Form(wPar, SIETitleImpSeg14),
  fnMap(fn), sOutMap(sDef)
{
  if ("" == sOutMap)
    sOutMap = fn.sFile;
  DomainType dmt;
  ArrayLarge<String> asCodes;
  Tranquilizer trq;
  trq.SetTitle("Import 1.4");
  trq.Start();
  SegmentMapImport::GetImportInfo(fn, dmt, vr, asCodes, &trq);
  trq.Stop();
  switch (dmt) {
    case dmtCLASS:
      iDom = 0;
      break;
    case dmtID:
      iDom = 1;
      break;
    case dmtVALUE:
      iDom = 2;
      break;  
    default:  
      iDom = 0;
  }
  
  sDescr = String(SIEDscSegMap_s.scVal(), fn.sFile);
  new StaticText(root, sDescr);
  rg = new RadioGroup(root, SIEUiDom, &iDom);
  new RadioButton(rg, SIEUiClass);
  new RadioButton(rg, SIEUiIdentifier);
  new RadioButton(rg, SIEUiValue);
//  sDom = fn.sFile;
  sDom = FileName(sOutMap).sRelative(false, fn.sPath());
  rg->SetCallBack((NotifyProc)&Import14SegmentMapForm::CallBack);
  fldDom = new FieldDataTypeCreate(root, SIEUiDomName, &sDom, ".dom", true);
  fldDom->SetCallBack((NotifyProc)&Import14Form::DomCallBack);
  
  sOutMap = FileName(sOutMap).sRelative(false, fn.sPath());
  new FieldSegmentMapCreate(root, SAFUiOutSegMap, &sOutMap);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  
  HideOnOk(true);
  SetMenHelpTopic(htpImport14SegmentMap);
  create();
}

int Import14SegmentMapForm::CallBack(Event*)
{
  rg->StoreData();
  if ((iDom == 0) || (iDom == 1)) {
    fldDom->Show();
    return DomCallBack(0);
  }  
  else  
    fldDom->Hide();
  return 0;  
}

int Import14SegmentMapForm::exec()
{
  HourglassCursor cur(this);
  Import14Form::exec();
  DomainType dmt;
  switch (iDom) {
    case 0: dmt = dmtCLASS; break;
    case 1: dmt = dmtID;    break;
    case 2: dmt = dmtVALUE; break;
  }  
  FileName fnDom;
  if (dmt == dmtCLASS || dmt == dmtID) 
    fnDom = sDom;
  CoordSystem cs;  
  FileName fnOut(sOutMap);
  SegmentMapImport::import(fnMap, fnOut, dmt, fnDom, vr, cs, sDescr);
  return 0;
}

Import14PolygonMapForm::Import14PolygonMapForm(CWnd* wPar, const FileName& fn, const String& sDef)
: Import14Form(wPar, SIETitleImpPol14),
  fnMap(fn), sOutMap(sDef)
{
  if ("" == sOutMap)
    sOutMap = fn.sFile;
  DomainType dmt;
  Array<String> asCodes;
  PolygonMapImport::GetImportInfo(fn, dmt, vr, asCodes);
  switch (dmt) {
    case dmtCLASS:
      iDom = 0;
      break;
    case dmtID:
      iDom = 1;
      break;
    case dmtVALUE:
      iDom = 2;
      break;  
    default:  
      iDom = 0;
  }
  
  sDescr = String(SIEDscPolMap_s.scVal(), fn.sFile);
  new StaticText(root, sDescr);
  rg = new RadioGroup(root, SIEUiDom, &iDom);
  new RadioButton(rg, SIEUiClass);
  new RadioButton(rg, SIEUiIdentifier);
  new RadioButton(rg, SIEUiValue);
  sDom = FileName(sOutMap).sRelative(false, fn.sPath());
//  sDom = fn.sFile;
  rg->SetCallBack((NotifyProc)&Import14PolygonMapForm::CallBack);
  fldDom = new FieldDataTypeCreate(root, SIEUiDomName, &sDom, ".dom", true);
  fldDom->SetCallBack((NotifyProc)&Import14Form::DomCallBack);
  
  sOutMap = FileName(sOutMap).sRelative(false, fn.sPath());
  new FieldPolygonMapCreate(root, SAFUiOutPolMap, &sOutMap);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();

  HideOnOk(true);
  SetMenHelpTopic(htpImport14PolygonMap);
  create();
}

int Import14PolygonMapForm::CallBack(Event*)
{
  rg->StoreData();
  if ((iDom == 0) || (iDom == 1)) {
    fldDom->Show();
    return DomCallBack(0);
  }  
  else  
    fldDom->Hide();
  return 0;  
}

int Import14PolygonMapForm::exec()
{
  HourglassCursor cur(this);
  Import14Form::exec();
  DomainType dmt;
  switch (iDom) {
    case 0: dmt = dmtCLASS; break;
    case 1: dmt = dmtID;    break;
    case 2: dmt = dmtVALUE; break;
  }  
  FileName fnDom;
  if (dmt == dmtCLASS || dmt == dmtID) 
    fnDom = sDom;
  CoordSystem cs;  
  FileName fnOut(sOutMap);
  PolygonMapImport::import(fnMap, fnOut, dmt, fnDom, vr, cs, sDescr);
  return 0;
}

Import14PointMapForm::Import14PointMapForm(CWnd* wPar, const FileName& fn, const String& sDef)
: Import14Form(wPar, SIETitleImpPnt14),
  fnMap(fn), sOutMap(sDef)
{
  if ("" == sOutMap)
    sOutMap = fn.sFile;
  bool fAttTablePossible;
  PointMapImport::GetImportInfo(fn, fNameCol, fAttTablePossible);
  sDescr = String(SIEDscPntMap_s.scVal(), fn.sFile);
  new StaticText(root, sDescr);
  iDom = fNameCol ? 1 : 0;
  rg = new RadioGroup(root, SIEUiDom, &iDom);
  if (fNameCol)
    new RadioButton(rg, SIEUiClass);
  new RadioButton(rg, SIEUiIdentifier);
  if (fNameCol)
    new RadioButton(rg, SIEUiValue);
//  sDom = fn.sFile;
  sDom = FileName(sOutMap).sRelative(false, fn.sPath());
  if (fNameCol || fAttTablePossible) {
    rg->SetCallBack((NotifyProc)&Import14PointMapForm::CallBack);
    fldDom = new FieldDataTypeCreate(root, SIEUiDomName, &sDom, ".dom", true);
    fldDom->SetCallBack((NotifyProc)&Import14Form::DomCallBack);
  }  
  
  if (fAttTablePossible)
    new FieldString(root, SIEUiAttTable, &sAttTbl);

  sOutMap = FileName(sOutMap).sRelative(false, fn.sPath());
  new FieldPointMapCreate(root, SAFUiOutPntMap, &sOutMap);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();

  HideOnOk(true);
  if (fNameCol)
    SetMenHelpTopic(htpImport14PointMap);
  else  
    SetMenHelpTopic(htpImport14PointMapNoName);
  create();
}

int Import14PointMapForm::CallBack(Event*)
{
  rg->StoreData();
  if ((iDom == 0) || (iDom == 1)) {
    fldDom->Show();
    return DomCallBack(0);
  }  
  else  
    fldDom->Hide();
  return 0;  
}

int Import14PointMapForm::exec()
{
  HourglassCursor cur(this);
  Import14Form::exec();
  DomainType dmt;
  if (fNameCol) {
    switch (iDom) {
      case 0: dmt = dmtCLASS; break;
      case 1: dmt = dmtID;    break;
      case 2: dmt = dmtVALUE; break;
    }  
  }
  else
    dmt = dmtID;  
  FileName fnDom;
  if (dmt == dmtCLASS || dmt == dmtID) 
    fnDom = sDom;
  CoordSystem cs;  
  FileName fnTbl = sAttTbl;
  fnTbl.sExt = ".tbt";
  FileName fnOut(sOutMap);
  PointMapImport::import(fnMap, fnOut, dmt, fnDom, cs, fnTbl, sDescr);
//  PointMapImport::import(fnMap, dmt, fnDom, cs, FileName(), sDescr);
  return 0;
}

Import14TableForm::Import14TableForm(CWnd* wPar, const FileName& fn, const String& sDef)
: Import14Form(wPar, SIETitleImpTbl14),
  fnTable(fn), sOutTbl(sDef)
{
  if ("" == sOutTbl)
    sOutTbl = fn.sFile;
  TableImport::GetImportInfo(fn, fNameCol);
  sDescr = String(SIEDscTable_s.scVal(), fn.sFile);
  new StaticText(root, sDescr);
  iDom = 0;
  rg = new RadioGroup(root, SIEUiDom, &iDom);
  if (fNameCol) {
    new RadioButton(rg, SIEUiClass);
    new RadioButton(rg, SIEUiIdentifier);
    SetMenHelpTopic(htpImport14Table);
  }
  else { 
    new RadioButton(rg, SIEUiNone);
    SetMenHelpTopic(htpImport14TableNoName);
  }
  if (fNameCol) {
//    sDom = fn.sFile;
    sDom = FileName(sOutTbl).sRelative(false, fn.sPath());
    fldDom = new FieldDataTypeCreate(root, SIEUiDomName, &sDom, ".dom", true);
    fldDom->SetCallBack((NotifyProc)&Import14Form::DomCallBack);
  }
  sOutTbl = FileName(sOutTbl).sRelative(false, fn.sPath());
  new FieldTableCreate(root, SAFUiOutTable, &sOutTbl);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();

  HideOnOk(true);
  create();
}

int Import14TableForm::exec()
{
  HourglassCursor cur(this);
  Import14Form::exec();
  DomainType dmt;
  if (fNameCol) {
    switch (iDom) {
      case 0: dmt = dmtCLASS; break;
      case 1: dmt = dmtID;    break;
    }  
  }
  else
    dmt = dmtNONE;
  FileName fnDom;
  if (dmt == dmtCLASS || dmt == dmtID) 
    fnDom = sDom;
  FileName fnOut(sOutTbl);
  TableImport::import(fnTable, fnOut, ".tbt", dmt, fnDom, sDescr);
  return 0;
}




