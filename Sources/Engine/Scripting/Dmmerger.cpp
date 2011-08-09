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
/* $Log: /ILWIS 3.0/Calculator/Dmmerger.cpp $
 * 
 * 3     9-12-99 9:46 Hendrikse
 * Header comment corrected
  
 * 2     2-12-99 12:05 Wind
 * CalcVariable now maintains a list of all InstLoad 's that it is used in
 * (was only one). This information is used in the DomainMerger
// Revision 1.6  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.5  1997/09/18 12:12:07  Wim
// Checks on strings in domain should go to including the last item.
// Also second domain first check if something is to be merged before asking
//
// Revision 1.4  1997-09-18 09:33:10+02  Wim
// Just to be sure also protect empty constructor
//
// Revision 1.3  1997-09-18 09:32:09+02  Wim
// Initialize all pointers on zero
//
// Revision 1.2  1997-09-01 16:09:17+02  Wim
// fAskMergeDomain() will now check if something is to merge
//
/* calcerr.c
   Source for calculator errors
   june 1997, Jelle Wind
	Last change:  WK   10 Aug 98   12:24 pm
*/
#define DMMERGER_C
#include "Engine\Scripting\DMMERGER.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\DAT.hs"

DomainMerger::DomainMerger()
: dmmt(dmmtNONE),
  pcvcs1(0), pdsrt1(0),
  pcvcs2(0), pdsrt2(0)
{
}

DomainMerger::DomainMerger(const CalcVariable& _cv1, const CalcVariable& _cv2)
: dmmt(dmmtNONE),
  pcvcs1(0), pdsrt1(0),
  pcvcs2(0), pdsrt2(0)
{
  cv1 = _cv1;
  cv2 = _cv2;
  if (cv1.fValid()) {
    pcvcs1 = cv1->pcvConstString();
    pdsrt1 = cv1->dvs.dm()->pdsrt();
  }
  if (cv2.fValid()) {
    pcvcs2 = cv2->pcvConstString();
    pdsrt2 = cv2->dvs.dm()->pdsrt();
  }
}

bool DomainMerger::fIgnore()
{
  return (0 == pcvcs1) && (0 == pcvcs2) && (0 == pdsrt1) && (0 == pdsrt2);
}


bool DomainMerger::fAskMergeDomain(DomainSort* pdsrt)
{
//  Domain dmNew(FileName::fnUnique(FileName("new", ".dom", true)), 0, dmtCLASS);
//  DomainSort* pdsrt = dmNew->pdsrt();
  Domain dm;
  dm.SetPointer(pdsrt);
  if (0 != pcvcs1) {
    String s = pcvcs1->sValue();
    if (s != sUNDEF)
      if (iUNDEF == pdsrt->iRaw(s)) {
        int iRet = MessageBox(0, String(TR("Add string '%S' to domain '%S'").c_str(), s, pdsrt->sName()).c_str(),
                     TR("Merging domains").c_str(), MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
        if (iRet == IDCANCEL)
          return false;
        if (iRet == IDYES)
          pdsrt->iAdd(s);
      }
    pcvcs1->SetDomainValueRangeStruct(dm);
  }
  else if (0 != pdsrt1) {
    if (pdsrt != pdsrt1) {
      bool fOk = true;
      for (long i = 1; fOk && i <= pdsrt1->iSize(); ++i) {
        String sClass = pdsrt1->sNameByRaw(i, 0);
        long iRaw = pdsrt->iRaw(sClass);
        if (iRaw == iUNDEF)
          fOk = false;
      }
      if (!fOk) {
        int iRet = MessageBox(0, String(TR("Merge strings of domain '%S' into domain '%S'\n\nWarning: the combination of iff() and merging of domains can lead to incorrect results\nPlease read the help topic 'Map and Table Calculation: Merging domains (workaround)'").c_str(),
                       pdsrt1->sName(), pdsrt->sName()).c_str(),
                       TR("Merging domains").c_str(), MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
        if (iRet == IDCANCEL)
          return false;
        if (iRet == IDYES)
          pdsrt->Merge(pdsrt1);
      }
    }
    if (cv1->aild().iSize() > 0)
      cv1->SetDomainValueRangeStruct(dm);
  }
  if (0 != pcvcs2) {
    String s = pcvcs2->sValue();
    if (s != sUNDEF)
      if (iUNDEF == pdsrt->iRaw(s)) {
        int iRet = MessageBox(0, String(TR("Add string '%S' to domain '%S'").c_str(), s, pdsrt->sName()).c_str(),
                     TR("Merging domains").c_str(), MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
        if (iRet == IDCANCEL)
          return false;
        if (iRet == IDYES)
          pdsrt->iAdd(s);
      }
    pcvcs2->SetDomainValueRangeStruct(dm);
  }
  else if (0 != pdsrt2) {
    if (pdsrt != pdsrt2) {
      bool fOk = true;
      for (long i = 1; fOk && i <= pdsrt2->iSize(); ++i) {
        String sClass = pdsrt2->sNameByRaw(i, 0);
        long iRaw = pdsrt->iRaw(sClass);
        if (iRaw == iUNDEF)
          fOk = false;
      }
      if (fOk)
        return true;
      int iRet = MessageBox(0, String(TR("Merge strings of domain '%S' into domain '%S'\n\nWarning: the combination of iff() and merging of domains can lead to incorrect results\nPlease read the help topic 'Map and Table Calculation: Merging domains (workaround)'").c_str(), pdsrt2->sName(), pdsrt->sName()).c_str(),
                     TR("Merging domains").c_str(), MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
      if (iRet == IDCANCEL)
        return false;
      if (iRet == IDYES)
        pdsrt->Merge(pdsrt2);
    }
    if (cv2->aild().iSize() > 0)
      cv2->SetDomainValueRangeStruct(dm);
  }
  return true;
//  if (pdsrt->iSize() > 0)
//    dm = dmNew;
}

void DomainMerger::ApplyMergeDomain(const Domain& dm)
{
  if (0 != pdsrt1) {
    Array<InstLoad*> aild = cv1->aild();
    if (aild.iSize() > 0) {
      cv1->SetDomainValueRangeStruct(dm);
      for (int i=0; i < aild.iSize(); i++)
       aild[i]->InitDomainConvert();
    }
  }
  if (0 != pdsrt2) {
    Array<InstLoad*> aild = cv2->aild();
    if (aild.iSize() > 0) {
      cv2->SetDomainValueRangeStruct(dm);
      for (int i=0; i < aild.iSize(); i++)
       aild[i]->InitDomainConvert();
    }
  }
}

ArrayDomainMerger::ArrayDomainMerger()
: Array<DomainMerger>()
{
}

ArrayDomainMerger::~ArrayDomainMerger()
{
}




