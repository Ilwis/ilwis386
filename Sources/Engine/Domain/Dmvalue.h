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
/* DomainValue
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    8 Oct 98    5:06 pm
*/

#ifndef ILWDOMVAL_H
#define ILWDOMVAL_H
#include "Engine\Domain\dm.h"

class DomainValue: public DomainPtr
{
friend class Domain;
friend class DomainPtr;

public:
  static DomainValue*         create(const FileName&);
  virtual                     ~DomainValue();
  virtual                     String sType() const;
  virtual unsigned long       iValues(bool& fReal) const = 0;
  virtual StoreType           stNeeded() const;
  virtual String              sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const=0;
  virtual long                iRaw(const String& sValue) const=0;
  virtual String              sValue(long iValue, short iWidth=-1) const=0;
  virtual String              sValue(double rValue, short iWidth=-1, short iDec=-1) const;
  virtual long                iValueByRaw(long iRaw) const=0;
  virtual long                iRaw(long iValue) const=0;
  virtual long                iRaw(double rValue) const;
  virtual bool                fValid(const String& sValue) const = 0;
  virtual RangeReal           rrMinMax() const = 0;
  virtual RangeInt            riMinMax() const = 0;
  virtual double              rStep() const;
  virtual bool fFixedRange() const = 0;
  virtual bool fFixedPrecision() const = 0;
  bool _export fUnit() const;
  String _export sUnit() const;

protected:
  DomainValue(const FileName&, bool fCreate = false);
  virtual void Store();
  long iRawUndef;
  bool m_fUnit;
  String m_sUnit;
};

class DATEXPORT DomainValueReal: public DomainValue
{
  friend DomainValue;
  friend class DomainValueFormView;
  friend class DomainTimeView;
protected:
  DomainValueReal(const FileName&);
  virtual void Store();
public:
  DomainValueReal(const FileName&, double min, double max, double step=1);
  virtual ~DomainValueReal();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual unsigned long iValues(bool& fReal) const;
  static DomainValue *create(const FileName& fn);

  String sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
  long iRaw(const String& sValue) const;
  String sValue(long iValue, short iWidth=-1) const;
  String sValue(double rValue, short iWidth=-1, short iDec=-1) const;
  long iValueByRaw(long iRaw) const;
  double rValueByRaw(long iRaw) const;
  virtual long iRaw(long iValue) const;
  virtual long iRaw(double rValue) const;
  double rValue(const String& sValue) const;

  bool fValid(const String& sValue) const;
  virtual RangeReal rrMinMax() const;
  virtual RangeInt riMinMax() const;
  virtual double rStep() const;
  int iDec() const { return _iDec; }
  virtual bool fFixedRange() const;
  virtual bool fFixedPrecision() const;
private:
  void _export init();
  double _rMin;
  double _rMax;
  double _rStep;
  double _r0;
  int _iDec;
  long _i0;
  bool m_fFixedRange, m_fFixedPrecision;
};


class DATEXPORT DomainValueInt: public DomainValue
{
  friend DomainValue;
  friend class DomainValueIntFormView;
protected:
  DomainValueInt(const FileName&);
  virtual void Store();
public:
  DomainValueInt(const FileName&, long min, long max);
  virtual ~DomainValueInt();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual unsigned long iValues(bool& fReal) const;

  virtual String sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
  virtual long iRaw(const String& sValue) const;
  virtual String sValue(long iValue, short iWidth=-1) const;
  virtual long iValueByRaw(long iRaw) const;
  virtual long iRaw(long iValue) const;

  virtual bool fValid(const String& sValue) const;
  virtual RangeReal rrMinMax() const;
  virtual RangeInt riMinMax() const;
  virtual bool fFixedRange() const;
  virtual bool fFixedPrecision() const;
private:
  void _export init();
  long _iMin;
  long _iMax;
  long _i0;
  bool m_fFixedRange;
};

class DATEXPORT DomainImage: public DomainValue
{
public:
  DomainImage();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual String sTypeName() const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual unsigned long iValues(bool& fReal) const;

  virtual String sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
  virtual long iRaw(const String& sValue) const;
  virtual String sValue(long iValue, short iWidth=-1) const;
  virtual long iValueByRaw(long iRaw) const;
  virtual long iRaw(long iValue) const;

  virtual bool fValid(const String& sValue) const;
  virtual RangeReal rrMinMax() const;
  virtual RangeInt riMinMax() const;
  virtual bool fFixedRange() const;
  virtual bool fFixedPrecision() const;
};

class DATEXPORT DomainBit: public DomainValue
{
public:
  DomainBit();
  virtual String sName(bool fExt, const String& sDirRelative) const;
  virtual String sTypeName() const;
  virtual bool fEqual(const IlwisObjectPtr&) const;
  virtual unsigned long iValues(bool& fReal) const;

  virtual String sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
  virtual long iRaw(const String& sValue) const;
  virtual String sValue(long iValue, short iWidth=-1) const;
  virtual long iValueByRaw(long iRaw) const;
  virtual long iRaw(long iValue) const;

  virtual bool fValid(const String& sValue) const;
  virtual RangeReal rrMinMax() const;
  virtual RangeInt riMinMax() const;
  virtual bool fFixedRange() const;
  virtual bool fFixedPrecision() const;
};

class DomainBool: public DomainValue
{
protected:
	virtual void Store();
public:
	DomainBool();   // open system domain bool
	DomainBool(const FileName&); // open other bool domain
	DomainBool(const FileName&, const String& sFalse, const String& sTrue);  // create other bool domain

	virtual String        sName(bool fExt, const String& sDirRelative) const;
	virtual String        sType() const;
	virtual bool          fEqual(const IlwisObjectPtr&) const;
	virtual unsigned long iValues(bool& fReal) const;
	
	void _export          SetBoolString(long iValue, const String& sVal);
	virtual String        sValueByRaw(long iRaw, short iWidth=-1, short iDec=-1) const;
	virtual long          iRaw(const String& sValue) const;
	virtual String        sValue(long iValue, short iWidth=-1) const;
	virtual long          iValueByRaw(long iRaw) const;
	virtual long          iRaw(long iValue) const;
	
	virtual bool          fValid(const String& sValue) const;
	virtual RangeReal     rrMinMax() const;
	virtual RangeInt      riMinMax() const;
	virtual bool          fFixedRange() const;
	virtual bool          fFixedPrecision() const;
private:  
	String sTrue;
	String sFalse;
};

#endif // ILWDOMVAL_H



