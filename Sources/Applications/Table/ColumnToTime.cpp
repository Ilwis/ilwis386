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
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblstore.h"
#include "Applications\Table\COLumntotime.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"


const char* ColumnToTime::sSyntax() 
{ return "ColumnToTime(SoourceColumn)"; }

IlwisObjectPtr * createColumnToTime(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnToTime::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnToTime(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

ColumnToTime::ColumnToTime(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr),
tblOther(Table())
{
	String s;
	ReadElement(sSection().c_str(), "ColumnToTime", s);
	colSource = Column(s, tbl);
}

ColumnToTime::ColumnToTime(const Table& tbl, const String& sColName, ColumnPtr& ptr,
						   const DomainValueRangeStruct& dvs, 
						   const Column& colS, const Table& _tblOther)
						   : ColumnVirtual(tbl, sColName, ptr, dvs, Table()), colSource(colS), tblOther(_tblOther)
{
	ptr.SetDomainValueRangeStruct(Domain("time"));
	objdep.Add(colSource.ptr());
	fNeedFreeze = true;
	ColumnVirtual::Replace(sExpression());
}

ColumnVirtual* ColumnToTime::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
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
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 1)
		ExpressionError(sExpr, sSyntax());
	Column colS;
	if (tblSearch.fValid()) {
		try {
			colS = Column(as[0], tblSearch);
		}
		catch (const ErrorObject&) {
		}
	}
	if (!colS.fValid()) {
		try {
			colS = Column(as[0], tbl);
		}
		catch (const ErrorObject&) {
		}
	}  
	if (!colS.fValid()) 
		ColumnNotFoundError(tbl->fnObj, as[0]);
	return new ColumnToTime(tbl, sColName, ptr, dvs, colS, tblSearch);
}

String ColumnToTime::sExpression() const
{
	return String("ColumnToTime(%S)",colSource->sName());
}

void ColumnToTime::Store()
{
	ColumnVirtual::Store();
	WriteEntry("ColumnToTime", colSource->sTableAndColumnName(fnObj.sPath()));
}


bool ColumnToTime::fFreezing()
{
	if ( !colSource.fValid())
		return false;

	for(int i=1; i < colSource->iRecs(); ++i) {
		String v = colSource->sValue(i);
		ILWIS::Time tim(v);
		ptr.PutVal(i, tim);
	}
	ptr.Store();
	return true;
}

ColumnToTime::~ColumnToTime()
{
}

bool ColumnToTime::fDomainChangeable() const
{
  return false;
}

bool ColumnToTime::fValueRangeChangeable() const
{
  return fDomainChangeable();
}







