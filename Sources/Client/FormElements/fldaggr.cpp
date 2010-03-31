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
/* Interface for FieldAggregate
   by Wim Koolhoven, january 1997
   (c) Ilwis System Development ITC
	Last change:  WK   29 May 98    5:13 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldaggr.h"
#include "Headers\Hs\Table.hs"

const int iAGFUNCS = 8;
const int iRGFUNCS = 5;
const int iAGLFUNCS = 5;

class FieldAggrFuncSimple: public FieldOneSelect
{
public:
  FieldAggrFuncSimple(FormEntry* par, String** sFunc, const String& sDefault)
  : FieldOneSelect(par, (long*)sFunc, true), m_sDef(sDefault)
  {
    SetWidth(75);
    int i = 0;
    ag[i].sFun = "Avg";
    ag[i++].sName = STBFunAggAvg;
    ag[i].sFun = "Sum";
    ag[i++].sName = STBFunAggSum;
    ag[i].sFun = "Std";
    ag[i++].sName = STBFunAggStd;
    ag[i].sFun = "Min";
    ag[i++].sName = STBFunAggMin;
    ag[i].sFun = "Max";
    ag[i++].sName = STBFunAggMax;
    ag[i].sFun = "Prd";
    ag[i++].sName = STBFunAggPrd;
    ag[i].sFun = "Med";
    ag[i++].sName = STBFunAggMed;
    ag[i].sFun = "Cnt";
    ag[i++].sName = STBFunAggCnt;
  }
  void create() {
    FieldOneSelect::create();
    for (int i = 0; i < iAGFUNCS; ++i) {
      int id = ose->AddString(ag[i].sName.sVal());
      ose->SetItemDataPtr(id,(void *)&ag[i].sFun);
    }
	int iRet = ose->FindString(0, m_sDef.scVal());
	if (iRet != CB_ERR)
		ose->SetCurSel(iRet);
	else	
		ose->SetCurSel(0);
  }
private:
  struct AggrStruct {
    String sFun;
    String sName;
  };
  AggrStruct ag[iAGFUNCS];
  const String& m_sDef;
};

FieldAggrFunc::FieldAggrFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fafs = new FieldAggrFuncSimple(this, sFunc, sDefault);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

void FieldAggrFunc::SetCallBack(NotifyProc np) 
{ 
  fafs->SetCallBack(np); 
  FormEntry::SetCallBack(np);
}

void FieldAggrFunc::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
  fafs->SetCallBack(np, cb); 
  FormEntry::SetCallBack(np, cb);
}

class FieldRegressionFuncSimple: public FieldOneSelect
{
public:
  FieldRegressionFuncSimple(FormEntry* par, String** sFunc, const String& sDefault)
  : FieldOneSelect(par, (long*)sFunc, true), m_sDef(sDefault)
  {
    SetWidth(75);
    int i = 0;
    rg[i].sFun = "polynomial";
    rg[i++].sName = STBFunRgrPolynomial;
    rg[i].sFun = "trigonometric";
    rg[i++].sName = STBFunRgrTrigonometric;
    rg[i].sFun = "power";
    rg[i++].sName = STBFunRgrPower;
    rg[i].sFun = "exponential";
    rg[i++].sName = STBFunRgrExponential;
    rg[i].sFun = "logarithmic";
    rg[i++].sName = STBFunRgrLogarithmic;
  }
  void create() {
    FieldOneSelect::create();
    for (int i = 0; i < iRGFUNCS; ++i) {
      int id = ose->AddString(rg[i].sName.sVal());
      ose->SetItemDataPtr(id,(void *)&rg[i].sFun);
    }
	int iRet = ose->FindString(0, m_sDef.scVal());
	if (iRet != CB_ERR)
		ose->SetCurSel(iRet);
	else	
		ose->SetCurSel(0);
  }
private:
  struct RegressionStruct {
    String sFun;
    String sName;
  };
  RegressionStruct rg[iRGFUNCS];
  const String& m_sDef;
};

FieldRegressionFunc::FieldRegressionFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  frfs = new FieldRegressionFuncSimple(this, sFunc, sDefault);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

void FieldRegressionFunc::SetCallBack(NotifyProc np) 
{ 
  frfs->SetCallBack(np);
  FormEntry::SetCallBack(np);
}

void FieldRegressionFunc::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
  frfs->SetCallBack(np, cb);
  FormEntry::SetCallBack(np, cb);
}

class FieldAggrLastFuncSimple: public FieldOneSelect
{
public:
  FieldAggrLastFuncSimple(FormEntry* par, String** sFunc, const String& sDefault)
  : FieldOneSelect(par, (long*)sFunc, true), m_sDef(sDefault)
  {
    SetWidth(75);
    int i = 0;
    ag[i].sFun = "Avg";
    ag[i++].sName = STBFunAggAvg;
    ag[i].sFun = "Sum";
    ag[i++].sName = STBFunAggSum;
    ag[i].sFun = "Min";
    ag[i++].sName = STBFunAggMin;
    ag[i].sFun = "Max";
    ag[i++].sName = STBFunAggMax;
    ag[i].sFun = "Last";
    ag[i++].sName = STBFunAggLast;
  }
  void create() {
    FieldOneSelect::create();
    for (int i = 0; i < iAGLFUNCS; ++i) {
      int id = ose->AddString(ag[i].sName.sVal());
      ose->SetItemDataPtr(id,(void *)&ag[i].sFun);
    }
	int iRet = ose->FindString(0, m_sDef.scVal());
	if (iRet != CB_ERR)
		ose->SetCurSel(iRet);
	else	
		ose->SetCurSel(0);
  }
private:
  struct AggrLastStruct {
    String sFun;
    String sName;
  };
  AggrLastStruct ag[iAGLFUNCS];
  const String& m_sDef;
};

FieldAggrLastFunc::FieldAggrLastFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fafs = new FieldAggrLastFuncSimple(this, sFunc, sDefault);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

void FieldAggrLastFunc::SetCallBack(NotifyProc np) 
{
  fafs->SetCallBack(np); 
  FormEntry::SetCallBack(np);
}

void FieldAggrLastFunc::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
  fafs->SetCallBack(np, cb); 
  FormEntry::SetCallBack(np, cb);
}





