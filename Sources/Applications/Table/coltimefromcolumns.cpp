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
oct. 1996, by Jelle Wind
Last change:  JEL  15 May 97    4:00 pm
*/
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblstore.h"
#include "Applications\Table\COLCUM.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Applications\Table\coltimefromcolumns.h"


const char* ColumnTimeFromColumns::sSyntax() { return "ColumnCum(col)\ncum(col)\nlumnCum(col,sortcol)\ncum(col,sortcol)"; }

IlwisObjectPtr * createColumnTimeFromColumns(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnTimeFromColumns::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnTimeFromColumns(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

ColumnTimeFromColumns::ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr)
{
	ReadElement(sSection().c_str(), "Year", year);
	ReadElement(sSection().c_str(), "Month", month);
	ReadElement(sSection().c_str(), "Day", day);
	ReadElement(sSection().c_str(), "Hour", hour);
	ReadElement(sSection().c_str(), "Minutes", minutes);
	ReadElement(sSection().c_str(), "Seconds", seconds);
	fNeedFreeze = true;
}

ColumnTimeFromColumns::ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr, 
											 const String& colYear,const String& colMonth,const String& colDay,
											 const String& colHours,const String& colMinutes, const String& colSeconds)
											 : ColumnVirtual(tbl, sColName, ptr, DomainValueRangeStruct(Domain("time")), Table()), year(colYear), month(colMonth), day(colDay), hour(colHours), minutes(colMinutes), seconds(colSeconds)
{
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
	fNeedFreeze = true;
	ColumnVirtual::Replace(sExpression());
}

ColumnTimeFromColumns* ColumnTimeFromColumns::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
													 const String& sExpression, const DomainValueRangeStruct& dvs)
{
	String sExpr = sExpression;
	Table tblSearch;
	String sTblSrch = ColumnVirtual::sTblSearch(sExpr);
	if (sTblSrch.length() != 0) {
		try {
			tblSearch = Table(sTblSrch);
		}
		catch (const ErrorObject& err) {
			err.Show();
		}
	}  
	Array<String> as;
	IlwisObjectPtr::iParseParm(sExpr, as);
	String year = as[0];
	String month = as[1];
	String day = as[2];
	String hours = as[3];
	String minutes = as[4];
	String seconds = as[5];
	return new ColumnTimeFromColumns(tbl, sColName, ptr, year, month, day, hours, minutes, seconds);
}

String ColumnTimeFromColumns::sExpression() const
{
	return String("ColumnTimeFromColumns=(%S,%S,%S,%S,%S,%S)",year, month, day, hour, minutes, seconds);
}

void ColumnTimeFromColumns::Store()
{
	ColumnVirtual::Store();
	WriteElement(sSection().c_str(), "Year", year);
	WriteElement(sSection().c_str(), "Month", month);
	WriteElement(sSection().c_str(), "Day", day);
	WriteElement(sSection().c_str(), "Hour", hour);
	WriteElement(sSection().c_str(), "Minutes", minutes);
	WriteElement(sSection().c_str(), "Seconds", seconds);
}

void ColumnTimeFromColumns::Replace(const String& sExpr)
{
	ColumnVirtual::Replace(sExpr);
	//fFrozen = false;
}



bool ColumnTimeFromColumns::fFreezing()
{
	Column colYear, colMonth, colDays, colHours, colMinutes, colSeconds;
	Table tbl(ptr.fnTbl);
	if ( year != "")
		colYear = tbl->col(year);
	if ( month != "")
		colMonth = tbl->col(month);
	if ( day != "")
		colDays = tbl->col(day);
	if ( hour != "")
		colHours = tbl->col(hour);
	if ( minutes != "")
		colMinutes = tbl->col(minutes);
	if ( seconds != "")
		colSeconds = tbl->col(seconds);
	for(int i=1; i < iRecs(); ++i) {
		int tyear = -4713, tmonth=1, tday=1,thours=0, tminutes=0;
		double tseconds=0;
		if ( colYear.fValid())
			tyear = colYear->rValue(i);
		if ( colMonth.fValid())
			tmonth = colMonth->rValue(i);
		if ( colDays.fValid())
			tday = colDays->rValue(i);
		if ( colHours.fValid())
			thours = colHours->rValue(i);
		if ( colMinutes.fValid())
			tminutes = colMinutes->rValue(i);
		if ( colSeconds.fValid())
			tseconds = colSeconds->rValue(i);
		ILWIS::Time ti(tyear, tmonth,tday, thours, tminutes, tseconds);
		pcs->PutVal(i, ti);

	}
	return true;
}

ColumnTimeFromColumns::~ColumnTimeFromColumns()
{
}

bool ColumnTimeFromColumns::fDomainChangeable() const
{
	return true;
}

bool ColumnTimeFromColumns::fValueRangeChangeable() const
{
	return fDomainChangeable();
}




