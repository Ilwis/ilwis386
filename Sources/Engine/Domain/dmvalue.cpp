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
	Last change:  WK   22 May 97    2:31 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\DomainTime.h"

DomainValue::DomainValue(const FileName& fn, bool fCreate)
: DomainPtr(fn, fCreate)
{ 
  m_fUnit = ReadElement("DomainValue", "Unit", m_sUnit) > 0;
}

DomainValue* DomainValue::create(const FileName& fn)
{
  String sType;
  ObjectInfo::ReadElement("DomainValue", "Type", fn, sType );
  if (fCIStrEqual(sType , "DomainValueInt"))
    return new DomainValueInt(fn);
  else if (fCIStrEqual(sType , "DomainValueReal"))
	  return DomainValueReal::create(fn);
  InvalidTypeError(fn, "DomainValue", sType);
  return 0;
}

DomainValue::~DomainValue()
{ }

String DomainValue::sType() const
{
  return "Domain Value";
}

void DomainValue::Store()
{
  DomainPtr::Store();
  WriteElement("Domain", "Type", "DomainValue");
  if (m_fUnit)
    WriteElement("DomainValue", "Unit", m_sUnit);
  else
    WriteElement("DomainValue", "Unit", (char*)0);
}

StoreType DomainValue::stNeeded() const
{
  bool fReal;
  unsigned long i = iValues(fReal);
  if (fReal)
    return stREAL;
  else
    return st(i);
}

String DomainValue::sValue(double rValue, short iWidth, short) const
{
  return sValue(longConv(rValue), iWidth);
}

long DomainValue::iRaw(double rValue) const
{
  return iRaw(longConv(rValue));
}

double DomainValue::rStep() const 
{
   return 1; 
}

bool DomainValue::fUnit() const
{
  return m_fUnit;
}

String DomainValue::sUnit() const
{
  if (m_fUnit)
    return m_sUnit;
  else
    return "";
}


/////////////////////////////////////////

DomainValue* DomainValueReal::create(const FileName& fn)
{
  String sType;
  ObjectInfo::ReadElement("DomainValueReal", "Type", fn, sType );
  if (fCIStrEqual(sType , ""))
    return new DomainValueReal(fn);
  else if (fCIStrEqual(sType , "DomainTime"))
	  return new DomainTime(fn);
  InvalidTypeError(fn, "DomainValueReal", sType);
  return 0;
}

DomainValueReal::DomainValueReal(const FileName& fn)
: DomainValue(fn)
{
// err:  min > max                ::DomainValueReal(fn)
  _rMin = rReadElement("DomainValueReal", "Min");
  _rMax = rReadElement("DomainValueReal", "Max");
  _rStep = rReadElement("DomainValueReal", "Step");
  ReadElement("DomainValueReal", "Fixed Range", m_fFixedRange);
  ReadElement("DomainValueReal", "Fixed Precision", m_fFixedPrecision);
  init();
}

DomainValueReal::DomainValueReal(const FileName& fn, double mn, double mx,
    double step)
: DomainValue(fn, true)
, _rMin(mn), _rMax(mx), _rStep(step)
, m_fFixedPrecision(false), m_fFixedRange(false)
{
  init();
  Updated();
}

DomainValueReal::~DomainValueReal()
{ }

void DomainValueReal::Store()
{
  if (fCIStrEqual(fnObj.sFile, "value") || fCIStrEqual(fnObj.sFile, "real"))
    return;
  DomainValue::Store();
  WriteElement("DomainValue", "Type", "DomainValueReal");
  WriteElement("DomainValueReal", "Min", _rMin);
  WriteElement("DomainValueReal", "Max", _rMax);
  WriteElement("DomainValueReal", "Step", _rStep);
  WriteElement("DomainValueReal", "Fixed Range", m_fFixedRange);
  WriteElement("DomainValueReal", "Fixed Precision", m_fFixedPrecision);
}

void DomainValueReal::init()
{
	_iDec = 0;
	double r = _rStep;
	if (r == 0)
		_iDec = -1;
	else while (r - floor(r) > 1e-10) {
		r *= 10;
		_iDec++;
		if (_iDec > 5)
			break;
	}
	if (_rMax < _rMin)
		_rMax = _rMin + 1;
	short iBeforeDec = floor(log10(max(abs(_rMin), abs(_rMax)))) + 1;
	if (_rMin < 0)
		iBeforeDec++;
	_iWidth = iBeforeDec + iDec();
	if (iDec() > 0)
		_iWidth++;
	if (_iWidth > 12)
		_iWidth = 12;
	double rMinDivStep = _rMin;
	double rMaxDivStep = _rMax;
	if (_rStep > 1e-30) {
		rMinDivStep /= _rStep;
		rMaxDivStep /= _rStep;
	}
	iRawUndef = iUNDEF;
	switch (stNeeded()) {
    case stREAL:
		_i0 = 0;
		_fRawAvail = false;
		break;
    case stLONG:
		if (rMinDivStep < -LONG_MAX || rMaxDivStep > LONG_MAX)
			//        _i0 = round(rMinDivStep / 2 + rMaxDivStep / 2); // 0.5 rounds to 1 !
			_i0 = round(rMinDivStep / 2 + rMaxDivStep / 2 - 0.0001);
		else
			_i0 = 0;
		break;
    case stINT:
		iRawUndef = shUNDEF;
		if (rMinDivStep < -SHRT_MAX || rMaxDivStep > SHRT_MAX)
			//        _i0 = round(rMinDivStep / 2 + rMaxDivStep / 2); // 0.5 rounds to 1 !
			_i0 = round(rMinDivStep / 2 + rMaxDivStep / 2 - 0.0001);
		else
			_i0 = 0;
		break;
    case stBYTE:
		iRawUndef = 0;
		if (rMinDivStep < 0 || rMaxDivStep > 255)
			_i0 = round(rMinDivStep)-1;
		else
			_i0 = -1;
		break;
    case stNIBBLE:
		iRawUndef = 0;
		if (rMinDivStep < 0 || rMaxDivStep > 15)
			_i0 = round(rMinDivStep)-1;
		else
			_i0 = -1;
		break;
    case stDUET:
		iRawUndef = 0;
		if (rMinDivStep < 0 || rMaxDivStep > 3)
			_i0 = round(rMinDivStep)-1;
		else
			_i0 = -1;
		break;
    case stBIT:
		iRawUndef = 0;
		_i0 = round(rMinDivStep)-1;
	}
	// read width from ODF
	_iWidth = max(_iWidth, iReadElement("Domain", "Width"));
}

double DomainValueReal::rStep() const 
{ 
	return _rStep; 
}

String DomainValueReal::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("Real(%S,%S,%g)", sValue(_rMin,0), sValue(_rMax,0), _rStep);
}

bool DomainValueReal::fEqual(const IlwisObjectPtr& ptr) const
{
	const DomainValueReal* dv = dynamic_cast<const DomainValueReal*>(&ptr);
	if (0 == dv)
		return false;
	
	// Compare doesn't care about stretch and lut
	if (_rMin != dv->_rMin || _rMax != dv->_rMax || _rStep != dv->_rStep)
		return false;
	return true;
}

unsigned long DomainValueReal::iValues(bool& fReal) const
{
  fReal = false;
  if (_rMin == _rMax)
    return 1;
  if (_rStep < 1e-30) {
    fReal = true;
    return iUNDEF;
  }
  double r = _rMax - _rMin;
  r /= _rStep;
  r += 1;
  fReal = r > LONG_MAX;
  if (fReal)
    return iUNDEF;
  else
    return floor(r+0.5);
}

String DomainValueReal::sValueByRaw(long iRaw, short w, short dec) const
{
  double r = rValueByRaw(iRaw);
  return sValue(r,w,dec);
}

long DomainValueReal::iRaw(const String& sValue) const
{
  if (stNeeded() == stREAL)
    return iUNDEF;
  double rValue = sValue.rVal();
  return iRaw(rValue);
}

String DomainValueReal::sValue(long iValue, short iWidth) const
{
  return sValue(doubleConv(iValue), iWidth, 0);
}

String DomainValueReal::sValue(double rValue, short w, short dec) const
{
  if (w < 0)
    w = iWidth();
  if (rValue == rUNDEF)
    return String("%*s", w, "?");
  String s;
  if (dec < 0)
    dec = iDec();
  if (dec < 0)  
    if (w == 0)
      s = String("%g", rValue);
    else
      s = String("%*g", w, rValue);
  else if (dec > 0) {
    if (w == 0) {
      s = String("%#.*f", dec, rValue);
      if (s.length() > 15 && s.length() > dec + 6)
        s = String("%.*e", dec, rValue);
    }  
    else {
      s = String("%#*.*f", w, dec, rValue);
      short dif = s.length() - w;
      if (dif > 0) {
        if (dif <= dec)
          s = String("%#*.*f", w, dec-dif, rValue);
        else {
          s = String("%#*.*g", w, dec, rValue);
          dif = s.length() - w;
          if (dif <= dec)
            s = String("%#*.*g", w, dec-dif, rValue);
          else  // too large
            s = String('*', w);
        }
      }
    }
  }
  else {
    s = String("%*li", w, rounding(rValue));
    if ((w > 0) && (s.length() > w)) // too large
      s = String('*', w);
  }
  return s;
}

long DomainValueReal::iValueByRaw(long iRaw) const
{
  return longConv(rValueByRaw(iRaw));
}

double DomainValueReal::rValueByRaw(long iRaw) const
{
  if (stNeeded() == stREAL)
    return rUNDEF;
  if (iRaw == iUNDEF || iRaw == iRawUndef)
    return rUNDEF;
//  return _i0 + _rStep * iRaw;
  return _rStep * (iRaw + _i0);
}

long DomainValueReal::iRaw(long iValue) const
{
  return iRaw(doubleConv(iValue));
}

long DomainValueReal::iRaw(double rVal) const
{
  if (stNeeded() == stREAL)
    return iUNDEF;
  if (rVal == rUNDEF || rVal < _rMin || rVal > _rMax)
    return iUNDEF;
  rVal /= _rStep;
//  rVal -= _i0;
  return round(rVal) - _i0;
}

double DomainValueReal::rValue(const String& sValue) const
{
  double r = sValue.rVal();
  if ((r >= _rMin) && (r <= _rMax))
    return r;
  return rUNDEF;
}

RangeInt DomainValueReal::riMinMax() const
{ return RangeInt(); }

RangeReal DomainValueReal::rrMinMax() const
{ return RangeReal(_rMin, _rMax); }


bool DomainValueReal::fValid(const String& sValue) const
{
  if (sValue == sUNDEF)
    return true;
  double r = sValue.rVal();
  return ((r >= _rMin) && (r <= _rMax));
}

bool DomainValueReal::fFixedRange() const
{
  return m_fFixedRange;
}

bool DomainValueReal::fFixedPrecision() const
{
  return m_fFixedPrecision;
}

////////////////////////////////////

DomainValueInt::DomainValueInt(const FileName& fn)
: DomainValue(fn)
{
  _iMin = iReadElement("DomainValueInt", "Min");
  _iMax = iReadElement("DomainValueInt", "Max");
  ReadElement("DomainValueInt", "Fixed Range", m_fFixedRange);
}

DomainValueInt::DomainValueInt(const FileName& fn, long mn, long mx)
: DomainValue(fn, true), _iMin(mn), _iMax(mx)
, m_fFixedRange(false)
{
  init();
  if (fCIStrEqual(fnObj.sFile, "int"))
    if (stNeeded() == stLONG)
      const_cast<String&>(fnObj.sFile) = "long";
}

DomainValueInt::~DomainValueInt()
{ }

void DomainValueInt::Store()
{
  if (fCIStrEqual(fnObj.sFile, "int") || fCIStrEqual(fnObj.sFile, "long"))
    return;
  DomainValue::Store();
  WriteElement("DomainValue", "Type", "DomainValueInt");
  WriteElement("DomainValueInt", "Min", _iMin);
  WriteElement("DomainValueInt", "Max", _iMax);
  WriteElement("DomainValueInt", "Fixed Range", m_fFixedRange);
}

void DomainValueInt::init()
{
  if (_iMax < _iMin)
    _iMax = _iMin;
  int w1;
  if (_iMin == 0)
    w1 = 1;
  else
    w1 = floor(log10(abs((double)_iMin))) + 1 + (_iMin < 0 ? 1 : 0);
  if (_iMax > 0) {
    int w2 = floor(log10((double)_iMax)) + 1;
    _iWidth = max(w1, w2);
  }
  else
    _iWidth = w1;
  iRawUndef = iUNDEF;
  switch (stNeeded()) {
    case stLONG:
      if (_iMin < -LONG_MAX || _iMax > LONG_MAX)
        _i0 = _iMin / 2 + _iMax / 2;
      else
        _i0 = 0;
      break;
    case stINT:
      iRawUndef = shUNDEF;
      if (_iMin < -SHRT_MAX || _iMax > SHRT_MAX)
        _i0 = _iMin / 2 + _iMax / 2;
      else
        _i0 = 0;
      break;
    case stBYTE:
      iRawUndef = 0;
      if (_iMin < 1 || _iMax > 255)
        _i0 = _iMin - 1;
      else
        _i0 = 0;
      break;
    case stNIBBLE:
      iRawUndef = 0;
      if (_iMin < 1 || _iMax > 15)
        _i0 = _iMin - 1;
      else
        _i0 = 0;
      break;
    case stDUET:
      iRawUndef = 0;
      if (_iMin < 1 || _iMax > 3)
        _i0 = _iMin - 1;
      else
        _i0 = 0;
      break;
    case stBIT:
      iRawUndef = 0;
      _i0 = _iMin - 1;
  }
	// Read width from ODF
	_iWidth = max(_iWidth, iReadElement("Domain", "Width"));
}

String DomainValueInt::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("Int(%li,%li)", _iMin, _iMax);
}

bool DomainValueInt::fEqual(const IlwisObjectPtr& ptr) const
{
	const DomainValueInt* dvi = dynamic_cast<const DomainValueInt*>(&ptr);
	if (0 == dvi)
		return false;

	if (_iMin != dvi->_iMin || _iMax != dvi->_iMax)
		return false;
	return true;
}

unsigned long DomainValueInt::iValues(bool& fReal) const
{
  fReal = false;
  return _iMax - _iMin + 2;  // undefined !
}

String DomainValueInt::sValueByRaw(long iRaw, short w, short) const
{
  long i = iValueByRaw(iRaw);
  return sValue(i,w);
}

long DomainValueInt::iRaw(const String& sValue) const
{

  if (sValue == sUNDEF)
    return iUNDEF;
  long iValue = sValue.iVal();
  return iRaw(iValue);
}

String DomainValueInt::sValue(long iValue, short w) const
{
  if (w < 0)
    w = iWidth();
  if (iValue == iUNDEF)
    return String("%*s", w, "?");
  String s("%*li", w, iValue);
  if ((w > 0) && (s.length() > w)) // too large
      s = String('*', w);
  return s;
}

long DomainValueInt::iRaw(long iValue) const
{
  if (iValue == iUNDEF || iValue < _iMin || iValue > _iMax)
      return iUNDEF;
  iValue -= _i0;
  return iValue;
}

long DomainValueInt::iValueByRaw(long iRaw) const
{
  if (iUNDEF == iRaw || iRawUndef == iRaw)
    return iUNDEF;
  return _i0 + iRaw;
}

RangeInt DomainValueInt::riMinMax() const
{ return RangeInt(_iMin, _iMax); }

RangeReal DomainValueInt::rrMinMax() const
{ return RangeReal(_iMin, _iMax); }

bool DomainValueInt::fValid(const String& sValue) const
{
  if (sValue == sUNDEF)
    return true;
  long i = sValue.iVal();
  return ((i >= _iMin) && (i <= _iMax));
}

bool DomainValueInt::fFixedRange() const
{
  return m_fFixedRange;
}

bool DomainValueInt::fFixedPrecision() const
{
  return true;
}

////////////////////////////////////

DomainImage::DomainImage()
: DomainValue(FileName("image", ".dom", true))
{
  _iWidth = 3;
}

String DomainImage::sName(bool fExt, const String&) const
{
	if (fExt)
		return "Image.dom";
	else
		return "Image";
}

String DomainImage::sTypeName() const
{
  return "Domain Image";
}

bool DomainImage::fEqual(const IlwisObjectPtr& ptr) const
{
  return (0 != dynamic_cast<const DomainImage*>(&ptr));
}

unsigned long DomainImage::iValues(bool& fReal) const
{
  fReal = false;
  return 256;
}

String DomainImage::sValueByRaw(long iRaw, short w, short) const
{
  long i = iValueByRaw(iRaw);
  return sValue(i,w);
}

long DomainImage::iRaw(const String& sValue) const
{
  if (sValue == sUNDEF)
    return iUNDEF;
  long iValue = sValue.iVal();
  return iRaw(iValue);
}

String DomainImage::sValue(long iValue, short w) const
{
  if (w < 0)
    w = iWidth();
  long iVal = iRaw(iValue);
  if (iVal == iUNDEF)
    iVal = 0;
  String s("%*li", w, iVal);
  if ((w > 0) && (s.length() > w)) // too large
      s = String('*', w);
  return s;
}

long DomainImage::iValueByRaw(long iRaw) const
{
  if (iRaw < 0 || iRaw > 255)
    return iUNDEF;
  else
    return iRaw;
}

long DomainImage::iRaw(long iValue) const
{
  if (iValue < 0 || iValue > 255)
    return iUNDEF;
  else
    return iValue;
}

bool DomainImage::fValid(const String& sValue) const
{
  if (sValue == sUNDEF)
    return false;
  long i = sValue.iVal();
  return ((i >= 0) && (i <= 255));
}

RangeInt DomainImage::riMinMax() const
{
  return RangeInt(0,255);
}

RangeReal DomainImage::rrMinMax() const
{
  return RangeReal(0,255);
}

bool DomainImage::fFixedRange() const
{
  return true;
}

bool DomainImage::fFixedPrecision() const
{
  return true;
}



DomainBit::DomainBit()
: DomainValue(FileName())
{
  _iWidth = 1;
}

String DomainBit::sName(bool fExt, const String&) const
{
	if (fExt)
	  return "Bit.dom";
	else
		return "Bit";
}

String DomainBit::sTypeName() const
{
  return "Domain Bit";
}

bool DomainBit::fEqual(const IlwisObjectPtr& ptr) const
{
  return (0 != dynamic_cast<const DomainBit*>(&ptr));
}

unsigned long DomainBit::iValues(bool& fReal) const
{
  fReal = false;
  return 2;
}

String DomainBit::sValueByRaw(long iRaw, short w, short) const
{
  long i = iValueByRaw(iRaw);
  return sValue(i,w);
}

long DomainBit::iRaw(const String& sValue) const
{
  if (sValue == sUNDEF)
    return iUNDEF;
  if (!isdigit(sValue[0])) {
    if ((sValue == "T") || (sValue == "True"))
      return 1;
    return 0;
  } 
  long iValue = sValue.iVal();
  return iRaw(iValue);
}

String DomainBit::sValue(long iValue, short w) const
{
  if (w < 0)
    w = iWidth();
  long iVal = iRaw(iValue);
  if (iVal == iUNDEF)
    iVal = 0;
  String s("%*li", w, iVal);
  if ((w > 0) && (s.length() > w)) // too large
      s = String('*', w);
  return s;
}

long DomainBit::iValueByRaw(long iRaw) const
{
  if (iRaw < 0 || iRaw > 1)
    return iUNDEF;
  else
    return iRaw;
}

long DomainBit::iRaw(long iValue) const
{
  if (iValue < 0 || iValue > 1)
    return iUNDEF;
  else
    return iValue;
}

bool DomainBit::fValid(const String& sValue) const
{
  if (sValue == sUNDEF)
    return false;
  long i = sValue.iVal();
  return ((i >= 0) && (i <= 1));
}

RangeInt DomainBit::riMinMax() const
{
  return RangeInt(0,1);
}

RangeReal DomainBit::rrMinMax() const
{
  return RangeReal(0,1);
}

bool DomainBit::fFixedRange() const
{
  return true;
}

bool DomainBit::fFixedPrecision() const
{
  return true;
}



DomainBool::DomainBool()
	: DomainValue(FileName("bool", ".dom", true))
{
	_iWidth = 5;
	sTrue = "True";
	sFalse = "False";
}

DomainBool::DomainBool(const FileName& fn)
	: DomainValue(fn)
{
	ReadElement("DomainBool", "True", sTrue);
	ReadElement("DomainBool", "False", sFalse);
}

// create other bool domain
DomainBool::DomainBool(const FileName& fn, const String& _sFalse, const String& _sTrue)
	: DomainValue(fn, true)
{
	sTrue = _sTrue;
	sFalse = _sFalse;
	_iWidth = max(sFalse.length(), sTrue.length());
}

void DomainBool::SetBoolString(long iValue, const String& sVal)
{
	switch (iValue)
	{
		case 1:	sTrue = sVal.sTrimSpaces();
				break;
		case 0:	sFalse = sVal.sTrimSpaces();
				break;
	}
	_iWidth = max(sFalse.length(), sTrue.length());
}

String DomainBool::sType() const
{
  return "Domain Bool";
}

String DomainBool::sName(bool fExt, const String& sDirRelative) const
{
	if (!fnObj.fValid())
		if (fExt)
			return "Bool.dom";
		else
			return "Bool";

	return DomainPtr::sName(fExt, sDirRelative);
}

unsigned long DomainBool::iValues(bool& fReal) const
{
	fReal = false;
	return 3;
}

long DomainBool::iValueByRaw(long iRaw) const
{
	if (iRaw < 1 || iRaw > 2)
		return iUNDEF;
	else
		return iRaw-1;
}

long DomainBool::iRaw(long iValue) const
{
	if (iValue < 0 || iValue > 1)
		return iUNDEF;
	else
		return iValue+1;
}

bool DomainBool::fEqual(const IlwisObjectPtr& ptr) const
{
	const DomainBool *pdb = dynamic_cast<const DomainBool*>(&ptr);
	if (pdb == 0)
		return false;

	return (fCIStrEqual(pdb->sFalse, sFalse) && fCIStrEqual(pdb->sTrue, sTrue));
}

String DomainBool::sValueByRaw(long iRaw, short w, short) const
{
	long i = iValueByRaw(iRaw);
	return sValue(i,w);
}

long DomainBool::iRaw(const String& sValue) const
{
	if (sValue == sUNDEF)
		return iUNDEF;

	String s = sValue.sTrimSpaces();
	if (s.length() == 1)
	{
		if (((tolower(s[0]) == tolower(sTrue[0]))) || (s[0] == '1'))
			return 2;
		if (((tolower(s[0]) == tolower(sFalse[0]))) || (s[0] == '0'))
			return 1;
		return 0;
	} 
	if (s == sTrue)
		return 2;
	else if (s == sFalse)
		return 1;
	return 0; // undef
}

void DomainBool::Store()
{
	if (fCIStrEqual(fnObj.sFile, "value") || fCIStrEqual(fnObj.sFile, "real"))
		return;
	
	DomainPtr::Store();
	WriteElement("Domain", "Type", "DomainBool");
	WriteElement("DomainBool", "True", sTrue);
	WriteElement("DomainBool", "False", sFalse);
}

String DomainBool::sValue(long iValue, short w) const
{
	if (w < 0)
		w = iWidth();

	String s;
	switch (iValue)
	{
		case 0:  s = sFalse; break;
		case 1:  s = sTrue;  break;
		default: s = sUNDEF; break;
	}

	if (0 == w)
		return s;
	else if (s.length() > w)  // string too large to fit, use only first characters
		return s.sLeft(w);
	else
		return String("%*S", (int)w, s); // prepend spaces to get the requested string length
}

bool DomainBool::fValid(const String& sValue) const
{
	if (sValue == sUNDEF)
		return false;
	return iRaw(sValue) != iUNDEF;
}

RangeInt DomainBool::riMinMax() const
{
	return RangeInt(0,1);
}

RangeReal DomainBool::rrMinMax() const
{
	return RangeReal(0,1);
}

bool DomainBool::fFixedRange() const
{
	return true;
}

bool DomainBool::fFixedPrecision() const
{
	return true;
}
