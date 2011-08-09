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
/* ColumnCumulative
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
#include "Engine\Base\Algorithm\Qsort.h"


const char* ColumnCumulative::sSyntax() { return "ColumnCum(col)\ncum(col)\nlumnCum(col,sortcol)\ncum(col,sortcol)"; }

IlwisObjectPtr * createColumnCumulative(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "") {
		String *sCol = (String *)parms[0];
		DomainValueRangeStruct *dvs = (DomainValueRangeStruct *)parms[1];
		return (IlwisObjectPtr *)ColumnCumulative::create(Table(fn), *sCol, (ColumnPtr &)ptr, sExpr, *dvs);
	}
	else {
		String *sCol = (String *)parms[0];
		return (IlwisObjectPtr *)new ColumnCumulative(Table(fn), *sCol, (ColumnPtr &)ptr);
	}
}

ColumnCumulative::ColumnCumulative(const Table& tbl, const String& sColName, ColumnPtr& ptr)
: ColumnVirtual(tbl, sColName, ptr)
{
  String s;
  ReadElement(sSection().c_str(), "InputColumn", s);
  if (s.length())
    colInp = Column(s, tbl);
  objdep.Add(colInp.ptr());
  ReadElement(sSection().c_str(), "SortColumn", s);
  if (s.length()) {
    colSort = Column(s, tbl);
    objdep.Add(colSort.ptr());
		}
  fNeedFreeze = true;
}

ColumnCumulative::ColumnCumulative(const Table& tbl, const String& sColName, ColumnPtr& ptr, 
                                   const DomainValueRangeStruct& dvs, const Column& colInput, const Column& colSorting)
: ColumnVirtual(tbl, sColName, ptr, dvs, Table()), colInp(colInput), colSort(colSorting)
{
  if (!colInp->fValues())
    ValueDomainError(colInp->dm()->sName(true, fnObj.sPath()), sName(), errColumnCumulative);
  if (!dm().fValid()) {
//    SetDomainValueRangeStruct(colInp->dvrs());
    ValueRange vr;
    RangeReal rr = colInp->rrMinMax();
    if (rr.fValid())
      if (rr.rLo() >= 0)
        vr = ValueRange(0, rr.rHi()*iRecs(), colInp->dvrs().rStep());
    ptr.SetDomainValueRangeStruct(DomainValueRangeStruct(Domain("value"), vr));
  } 
  objdep.Add(colInp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = true;
  ColumnVirtual::Replace(sExpression());
}

ColumnCumulative* ColumnCumulative::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
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
  if ((iParms != 1) && (iParms != 2))
    ExpressionError(sExpr, sSyntax());
  Column colInp;
  if (tblSearch.fValid()) {
    try {
      colInp = Column(as[0], tblSearch);
    }
    catch (const ErrorObject&) {
    }
  }
  if (!colInp.fValid()) {
    try {
      colInp = Column(as[0], tbl);
    }
    catch (const ErrorObject&) {
    }
  }  
  if (!colInp.fValid()) 
    ColumnNotFoundError(tbl->fnObj, as[0]);
	Column colSort;
	if (iParms == 2) {
		if (tblSearch.fValid()) {
			try {
				colSort = Column(as[1], tblSearch);
			}
			catch (const ErrorObject&) {
			}
		}
		if (!colSort.fValid()) {
			try {
				colSort = Column(as[1], tbl);
			}
			catch (const ErrorObject&) {
			}
		}  
		if (!colSort.fValid()) 
			ColumnNotFoundError(tbl->fnObj, as[1]);
	}
  return new ColumnCumulative(tbl, sColName, ptr, dvs, colInp, colSort);
}

String ColumnCumulative::sExpression() const
{
  String sColInp;
  if (colInp->fnTbl.sFullName() != ptr.fnTbl.sFullName())
    sColInp = colInp->sTableAndColumnName(fnObj.sPath());
  else
    sColInp = colInp->sNameQuoted();
  if (!colSort.fValid())
    return String("cum(%S)", sColInp);
	String sColSort;
  if (colSort->fnTbl.sFullName() != ptr.fnTbl.sFullName())
    sColSort = colSort->sTableAndColumnName(fnObj.sPath());
  else
    sColSort = colSort->sNameQuoted();
  return String("cum(%S,%S)", sColInp, sColSort);
}

void ColumnCumulative::Store()
{
  ColumnVirtual::Store();
  WriteEntry("InputColumn", colInp->sTableAndColumnName(fnObj.sPath()));
	if (colSort.fValid())
    WriteEntry("SortColumn", colSort->sTableAndColumnName(fnObj.sPath()));
}

void ColumnCumulative::Replace(const String& sExpr)
{
  ColumnVirtual::Replace(sExpr);
//fFrozen = false;
}

struct si {
	String s;
	int ind;
};

struct sr {
	double r;
	int ind;
};


bool fLessS(long i, long j, void* p)
{
	vector<si>* v = static_cast< vector<si>* >(p);
	return (*v)[i].s < (*v)[j].s;
}

void SwapS(long i, long j, void* p)
{
	vector<si>* v = static_cast< vector<si>* >(p);
	si sih = (*v)[i];
	(*v)[i] = (*v)[j];
	(*v)[j] = sih;
}

bool fLessR(long i, long j, void* p)
{
	vector<sr>* v = static_cast< vector<sr>* >(p);
	return (*v)[i].r < (*v)[j].r;
}

void SwapR(long i, long j, void* p)
{
	vector<sr>* v = static_cast< vector<sr>* >(p);
	sr srh = (*v)[i];
	(*v)[i] = (*v)[j];
	(*v)[j] = srh;
}

bool ColumnCumulative::fFreezing()
{
  double rCum = 0;
	if (!colSort.fValid()) {
    DomainSort* pdsrt = ptr.dmKey()->pdsrt();
    if (0 == pdsrt) {
      for (long i=colInp->iOffset(); i < colInp->iOffset()+colInp->iRecs(); ++i) {
        double rVal = colInp->rValue(i);
        if (rVal != rUNDEF) {
          rCum += rVal;
					pcs->PutVal(i, rCum);
				}
				else 
					pcs->PutVal(i, rUNDEF);
			}  
		}
		else {
			long iRecs = pdsrt->iSize();
			for (long i=1; i <= iRecs; ++i) {
				long iRec = pdsrt->iKey(i);
				double rVal = colInp->rValue(iRec);
				if (rVal != rUNDEF) {
					rCum += rVal;
					pcs->PutVal(iRec, rCum);
				}
				else 
					pcs->PutVal(iRec, rUNDEF);
			}
		}
	}
	else {
		if (colSort->fValues()) {
			vector<sr> aSort(colSort->iRecs());
			for (int i=0; i < aSort.size(); i++) {
				aSort[i].r = colSort->rValue(i+colSort->iOffset());
				aSort[i].ind = i + colSort->iOffset();
			}
			QuickSort(0, aSort.size()-1, &fLessR, &SwapR, &aSort);
			for (int i=colInp->iOffset(); i < colInp->iOffset()+colInp->iRecs(); ++i) {
				int ind = aSort[i-colInp->iOffset()].ind;
				double rVal = colInp->rValue(ind);
				if (rVal != rUNDEF) {
						rCum += rVal;
						pcs->PutVal(ind, rCum);
				}
				else 
					pcs->PutVal(ind, rUNDEF);
			}  
		}
		else {
			vector<si> aSort(colSort->iRecs());
			for (int i=0; i < aSort.size(); i++) {
				aSort[i].s = colSort->sValue(i+colSort->iOffset());
				aSort[i].ind = i + colSort->iOffset();
			}
			QuickSort(0, aSort.size()-1, &fLessS, &SwapS, &aSort);
			for (int i=colInp->iOffset(); i < colInp->iOffset()+colInp->iRecs(); ++i) {
				int ind = aSort[i-colInp->iOffset()].ind;
				double rVal = colInp->rValue(ind);
				if (rVal != rUNDEF) {
						rCum += rVal;
						pcs->PutVal(ind, rCum);
				}
				else 
					pcs->PutVal(ind, rUNDEF);
			}  
		}
	}
  return true;
}

ColumnCumulative::~ColumnCumulative()
{
}

bool ColumnCumulative::fDomainChangeable() const
{
  return true;
}

bool ColumnCumulative::fValueRangeChangeable() const
{
  return fDomainChangeable();
}




