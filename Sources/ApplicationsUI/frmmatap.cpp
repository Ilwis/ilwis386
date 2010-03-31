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
/* Form Matrix Applications
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    6 Sep 96    6:26 pm
*/
//#include "Client\MainWindow\mainwind.h"

#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fldlist.h"
#include "ApplicationsUI\frmmatap.h"
#include "Client\ilwis.h"

LRESULT Cmdprinccmp(CWnd *wnd, const String& s)
{
	new FormMatrixPrincCmp(wnd, s.scVal());
	return -1;
}

FormMatrixPrincCmp::FormMatrixPrincCmp(CWnd* mw, const char* sPar)
: FormMatrixCreate(mw, SAFTitlePrincCmp)
{
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
      if (fn.sExt == "" || fn.sExt == ".mpl")
        if (sMapList == "") {
          sMapList = fn.sFullNameQuoted(false);
          continue;
        }  
      if (fn.sExt == "" || fn.sExt == ".mat")
        if (sOutMat == "")
          sOutMat = fn.sFullNameQuoted(false);  
    }
  }
  fldMpl = new FieldMapList(root, SAFUiMapList, &sMapList);
  fldMpl->SetCallBack((NotifyProc)&FormMatrixPrincCmp::MapListCallBack);
  fldOutBands = new FieldInt(root, SAFUINrOutputBands, &iOutBands, ValueRange(1,99), true);
  fldOutBands->SetCallBack((NotifyProc)&FormMatrixPrincCmp::OutBandsCallBack);
  initMatrixOut();
  SetAppHelpTopic(htpMatrixPrincCmp);
  create();
}                    

int FormMatrixPrincCmp::exec() 
{
  FormMatrixCreate::exec();
  String sExpr;
  FileName fn(sOutMat);
  FileName fnMapList(sMapList); 
  sMapList = fnMapList.sRelativeQuoted(false,fn.sPath());
  sExpr = String("MatrixPrincComp(%S, %i)", 
                  sMapList, iOutBands);
  execMatrixOut(sExpr);  
  return 0;
}

int FormMatrixPrincCmp::MapListCallBack(Event*)
{
  fldMpl->StoreData();
  try {
    FileName fnMpl(sMapList);
    if (fnMpl.sFile == "")
      return 0;
    mpl = MapList(fnMpl);
    fldOutBands->SetVal(mpl->iSize());
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormMatrixPrincCmp::OutBandsCallBack(Event*)
{
  if (!mpl.fValid())
    return 0;
  fldOutBands->StoreData();
  if (iOutBands <= 0 || iOutBands > mpl->iSize())
    DisableOK();
  else
    EnableOK();
  return 0;
}

LRESULT Cmdfactanal(CWnd *wnd, const String& s)
{
	new FormMatrixFactorAnal(wnd, s.scVal());
	return -1;
}

FormMatrixFactorAnal::FormMatrixFactorAnal(CWnd* mw, const char* sPar)
: FormMatrixCreate(mw, SAFTitleFactorAnal)
{
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
      if (fn.sExt == "" || fn.sExt == ".mpl")
        if (sMapList == "") {
          sMapList = fn.sFullNameQuoted(false);
          continue;
        }  
      if (fn.sExt == "" || fn.sExt == ".mat")
        if (sOutMat == "")
          sOutMat = fn.sFullName(false);  
    }
  }
  fldMpl = new FieldMapList(root, SAFUiMapList, &sMapList);
  fldMpl->SetCallBack((NotifyProc)&FormMatrixFactorAnal::MapListCallBack);
  fldOutBands = new FieldInt(root, SAFUINrOutputBands, &iOutBands, ValueRange(1,99), true);
  fldOutBands->SetCallBack((NotifyProc)&FormMatrixFactorAnal::OutBandsCallBack);
  initMatrixOut();
  SetAppHelpTopic(htpMatrixFactorAnal);
  create();
}                    

int FormMatrixFactorAnal::exec() 
{
  FormMatrixCreate::exec();
  String sExpr;
  FileName fn(sOutMat);
  FileName fnMapList(sMapList); 
  sMapList = fnMapList.sRelativeQuoted(false,fn.sPath());
  sExpr = String("MatrixFactorAnal(%S,%i)", 
                  sMapList, iOutBands);
  execMatrixOut(sExpr);  
  return 0;
}

int FormMatrixFactorAnal::MapListCallBack(Event*)
{
  fldMpl->StoreData();
  try {
    FileName fnMpl(sMapList);
    if (fnMpl.sFile == "")
      return 0;
    mpl = MapList(fnMpl);
    fldOutBands->SetVal(mpl->iSize());
  }
  catch (ErrorObject&) {}
  return 0;
}

int FormMatrixFactorAnal::OutBandsCallBack(Event*)
{
  if (!mpl.fValid())
    return 0;
  fldOutBands->StoreData();
  if (iOutBands <= 0 || iOutBands > mpl->iSize())
    DisableOK();
  else
    EnableOK();
  return 0;
}

LRESULT Cmdmatvarcov(CWnd *wnd, const String& s)
{
	new FormMatrixVarCov(wnd, s.scVal());
	return -1;
}

FormMatrixVarCov::FormMatrixVarCov(CWnd* mw, const char* sPar)
: FormMatrixCreate(mw, SAFTitleVarCovMatrix)
{
	fbs	&= ~fbsAPPLIC;
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
      if (fn.sExt == "" || fn.sExt == ".mpl")
        if (sMapList == "") {
          sMapList = fn.sFullNameQuoted(false);
          continue;
        }  
    }
  }
  new FieldMapList(root, SAFUiMapList, &sMapList);
  fShow = true;
  SetAppHelpTopic(htpMatrixVarCov);
  create();
}                    

int FormMatrixVarCov::exec() 
{
  FormMatrixCreate::exec();
  FileName fnMapList(sMapList); 
  sMapList = fnMapList.sRelativeQuoted(false,IlwWinApp()->sGetCurDir());
  String sExpr;
  sExpr = String("MatrixVarCov(%S)", 
                  sMapList);
  execMatrixOut(sExpr);  
  return 0;
}

LRESULT Cmdmatcorr(CWnd *wnd, const String& s)
{
	new FormMatrixCorr(wnd, s.scVal());
	return -1;
}
FormMatrixCorr::FormMatrixCorr(CWnd* mw, const char* sPar)
: FormMatrixCreate(mw, SAFTitleCorrMatrix)
{
	fbs	&= ~fbsAPPLIC;
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
      if (fn.sExt == "" || fn.sExt == ".mpl")
        if (sMapList == "") {
          sMapList = fn.sFullNameQuoted(false);
          continue;
        }  
    }
  }
  new FieldMapList(root, SAFUiMapList, &sMapList);
  fShow = true;
  SetAppHelpTopic(htpMatrixCorr);
  create();
}                    

int FormMatrixCorr::exec() 
{
  FormMatrixCreate::exec();
  FileName fnMapList(sMapList); 
  sMapList = fnMapList.sRelativeQuoted(false,IlwWinApp()->sGetCurDir());
  String sExpr;
  sExpr = String("MatrixCorr(%S)", 
                  sMapList);
  execMatrixOut(sExpr);  
  return 0;
}






