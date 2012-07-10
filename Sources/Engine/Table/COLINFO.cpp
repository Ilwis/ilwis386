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
/* ColumnInfo
Copyright Ilwis System Development ITC
april 1997, by Jelle Wind
Last change:  JEL  14 May 97   10:59 am
*/
#define COLINFO_C
#include "Engine\Table\COLINFO.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\objdepen.h"

ColumnInfo::ColumnInfo(): _valueIsShared(false)
{
}

ColumnInfo::ColumnInfo(const FileName& fnTbl, long iCol)
{
	String sColName;
	if (0 == ObjectInfo::ReadElement("TableStore", String("Col%li", iCol).c_str(), fnTbl, sColName))
		return;
	Read(fnTbl, sColName);
	_valueIsShared = false;
}

ColumnInfo::ColumnInfo(const FileName& fnTbl, const String& sColName)
{
	_iOffset = 0;
	_valueIsShared = false;
	Read(fnTbl, sColName);
}

ColumnInfo::ColumnInfo(const ColumnInfo& colinf)
{
	Init(colinf);
}

void ColumnInfo::operator = (const ColumnInfo& colinf)
{
	Init(colinf);
}

void ColumnInfo::Init(const ColumnInfo& colinf) 
{
	_sName = colinf.sName();
	_fnTbl = colinf.fnTbl();;
	_dminf = colinf.dminf();
	_vr = colinf.vr();
	_st = colinf.st();
	// _iOffset = colinf.iOffset();
	_iRecs = colinf.iRecs();
	_riMinMax = colinf.riMinMax();
	_rrMinMax = colinf.rrMinMax();
	_objtime = colinf.objtime();
	_sDescription = colinf.sDescription();
	_fDependent = colinf.fDependent();
	_sExpression = colinf.sExpression();
	_fReadOnly = colinf.fReadOnly();
	_index = colinf.getIndex();
	_ct = colinf.getColumnType();
	_valueIsShared = false;
	//  _fDataReadOnly = colinf.fDataReadOnly();
	//  _fPropReadOnly = colinf.fPropReadOnly();
}

bool ColumnInfo::fValid() const
{
	return sName().length() != 0;
}

Domain ColumnInfo::dm() const 
{
	if (dminf().fnDom().fValid())
		return Domain(fnDom());
	else
	{
		String sSection("Col:%S", sName().sQuote());
		Domain dom;
		ObjectInfo::ReadElement(sSection.c_str(), "Domain", _fnTbl, dom);
		return dom;
	}
}

DomainValueRangeStruct ColumnInfo::dvrs() const
{
	return _dvrs;
}

void ColumnInfo::Read(const FileName& fnTbl, long iCol, const String& prefix) {
	String sColName;
	String section = prefix + "TableStore";
	if (0 == ObjectInfo::ReadElement(section.c_str(), String("Col%li", iCol).c_str(), fnTbl, sColName))
		return;
	Read(fnTbl, sColName, prefix);
}

void ColumnInfo::Read(const FileName& fnTbl, const String& sColName, const String& prefix)
{
	String sSection("%SCol:%S", prefix, sColName.sQuote());
	String s;
	ObjectInfo::ReadElement(sSection.c_str(), (char*)0, fnTbl, s);
	if (s.length() == 0)
		return;
	_sName = sColName;
	_fnTbl = fnTbl;
	ObjectInfo::ReadElement(String(prefix + "Table").c_str(), "Records", _fnTbl, _iRecs);
	ObjectInfo::ReadElement(sSection.c_str(), "Description", _fnTbl, _sDescription);
	_dminf = DomainInfo(fnTbl, sSection.c_str());
	if (!_dminf.fnDom().fValid()) {
		Domain dom;
		ObjectInfo::ReadElement(sSection.c_str(), "Domain", _fnTbl, dom);
		_dminf = DomainInfo(dom);
	}
	if (_dminf.fValues()) {
		ObjectInfo::ReadElement(sSection.c_str(), "Range", _fnTbl, _vr);
		if ( _vr->vrr() )
			_rValueOffset = _vr->vrr()->rRaw0();
		if ( _vr->vri())
			_rValueOffset = _vr->vri()->iRaw0();

	}
	ObjectInfo::ReadElement(sSection.c_str(), "Time", _fnTbl, _objtime);

	_fReadOnly =  true;
	ObjectInfo::ReadElement(sSection.c_str(), "ReadOnly", _fnTbl, _fReadOnly);
	_fOwnedByTable = false;
	if (ObjectInfo::fDependent(_fnTbl))
		ObjectInfo::ReadElement(sSection.c_str(), "OwnedByTable", _fnTbl,  _fOwnedByTable);
	if (_dminf.fValues()) {
		if (_dminf.fRealValues()) {
			ObjectInfo::ReadElement(sSection.c_str(), "MinMax", _fnTbl, _rrMinMax);
			_riMinMax = RangeInt(longConv(_rrMinMax.rLo()),
				longConv(_rrMinMax.rHi()));
		}
		else {
			ObjectInfo::ReadElement(sSection.c_str(), "MinMax", _fnTbl, _riMinMax);
			_rrMinMax = RangeReal(doubleConv(_riMinMax.iLo()), doubleConv(_riMinMax.iHi()));
		}
	}
	if (_vr.fValid())
		_st = _vr->stUsed();
	else
		_st = _dminf.st();
	s = String();
	ObjectInfo::ReadElement(sSection.c_str(), "Type", _fnTbl, s);
	_fDependent = s != "ColumnStore";
	if (_fDependent)
		ObjectInfo::ReadElement(sSection.c_str(), "Expression", _fnTbl, _sExpression);

	DomainValue *pdv = 0;
	if (_dminf.dmt() == dmtCOORD) {
		String _sCsy;
		ObjectInfo::ReadElement(sSection.c_str(), "Domain", _fnTbl, _sCsy);	
		_dvrs = DomainValueRangeStruct(Domain(FileName(_sCsy)));
	} else {
		pdv = dm()->pdv();
		_dvrs = DomainValueRangeStruct(Domain(fnDom()), _dminf.vr());
	}

	if ( pdv) {
		_rStep = _dvrs.rStep();
	} else {
		_rStep = 1;
		_rValueOffset = 0;
	}

	if ( st() == stLONG || st() == stBYTE || st() == stINT)
		_ct = ctRAW;
	else if ( st() == stREAL)
		_ct = ctREAL;
	else if ( st() == stSTRING)
		_ct = ctSTRING;
	else if (st() == stCRD)
		_ct = ctCRD;
	else if ( st() == stCRD3D)
		_ct = ctCRD3D;
	else if (st() == stBINARY)
		_ct = ctBINARY;
	else if (st() == stCOORDBUF)
		_ct = ctBINARY;

}

FileName ColumnInfo::fnTbl() const
{
	return _fnTbl;
}

String ColumnInfo::sName() const
{
	return _sName;
}

DomainInfo ColumnInfo::dminf() const
{
	return _dminf;
}

FileName ColumnInfo::fnDom() const
{
	return dminf().fnDom();
}

DomainType ColumnInfo::dmt() const
{
	return dminf().dmt();
}

ValueRange ColumnInfo::vr() const
{
	return _vr;
}

StoreType ColumnInfo::st() const
{
	return _st;
}

long ColumnInfo::getOffset() const
{
	return _iOffset;
}

long ColumnInfo::iRecs() const
{
	return _iRecs;
}

RangeInt ColumnInfo::riMinMax() const
{
	return _riMinMax;
}

RangeReal ColumnInfo::rrMinMax() const
{
	return _rrMinMax;
}

ObjectTime ColumnInfo::objtime() const
{
	return _objtime;
}

String ColumnInfo::sDescription() const
{
	return _sDescription;
}

bool ColumnInfo::fDependent() const
{
	return _fDependent;
}

bool ColumnInfo::fReadOnly() const
{
	return _fReadOnly;
}

bool ColumnInfo::fDataReadOnly() const
{
	//  return _fDataReadOnly;
	return fReadOnly() || fDependent();
}
/*
bool ColumnInfo::fPropReadOnly() const
{
return _fPropReadOnly;
}
*/
bool ColumnInfo::fOwnedByTable() const
{
	return _fOwnedByTable;
}

String ColumnInfo::sExpression() const
{
	return _sExpression;
}

void ColumnInfo::DependencyNames(Array<String>& asNames)
{
	if (!_fDependent)
		return;
	ObjectDependency::ReadNames(String("Col:%S", _sName.sQuote()), _fnTbl, asNames);
}

void ColumnInfo::setIndex(long i){
	_index = i;
}
long ColumnInfo::getIndex() const {
	return _index;
}

void ColumnInfo::setOffset(long off) {
	_iOffset = off;
}

ColumnInfo::ColumnType ColumnInfo::getColumnType() const {
	return _ct;
}

double ColumnInfo::getStep() const {
	return _rStep;
}

double ColumnInfo::getValueOffset() const {
	return _rValueOffset;
}

void ColumnInfo::sharedValue(bool yesno) {
	_valueIsShared = yesno;
}

bool ColumnInfo::isSharedValue() const {
	return _valueIsShared;
}



