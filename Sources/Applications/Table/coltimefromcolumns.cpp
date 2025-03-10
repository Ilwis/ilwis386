/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52�North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Engine\Domain\DomainTime.h"


const char* ColumnTimeFromColumns::sSyntax() { return "ColumnTimeFromColumns(year,month,day,hours,minutes,seconds)\nColumnTimeFromColumns(string,template)"; }

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
	ReadElement(sSection().c_str(), "String", stringColumn);
	ReadElement(sSection().c_str(), "Template", formatTemplate);
	if ( stringColumn == "") {
		ReadElement(sSection().c_str(), "Year", year);
		ReadElement(sSection().c_str(), "Month", month);
		ReadElement(sSection().c_str(), "Day", day);
		ReadElement(sSection().c_str(), "Hour", hour);
		ReadElement(sSection().c_str(), "Minutes", minutes);
		ReadElement(sSection().c_str(), "Seconds", seconds);
	}
	fNeedFreeze = true;
}

ColumnTimeFromColumns::ColumnTimeFromColumns(const Table& tbl, const String& sColName, ColumnPtr& ptr,const String& stCol, const String& templa) :
ColumnVirtual(tbl, sColName, ptr, DomainValueRangeStruct(Domain("time")), Table()), 
stringColumn(stCol), 
formatTemplate(templa)
{
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
	fNeedFreeze = true;
	ColumnVirtual::Replace(sExpression());
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
	if ( as.size() > 2) {
		String year = as[0];
		String month = as[1];
		String day = as[2];
		String hours = as[3];
		String minutes = as[4];
		String seconds = as[5];
		return new ColumnTimeFromColumns(tbl, sColName, ptr, year, month, day, hours, minutes, seconds);
	} else {
		return new ColumnTimeFromColumns(tbl, sColName, ptr,as[0], as[1]);
	}
}

String ColumnTimeFromColumns::sExpression() const
{
	if ( stringColumn.size() == 0)
		return String("ColumnTimeFromColumns=(%S,%S,%S,%S,%S,%S)",year, month, day, hour, minutes, seconds);
	return String("ColumnTimeFromColumns=(%S,%S)",stringColumn, formatTemplate);

}

void ColumnTimeFromColumns::Store()
{
	ColumnVirtual::Store();
	if ( stringColumn.size() == 0) {
		WriteElement(sSection().c_str(), "Year", year);
		WriteElement(sSection().c_str(), "Month", month);
		WriteElement(sSection().c_str(), "Day", day);
		WriteElement(sSection().c_str(), "Hour", hour);
		WriteElement(sSection().c_str(), "Minutes", minutes);
		WriteElement(sSection().c_str(), "Seconds", seconds);
	} 
	else {
		WriteElement(sSection().c_str(), "String", stringColumn);
		WriteElement(sSection().c_str(), "Template", formatTemplate);

	}
}

void ColumnTimeFromColumns::Replace(const String& sExpr)
{
	ColumnVirtual::Replace(sExpr);
	//fFrozen = false;
}

bool ColumnTimeFromColumns::isNumber(String & str)
{
	bool fRet = true;
	for (int i = 0; i < str.size(); ++i)
		fRet = fRet && ((str[i] >= '0' && str[i] <= '9') || (str[i] == '.'));
	return fRet;
}

bool ColumnTimeFromColumns::fFreezing()
{
	Table tbl(ptr.fnTbl);
	vector<ILWIS::Time> times;
	double step = 0;
	double rmax=-1e100, rmin=1e100;
	if ( stringColumn.size() != 0) {
		ILWIS::TimeParser parser (formatTemplate.sUnQuote());
		Column col = tbl->col(stringColumn);
		for(int i=1; i <= iRecs(); ++i) {		
			String val = col->sValue(i);
			ILWIS::Time ti(val, parser);
			if ( ti.isValid()) {
				times.push_back(ti);
				step = 0;
			} else {
				times.push_back(tUNDEF);
			}
			rmax = max((double)ti,rmax);
			if ( ti != tUNDEF)
				rmin = min((double)ti, rmin);
		}
	}else {
		int dyear = -4713, dmonth=1, dday=1, dhours=0, dminutes=0;
		double dseconds = 0;
		Column colYear, colMonth, colDays, colHours, colMinutes, colSeconds;
		if ( year != "" ) {
			if (isNumber(year))
				dyear = year.rVal();
			else
				colYear = tbl->col(year);
		}
		if ( month != "" ) {
			if (isNumber(month))
				dmonth = month.rVal();
			else
				colMonth = tbl->col(month);
		}
		if ( day != "" ) {
			if (isNumber(day))
				dday = day.rVal();
			else
				colDays = tbl->col(day);
		}
		if ( hour != "" ) {
			if (isNumber(hour))
				dhours = hour.rVal();
			else
				colHours = tbl->col(hour);
		}
		if ( minutes != "" ) {
			if (isNumber(minutes))
				dminutes = minutes.rVal();
			else
				colMinutes = tbl->col(minutes);
		}
		if ( seconds != "" ) {
			if (isNumber(seconds))
				dseconds = seconds.rVal();
			else
				colSeconds = tbl->col(seconds);
		}

		for(int i=1; i <= iRecs(); ++i) {
			int tyear = dyear, tmonth=dmonth, tday=dday,thours=dhours, tminutes=dminutes;
			double tseconds=dseconds;
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
			times.push_back(ti);
			rmax = max((double)ti,rmax);
			if ( (double)ti != rUNDEF)
				rmin = min((double)ti, rmin);

		}
		step = 1;
		if ( colMinutes.fValid() || colSeconds.fValid() || colHours.fValid()) // || isNumber(hour) || isNumber(minutes) || isNumber(seconds)
			step = 0;
	}
	Domain dm;
	dm.SetPointer( new DomainTime(FileName(ptr.sNam,".dom"),ILWIS::TimeInterval(rmin, rmax),step == 0 ? ILWIS::Time::mDATETIME : ILWIS::Time::mDATE));
	ptr.SetDomainValueRangeStruct(dm);
	CreateColumnStore();
	for(int i=1; i <= iRecs(); ++i) {
		pcs->PutVal(i, times[i-1]);
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




