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
/* Interface for AggregateFunctions
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  JEL   9 Jul 97    4:12 pm
*/
#ifndef AGGFUNCS_H
#define AGGFUNCS_H
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"

struct HashAggLL {
  HashAggLL(long iRw, long _i = iUNDEF) {
    iRaw = iRw; i = _i;
  }
  bool operator ==(const HashAggLL& hall)
  { return hall.iRaw == iRaw; }  
  long iRaw;
  long i;
};

struct HashAggRL {
  HashAggRL(double rVl, long _i = iUNDEF) {
    rVal = rVl; i = _i;
  }
  bool operator ==(const HashAggRL& harl)
  { return harl.rVal == rVal; }  
  double rVal;
  long i;
};

struct HashAggRR {
  HashAggRR(double rV, double _r = rUNDEF) {
    rVal = rV; r = _r;
  }
  bool operator ==(const HashAggRR& harr)
  { return harr.rVal == rVal; }  
  double rVal;
  double r;
};

struct HashAggSR {
  HashAggSR(const String& sV, double _r = rUNDEF) {
    sVal = sV; r = _r;
  }
  bool operator ==(const HashAggSR& hasr)
  { return hasr.sVal == sVal; }  
  String sVal;
  double r;
};

struct HashAggSL {
  HashAggSL(const String& sVl, long _i = iUNDEF) {
    sVal = sVl; i = _i;
  }
  bool operator ==(const HashAggSL& hasl)
  { return hasl.sVal == sVal; }  
  String sVal;
  long i;
};

struct HashAggLR {
  HashAggLR(long iRw, double _r = rUNDEF) {
    iRaw = iRw; r = _r;
  }
  bool operator ==(const HashAggLR& halr)
  { return halr.iRaw == iRaw; }  
  long iRaw;
  double r;
};

struct HashAggLRR {
  HashAggLRR(long iRw, double _r1 = rUNDEF, double _r2 = rUNDEF) {
    iRaw = iRw; r1 = _r1; r2 = _r2; 
  }
  bool operator ==(const HashAggLRR& halrr)
  { return halrr.iRaw == iRaw; }  
  long iRaw;
  double r1, r2;
};

struct HashAggRRR {
  HashAggRRR(double rVl, double _r1 = rUNDEF, double _r2 = rUNDEF) {
    rVal = rVl; r1 = _r1; r2 = _r2; 
  }
  bool operator ==(const HashAggRRR& harrr)
  { return harrr.rVal == rVal; }  
  double rVal, r1, r2;
};

struct HashAggSRR {
  HashAggSRR(const String& sVl, double _r1 = rUNDEF, double _r2 = rUNDEF) {
    sVal = sVl; r1 = _r1; r2 = _r2; 
  }
  bool operator ==(const HashAggSRR& hasrr)
  { return hasrr.sVal == sVal; }  
  String sVal;
  double r1, r2;
};

struct HashAggLRRR {
  HashAggLRRR(long iRw, double _r1 = rUNDEF, double _r2 = rUNDEF, double _r3 = rUNDEF) {
    iRaw = iRw; r1 = _r1; r2 = _r2; r3 = _r3;
  }
  bool operator ==(const HashAggLRRR& halrrr)
  { return halrrr.iRaw == iRaw; }  
  long iRaw;
  double r1, r2, r3;
};

struct HashAggRRRR {
  HashAggRRRR(double rVl, double _r1 = rUNDEF, double _r2 = rUNDEF, double _r3 = rUNDEF) {
    rVal = rVl; r1 = _r1; r2 = _r2; r3 = _r3;
  }
  bool operator ==(const HashAggRRRR& harrrr)
  { return harrrr.rVal == rVal; }  
  double rVal, r1, r2, r3;
};

struct HashAggSRRR {
  HashAggSRRR(const String& sVl, double _r1 = rUNDEF, double _r2 = rUNDEF, double _r3 = rUNDEF) {
    sVal = sVl; r1 = _r1; r2 = _r2; r3 = _r3;
  }
  bool operator ==(const HashAggSRRR& hasrrr)
  { return hasrrr.sVal == sVal; }  
  String sVal;
  double r1, r2, r3;
};

struct HashAggPrdMed {
  HashAggPrdMed(double rVl) {
    rVal = rVl;
    rRes = rUNDEF;
  }
  HashAggPrdMed(const HashAggPrdMed& hapm) {
    rVal = hapm.rVal;
    arVal = hapm.arVal;
    arWeight = hapm.arWeight;
    rRes = hapm.rRes;
  }
  HashAggPrdMed& operator=(const HashAggPrdMed& hapm) {
    rVal = hapm.rVal;
    arVal = hapm.arVal;
    arWeight = hapm.arWeight;
    rRes = hapm.rRes;
    return *this;
  }
  bool operator ==(const HashAggPrdMed& hapm)
  { return hapm.rVal == rVal; }  
  bool fEmpty() const { return arVal.iSize() == 0; }
  double rVal, rRes;
  RealArray arVal;
  RealArray arWeight;
};

struct HashAggSPrdMed {
  HashAggSPrdMed(const String& sVl) {
    sVal = sVl;
    rRes = rUNDEF;
  }
  HashAggSPrdMed(const HashAggSPrdMed& haspm) {
    sVal = haspm.sVal;
    arVal = haspm.arVal;
    arWeight = haspm.arWeight;
    rRes = haspm.rRes;
  }
  HashAggSPrdMed& operator=(const HashAggSPrdMed& haspm) {
    sVal = haspm.sVal;
    arVal = haspm.arVal;
    arWeight = haspm.arWeight;
    rRes = haspm.rRes;
    return *this;
  }
  bool operator ==(const HashAggSPrdMed& haspm)
  { return haspm.sVal == sVal; }  
  bool fEmpty() const { return arVal.iSize() == 0; }
  String sVal;
  double rRes;
  RealArray arVal;
  RealArray arWeight;
};

class AggregateFunction
{
protected:
  AggregateFunction(const String& sName)
  {
    _sName = sName;
    _fUseWeight = false;
  }  
public:  
  virtual ~AggregateFunction();
  static AggregateFunction _export *create(const String& sName);
  virtual String sName(bool fJoin=false) const;
  virtual void AddVal(double rInd, double rVal) {}
  virtual void AddVal(const String& sInd, double rVal) {}
  virtual void AddRaw(long iInd, double rVal) {}
  virtual void AddVal(double rInd, long iRaw) {}
  virtual void AddVal(const String& sInd, long iRaw) {}
  virtual void AddRaw(long iInd, long iRaw) {}
  virtual void AddVal(double rInd, double rVal, double rWeight) {}
  virtual void AddVal(const String& sInd, double rVal, double rWeight) {}
  virtual void AddRaw(long iInd, double rVal, double rWeight) {}
  virtual void AddVal(double rInd, const String&) {}
  virtual void AddVal(const String& sInd, const String&) {}
  virtual void AddRaw(long iInd, const String&) {}
  virtual double rValResult(double rInd) { return rUNDEF; }
  virtual double rValResult(long iInd) { return rUNDEF; }
  virtual double rValResult(const String& sInd) { return rUNDEF; }
  virtual long iRawResult(double rInd) { return iUNDEF; }
  virtual long iRawResult(long iInd) { return iUNDEF; }
  virtual long iRawResult(const String& sInd) { return iUNDEF; }
//  virtual String sResult(double rInd) { return sUNDEF; }
//  virtual String sResult(long iInd) { return sUNDEF; }
//  virtual String sResult(const String& sInd) { return sUNDEF; }
  virtual void reset() {}
  virtual void reset(bool fRaw, bool fUseString) {} 
  virtual bool fValuesNeeded() const
  { return true; }
  virtual DomainValueRangeStruct dvrsDflt(const Column& colVal) const
  { return colVal->dvrs(); }
  virtual bool fWeightPossible() 
  { return false; }
  void SetUseWeight() 
  { _fUseWeight = true; }
protected:
  String _sName;
//  LongArrayLarge _iNr;
  bool _fUseWeight;
};

class DATEXPORT AggregateMin : public AggregateFunction
{
public:
  AggregateMin() : AggregateFunction("aggmin") {}
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);    
  virtual double rValResult(double rInd);
  virtual double rValResult(long iInd);
  virtual double rValResult(const String& sInd);
  virtual bool fValuesNeeded() const
  { return true; }
  HashTable<HashAggLR> htiMin;
  HashTable<HashAggRR> htrMin;
  HashTable<HashAggSR> htsMin;
};

class DATEXPORT AggregateMax : public AggregateFunction
{
public:
  AggregateMax() : AggregateFunction("aggmax") {}
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);
  virtual double rValResult(double rInd);
  virtual double rValResult(long iInd);
  virtual double rValResult(const String& sInd);
  virtual bool fValuesNeeded() const
  { return true; }
  HashTable<HashAggLR> htiMax;
  HashTable<HashAggRR> htrMax;
  HashTable<HashAggSR> htsMax;
};

class DATEXPORT AggregateSum : public AggregateFunction
{
public:
  AggregateSum() : AggregateFunction("aggsum") {}
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);
  virtual double rValResult(double rInd);
  virtual double rValResult(long iInd);
  virtual double rValResult(const String& sInd);
  virtual bool fValuesNeeded() const
  { return true; }
  virtual DomainValueRangeStruct dvrsDflt(const Column& colVal) const;
  HashTable<HashAggLR> htiSum;
  HashTable<HashAggRR> htrSum;
  HashTable<HashAggSR> htsSum;
};

class DATEXPORT AggregateCnt : public AggregateFunction
{
public:
  AggregateCnt() : AggregateFunction("aggcnt") {}
  virtual DomainValueRangeStruct dvrsDflt(const Column& colVal) const;
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double);
  virtual void AddVal(const String& sInd, double);
  virtual void AddRaw(long iInd, double);
  virtual void AddVal(double rInd, long);
  virtual void AddVal(const String& sInd, long);
  virtual void AddRaw(long iInd, long);
  virtual void AddVal(double rInd, const String&);
  virtual void AddVal(const String& sInd, const String&);
  virtual void AddRaw(long iInd, const String&);
  virtual double rValResult(double rInd);
  virtual double rValResult(const String& sInd);
  virtual double rValResult(long iInd);
  virtual bool fValuesNeeded() const
  { return false; }
  HashTable<HashAggRL> htrCount;
  HashTable<HashAggLL> htiCount;
  HashTable<HashAggSL> htsCount;
};

class DATEXPORT AggregateAvg : public AggregateFunction
{
public:
  AggregateAvg() : AggregateFunction("aggavg") { }
  virtual bool fWeightPossible() { return true; }
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);
  virtual void AddVal(double rInd, double rVal, double rWeight);
  virtual void AddVal(const String& sInd, double rVal, double rWeight);
  virtual void AddRaw(long iInd, double rVal, double rWeight);
  virtual double rValResult(double rInd);
  virtual double rValResult(long iInd);
  virtual double rValResult(const String& sInd);
  virtual bool fValuesNeeded() const
  { return true; }
  HashTable<HashAggLRR> htiAvg;
  HashTable<HashAggRRR> htrAvg;
  HashTable<HashAggSRR> htsAvg;
};


class DATEXPORT AggregateStd : public AggregateFunction
{
public:
  AggregateStd() : AggregateFunction("aggstd") {}

  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);
  virtual void AddVal(double rInd, double rVal, double rWeight);
  virtual void AddVal(const String& sInd, double rVal, double rWeight);
  virtual void AddRaw(long iInd, double rVal, double rWeight);
  virtual double rValResult(double rInd);
  virtual double rValResult(const String& sInd);
  virtual double rValResult(long iInd);
  virtual bool fValuesNeeded() const
  { return true; }
  // r1 = SumX, r2 = SumX2, r3 = SumWeight
  HashTable<HashAggLRRR> htiStd;
  HashTable<HashAggRRRR> htrStd;
  HashTable<HashAggSRRR> htsStd;

  virtual bool fWeightPossible() { return true; }
  virtual DomainValueRangeStruct dvrsDflt(const Column& colVal) const;
};


class DATEXPORT AggregatePrd : public AggregateFunction
{
public:
  AggregatePrd(bool fUniq=false) : AggregateFunction("aggprd") { fUnique = fUniq; }
  virtual String sName(bool fJoin = false) const;
  virtual void reset();
  virtual void reset(bool fRaw, bool fUseString);
  virtual void AddVal(double rInd, double rVal);
  virtual void AddVal(double rInd, long iRaw);
  virtual void AddVal(const String& sInd, long iRaw);
  virtual void AddVal(const String& sInd, double rVal);
  virtual void AddRaw(long iInd, double rVal);
  virtual void AddRaw(long iInd, long iRaw);
  virtual void AddVal(double rInd, double rVal, double rWeight);
  virtual void AddVal(const String& sInd, double rVal, double rWeight);
  virtual void AddRaw(long iInd, double rVal, double rWeight);
  virtual double rValResult(long iInd);
  virtual double rValResult(const String& sInd);
  virtual double rValResult(double rInd);
  virtual long iRawResult(long iInd);
  virtual long iRawResult(double rInd);
  virtual long iRawResult(const String& sInd);
  virtual double rValResult(RealArray& arVal, RealArray& arWeight);
  virtual bool fValuesNeeded() const
  { return false; }
  virtual bool fWeightPossible() { return true; }
  HashTable<HashAggPrdMed> htpm;
  HashTable<HashAggSPrdMed> htspm;
  bool fUnique;
};

class DATEXPORT AggregateMed : public AggregatePrd
{
public:
  AggregateMed() : AggregatePrd() { _sName = "aggmed"; }
  virtual double rValResult(RealArray& arVal, RealArray& arWeight);
};

#endif




