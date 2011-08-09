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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapcl.cpp $
 * 
 * 9     16-06-05 13:45 Retsios
 * Added new application maplistcalc
 * 
 * 8     18/09/00 17:34 Willem
 * CheckData now returns the FieldMapCalc pointer when the expression is
 * not correct: this returns the focus to the expression
 * 
 * 7     13-07-00 10:10a Martin
 * changed path to relative
 * 
 * 6     14-01-00 17:27 Koolhoven
 * Include extension (and thus icon) in input map name
 * 
 * 5     11-10-99 11:35a Martin
 * pure virtual call prevented
 * 
 * 4     3/22/99 9:17a Martin
 * Some dependecies of .h files had changed. WinExec removed from all
 * 
 * 3     5-03-99 12:16 Koolhoven
 * Corrected header comments
 * 
 * 2     3/04/99 10:20a Martin
 * Proted to Win32; missed it in the initial port
// Revision 1.4  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.3  1997/09/08 13:30:36  Wim
// Start map of mapiter limited to reasonable types
//
// Revision 1.2  1997-07-30 10:47:03+02  Wim
// Propagation is default
//
/* Form Map Calculation
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    8 Sep 97    3:28 pm
*/
#include "Client\Headers\AppFormsPCH.h"
//#include "Client\MainWindow\mainwind.h"
#include "Client\FormElements\fldcs.h"
#include "ApplicationsUI\frmmapcl.h"
#include "Client\FormElements\fldlist.h" // for FieldMapList
#include "Headers\Err\Ilwisapp.err"

//////////////////////////////////////////////////////////////
// FieldMapCalc
//////////////////////////////////////////////////////////////

class FieldMapCalc: public FieldStringMulti
{
public:
  FieldMapCalc(FormEntry* parent, String *psVal)
  : FieldStringMulti(parent, psVal) {}
  FormEntry* CheckData();
};
  
FormEntry* FieldMapCalc::CheckData() 
{ 
	try
	{
		String sExpr = sVal();                                 
		if (sExpr == "")
			return this;
		Map mp(FileName(String("$tmcalc"), ".mpr", true), sExpr);
		if (mp.fValid())
			mp->fErase = true;
		else
			return this;
	}
	catch (const ErrorObject& err)
	{
		MessageBeep(MB_ICONASTERISK);
		err.Show();
		return this;
	}
	return 0;
};           

//////////////////////////////////////////////////////////////
// FormMapCalc
//////////////////////////////////////////////////////////////

LRESULT Cmdmapcalc(CWnd *wnd, const String& s)
{
	new FormMapCalc(wnd, s.c_str());
	return -1;
}

FormMapCalc::FormMapCalc(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Map Calculation"))
{
  if (sPar) {
		TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sOutMap == "")
          sOutMap = fn.sFullName(false);
    }
  }
  new StaticText(root, TR("&Expression:"));
//  fsExpr = new FieldStringMulti(root, &sExpr);
  fsExpr = new FieldMapCalc(root, &sExpr);
  fsExpr->SetWidth(240);
  fsExpr->SetHeight(100);
  fsExpr->SetIndependentPos();
  initMapOut(false, true);
  PushButton* pb = new PushButton(root, TR("&Defaults"), 
                   (NotifyProc)&FormMapCalc::SetDefaults);
  pb->SetIndependentPos();
  pb->Align(fdc, AL_AFTER);
  SetHelpItem("ilwisapp\\map_calculation_dialog_box.htm");
  create();
}                    

   
int FormMapCalc::exec() 
{
  FormMapCreate::exec();
  for(unsigned int i=0; i< sExpr.size(); ++i)
    if (sExpr[i] == '\n' || sExpr[i] == '\r')
      sExpr[i] = ' ';
  execMapOut(sExpr);  
  return 0;
}

int FormMapCalc::SetDefaults(Event*)
{
  fsExpr->StoreData();
  if ("" == sExpr) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }  
  try {                                                          
    Map mp(FileName(String("$tm%u", this->m_hWnd), ".mpr", true), sExpr);
    if (!mp.fValid())
      return 0;
    mp->fErase = true;
    fdc->SetVal(mp->dm()->sName());
    DomainCallBack(0);
    SetDefaultValueRange(mp->vr());
    if (mp->fRealValues()) {
      ValueRangeReal* vrr = mp->dvrs().vr()->vrr();
      if (0 != vrr)
        vrr->AdjustRangeToStep();
    }
  }
  catch (const ErrorObject& err) {
    MessageBeep(MB_ICONASTERISK);
    err.Show();
  }
  return 0;
}

//////////////////////////////////////////////////////////////
// FormMapIter
//////////////////////////////////////////////////////////////

LRESULT Cmdmapiter(CWnd *wnd, const String& s)
{
	new FormMapIter(wnd, s.c_str());
	return -1;
}

FormMapIter::FormMapIter(CWnd* mw, const char* sPar)
: FormMapCreate(mw, TR("Map Iteration"))
{
  fPropagation = true;
  iStopCriterium = 0;
  iNrIterations = 1;
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
      if (fn.sExt == "" || fn.sExt == ".mpr")
        if (sInitMap == "")
          sInitMap = fn.sRelative(true);
        else if (sOutMap == "")
          sOutMap = fn.sRelative(false);
    }
  }
  new FieldDataType(root, TR("&Start Map"), &sInitMap,
                    new MapListerDomainType(dmBOOL|dmVALUE|dmCLASS|dmIDENT),true);
//  new FieldMap(root, TR("&Start Map"), &sInitMap);
  new StaticText(root, TR("&Expression:"));
//  fsExpr = new FieldStringMulti(root, &sExpr);
  fsExpr = new FieldMapCalc(root, &sExpr);
  fsExpr->SetWidth(240);
  fsExpr->SetHeight(100);
  fsExpr->SetIndependentPos();

  RadioGroup* rg = new RadioGroup(root, TR("&Stop Criterium"), &iStopCriterium);
  new RadioButton(rg, TR("Until &No changes"));
  RadioButton* rb = new RadioButton(rg, TR("Number of &Iterations"));
  new FieldInt(rb, "", &iNrIterations, ValueRange(1,100000L));

  CheckBox* cbProp = new CheckBox(root, TR("&Propagation"), &fPropagation);
  cbProp->Align(rg, AL_UNDER);

  initMapOut(false, true);
  PushButton* pb = new PushButton(root, TR("&Defaults"), 
                   (NotifyProc)&FormMapIter::SetDefaults);
  pb->SetIndependentPos();
  pb->Align(fdc, AL_AFTER);
  SetHelpItem("ilwisapp\\iteration_dialog_box.htm");
  create();
}                    
   
int FormMapIter::exec()
{
  FormMapCreate::exec();
  for(unsigned int i=0; i< sExpr.size(); ++i)
    if (sExpr[i] == '\n' || sExpr[i] == '\r')
      sExpr[i] = ' ';
  String s1, s2;
	FileName fn(sInitMap);
	sInitMap = fn.sRelative();
  if (fPropagation)
    s1 = "Prop";
  if (iStopCriterium == 1)
    s2 = String(",%i", iNrIterations);
  String str("MapIter%S(%S,%S%S)", s1, sInitMap, sExpr, s2);
  execMapOut(str);
  return 0;
}

int FormMapIter::SetDefaults(Event*)
{
  fsExpr->StoreData();
  if ("" == sExpr) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }  
  try {                                                          
    Map mp(FileName(String("$tm%u", m_hWnd), ".mpr", true), sExpr);
    if (!mp.fValid())
      return 0;
    mp->fErase = true;
    fdc->SetVal(mp->dm()->sName());
    DomainCallBack(0);
    SetDefaultValueRange(mp->vr());
    if (mp->fRealValues()) {
      ValueRangeReal* vrr = mp->dvrs().vr()->vrr();
      if (0 != vrr)
        vrr->AdjustRangeToStep();
    }
  }
  catch (const ErrorObject& err) {
    MessageBeep(MB_ICONASTERISK);
    err.Show();
  }
  return 0;
}

//////////////////////////////////////////////////////////////
// FieldMapListCalc
//////////////////////////////////////////////////////////////

class FieldMapListCalc: public FieldStringMulti
{
public:
  FieldMapListCalc(FormEntry* parent, String *psVal, FormMapListCalc* fmlc)
  : m_fmlc(fmlc)
	, FieldStringMulti(parent, psVal) {}
  FormEntry* CheckData();
private:
	FormMapListCalc* m_fmlc;
};
  
FormEntry* FieldMapListCalc::CheckData() 
{ 
	try
	{
		String sExpr = sVal();                                 
		if (sExpr == "")
			return this;
		int iCountMapLists = 0;
		for (int i = 0; i < sExpr.length(); ++i)
		{
			if (sExpr[i] == '@') // digits should follow
			{
				int j = i+1;
				while ((j < sExpr.length()) && isdigit(sExpr[j]))
					++j;
				if (j > i+1) // something found
				{
					String sNum (sExpr.substr(i+1, j-i-1));
					int iNum = atoi(sNum.c_str());
					if (iNum > iCountMapLists)
						iCountMapLists = iNum;
				}
			}
		}
		if (m_fmlc->iNrMpl() != iCountMapLists)
		{
			ErrorObject err (WhatError("The number of MapLists in the expression does not match the number of MapLists provided.",errMapListCalculate), WhereError());
			err.Show();
			return this;
		}
		else
			return 0;
	}
	catch (const ErrorObject& err)
	{
		MessageBeep(MB_ICONASTERISK);
		err.Show();
		return this;
	}
	return 0;
};           

//////////////////////////////////////////////////////////////
// FormMapListCalc
//////////////////////////////////////////////////////////////

LRESULT Cmdmaplistcalc(CWnd *wnd, const String& s)
{
	new FormMapListCalc(wnd, s.c_str());
	return -1;
}

FormMapListCalc::FormMapListCalc(CWnd* mw, const char* sPar)
: FormMapCreate(mw, "MapList Calculation")
, m_iStart(iUNDEF)
, m_iEnd(iUNDEF)
, iMAX(4)
{
  if (sPar) {
		TextInput ip(sPar);
    TokenizerBase tokenizer(&ip);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpl") {
        if (sOutMap == "")
          sOutMap = fn.sFullName(false);
			}
      else if (m_iStart == iUNDEF) {
          int iVal = sVal.iVal();
          if (iVal != iUNDEF)
              m_iStart = iVal;
      }  
      else if (m_iEnd == iUNDEF) {
          int iVal = sVal.iVal();
          if (iVal != iUNDEF)
              m_iEnd = iVal;
      }  
    }
  }

	m_iNrMpl = iMAX;

	m_vfml.resize(iMAX);
	m_sml.resize(iMAX); // this is so that the thing "remembers" what the user selected while he resizes the list

  new StaticText(root, TR("&Expression:"));
  fsExpr = new FieldMapListCalc(root, &sExpr, this);
  fsExpr->SetWidth(240);
  fsExpr->SetHeight(100);
  fsExpr->SetIndependentPos();
  StaticText *st = new StaticText(root, "Start Band");
  st->Align(fsExpr, AL_UNDER);
  m_fieldMinBand = new FieldInt(root, "", &m_iStart, ValueRange(1,60), true, true);
  m_fieldMinBand->Align(st, AL_AFTER);
  m_fieldMinBand->SetIndependentPos();
  m_fieldMaxBand = new FieldInt(root, "End Band", &m_iEnd, ValueRange(1,60), true, true);
  m_fieldMaxBand->Align(m_fieldMinBand, AL_AFTER);
  m_fieldMaxBand->SetIndependentPos();

	m_fiNrMpl = new FieldInt(root, "Input MapLists", &m_iNrMpl, ValueRange(1, iMAX), true);
	m_fiNrMpl->Align(st, AL_UNDER);
	m_fiNrMpl->SetCallBack((NotifyProc)&FormMapListCalc::MapListNrChangeCallback);
	
	for (int i=0; i<iMAX; ++i)
	{
		m_vfml[i] = new FieldMapList(root, String("MapList @%d", i+1), &(m_sml)[i], true);
		if (i > 0)
			m_vfml[i]->Align(m_vfml[i-1], AL_UNDER);
		else
		{
			m_vfml[i]->Align(m_fiNrMpl, AL_UNDER);
			m_vfml[i]->SetCallBack((NotifyProc)&FormMapListCalc::MapListChangeCallback);
		}
	}

	fOutMapList = true; // not Output Raster Map but Output MapList
  initMapOut(false, false); // fAskGeoref, fAskDomain
	fmc->Align(m_vfml[iMAX-1], AL_UNDER);

  create();
}                    

int FormMapListCalc::MapListNrChangeCallback(Event*)
{
		m_fiNrMpl->StoreData();

		for (int i=0; i<m_iNrMpl; ++i)
			m_vfml[i]->Show();
		for (int i=m_iNrMpl; i<iMAX; ++i)
			m_vfml[i]->Hide();

		return 0;
}

int FormMapListCalc::MapListChangeCallback(Event*)
{
	String sMapListPrev = m_sml[0]; // opening a big maplist takes time .. avoid it when possible
	m_vfml[0]->StoreData();
	bool fNewMapList = (m_sml[0] != sMapListPrev);

	if (fNewMapList) // there is some change
	{
		try
		{
				if (m_sml[0].length() > 0)
				{
						MapList mpl(m_sml[0]);
						m_iMinBand = mpl->iLower();
						m_iMaxBand = mpl->iUpper();
						m_iOffset = m_iMinBand - 1; // check implementation of mpl->iLower() .. some improvement may be needed there

						m_iMinBand -= m_iOffset;
						m_iMaxBand -= m_iOffset;

						m_fieldMinBand->SetvalueRange(ValueRange(m_iMinBand, m_iMaxBand));
						m_fieldMaxBand->SetvalueRange(ValueRange(m_iMinBand, m_iMaxBand));
						m_fieldMinBand->SetVal(m_iMinBand);
						m_fieldMaxBand->SetVal(m_iMaxBand);
				}
		}
		catch (...)
		{
				// do nothing
		}
	}

	return 0;
}
   
int FormMapListCalc::exec() 
{
  FormMapCreate::exec(); // sets fOutMapList = false
	fOutMapList = true;
  for(unsigned int i=0; i< sExpr.size(); ++i)
    if (sExpr[i] == '\n' || sExpr[i] == '\r')
      sExpr[i] = ' ';

	String sMapListExpr ("maplistcalculate(\"%S\",%d,%d", sExpr, m_iStart + m_iOffset, m_iEnd + m_iOffset);
	for (int i = 0; i < m_iNrMpl; ++i)
		sMapListExpr += String (",%S", FileName(m_sml[i]).sRelativeQuoted());
	sMapListExpr += ")";
  execMapOut(sMapListExpr);
  return 0;
}

int FormMapListCalc::iNrMpl()
{
	return m_fiNrMpl->iVal();
}
