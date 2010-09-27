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
/* TableInfo
   Copyright Ilwis System Development ITC
   may 1997, by Jelle Wind
	Last change:  JEL   2 Jun 97    3:43 pm
*/
#define TBLINFO_C
#include "Engine\Table\tblinfo.h"
#include "Engine\Base\objdepen.h"

TableInfo::TableInfo()
{
}

TableInfo::TableInfo(const FileName& fnTbl)
{
	Read(fnTbl);
}

void TableInfo::Read(const FileName& fnTbl)
{
	if (!fnTbl.fValid())
		return;

	_fnTbl = fnTbl;
	ObjectInfo::ReadElement("Table", "Columns", _fnTbl, _iCols);
	ObjectInfo::ReadElement("Table", "Records", _fnTbl, _iRecs);
	if (0 == ObjectInfo::ReadElement("Table", "iOffset", _fnTbl, _iOffset))
		_iOffset = 1;
	ObjectInfo::ReadElement("Table", "Description", _fnTbl, _sDescription);

	_dminf = DomainInfo(fnTbl, "Table");
	String s;
	if (!_dminf.fnDom().fValid())
	{
		Domain dom;
		ObjectInfo::ReadElement("Table", "Domain", _fnTbl, dom);
		_dminf = DomainInfo(dom);
	}
	ObjectInfo::ReadElement("Table", "Time", _fnTbl, _objtime);
	_fReadOnly = _access(fnTbl.sFullPath().scVal(), 2)==-1;

	ObjectInfo::ReadElement("Table", "Type", _fnTbl, s);
	_fDependent = s != "TableStore";
	if (_fDependent)
		ObjectInfo::ReadElement("Table", "Expression", _fnTbl, _sExpression);

	// read column info
	for (long i=0; i < _iCols; ++i)
	{
		String sCol;
		ObjectInfo::ReadElement("TableStore", String("Col%li", i).scVal(), _fnTbl, sCol);
		aci &= ColumnInfo(_fnTbl, sCol.sUnQuote());
	}
	if (!fDependent())
		_sType = "Table";
	else
	{
		if (0 != ObjectInfo::ReadElement("TableVirtual", "DependentType", _fnTbl, s))
			_sType = s;
		else
			_sType = "Dependent Table";
	}
}

FileName TableInfo::fnTbl() const
{
	return _fnTbl;
}

Domain TableInfo::dm() const
{
  return Domain(fnDom());
}

DomainInfo TableInfo::dminf() const
{
  return _dminf;
}

FileName TableInfo::fnDom() const
{
  return dminf().fnDom();
}

DomainType TableInfo::dmt() const
{
  return dminf().dmt();
}

long TableInfo::iCols() const
{
  return _iCols;
}

long TableInfo::iOffset() const
{
  return _iOffset;
}

long TableInfo::iRecs() const
{
  return _iRecs;
}

ObjectTime TableInfo::objtime() const
{
  return _objtime;
}

String TableInfo::sDescription() const
{
  return _sDescription;
}

bool TableInfo::fDependent() const
{
  return _fDependent;
}

bool TableInfo::fReadOnly() const
{
  return _fReadOnly;
}

bool TableInfo::fDataReadOnly() const
{
//  return _fDataReadOnly;
  return fReadOnly();
}
/*
bool TableInfo::fPropReadOnly() const
{
  return _fPropReadOnly;
}
*/
String TableInfo::sExpression() const
{
  return _sExpression;
}

void TableInfo::DependencyNames(Array<String>& asNames)
{
  if (!_fDependent)
    return;
  ObjectDependency::ReadNames(_fnTbl, asNames);
}

bool TableInfo::fExistCol(const FileName& fnTbl, const String& sSectionPrefix, const String& sColName)
{
  long iCols = 0;
  ObjectInfo::ReadElement(String("%STable", sSectionPrefix).scVal(), "Columns", fnTbl, iCols);
  for (long i=0; i < iCols; ++i) {
    String sCol;
    ObjectInfo::ReadElement(String("%STableStore", sSectionPrefix).scVal(), String("Col%li", i).scVal(), fnTbl, sCol);
		sCol = sCol.sUnQuote();
    if (fCIStrEqual(sCol, sColName))
      return true;
  }
  return false;
}





