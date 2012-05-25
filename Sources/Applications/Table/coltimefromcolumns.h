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
/* ColumnTimeFromColumns
   Copyright Ilwis System Development ITC
   oct 1996, by Jelle Wind
	Last change:  JEL   6 May 97    3:52 pm
*/
#pragma once
#include "Engine\Applications\COLVIRT.H"

IlwisObjectPtr * createColumnTimeFromColumns(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );


class DATEXPORT ColumnTimeFromColumns: public ColumnVirtual
{
  friend class ColumnVirtual;
public:
  ~ColumnTimeFromColumns();
  bool fFreezing();
  static const char* sSyntax();
  virtual void Store();
  virtual String sExpression() const;
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  static ColumnTimeFromColumns* create(const Table& tbl, const String& sColName, ColumnPtr& p,
                                 const String& sExpression, const DomainValueRangeStruct& dvs);
  ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr, 
                                   const String& colYear,const String& colMonth,const String& colDay,
								   const String& colHours,const String& colMinutes, const String& colSeconds);
	ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr);
	ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr,const String& stCol);
private:
  ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& p, const DomainValueRangeStruct& dvs,
                   const Column& colInp, const Column& colSort = Column());
  virtual void Replace(const String& sExpression);
	String year;
	String month;
	String day;
	String hour;
	String minutes;
	String seconds;
	String output;
	String stringColumn;
};






