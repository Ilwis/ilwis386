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
/* ValueRange
   Copyright Ilwis System Development ITC
   october 1995, by Wim Koolhoven
	Last change:  WK   25 Jun 98    3:25 pm
*/

#ifndef ILWVALRANGE_H
#define ILWVALRANGE_H
#include "Engine\Domain\dm.h"


class ValueRangePtr;
enum StoreType;

class ValueRange
{
public:
  _export ValueRange();
  _export ValueRange(const Domain& dom);
  _export ValueRange(const RangeInt& ri);
  _export ValueRange(const RangeReal& rr, double rStep=1);
  _export ValueRange(const ValueRangeReal& rr);
  _export ValueRange(double rMin, double rMax, double rStep=1);
  _export ValueRange(const String& sRange);
  _export ValueRange(const ValueRange& vr);
  _export ValueRange(const ILWIS::TimeInterval& tiv);
  _export ~ValueRange();

  void _export   operator = (const ValueRange& vr)
                    { SetPointer(vr.ptr); }
  ValueRangePtr* operator -> () const 
                    { return ptr; }
  bool           fValid() const
                    { return 0 != ptr; }
  bool _export   fEqual(const ValueRange& vr) const;
  bool           operator == (const ValueRange& vr) const
                    { return fEqual(vr); }
  bool           operator != (const ValueRange& vr) const
                    { return !fEqual(vr); }
  void _export   SetStep(double rStep); // can change ptr type!

  private:    
  void _export   SetPointer(ValueRangePtr* p);
  ValueRangePtr* ptr;
};

class ValueRangePtr
{
friend class ValueRange;

public:  
    virtual RangeInt  riMinMax() const;
    virtual RangeReal rrMinMax() const; 
    virtual double    rStep() const;
    virtual int       iDec() const;
    virtual bool      fRealValues() const;
    StoreType         stUsed() const 
                                { return st; }
    long              iUndef() const 
                        { return iRawUndef; }
    short             iWidth() const 
                        { return _iWidth; }
    ValueRangeInt _export  *vri() const;
    ValueRangeReal _export *vrr() const;
    virtual String    sRange(bool fInternal=false) const=0;
    virtual long      iValue(long iRaw) const=0;
    virtual double    rValue(long iRaw) const=0;
    virtual long      iRaw(long iValue) const=0;
    virtual long      iRaw(double rValue) const=0;
    virtual String    sValueByRaw(const Domain&, long iRaw, short iWidth=-1, short iDec=-1) const=0;
    virtual String    sValue(double rValue, short iWidth=-1, short iDec=-1) const;
    bool                    operator==(const ValueRangePtr& vr) const 
                                { return fEqual(vr); }
    bool                    operator!=(const ValueRangePtr& vr) const
                                { return !fEqual(vr); }
    virtual bool      fEqual(const ValueRangePtr& vr) const;
    virtual void      AdjustRangeToStep();

protected:
    ValueRangePtr();

    static double     rDefaultRaw0(const RangeReal& rr, double rStep, StoreType st);
    static long       iDefaultRaw0(const RangeInt& ri, StoreType st);

    long              iRawUndef;
    short             _iWidth;
    StoreType         st;

private:  
    long              iRef;
};

class DATEXPORT ValueRangeInt: public ValueRangePtr, public RangeInt
{
public:
    ValueRangeInt();
    ValueRangeInt(const ValueRangeInt& vr);
    _export ValueRangeInt(long min, long max, long iRaw0=iUNDEF);
    ValueRangeInt(const RangeInt&);
    ValueRangeInt(const Domain&);

    virtual long      iValue(long iRaw) const;
    virtual double    rValue(long iRaw) const;
    virtual long      iRaw(long iValue) const;
    virtual long      iRaw(double rValue) const;
    virtual String    sValueByRaw(const Domain&, long iRaw, short iWidth=-1, short iDec=-1) const;
    virtual String    sValue(double rVal, short iWidth=-1, short iDec=-1) const;
    virtual bool      fEqual(const ValueRangePtr& vr) const;
    virtual RangeInt  riMinMax() const; // { return RangeInt(iLo(), iHi()); }
    virtual RangeReal rrMinMax() const; // { return RangeReal(iLo(), iHi()); }
    virtual double    rStep() const;
    long              iRaw0() const 
                         { return _i0; }
    void              init(long iRaw0 = iUNDEF);
    virtual String    sRange(bool fInternal=false) const;
    long              iDefaultRaw0() const;

private:
    long _i0;
};

class ValueRangeReal: public ValueRangePtr, public RangeReal
{
public:
  ValueRangeReal();
  ValueRangeReal(const ValueRangeReal& vr);
  _export ValueRangeReal(double min, double max, double step=1, double rRaw0=rUNDEF);
  ValueRangeReal(const RangeReal&, double step=1);
  ValueRangeReal(const Domain&);
  ValueRangeReal(const ILWIS::TimeInterval& tiv);

  virtual double    rStep() const;
  virtual int       iDec() const;
  virtual long      iValue(long iRaw) const;
  virtual double    rValue(long iRaw) const;
  virtual long      iRaw(long iValue) const;
  virtual long      iRaw(double rValue) const;
  virtual String    sValueByRaw(const Domain&, long iRaw,
                             short iWidth=-1, short iDec=-1) const;
  virtual String _export sValue(double rVal, short iWidth=-1, short iDec=-1) const;
  virtual bool      fEqual(const ValueRangePtr& vr) const;
  virtual RangeInt  riMinMax() const;
  virtual RangeReal rrMinMax() const; 
  double            rRaw0() const { return _r0; }
  void              init(double rRaw0 = rUNDEF);
  virtual String    sRange(bool fInternal=false) const;
  double            rDefaultRaw0() const;
  virtual void      AdjustRangeToStep();
  virtual bool      fRealValues() const;
	static bool _export  fEqual(double r1, double r2);

private:
  double            _rStep;
  int _iDec;
  double _r0;
};

class DomainValueRangeStruct
{
public:
    _export DomainValueRangeStruct();
    _export DomainValueRangeStruct(const DomainValueRangeStruct& dvs);
    _export DomainValueRangeStruct(const Domain& dm);
    _export DomainValueRangeStruct(const Domain& dm, const ValueRange& vr);
    _export DomainValueRangeStruct(const ValueRange& vr);
    _export DomainValueRangeStruct(long iMin, long iMax);
    _export DomainValueRangeStruct(double rMin, double rMax, double rStep);
    _export ~DomainValueRangeStruct();

    void _export     operator=(const DomainValueRangeStruct& dvs);
    bool _export     operator == (const DomainValueRangeStruct& dvs) const;
    bool _export     operator != (const DomainValueRangeStruct& dvs) const
                       {  return !operator==(dvs); }
    void   _export   SetDomain(const Domain& dm, bool fDefaultValueRange=false);
    void   _export   SetValueRange(const ValueRange& vr);
    const            Domain& dm() const
                       { return _dm; }
    const ValueRange _export &vr() const
                       { return _vr; }
    ValueRange _export &vr() 
                       { return _vr; }

	// Retrieve value type information, now calculated based on current Domain and ValueRange
    StoreType _export st() const;
    bool _export      fUseReals() const;
    bool _export      fValues() const;
    bool _export      fRawAvailable() const;
    bool _export      fRawIsValue() const;
    bool _export      fRealValues() const;
    bool _export      fStringRep() const;

    bool   _export   fCoords() const;
    String _export   sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
    String _export   sValue(long iValue, short iWidth=-1) const;
    String _export   sValue(double rValue, short iWidth=-1, short iDec=-1) const;
    String _export   sValue(const Coord& crd, short iWidth=-1) const;
    long   _export   iRaw(const String& sValue) const;
    long   _export   iValue(long iRaw) const;
    double _export   rValue(long iRaw) const;
    double _export   rValue(const String& sValue) const;
    long   _export   iRaw(long iValue) const;
    long   _export   iRaw(double rValue) const;
    bool   _export   fValid(const String& sValue) const;
    RangeInt _export riMinMax() const;
    RangeReal _export rrMinMax() const;
    double _export   rStep() const;
    long   _export   iWidth() const;
    long   _export   iDec() const;
    long   _export   iRawUndef() const;

private:
    void             SetPrivateMembers();
    Domain           _dm;
    ValueRange       _vr;
 };

#endif // ILWVALRANGE_H





