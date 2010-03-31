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
/*
// $Log: /ILWIS 3.0/Column/AGGFUNCS.cpp $
 * 
 * 3     16-02-00 12:01 Wind
 * comment
 * 
 * 2     16-02-00 11:44 Wind
 * stdev gave incorrect result
 */
// Revision 1.6  1997/11/18 08:42:10  janh
// In AggregateAvg::AddRaw I changede 2X rVal into rVal*rWeight
//
// Revision 1.5  1997/10/04 12:42:46  Wim
// In AggregateSum::dvrsDflt(colVal) only multiply upper limit with nr of records
//
// Revision 1.4  1997-09-15 21:21:02+02  Wim
// Changed defaults of AggregateSum
// Made dvrsDefault() of AggregateCnt readable
//
// Revision 1.3  1997-09-12 19:32:42+02  Wim
// AggregateCnt Add...() corrected some initializations
//
// Revision 1.2  1997-09-03 12:39:09+02  Wim
// Removed *rWeight in StdDev AddRaw()
//
/* AggregateFunctions
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  JHE  17 Nov 97    4:47 pm
*/

//#define AGGFUNCS_C
#include "Applications\Table\AGGFUNCS.H"

static int iHash(const HashAggLL& hall) {
  word *pw = static_cast<word*>((void*)&hall.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRL& harl) {
  word *pw = static_cast<word*>((void*)&harl.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggLR& halr) {
  word *pw = static_cast<word*>((void*)&halr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}
static int iHash(const HashAggSL& hasl) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasl.sVal.scVal());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggSR& hasr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasr.sVal.scVal());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggRR& harr) {
  word *pw = static_cast<word*>((void*)&harr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggLRR& halrr) {
  word *pw = static_cast<word*>((void*)&halrr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRRR& harrr) {
  word *pw = static_cast<word*>((void*)&harrr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggSRR& hasrr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasrr.sVal.scVal());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}

static int iHash(const HashAggLRRR& halrrr) {
  word *pw = static_cast<word*>((void*)&halrrr.iRaw);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 1; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggRRRR& harrrr) {
  word *pw = static_cast<word*>((void*)&harrrr.rVal);
  word w = *pw;
  ++pw;
  for (int i = 1; i <= 3; ++i, ++pw)
    w ^= *pw;
  return w % 16000;
}

static int iHash(const HashAggSRRR& hasrrr) {
  unsigned long h = 0;
  int i=0;  
  char *ps = const_cast<char *>(hasrrr.sVal.scVal());
  while (*ps)
    h = (h + (i++) * tolower(*ps++)) % 16001;
  return (int)h;  
}


AggregateFunction::~AggregateFunction() 
{}

String AggregateFunction::sName(bool fJoin) const
{
  if (fJoin)
    return String("join%S", _sName.sRight(_sName.length()-3));
  return _sName;
}

void AggregateMin::reset()
{
  htiMin.Resize(1);
}

void AggregateMin::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiMin.Resize(16000);
  else if (fString)
    htsMin.Resize(16000);
  else
    htrMin.Resize(16000);
}

void AggregateMin::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggRR harr(rInd);
  HashAggRR& harrHash = htrMin.get(harr);
  if (harrHash.r == rUNDEF) { // not found
    harr.r = rVal;
    htrMin.add(harr);
  } 
  else
    harrHash.r = min(harrHash.r, rVal);
}

void AggregateMin::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsMin.get(hasr);
  if (hasrHash.r == rUNDEF) { // not found
    hasr.r = rVal;
    htsMin.add(hasr);
  } 
  else
    hasrHash.r = min(hasrHash.r, rVal);
}

void AggregateMin::AddRaw(long iInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiMin.get(halr);
  if (halrHash.r == rUNDEF) { // not found
    halr.r = rVal;
    htiMin.add(halr);
  } 
  else
    halrHash.r = min(halrHash.r, rVal);
}  
    
double AggregateMin::rValResult(double rInd){
  HashAggRR harl(rInd);
  HashAggRR& harlHash = htrMin.get(harl);
  return harlHash.r;
}

double AggregateMin::rValResult(const String& sInd){
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsMin.get(hasr);
  return hasrHash.r;
}  

double AggregateMin::rValResult(long iInd) {
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiMin.get(halr);
  return halrHash.r;
}  

void AggregateMax::reset()
{
  htiMax.Resize(1);
}

void AggregateMax::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiMax.Resize(16000);
  else if (fString)
    htsMax.Resize(16000);
  else
    htrMax.Resize(16000);
}

void AggregateMax::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggRR harr(rInd);
  HashAggRR& harrHash = htrMax.get(harr);
  if (harrHash.r == rUNDEF) { // not found
    harr.r = rVal;
    htrMax.add(harr);
  } 
  else
    harrHash.r = max(harrHash.r, rVal);
}

void AggregateMax::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsMax.get(hasr);
  if (hasrHash.r == rUNDEF) { // not found
    hasr.r = rVal;
    htsMax.add(hasr);
  } 
  else
    hasrHash.r = max(hasrHash.r, rVal);
}
void AggregateMax::AddRaw(long iInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiMax.get(halr);
  if (halrHash.r == rUNDEF) { // not found
    halr.r = rVal;
    htiMax.add(halr);
  } 
  else
    halrHash.r = max(halrHash.r, rVal);
}  
    
double AggregateMax::rValResult(double rInd){
  HashAggRR harr(rInd);
  HashAggRR& harrHash = htrMax.get(harr);
  return harrHash.r;
}  

double AggregateMax::rValResult(const String& sInd){
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsMax.get(hasr);
  return hasrHash.r;
}  

double AggregateMax::rValResult(long iInd) {
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiMax.get(halr);
  return halrHash.r;
}  

void AggregateSum::reset()
{
  htiSum.Resize(1);
}

void AggregateSum::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiSum.Resize(16000);
  else if (fString)
    htsSum.Resize(16000);
  else
    htrSum.Resize(16000);
}

void AggregateSum::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggRR harr(rInd);
  HashAggRR& harrHash = htrSum.get(harr);
  if (harrHash.r == rUNDEF) { // not found
    harr.r = rVal;
    htrSum.add(harr);
  } 
  else
    harrHash.r += rVal;
}

void AggregateSum::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsSum.get(hasr);
  if (hasrHash.r == rUNDEF) { // not found
    hasr.r = rVal;
    htsSum.add(hasr);
  } 
  else
    hasrHash.r += rVal;
}
void AggregateSum::AddRaw(long iInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiSum.get(halr);
  if (halrHash.r == rUNDEF) { // not found
    halr.r = rVal;
    htiSum.add(halr);
  } 
  else
    halrHash.r += rVal;
}  
    
double AggregateSum::rValResult(double rInd){
  HashAggRR harr(rInd);
  HashAggRR& harrHash = htrSum.get(harr);
  return harrHash.r;
}  

double AggregateSum::rValResult(const String& sInd){
  HashAggSR hasr(sInd);
  HashAggSR& hasrHash = htsSum.get(hasr);
  return hasrHash.r;
}  

double AggregateSum::rValResult(long iInd) {
  HashAggLR halr(iInd);
  HashAggLR& halrHash = htiSum.get(halr);
  return halrHash.r;
}  

DomainValueRangeStruct AggregateSum::dvrsDflt(const Column& colVal) const
{
  Domain dm("value");
  RangeReal rr = colVal->rrMinMax();
  double rStep = 0;
  ValueRange vr = colVal->vr();
  if (vr.fValid()) {
    rStep = vr->rStep();
    if (!rr.fValid())
      rr = vr->rrMinMax();
  }
  if (!rr.fValid())
    rr = RangeReal(-1e100,1e100);
  else {
//    rr.rLo() *= colVal->iRecs();
    rr.rHi() *= colVal->iRecs();
  }
  ValueRange vrr(rr,rStep);
  return DomainValueRangeStruct(dm,vrr);
}

void AggregateCnt::reset()
{
  htiCount.Resize(1);
}

void AggregateCnt::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiCount.Resize(16000);
  else if (fString)
    htsCount.Resize(16000);
  else
    htrCount.Resize(16000);
}

void AggregateCnt::AddVal(double rVal, double r) {
  if ((rVal == 0) || (rVal == rUNDEF))
    return;
  if (r == rUNDEF)
    return;
  HashAggRL harl(rVal);
  HashAggRL& harlHash = htrCount.get(harl);
  if (harlHash.i == iUNDEF) { // not found
    harl.i = 1;
    htrCount.add(harl);
  } 
  else
    harlHash.i++;
}

void AggregateCnt::AddVal(const String& sVal, double r) {
  if (sVal == sUNDEF)
    return;
  if (r == rUNDEF)
    return;
  HashAggSL hasl(sVal);
  HashAggSL& haslHash = htsCount.get(hasl);
  if (haslHash.i == iUNDEF) { // not found
    hasl.i = 1;
    htsCount.add(hasl);
  } 
  else
    haslHash.i++;
}

void AggregateCnt::AddRaw(long iInd, double r) {
  if (iInd == iUNDEF)
    return;
  if (r == rUNDEF)
    return;
  HashAggLL hall(iInd);
  HashAggLL& hallHash = htiCount.get(hall);
  if (hallHash.i == iUNDEF) { // not found
    hall.i = 1;
    htiCount.add(hall);
  }
  else  
    hallHash.i++;
}  

void AggregateCnt::AddVal(double rVal, long l) {
  if ((rVal == 0) || (rVal == rUNDEF))
    return;
  if (l == iUNDEF)
    return;
  HashAggRL harl(rVal);
  HashAggRL& harlHash = htrCount.get(harl);
  if (harlHash.i == iUNDEF) { // not found
    harl.i = 1;
    htrCount.add(harl);
  } 
  else
    harlHash.i++;
}

void AggregateCnt::AddRaw(long iInd, long l) {
  if (iInd == iUNDEF)
    return;
  if (l == iUNDEF)
    return;
  HashAggLL hall(iInd);
  HashAggLL& hallHash = htiCount.get(hall);
  if (hallHash.i == iUNDEF) { // not found
    hall.i = 1;
    htiCount.add(hall);
  }
  else  
    hallHash.i++;
}  

void AggregateCnt::AddVal(const String& sInd, long) {
  if (sInd == sUNDEF)
    return;
  HashAggSL hasl(sInd);
  HashAggSL& haslHash = htsCount.get(hasl);
  if (haslHash.i == iUNDEF) { // not found
    hasl.i = 1;
    htsCount.add(hasl);
  }
  else  
    haslHash.i++;
}  

void AggregateCnt::AddVal(double rVal, const String& s) {
  if ((rVal == 0) || (rVal == rUNDEF))
    return;
  if (s == sUNDEF)
    return;
  HashAggRL harl(rVal);
  HashAggRL& harlHash = htrCount.get(harl);
  if (harlHash.i == iUNDEF) { // not found
    harl.i = 1;
    htrCount.add(harl);
  } 
  else
    harlHash.i++;
}

void AggregateCnt::AddVal(const String& sVal, const String& s) {
  if (sVal == sUNDEF)
    return;
  if (s == sUNDEF)
    return;
  HashAggSL hasl(sVal);
  HashAggSL& haslHash = htsCount.get(hasl);
  if (haslHash.i == iUNDEF) { // not found
    hasl.i = 1;
    htsCount.add(hasl);
  } 
  else
    haslHash.i++;
}

void AggregateCnt::AddRaw(long iInd, const String& s) {
  if (iInd == iUNDEF)
    return;
  if (s == sUNDEF)
    return;
  HashAggLL hall(iInd);
  HashAggLL& hallHash = htiCount.get(hall);
  if (hallHash.i == iUNDEF) { // not found
    hall.i = 1;
    htiCount.add(hall);
  }
  else  
    hallHash.i++;
}  

    
double AggregateCnt::rValResult(double rInd){
  HashAggRL harl(rInd);
  HashAggRL& harlHash = htrCount.get(harl);
  return doubleConv(harlHash.i);
}  

double AggregateCnt::rValResult(const String& sInd){
  HashAggSL hasl(sInd);
  HashAggSL& haslHash = htsCount.get(hasl);
  return doubleConv(haslHash.i);
}  

double AggregateCnt::rValResult(long iInd) {
  HashAggLL hall(iInd);
  HashAggLL& hallHash = htiCount.get(hall);
  return doubleConv(hallHash.i);
}  

DomainValueRangeStruct AggregateCnt::dvrsDflt(const Column& colVal) const
{
  Domain dm("count");
  ValueRange vr(0, colVal->iRecs());
  return DomainValueRangeStruct(dm,vr);
}


void AggregateAvg::reset()
{
  htiAvg.Resize(1);
}

void AggregateAvg::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiAvg.Resize(16000);
  else if (fString)
    htsAvg.Resize(16000);
  else
    htrAvg.Resize(16000);
}

void AggregateAvg::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggRRR harrr(rInd);
  HashAggRRR& harrrHash = htrAvg.get(harrr);
  if (harrrHash.r1 == rUNDEF) { // not found
    harrr.r1 = rVal;
    harrr.r2 = 1;
    htrAvg.add(harrr);
  } 
  else {
    harrrHash.r1 += rVal;
    harrrHash.r2++;
  } 
}

void AggregateAvg::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSRR hasrr(sInd);
  HashAggSRR& hasrrHash = htsAvg.get(hasrr);
  if (hasrrHash.r1 == rUNDEF) { // not found
    hasrr.r1 = rVal;
    hasrr.r2 = 1;
    htsAvg.add(hasrr);
  } 
  else {
    hasrrHash.r1 += rVal;
    hasrrHash.r2++;
  } 
}

void AggregateAvg::AddRaw(long iInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggLRR halrr(iInd);
  HashAggLRR& halrrHash = htiAvg.get(halrr);
  if (halrrHash.r1 == rUNDEF) { // not found
    halrr.r1 = rVal;
    halrr.r2 = 1;
    htiAvg.add(halrr);
  } 
  else {
    halrrHash.r1 += rVal;
    halrrHash.r2++;
  } 
}  

void AggregateAvg::AddVal(double rInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  HashAggRRR harrr(rInd);
  HashAggRRR& harrrHash = htrAvg.get(harrr);
  if (harrrHash.r1 == rUNDEF) { // not found
    harrr.r1 = rVal * rWeight;
    harrr.r2 = rWeight;
    htrAvg.add(harrr);
  } 
  else {
    harrrHash.r1 += rVal * rWeight;
    harrrHash.r2 += rWeight;
  } 
}

void AggregateAvg::AddVal(const String& sInd, double rVal, double rWeight) {
  if (rVal == rUNDEF)
    return;
  HashAggSRR hasrr(sInd);
  HashAggSRR& hasrrHash = htsAvg.get(hasrr);
  if (hasrrHash.r1 == rUNDEF) { // not found
    hasrr.r1 = rVal*rWeight;
    hasrr.r2 = rWeight;
    htsAvg.add(hasrr);
  } 
  else {
    hasrrHash.r1 += rVal*rWeight;
    hasrrHash.r2 += rWeight;
  } 
}

void AggregateAvg::AddRaw(long iInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  HashAggLRR halrr(iInd);
  HashAggLRR& halrrHash = htiAvg.get(halrr);
  rWeight = fabs(rWeight);
  if (halrrHash.r1 == rUNDEF) { // not found
    halrr.r1 = rVal*rWeight;;
    halrr.r2 = rWeight;
    htiAvg.add(halrr);
  } 
  else {
    halrrHash.r1 += rVal*rWeight;;
    halrrHash.r2 += rWeight;
  } 
}  
    
double AggregateAvg::rValResult(double rInd){
  HashAggRRR harrr(rInd);
  HashAggRRR& harrrHash = htrAvg.get(harrr);
  if (harrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (harrrHash.r2 <= 0)
    return rUNDEF;
  return harrrHash.r1 / harrrHash.r2;
}  

double AggregateAvg::rValResult(const String& sInd){
  HashAggSRR hasrr(sInd);
  HashAggSRR& hasrrHash = htsAvg.get(hasrr);
  if (hasrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (hasrrHash.r2 <= 0)
    return rUNDEF;
  return hasrrHash.r1 / hasrrHash.r2;
}  

double AggregateAvg::rValResult(long iInd) {
  HashAggLRR halrr(iInd);
  HashAggLRR& halrrHash = htiAvg.get(halrr);
  if (halrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (halrrHash.r2 <= 0)
    return rUNDEF;
  return halrrHash.r1 / halrrHash.r2;
}  

void AggregateStd::reset()
{
  htiStd.Resize(1);
}

void AggregateStd::reset(bool fRaw, bool fString)
{ if (fRaw)
    htiStd.Resize(16000);
  else if (fString)
    htsStd.Resize(16000);
  else
    htrStd.Resize(16000);
}

void AggregateStd::AddVal(double rInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggRRRR harrrr(rInd);
  HashAggRRRR& harrrrHash = htrStd.get(harrrr);
  if (harrrrHash.r1 == rUNDEF) { // not found
    harrrr.r1 = rVal;
    harrrr.r2 = rVal*rVal;
    harrrr.r3 = 1;
    htrStd.add(harrrr);
  } 
  else {
    harrrrHash.r1 += rVal;
    harrrrHash.r2 += rVal*rVal;
    harrrrHash.r3++;
  } 
}

void AggregateStd::AddVal(const String& sInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggSRRR hasrrr(sInd);
  HashAggSRRR& hasrrrHash = htsStd.get(hasrrr);
  if (hasrrrHash.r1 == rUNDEF) { // not found
    hasrrr.r1 = rVal;
    hasrrr.r2 = rVal*rVal;
    hasrrr.r3 = 1;
    htsStd.add(hasrrr);
  } 
  else {
    hasrrrHash.r1 += rVal;
    hasrrrHash.r2 += rVal*rVal;
    hasrrrHash.r3++;
  } 
}

void AggregateStd::AddRaw(long iInd, double rVal) {
  if (rVal == rUNDEF)
    return;
  HashAggLRRR halrrr(iInd);
  HashAggLRRR& halrrrHash = htiStd.get(halrrr);
  if (halrrrHash.r1 == rUNDEF) { // not found
    halrrr.r1 = rVal;
    halrrr.r2 = rVal*rVal;
    halrrr.r3 = 1;
    htiStd.add(halrrr);
  } 
  else {
    halrrrHash.r1 += rVal;
    halrrrHash.r2 += rVal*rVal;
    halrrrHash.r3++;
  } 
}  

void AggregateStd::AddVal(double rInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  rWeight = fabs(rWeight);
  HashAggRRRR harrrr(rInd);
  HashAggRRRR& harrrrHash = htrStd.get(harrrr);
  double r = rVal * rWeight;
  if (harrrrHash.r1 == rUNDEF) { // not found
    harrrr.r1 = r;
    harrrr.r2 = r * rVal;
    harrrr.r3 = rWeight;
    htrStd.add(harrrr);
  } 
  else {
    harrrrHash.r1 += r;
    harrrrHash.r2 += r * rVal;
    harrrrHash.r3 += rWeight;
  } 
}

void AggregateStd::AddVal(const String& sInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  rWeight = fabs(rWeight);
  HashAggSRRR hasrrr(sInd);
  HashAggSRRR& hasrrrHash = htsStd.get(hasrrr);
  double r = rVal * rWeight;
  if (hasrrrHash.r1 == rUNDEF) { // not found
    hasrrr.r1 = r;
    hasrrr.r2 = r * rVal;
    hasrrr.r3 = rWeight;
    htsStd.add(hasrrr);
  } 
  else {
    hasrrrHash.r1 += r;
    hasrrrHash.r2 += r * rVal;
    hasrrrHash.r3 += rWeight;
  } 
}

void AggregateStd::AddRaw(long iInd, double rVal, double rWeight) {
  if ((rVal == rUNDEF) || (rWeight == rUNDEF))
    return;
  rWeight = fabs(rWeight);
  HashAggLRRR halrrr(iInd);
  HashAggLRRR& halrrrHash = htiStd.get(halrrr);
  rWeight = fabs(rWeight);
  double r = rVal * rWeight;
  if (halrrrHash.r1 == rUNDEF) { // not found
    halrrr.r1 = r;
    halrrr.r2 = r * rVal;
    halrrr.r3 = rWeight;
    htiStd.add(halrrr);
  } 
  else {
    halrrrHash.r1 += r;
    halrrrHash.r2 += r * rVal;
    halrrrHash.r3 += rWeight;
  } 
}  
    
double AggregateStd::rValResult(double rInd){
  HashAggRRRR harrrr(rInd);
  HashAggRRRR& harrrrHash = htrStd.get(harrrr);
  if (harrrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (harrrrHash.r3 <= 0)
    return rUNDEF;
  return sqrt((harrrrHash.r2 * harrrrHash.r3 - sqr(harrrrHash.r1)) / harrrrHash.r3 / (harrrrHash.r3-1) );
}  

double AggregateStd::rValResult(const String& sInd){
  HashAggSRRR hasrrr(sInd);
  HashAggSRRR& hasrrrHash = htsStd.get(hasrrr);
  if (hasrrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (hasrrrHash.r3 <= 0)
    return rUNDEF;
  return sqrt((hasrrrHash.r2 * hasrrrHash.r3 - sqr(hasrrrHash.r1)) / hasrrrHash.r3 / (hasrrrHash.r3-1) );
}  

double AggregateStd::rValResult(long iInd) {
  HashAggLRRR halrrr(iInd);
  HashAggLRRR& halrrrHash = htiStd.get(halrrr);
  if (halrrrHash.r1 == rUNDEF)
    return rUNDEF;
  if (halrrrHash.r3 <= 0)
    return rUNDEF;
  return sqrt((halrrrHash.r2 * halrrrHash.r3 - sqr(halrrrHash.r1)) / halrrrHash.r3 / (halrrrHash.r3-1) );
}  

DomainValueRangeStruct AggregateStd::dvrsDflt(const Column& colVal) const
{ 
  return DomainValueRangeStruct(Domain("value")); 
}
