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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapfl.cpp $
 * 
 * 12    6-08-02 14:36 Koolhoven
 * no limitation anymore on possible domain of input map
 * 
 * 11    8-08-01 9:48 Koolhoven
 * adaptions to allow start with maplist as first parameter
 * 
 * 10    7-08-01 19:48 Koolhoven
 * MapFilter now also allows a maplist as input
 * 
 * 9     9/03/01 9:52 Willem
 * Added extra check on valid file name before trying to open Domain in
 * DomainCallBack()
 * 
 * 8     8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 7     28/08/00 16:00 Willem
 * FormFilter now initializes the Create Georef form when the create
 * button is clicked
 * 
 * 6     5/23/00 12:45p Wind
 * - accept  raster input maps with any domain in FormMapFilter
 * 
 * 5     11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 4     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 3     24-03-99 11:15 Koolhoven
 * Header comment
 * 
 * 2     3/22/99 9:17a Martin
 * Some dependecies of .h files had changed. WinExec removed from all
// Revision 1.3  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.2  1997/09/10 08:10:33  Wim
// Remove path from filter name
//
/* Form Map Applications Filtering
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    9 Jun 98   10:30 am
*/

#include "Client\Headers\AppFormsPCH.h"
//#include "Client\MainWindow\mainwind.h"
#include "Client\FormElements\fldlist.h"
#include "Engine\Function\FILTER.H"
#include "Client\FormElements\flddat.h"
#include "Client\FormElements\fldfil.h"
#include "ApplicationsUI\frmmapip.h"
#include "Applications\Raster\MAPFILTR.H"

class FieldFilterTypeSimple: public FieldOneSelect
{
public:
  FieldFilterTypeSimple(FormEntry* par, long* iType)
  : FieldOneSelect(par, iType), iTyp(iType)
  {
    SetWidth(60);
    int i = 0;
    sTypes[i++] = TR("Linear.fil");
    sTypes[i++] = TR("Average.fil");
    sTypes[i++] = TR("Majority.fil");
    sTypes[i++] = TR("Rank Order.fil");
    sTypes[i++] = TR("Median.fil");
    sTypes[i++] = TR("Pattern.fil");
    sTypes[i++] = TR("Binary.fil");
    sTypes[i++] = TR("Standard Dev.fil");
  }
  void create() {
    FieldOneSelect::create();
    for (int i = 0; i < 8; ++i)
      ose->AddString(sTypes[i].c_str());
    ose->SetCurSel(*iTyp);
  }
private:
  long* iTyp;
  String sTypes[8];  
};

class FieldFilterType: public FieldGroup
{
public:
  FieldFilterType(FormEntry* parent, const String& sQuestion, long* iType)
  : FieldGroup(parent)
  {
    if (sQuestion.length() != 0)
      new StaticTextSimple(this, sQuestion);
    ffts = new FieldFilterTypeSimple(this, iType);
    if (children.iSize() > 1) // also static text
      children[1]->Align(children[0], AL_AFTER);
  }
  virtual void SetCallBack(NotifyProc np) 
    { ffts->SetCallBack(np); }
private:
  FieldFilterTypeSimple* ffts;
};

LRESULT Cmdfilter(CWnd *wnd, const String& s)
{
	new FormFilterMap(wnd, s.c_str());
	return -1;
}

FormFilterMap::FormFilterMap(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Filtering"))
{
  iType = 0;
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == ".fil") {
        try {
          sFil = fn.sFullNameQuoted(false);
          Filter fil(sFil);
          fn = fil->fnObj;
          String sType;
          ObjectInfo::ReadElement("Filter", "Type", fn, sType);
          if ("FilterLinear" == sType) iType = 0;
          else if ("FilterRankOrder" == sType) iType = 3;
          else if ("FilterMajority" == sType) iType = 2;
          else if ("FilterBinary" == sType) iType = 6;
        }
        catch (ErrorObject&) {}  
      }  
      else if (fn.sExt == "" || fn.sExt == ".mpr" || fn.sExt == ".mpl")
        if (sMap == "")
          sMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldDataType(root, TR("&Raster Map"), &sMap, new MapListerDomainType(0,true,true), true);
  fldMap->SetCallBack((NotifyProc)&FormFilterMap::MapCallBack);
  stMapRemark = new StaticText(root, String('x',50));
  stMapRemark->SetIndependentPos();

  fPreDef = true;
  iRgVal = 0;
  rc.Row = 3;
  rc.Col = 3;
  iRank = 5;
  rThreshold = 25;
  fThreshold = false;
  fUndefOnly = false;
  String sFill('X', 50);
  CheckBox *cbPreDef, *cbThreshold;

  fft = new FieldFilterType(root, TR("Filter &Type"), &iType);
  fft->SetCallBack((NotifyProc)&FormFilterMap::FilTypeCallBack);
  
  fgLin = new FieldGroup(root);
  fgLin->Align(fft, AL_UNDER);
  new FieldBlank(fgLin);
  fldFilLin = new FieldFilterC(fgLin, TR("&Filter Name"), &sFil, filLINEAR);
  fldFilLin->SetCallBack((NotifyProc)&FormFilterMap::FilLinCallBack);
  stFilLin = new StaticText(fgLin, sFill);
  stFilLin->SetIndependentPos();
  
  fgBin = new FieldGroup(root);
  fgBin->Align(fft, AL_UNDER);
  new FieldBlank(fgBin);
  fldFilBin = new FieldFilter(fgBin, TR("&Filter Name"), &sFil, filBINARY);
  fldFilBin->SetCallBack((NotifyProc)&FormFilterMap::FilBinCallBack);
  stFilBin = new StaticText(fgBin, sFill);
  stFilBin->SetIndependentPos();
  
  fgAvg = new FieldGroup(root);
  fgAvg->Align(fft, AL_UNDER);
  new FieldBlank(fgAvg);
  (new FieldRowCol(fgAvg, TR("&Rows, Columns"), &rc))->SetCallBack((NotifyProc)&FormFilterMap::FilAvgCallBack);

  fgMaj = new FieldGroup(root);
  fgMaj->Align(fft, AL_UNDER);
  cbPreDef = new CheckBox(fgMaj, TR("&Predefined"), &fPreDef);
  cbPreDef->SetCallBack((NotifyProc)&FormFilterMap::FilMajPrdCallBack);
  fldFilMaj = new FieldFilter(fgMaj, TR("&Filter Name"), &sFil, filMAJORITY);
  fldFilMaj->SetCallBack((NotifyProc)&FormFilterMap::FilMajCallBack);
  stFilMaj = new StaticText(fgMaj, sFill);
  stFilMaj->SetIndependentPos();
  fgUMaj = new FieldGroup(fgMaj);
  fgUMaj->Align(cbPreDef, AL_UNDER);
  (new FieldRowCol(fgUMaj, TR("&Rows, Columns"), &rc))->SetCallBack((NotifyProc)&FormFilterMap::FilMajCallBack);
  (new CheckBox(fgUMaj, TR("&Undefined"), &fUndefOnly))->SetCallBack((NotifyProc)&FormFilterMap::FilMajCallBack);
  
  fgMed = new FieldGroup(root);
  fgMed->Align(fft, AL_UNDER);
  (new FieldBlank(fgMed))->SetCallBack((NotifyProc)&FormFilterMap::FilMedCallBack);
  (new FieldRowCol(fgMed, TR("&Rows, Columns"), &rc))->SetCallBack((NotifyProc)&FormFilterMap::FilMedCallBack);
  cbThreshold = new CheckBox(fgMed, TR("&Threshold"), &fThreshold);
  cbThreshold->SetCallBack((NotifyProc)&FormFilterMap::FilMedCallBack);
  (new FieldReal(cbThreshold, "", &rThreshold))->SetCallBack((NotifyProc)&FormFilterMap::FilMedCallBack);

  fgRnk = new FieldGroup(root);
  fgRnk->Align(fft, AL_UNDER);
  cbPreDef = new CheckBox(fgRnk, TR("&Predefined"), &fPreDef);
  cbPreDef->SetCallBack((NotifyProc)&FormFilterMap::FilRnkPrdCallBack);
  fldFilRnk = new FieldFilter(fgRnk, TR("&Filter Name"), &sFil, filRANK);
  fldFilRnk->SetCallBack((NotifyProc)&FormFilterMap::FilRnkCallBack);
  stFilRnk = new StaticText(fgRnk, sFill);
  stFilRnk->SetIndependentPos();
  fgURnk = new FieldGroup(fgRnk);
  fgURnk->Align(cbPreDef, AL_UNDER);
  (new FieldRowCol(fgURnk, TR("&Rows, Columns"), &rc))->SetCallBack((NotifyProc)&FormFilterMap::FilRnkCallBack);
  (new FieldInt(fgURnk, TR("&Rank"), &iRank))->SetCallBack((NotifyProc)&FormFilterMap::FilRnkCallBack);
  cbThreshold = new CheckBox(fgURnk, TR("&Threshold"), &fThreshold);
  cbThreshold->SetCallBack((NotifyProc)&FormFilterMap::FilRnkCallBack);
  (new FieldReal(cbThreshold, "", &rThreshold))->SetCallBack((NotifyProc)&FormFilterMap::FilRnkCallBack);
  
  fgPat = new FieldGroup(root);
  fgPat->Align(fft, AL_UNDER);
  new FieldBlank(fgPat);
  (new FieldReal(fgPat, TR("&Threshold"), &rThreshold))->SetCallBack((NotifyProc)&FormFilterMap::FilPatCallBack);

  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbThreshold, AL_UNDER);
  
  initMapOut(false, true);
//  fmc->Align(cbThreshold, AL_UNDER);
  fdc->SetCallBack((NotifyProc)&FormFilterMap::DomainCallBack);
  SetHelpItem("ilwisapp\\filter_dialog_box.htm");
  create();
}                    

int FormFilterMap::FilTypeCallBack(Event*) 
{
  fft->StoreData();
  fgLin->Hide();                  
  fgBin->Hide();                  
  fgAvg->Hide();                  
  fgMaj->Hide();                  
  fgRnk->Hide();                  
  fgMed->Hide();                  
  fgPat->Hide();                  
  
  switch (iType) {
    case 0: fgLin->Show(); fPreDef = true; return 1;
    case 1: fgAvg->Show(); fPreDef = false; return 1;
    case 2: fgMaj->Show(); return FilMajPrdCallBack(0);
    case 3: fgRnk->Show(); return FilRnkPrdCallBack(0);
    case 4: fgMed->Show(); fPreDef = false; return 1;
    case 5: fgPat->Show(); fPreDef = false; return 1;
    case 6: fgBin->Show(); fPreDef = true; return 1;
    case 7: fgAvg->Show(); fPreDef = false; return 1;
  }
  
  return 1;
}

int FormFilterMap::FilLinCallBack(Event*) 
{
  fldFilLin->StoreData();
  if ("" == sFil) {
    stFilLin->SetVal("");
    return 1;
  }  
  try {
    Filter fil(sFil);
    stFilLin->SetVal(fil->sDescription);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
  return CallBack(0);
}

int FormFilterMap::FilBinCallBack(Event*) 
{
  fldFilBin->StoreData();
  if ("" == sFil) {
    stFilBin->SetVal("");
    return 1;
  }  
  try {
    Filter fil(sFil);
    stFilBin->SetVal(fil->sDescription);
  }
  catch (ErrorObject& err) {
    err.Show();
  }
  return CallBack(0);
}

int FormFilterMap::FilAvgCallBack(Event*) 
{
  fgAvg->StoreData();
  if ((rc.Row % 2) && rc.Col % 2) {
    if (iType == 1)
      sFil = String("Average(%li,%li)", rc.Row, rc.Col);
    else if (iType == 7)
      sFil = String("FilterStandardDev(%li,%li)", rc.Row, rc.Col);
  }      
  else
    sFil = "";  
  return CallBack(0);
}

int FormFilterMap::FilMajPrdCallBack(Event*) 
{
  fgMaj->StoreData();
  if (fPreDef) {
    fldFilMaj->Show();
    stFilMaj->Show();
    fgUMaj->Hide();
  }
  else {  
    fldFilMaj->Hide();
    stFilMaj->Hide();
    fgUMaj->Show();
  }  
  return FilMajCallBack(0);
}

int FormFilterMap::FilMajCallBack(Event*) 
{
  fgMaj->StoreData();
  if (fPreDef) {
    if ("" == sFil) {
      stFilMaj->SetVal("");
      return 1;
    }  
    try {
      Filter fil(sFil);
      stFilMaj->SetVal(fil->sDescription);
    }
    catch (ErrorObject& err) {
      err.Show();
    }
  }
  else {
    if ((rc.Row % 2) && rc.Col % 2)
      if (fUndefOnly)  
        sFil = String("UndefMajority(%li,%li)", rc.Row, rc.Col);
      else  
        sFil = String("Majority(%li,%li)", rc.Row, rc.Col);
    else
      sFil = "";    
  }  
  return CallBack(0);
}

int FormFilterMap::FilRnkPrdCallBack(Event*) 
{
  fgRnk->StoreData();
  if (fPreDef) {
    fldFilRnk->Show();
    stFilRnk->Show();
    fgURnk->Hide();
  }
  else {
    fldFilRnk->Hide();
    stFilRnk->Hide();
    fgURnk->Show();
  }
  return FilRnkCallBack(0);  
}    
    
int FormFilterMap::FilRnkCallBack(Event*) 
{
//  fldFilRnk->StoreData();
  fgRnk->StoreData();
  if (fPreDef) {
    if ("" == sFil) {
      stFilRnk->SetVal("");
      return 1;
    }  
    try {
      Filter fil(sFil);
      stFilRnk->SetVal(fil->sDescription);
    }
    catch (ErrorObject& err) {
      err.Show();
    }
  }
  else {
    if ((rc.Row % 2) && rc.Col % 2)
      if (fThreshold)  
        sFil = String("RankOrder(%li,%li,%i,%f)", rc.Row, rc.Col, iRank, rThreshold);
      else  
        sFil = String("RankOrder(%li,%li,%i)", rc.Row, rc.Col, iRank);
    else
      sFil = "";    
  }
  return CallBack(0);
}

int FormFilterMap::FilMedCallBack(Event*) 
{
  fgMed->StoreData();
  if ((rc.Row % 2) && rc.Col % 2)
    if (fThreshold)  
      sFil = String("Median(%li,%li,%f)", rc.Row, rc.Col, rThreshold);
    else  
      sFil = String("Median(%li,%li)", rc.Row, rc.Col);
  else
    sFil = "";    
  return CallBack(0);
}

int FormFilterMap::FilPatCallBack(Event*) 
{
  fgPat->StoreData();
  sFil = String("Pattern(%f)", rThreshold);
  return CallBack(0);
}

int FormFilterMap::exec() 
{
  FormMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  fOutMapList = ".mpl" == fnMap.sExt;
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sMapList;
  if (fOutMapList) {
    sMapList = sMap;
    sMap = "##";
  }
  FileName fnFil(sFil);
  if (fnFil.sExt == ".fil") // not inline ;-)
    sFil = fnFil.sRelativeQuoted(true,fn.sPath());

  sExpr = String("MapFilter(%S,%S,%S)", sMap, sFil, sDomain);
  if (fOutMapList)
    sExpr = String("MapListApplic(%S,%S)", sMapList, sExpr);
  execMapOut(sExpr);  
  return 0;
}

int FormFilterMap::MapCallBack(Event*) 
{
  fldMap->StoreData();
  try {
    FileName fnMap(sMap);
    if (fnMap.sFile == "") {
      if (stMapRemark)
        stMapRemark->SetVal("");
      return 0;
    }  
    if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
    {
      MapList mpl(fnMap);
      mp = mpl->map(mpl->iLower());
    }
    else 
      mp = Map(fnMap);
	  if (fgr) 
      fgr->SetBounds(mp->cs(), mp->cb());

    Domain dm = mp->dm();
    DomainValueRangeStruct dvs = mp->dvrs();
    RangeReal rr = mp->rrMinMax();
    if (dm->pdvi() || dm->pdvr() || dm->pdi()) {
      if (rr.fValid()) {
        String sRemark(TR("Minimum: %S  Maximum: %S").c_str(),
                 dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
        stMapRemark->SetVal(sRemark);
      }
      else {
        RangeReal rr = dvs.rrMinMax();
        String sRemark(TR("Ranges from %S to %S").c_str(), 
                 dvs.sValue(rr.rLo()), dvs.sValue(rr.rHi()));
        stMapRemark->SetVal(sRemark);
      } 
      if (fvr)  
        fvr->Show();
    }
    else {
      if (fvr)  
        fvr->Hide();
      String sRemark(dm->sTypeName());
      stMapRemark->SetVal(sRemark);
    }  
  }
  catch (ErrorObject&) {
    if (stMapRemark)
      stMapRemark->SetVal("");
  }
  return CallBack(0);
}

void FormFilterMap::EnableOutFields()
{
  fmc->enable();
  fdc->enable();
}

void FormFilterMap::DisableOutFields()
{
  DisableOK();
  fmc->disable();
  fdc->DisableUnset();
  FormMapCreate::DomainCallBack(0);
}

int FormFilterMap::CallBack(Event*) 
{
  try {
    if (!mp.fValid()) {
      FileName fnMap(sMap);
      if (fnMap.sFile == "") {
        DisableOutFields();
        return 0;
      }  
      if (".mpl" == fnMap.sExt && "" == fnMap.sSectionPostFix)
      {
        MapList mpl(fnMap);
        mp = mpl->map(mpl->iLower());
      }
      else 
        mp = Map(fnMap);
    }  
    Domain dmMap = mp->dm();
    Domain dm;
    if (sFil != "") {
      flt = Filter(sFil);
      dm = MapFilter::dmDefault(mp, flt);
    }  
    if (dm.fValid()) {
      String s = dm->sName();
      fdc->SetVal(s);
      EnableOutFields();
      DomainCallBack(0);
    }
    else {
      DisableOutFields();
    }  
  }
  catch (ErrorObject&) {
    DisableOutFields();
  }
  return 0;
}

int FormFilterMap::DomainCallBack(Event* Evt) 
{
	FormMapCreate::DomainCallBack(Evt);
	fldMap->StoreData();
	try
	{
		if (!mp.fValid())
		{
			FileName fnMap(sMap);
			if (fnMap.sFile == "")
				return 0;

			mp = Map(fnMap);
		}  
		FileName fn(sDomain);
		if (!fn.fValid())
		{
			DisableOK();
			return 0;
		}
		Domain dm(fn); 
		if (!flt.fValid())
		{
			if (sFil == "")
			{
				DisableOK();
				return 0;
			}  
			flt = Filter(sFil);
		}  
		if (dm->pdv()) {
			ValueRange vr = MapFilter::vrDefault(mp, flt, dm);
			SetDefaultValueRange(vr);
		}
		EnableOK();
	}
	catch (ErrorObject&)
	{ 
		DisableOK();
	}  
	return 0;
}







