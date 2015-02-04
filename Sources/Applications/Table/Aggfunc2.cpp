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
/* AggregateFunctions
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  JEL   2 Dec 96   10:47 pm
*/

#define AGGFUNC2_C
#include "Applications\Table\AGGFUNCS.H"
#include "Engine\Base\Algorithm\Qsort.h"


static int iHash(const HashAggSPrdMed& haspm) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(haspm.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower((unsigned char)*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggPrdMed& hapm) {
  word *pw = static_cast<word*>((void*)&hapm.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggSCrdPrdMed& hascpm) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hascpm.sVal.c_str());
  while (*ps)
    h = (h + (i++) * tolower((unsigned char)*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggCrdPrdMed& hacpm) {
  word *pw = static_cast<word*>((void*)&hacpm.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

void AggregatePrd::reset()
{
  htpm.Resize(1);
}
void AggregatePrd::reset(bool fRaw, bool fString)
{ 
  if (fString) {
    htspm.Resize(16000);
    htscpm.Resize(16000);
  } else {
    htpm.Resize(16000);
    htcpm.Resize(16000);
  }
}

void AggregatePrd::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggPrdMed hapm(rInd);
  HashAggPrdMed& hapmHash = htpm.get(hapm);
  if (hapmHash.fEmpty()) { // not found
    hapm.arVal &= rVal;
    htpm.add(hapm);
  } 
  else
    hapmHash.arVal &= rVal;
}

void AggregatePrd::AddVal(double rInd, long iRaw) {
  AddVal(rInd, doubleConv(iRaw));
}  

void AggregatePrd::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSPrdMed haspm(sInd);
  HashAggSPrdMed& haspmHash = htspm.get(haspm);
  if (haspmHash.fEmpty()) { // not found
    haspm.arVal &= rVal;
    htspm.add(haspm);
  } 
  else
    haspmHash.arVal &= rVal;
}

void AggregatePrd::AddVal(double rInd, const Coord & crd) {
  if (crd == crdUNDEF)
    return;
  HashAggCrdPrdMed hacpm(rInd);
  HashAggCrdPrdMed& hacpmHash = htcpm.get(hacpm);
  if (hacpmHash.fEmpty()) { // not found
    hacpm.arCrd &= crd;
    htcpm.add(hacpm);
  } 
  else
    hacpmHash.arCrd &= crd;
}

void AggregatePrd::AddVal(const String& sInd, const Coord & crd) {
  if (crd == crdUNDEF)
    return;
  HashAggSCrdPrdMed hascpm(sInd);
  HashAggSCrdPrdMed& hascpmHash = htscpm.get(hascpm);
  if (hascpmHash.fEmpty()) { // not found
    hascpm.arCrd &= crd;
    htscpm.add(hascpm);
  } 
  else
    hascpmHash.arCrd &= crd;
}

void AggregatePrd::AddRaw(long iInd, const Coord & crd) {
  AddVal(doubleConv(iInd), crd);
}

void AggregatePrd::AddVal(const String& sInd, long iRaw) {
  AddVal(sInd, doubleConv(iRaw));
}  

void AggregatePrd::AddRaw(long iInd, double rVal) {
  AddVal(doubleConv(iInd), rVal);
}  

void AggregatePrd::AddRaw(long iInd, long iRaw) {
  AddVal(doubleConv(iInd), doubleConv(iRaw));
}  

void AggregatePrd::AddVal(double rInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  rWeight = fabs(rWeight);
  HashAggPrdMed hapm(rInd);
  HashAggPrdMed& hapmHash = htpm.get(hapm);
  if (hapmHash.fEmpty()) { // not found
    hapm.arVal &= rVal;
    hapm.arWeight &= rWeight;
    htpm.add(hapm);
  } 
  else {
    hapmHash.arVal &= rVal;
    hapmHash.arWeight &= rWeight;
  } 
}

void AggregatePrd::AddVal(const String& sInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  rWeight = fabs(rWeight);
  HashAggSPrdMed haspm(sInd);
  HashAggSPrdMed& haspmHash = htspm.get(haspm);
  if (haspmHash.fEmpty()) { // not found
    haspm.arVal &= rVal;
    haspm.arWeight &= rWeight;
    htspm.add(haspm);
  } 
  else {
    haspmHash.arVal &= rVal;
    haspmHash.arWeight &= rWeight;
  } 
}

void AggregatePrd::AddRaw(long iInd, double rVal, double rWeight) {
  AddVal(doubleConv(iInd), rVal, rWeight);
}  
    
double AggregatePrd::rValResult(long iInd)
{ HashAggPrdMed hapm(doubleConv(iInd));
  HashAggPrdMed& hapmHash = htpm.get(hapm);
  if (hapmHash.rRes == rUNDEF)
    hapmHash.rRes = rValResult(hapmHash.arVal, hapmHash.arWeight);
  return hapmHash.rRes;
}  

long AggregatePrd::iRawResult(long iInd)
{ return longConv(rValResult(iInd)); }

double AggregatePrd::rValResult(double rInd)
{ HashAggPrdMed hapm(rInd);
  HashAggPrdMed& hapmHash = htpm.get(hapm);
  if (hapmHash.rRes == rUNDEF)
    hapmHash.rRes = rValResult(hapmHash.arVal, hapmHash.arWeight);
  return hapmHash.rRes;
}  

double AggregatePrd::rValResult(const String& sInd)
{ HashAggSPrdMed haspm(sInd);
  HashAggSPrdMed& haspmHash = htspm.get(haspm);
  if (haspmHash.rRes == rUNDEF)
    haspmHash.rRes = rValResult(haspmHash.arVal, haspmHash.arWeight);
  return haspmHash.rRes;
}  

long AggregatePrd::iRawResult(double rInd)
{ return longConv(rValResult(rInd)); }

long AggregatePrd::iRawResult(const String& sInd)
{ return longConv(rValResult(sInd)); }

Coord AggregatePrd::crdResult(long iInd)
{ HashAggCrdPrdMed hacpm(doubleConv(iInd));
  HashAggCrdPrdMed& hacpmHash = htcpm.get(hacpm);
  if (hacpmHash.crdRes == crdUNDEF)
    hacpmHash.crdRes = crdResult(hacpmHash.arCrd, hacpmHash.arWeight);
  return hacpmHash.crdRes;
}  

Coord AggregatePrd::crdResult(double rInd)
{ HashAggCrdPrdMed hacpm(rInd);
  HashAggCrdPrdMed& hacpmHash = htcpm.get(hacpm);
  if (hacpmHash.crdRes == crdUNDEF)
    hacpmHash.crdRes = crdResult(hacpmHash.arCrd, hacpmHash.arWeight);
  return hacpmHash.crdRes;
}  

Coord AggregatePrd::crdResult(const String& sInd)
{ HashAggSCrdPrdMed hascpm(sInd);
  HashAggSCrdPrdMed& hascpmHash = htscpm.get(hascpm);
  if (hascpmHash.crdRes == crdUNDEF)
    hascpmHash.crdRes = crdResult(hascpmHash.arCrd, hascpmHash.arWeight);
  return hascpmHash.crdRes;
}  

struct RawArrayWeightArray
{
  RawArrayWeightArray(LongArray* aRaw, RealArray* aW)
  { aiRaw = aRaw; arWeight = aW; }
  LongArray* aiRaw;
  RealArray* arWeight;
};

struct ValArrayWeightArray
{
  ValArrayWeightArray(RealArray* aVal, RealArray* aW)
  { arVal = aVal; arWeight = aW; }
  RealArray* arVal;
  RealArray* arWeight;
};

static bool fLessVal(long i, long j, void* p)
{
  RealArray* ar = static_cast<ValArrayWeightArray*>(p)->arVal;
  double ri = (*ar)[i];
  double rj = (*ar)[j];
  return ri < rj;
//  return (*ar)[i] < (*ar)[j];
}

static void SwapVal(long i, long j, void* p)
{
  RealArray* arVal = static_cast<ValArrayWeightArray*>(p)->arVal;
  RealArray* arWeight = static_cast<ValArrayWeightArray*>(p)->arWeight;
//  double ri = (*arVal)[i];
//  double rj = (*arVal)[j];
  double r = (*arVal)[i]; (*arVal)[i] = (*arVal)[j]; (*arVal)[j] = r;
  if (0 != arWeight) {
    r = (*arWeight)[i]; (*arWeight)[i] = (*arWeight)[j]; (*arWeight)[j] = r;
  }  
}

struct CrdArrayWeightArray
{
  CrdArrayWeightArray(Array<Coord>* aCrd, RealArray* aW)
  { arCrd = aCrd; arWeight = aW; }
  Array<Coord>* arCrd;
  RealArray* arWeight;
};

static bool fLessCrd(long i, long j, void* p)
{
  Array<Coord>* ar = static_cast<CrdArrayWeightArray*>(p)->arCrd;
  Coord crdi = (*ar)[i];
  Coord crdj = (*ar)[j];
  return (crdi.y < crdj.y) || ((crdi.y == crdj.y) && (crdi.x < crdj.x));
//  return (*ar)[i] < (*ar)[j];
}

static void SwapCrd(long i, long j, void* p)
{
  Array<Coord>* arCrd = static_cast<CrdArrayWeightArray*>(p)->arCrd;
  RealArray* arWeight = static_cast<CrdArrayWeightArray*>(p)->arWeight;
//  double ri = (*arVal)[i];
//  double rj = (*arVal)[j];
  Coord crd = (*arCrd)[i]; (*arCrd)[i] = (*arCrd)[j]; (*arCrd)[j] = crd;
  if (0 != arWeight) {
    double r = (*arWeight)[i]; (*arWeight)[i] = (*arWeight)[j]; (*arWeight)[j] = r;
  }  
}

String AggregatePrd::sName(bool fJoin) const
{
  if (fJoin) {
    if (fUnique)
      return "join2ndkey";
    return String("join%S", _sName.sRight(_sName.length()-3));
  }  
  return _sName;
}

double AggregatePrd::rValResult(RealArray& arVal, RealArray& arWeight)
{
  if (arVal.iSize() == 0)
    return rUNDEF;
  if (fUnique){
    if (arVal.iSize() > 1)
      return rUNDEF;
    return arVal[0];
  }
  if (_fUseWeight)
    QuickSort(0, arVal.iSize()-1, fLessVal, SwapVal, (void*)&ValArrayWeightArray(&arVal, &arWeight));
  else
    QuickSort(0, arVal.iSize()-1, fLessVal, SwapVal, (void*)&ValArrayWeightArray(&arVal, 0));
  // find predominant
  double rPred = arVal[0];
  double rPrev = arVal[0];
  if (_fUseWeight) {
    double rWeightSum = arWeight[0];
    double rWeightPred = arWeight[0];
    for (unsigned long i=1; i < arVal.iSize(); ++i) {
      double r = arVal[i];
      if (r != rPrev) {
        if (rWeightSum > rWeightPred) {
          rWeightPred = rWeightSum;
          rPred = rPrev;
        }
        rWeightSum = 0;
      }  
      rWeightSum += arWeight[i];
      rPrev = r;
    }
    if (rWeightSum > rWeightPred)
      rPred = rPrev;
  }
  else {
    long iCount = 1;
    long iCountPred = 1;
    for (unsigned long i=1; i < arVal.iSize(); ++i) {
      double r = arVal[i];
      if (r != rPrev) {
        if (iCount > iCountPred) {
          iCountPred = iCount;
          rPred = rPrev;
        }
        iCount = 0;
      }  
      iCount++;
      rPrev = r;
    }
    if (iCount > iCountPred)
      rPred = rPrev;
  }
  return rPred;
}

Coord AggregatePrd::crdResult(Array<Coord>& arCrd, RealArray& arWeight)
{
  if (arCrd.iSize() == 0)
    return crdUNDEF;
  if (fUnique){
    if (arCrd.iSize() > 1)
      return crdUNDEF;
    return arCrd[0];
  }
  if (_fUseWeight)
    QuickSort(0, arCrd.iSize()-1, fLessCrd, SwapCrd, (void*)&CrdArrayWeightArray(&arCrd, &arWeight));
  else
    QuickSort(0, arCrd.iSize()-1, fLessCrd, SwapCrd, (void*)&CrdArrayWeightArray(&arCrd, 0));
  // find predominant
  Coord crdPred = arCrd[0];
  Coord crdPrev = arCrd[0];
  if (_fUseWeight) {
    double rWeightSum = arWeight[0];
    double rWeightPred = arWeight[0];
    for (unsigned long i=1; i < arCrd.iSize(); ++i) {
      Coord crd = arCrd[i];
      if (crd != crdPrev) {
        if (rWeightSum > rWeightPred) {
          rWeightPred = rWeightSum;
          crdPred = crdPrev;
        }
        rWeightSum = 0;
      }  
      rWeightSum += arWeight[i];
      crdPrev = crd;
    }
    if (rWeightSum > rWeightPred)
      crdPred = crdPrev;
  }
  else {
    long iCount = 1;
    long iCountPred = 1;
    for (unsigned long i=1; i < arCrd.iSize(); ++i) {
      Coord crd = arCrd[i];
      if (crd != crdPrev) {
        if (iCount > iCountPred) {
          iCountPred = iCount;
          crdPred = crdPrev;
        }
        iCount = 0;
      }  
      iCount++;
      crdPrev = crd;
    }
    if (iCount > iCountPred)
      crdPred = crdPrev;
  }
  return crdPred;
}

double AggregateMed::rValResult(RealArray& arVal, RealArray& arWeight)
{
  if (arVal.iSize() == 0)
    return rUNDEF;
  if (_fUseWeight)
    QuickSort(0, arVal.iSize()-1, fLessVal, SwapVal, (void*)&ValArrayWeightArray(&arVal, &arWeight));
  else
    QuickSort(0, arVal.iSize()-1, fLessVal, SwapVal, (void*)&ValArrayWeightArray(&arVal, 0));
  if (!_fUseWeight)
    return arVal[arVal.iSize() /2];
  double rWeightSumMed = 0;
  for (unsigned long i=0; i < arVal.iSize(); ++i)
    rWeightSumMed += arWeight[i];
  rWeightSumMed /= 2;
  double rWeightSum = arWeight[arVal.iSize()-1];
  for (int i=arVal.iSize()-1; i >= 0; --i) {
    if (rWeightSum > rWeightSumMed)
      return arVal[i];
    rWeightSum += arWeight[i-1];
  }
  return arVal[0];
}

AggregateFunction* AggregateFunction::create(const String& sName)
{
  if (fCIStrEqual(sName , "aggmin") || fCIStrEqual(sName , "ColumnAggregateMin"))
    return new AggregateMin();
  if (fCIStrEqual(sName , "aggmax") || fCIStrEqual(sName , "ColumnAggregateMax"))
    return new AggregateMax();
  if (fCIStrEqual(sName , "aggcnt") || fCIStrEqual(sName , "ColumnAggregateCnt"))
    return new AggregateCnt();
  if (fCIStrEqual(sName , "aggsum") || fCIStrEqual(sName , "ColumnAggregateSum"))
    return new AggregateSum();
  if (fCIStrEqual(sName , "aggavg") || fCIStrEqual(sName , "ColumnAggregateAvg"))
    return new AggregateAvg();
  if (fCIStrEqual(sName , "aggstd") || fCIStrEqual(sName , "ColumnAggregateStd"))
    return new AggregateStd();
  if (fCIStrEqual(sName , "aggprd") || fCIStrEqual(sName , "ColumnAggregatePrd"))
    return new AggregatePrd();
  if (fCIStrEqual(sName , "aggmed") || fCIStrEqual(sName , "ColumnAggregateMed"))
    return new AggregateMed();
  if (fCIStrEqual(sName , "joinmin") || fCIStrEqual(sName , "ColumnJoinMin"))
    return new AggregateMin();
  if (fCIStrEqual(sName , "joinmax") || fCIStrEqual(sName , "ColumnJoinMax"))
    return new AggregateMax();
  if (fCIStrEqual(sName , "joincnt") || fCIStrEqual(sName , "ColumnJoinCnt"))
    return new AggregateCnt();
  if (fCIStrEqual(sName , "joinsum") || fCIStrEqual(sName , "ColumnJoinSum"))
    return new AggregateSum();
  if (fCIStrEqual(sName , "joinavg") || fCIStrEqual(sName , "ColumnJoinAvg"))
    return new AggregateAvg();
  if (fCIStrEqual(sName , "joinstd") || fCIStrEqual(sName , "ColumnJoinStd"))
    return new AggregateStd();
  if (fCIStrEqual(sName , "joinprd") || fCIStrEqual(sName , "ColumnJoinPrd"))
    return new AggregatePrd();
  if (fCIStrEqual(sName , "join2") || fCIStrEqual(sName , "ColumnJoin2") || 
      fCIStrEqual(sName , "join2ndKey") || fCIStrEqual(sName , "ColumnJoin2ndKey"))
    return new AggregatePrd(true);
  if (fCIStrEqual(sName , "joinmed") || fCIStrEqual(sName , "ColumnJoinMed"))
    return new AggregateMed();
  return 0;
}




