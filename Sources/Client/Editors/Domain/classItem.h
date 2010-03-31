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
// classItem.h: interface for the ClassItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLASSITEM_H__09700C06_E291_11D2_9C9F_00A0C9D5320B__INCLUDED_)
#define AFX_CLASSITEM_H__09700C06_E291_11D2_9C9F_00A0C9D5320B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ClassItem  
{
public:
	_export ClassItem();
	_export ClassItem(const String& sName, const String& sCode = "", const String& sDescr = "");
	virtual _export ~ClassItem();

	void _export SetDescription(const String& sDescr);
	void _export SetCode(const String& sCode);
	void _export SetName(const String& sName);
	const String _export &sDescription() const;
	const String _export &sCode() const;
	const String _export &sName() const;
	String _export sNameCode() const;

private:
	String _sDescr;
	String _sCode;
	String _sName;
};

class GroupItem : public ClassItem {
public:
	_export GroupItem();
	_export GroupItem(const String& sName, const String& sCode = "", const String& sDescr = "", double rBound = rUNDEF);
	virtual _export ~GroupItem();
	
	void _export SetBound(double rBound);
	double _export rBound() const;
private:
	double _rBound;
};

#endif // !defined(AFX_CLASSITEM_H__09700C06_E291_11D2_9C9F_00A0C9D5320B__INCLUDED_)
