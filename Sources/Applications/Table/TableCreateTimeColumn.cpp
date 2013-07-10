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
// TableCreateTimeColumn.cpp: implementation of the TableCreateTimeColumn class.
//
//////////////////////////////////////////////////////////////////////
#include "headers\toolspch.h"
#include "Engine\Applications\TBLVIRT.H"
#include "Applications\Table\TableCreateTimeColumn.h"
#include "Headers\Hs\maplist.hs"

using namespace std;

namespace {
  const char* sSyntax() 
  {
    return "TableCreateTimeColumn(inputmaplist,parseformat)";
  }
}

IlwisObjectPtr * createTableCreateTimeColumn(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *) TableCreateTimeColumn::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableCreateTimeColumn(fn, (TablePtr &) ptr);
}

TableCreateTimeColumn::TableCreateTimeColumn(const FileName& fn, TablePtr& ptr)
: TableVirtual(fn, ptr)
{
  ReadElement("TableCreateTimeColumn", "TimeParseExpression", sParseExpr);
  ReadElement("TableCreateTimeColumn", "MapList", mplInput);
  Init();
}

TableCreateTimeColumn::TableCreateTimeColumn(const FileName& fn, TablePtr& ptr,const MapList& mplIn, const String& sExpr)
: TableVirtual(fn, ptr, false), sParseExpr(sExpr), mplInput(mplIn)
{
	ptr.SetDomain(Domain("none"));
}

TableCreateTimeColumn *TableCreateTimeColumn::create(const FileName& fn, TablePtr& ptr, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2)
    ExpressionError(sExpr, sSyntax());  
  CheckExpression(as[1]);
  FileName fnIn(as[0],".mpl");
  MapList mplIn(fnIn);
  String exp = as[1].sSub(1,as[1].size() - 2);

  return new TableCreateTimeColumn(fn, ptr,mplIn, exp);

}

TableCreateTimeColumn::~TableCreateTimeColumn()
{
}

void TableCreateTimeColumn::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableCreateTimeColumn");
  WriteElement("TableCreateTimeColumn", "TimeParseExpression", sParseExpr);
  WriteElement("TableCreateTimeColumn", "MapList", mplInput);
 
}

void TableCreateTimeColumn::Init()
{
  sFreezeTitle = "TableCreateTimeColumn";
}

String TableCreateTimeColumn::sExpression() const
{
	String sExpr ("TableCreateTimeColumn(%S,%S)", mplInput->fnObj.sRelative(), sParseExpr);
    return sExpr;
}

void TableCreateTimeColumn::CheckExpression(const String& parseExpr)
{
	
}

bool TableCreateTimeColumn::fFreezing()
{ 
  try {
	Array<String> parts;
	Split(sParseExpr, parts,";");
	
	std::map<String,String> timeParts;
	for(int i = 0; i < parts.size(); ++i) {
		String name = parts[i].sHead("=").toLower();
		String format = parts[i].sTail("=");
		timeParts[name] = format;
	}
	vector<ILWIS::Time> isoStrings;
	ILWIS::TimeInterval interval;
	bool useDate = true;
	int oldYear = iUNDEF;
	int offset = 0;
	for(int i = 0; i < mplInput->iSize(); ++i) {
		Map mp = mplInput[mplInput->iLower() + i];
		String name = mp->sName();
		int year=iUNDEF,month=iUNDEF,day=iUNDEF,hour=iUNDEF,minute=iUNDEF;
		ILWIS::Time t = ILWIS::Time::now();
		year = t.get(ILWIS::Time::tpYEAR); //  default
		for(std::map<String,String>::iterator cur=timeParts.begin(); cur != timeParts.end(); ++cur) {
			String part = (*cur).first;
			if (  part == "year") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int base = iUNDEF;;
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				if ( parts.size() == 3)
					base = parts[2].iVal();
				int nyear = name.sSub(start, end - start + 1 ).iVal();
				if ( nyear != iUNDEF) {
					year = nyear;
				}

				if ( base != iUNDEF) {
					if ( oldYear > year && year < 100) {
						offset = 100;
					}
					oldYear = year;
					year += base + offset;

				}
			}
			if ( part == "month") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				month = name.sSub(start, end - start + 1).iVal();
				if ( month == iUNDEF)
					throw ErrorObject(TR("Invalid indexes for determining time"));
			}
			if ( part == "day") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				day = name.sSub(start, end - start + 1).iVal();
				if ( day == iUNDEF)
					throw ErrorObject(TR("Invalid indexes for determining time"));
			}
			if ( part == "hour") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				hour = name.sSub(start, end - start + 1).iVal();
				if ( hour == iUNDEF)
					throw ErrorObject(TR("Invalid indexes for determining time"));
			}
			if ( part == "minute") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				minute = name.sSub(start, end - start + 1).iVal();
				if ( minute == iUNDEF)
					throw ErrorObject(TR("Invalid indexes for determining time"));
			}
			if ( part == "dekad") {
				Array<String> parts;
				Split((*cur).second, parts,":");
				int start = parts[0].iVal();
				int end = parts[1].iVal();
				int dek = name.sSub(start, end - start + 1).iVal();
				if ( dek == iUNDEF)
					throw ErrorObject(TR("Invalid indexes for determining time"));
				day = ((dek - 1) % 3) * 10 + 1;
				month = 1 + (dek - 1) / 3;
			}
		}
		if ( month == 2 && day > 28)
			day = 28;
		String format("%d",year);
		if ( month != iUNDEF) {
			format += String("%02d", month);
		}
		if ( day != iUNDEF) {
			format += String("%02d", day);
		}
		if ( hour != iUNDEF) {
			format += String("%02d", hour);
		}
		if ( minute != iUNDEF) {
			format += String("%02d", minute);
		}
		
		ILWIS::Time itime(format);
		interval += itime;
		isoStrings.push_back(itime);
		if (hour != iUNDEF && minute != iUNDEF)
			useDate = false;
	}
	ptr.iRecNew(mplInput->iSize());
	Domain dm(FileName(mplInput->fnObj, ".dom"),interval, useDate ? ILWIS::Time::mDATE : ILWIS::Time::mDATETIME);
	Table tbl;
	tbl.SetPointer(&ptr);
	Column col(tbl, "Time",dm);
	for(int i = 0; i < tbl->iRecs(); ++i) {
		col->PutVal(i+1,isoStrings.at(i));
	}
  }
  catch (ErrorObject& err)
  {
    err.Show();
     return false;
  }
  return true;
}
