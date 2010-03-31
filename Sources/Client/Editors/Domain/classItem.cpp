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
// classItem.cpp: implementation of the ClassItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Base\AssertD.h"
#include "Client\Editors\Domain\classItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClassItem::ClassItem()
{}

ClassItem::ClassItem(const String &sName, const String &sCode, const String &sDescr)
{
	if (sName.iPos(':') != shUNDEF)
	{
		ISTRUE(fIEqual, (short)sCode.length(), (short)0);  // sCode should not be specified twice
		_sCode = sName.sHead(":").sTrimSpaces();
		_sName = sName.sTail(":").sTrimSpaces();
	}
	else
	{
		_sName = sName.sTrimSpaces();
		_sCode = sCode.sTrimSpaces();
	}
	_sDescr = sDescr.sTrimSpaces();
}

ClassItem::~ClassItem()
{

}

const String& ClassItem::sName() const 
{
	return _sName;
}

const String& ClassItem::sCode() const 
{
	return _sCode;
}

const String& ClassItem::sDescription() const 
{
	return _sDescr;
}

void ClassItem::SetName(const String &sName)
{
	_sName = sName.sTrimSpaces();
}

void ClassItem::SetCode(const String &sCode)
{
	_sCode = sCode.sTrimSpaces();
}

void ClassItem::SetDescription(const String &sDescr)
{
	_sDescr = sDescr.sTrimSpaces();
}

String ClassItem::sNameCode() const 
{
	if (_sCode.length() == 0)
		return _sName;
	else
		return String("%S:%S", _sCode, _sName);
}

// GroupItem
GroupItem::GroupItem()
{
	_rBound = rUNDEF;
}

GroupItem::GroupItem(const String& sName, const String& sCode, const String& sDescr, double rBound)
	: ClassItem(sName, sCode, sDescr), _rBound(rBound)
{}

GroupItem::~GroupItem()
{}

void GroupItem::SetBound(double rBound)
{
	_rBound = rBound;
}

double GroupItem::rBound() const
{
	return _rBound;
}
