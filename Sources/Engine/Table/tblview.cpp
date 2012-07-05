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
/* $Log: /ILWIS 3.0/TableWindow/tblview.cpp $
 * 
 * 28    12/06/01 9:45a Martin
 * added try catch to catch errors fro foreign format at an appropriate
 * place
 * 
 * 27    25-10-01 19:55 Retsios
 * Prevent ILWIS from crashing when some columns are invalid (due to
 * invalid odf)
 * 
 * 26    3/20/01 20:38 Retsios
 * Don't confuse records with rows: compare records to records and rows to
 * rows (rows == netto_size)
 * 
 * 25    14/02/01 16:12 Willem
 * In iCol() unquote the column name before comparing it with internal
 * column list
 * 
 * 24    14-02-01 10:29a Martin
 * partly solved sorting bug. Sorting is now correct and table looks
 * correct, but the crash bug is still there (if you close tablewindow)
 * 
 * 23    12-02-01 9:00a Martin
 * protection against errors in the ODF. Prevent a crash of ilwis if some
 * columns do not exist (should not happen, but ilwis should not crash
 * anyway)
 * 
 * 22    10-11-00 3:30p Martin
 * changed format of the sum field of a column (now uses correct way to
 * display)
 * 
 * 21    28-07-00 14:15 Koolhoven
 * bug solved: when a table contains more records than the netto size of
 * the domain, this too high number also became the nr of recs of the
 * TableView
 * 
 * 20    5/10/00 5:43p Wind
 * index out of range for cv(int i)
 * 
 * 19    4/11/00 3:22p Wind
 * added fReadOnly that calls TablePtr::fReadOnly()  (bug 1367)
 * 
 * 18    3/22/00 5:11p Wind
 * two bugs and change in text on row button
 * 
 * 17    15-03-00 9:50 Wind
 * added function fnTbl()
 * check on invalid range for minmax of column
 * 
 * 16    14-03-00 11:13 Wind
 * - added command sorton colname -sortorder=ascending|descending
 * - added to context sensitive menu of column
 * - added sort order to sort column dialogue
 * 
 * 15    13-03-00 15:17 Wind
 * added mean, stddev and sum per column for display in tablewindow
 * statistics pane
 * 
 * 14    1-03-00 12:56 Wind
 * allow that member TableView* of ColumnView may be empty
 * 
 * 13    26-01-00 10:34a Martin
 * bounds check when retrieving an column through an index
 * 
 * 12    26-01-00 9:04a Martin
 * bounds checking of arrays now correct
 * 
 * 11    5-01-00 18:07 Wind
 * changed CheckNrRecs
 * 
 * 10    10-12-99 13:18 Wind
 * made laRecRow and laRowRec zero based
 * 
 * 9     7-12-99 18:24 Koolhoven
 * Show minimum and maximum of columns in TableSummaryPaneView
 * 
 * 8     29-10-99 12:51 Wind
 * case sensitive stuff
 * 
 * 7     10-09-99 16:44 Koolhoven
 * - width of row buttons also correct after sorting
 * - contents of row button should change with sorting
 * 
 * 6     9/08/99 10:29a Wind
 * adpated to use of quoted file names and column names
 * 
 * 5     30-07-99 10:07 Koolhoven
 * In CheckNrRecs() also set _iRecs
 * 
 * 4     21-06-99 16:47 Koolhoven
 * 
 * 3     6/15/99 4:32p Visser
 * // -> /*
 * 
 * 2     6/15/99 4:28p Visser
 * TableView has been moved to TableWindow
// Revision 1.10  1998/09/16 17:25:30  Wim
// 22beta2
//
// Revision 1.9  1997/09/19 16:22:06  Wim
// Do not give "true' in constructor to tell that a new table should be created
//
// Revision 1.8  1997-09-17 16:07:36+02  Wim
// Do not read more columns than there are in the TableView section
//
// Revision 1.7  1997-09-17 09:34:10+02  Wim
// Saving and reloading of column sequence implemented
//
// Revision 1.6  1997-09-16 22:19:06+02  Wim
// Changed Storing and updating of tableview
// Column widths etc are stored.
// TableView only if extension is .tvw
//
// Revision 1.5  1997-09-12 20:19:39+02  Wim
// Use ac.ind() instead of ac[] to get const Column& instead of Column
// Removed ".tvw" in constructor
//
// Revision 1.4  1997-09-12 16:30:55+02  Wim
// moved col() functions from .h file to .c file
//
// Revision 1.3  1997-08-28 10:56:43+02  Wim
// Removed all references to ".tvw" extension
//
// Revision 1.2  1997-07-25 12:51:29+02  Wim
// Updated added
//
/* TableViTew, ColumnView
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   19 Sep 97    6:17 pm
*/
#define TBLVIEW_C
#include "Engine\Table\tblview.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Engine\Table\tbl2dim.h"

Column colUNDEF;
ColumnView colvUNDEF;

ColumnView::ColumnView()
{
  iWidth = iDec = 0;
  tvw = 0;
}

ColumnView::ColumnView(const TableView* view,
                       const Table& tbl, const String& sColName)
: Column(tbl, sColName)
{
	if (ptr() == NULL) return;
  tvw = const_cast<TableView*>(view);
  String sSection = ptr()->sSection();
  iWidth = shortConv(tvw->iReadElement(sSection.c_str(), "Width"));
  if (iWidth < 0) {
    iWidth = ptr()->dvrs().iWidth();
    int iLen = sColName.length();
    if (iLen > iWidth)
      iWidth = iLen;
  }
  if (iWidth > 100)
    iWidth = 100;
  iDec = 0;
  if (ptr()->fRealValues() || ptr()->fCoords()) {
    iDec = shortConv(tvw->iReadElement(sSection.c_str(), "Decimals"));
    if (iDec < 0)
      iDec = ptr()->dvrs().iDec();
  }
  tvw->ReadElement(sSection.c_str(), "Title", sTitle);
  ptr()->MakeUsable();
//  if (ptr()->pcv())
//    if (!ptr()->pcv()->fUsable())
//      ptr()->pcv()->Freeze();
}

ColumnView::ColumnView(const TableView* view, const Column& col)
: Column(col), iWidth(iUNDEF)
{
  tvw = const_cast<TableView*>(view);
	if ( ptr() == NULL) return;
	
  String sSection = ptr()->sSection();
  if (0 != tvw)
    iWidth = shortConv(tvw->iReadElement(sSection.c_str(), "Width"));
  if (iWidth < 0) {
    iWidth = ptr()->dvrs().iWidth();
    int iLen = col->sName().length();
    if (iLen > iWidth)
      iWidth = iLen;
  }
  if (iWidth > 200)
    iWidth = 200;
  iDec = 0;
  if (ptr()->fRealValues() || ptr()->fCoords()) {
    if (0 != tvw)
      iDec = shortConv(tvw->iReadElement(sSection.c_str(), "Decimals"));
    else
      iDec = iUNDEF;
    if (iDec < 0)
      iDec = ptr()->dvrs().iDec();
  }
  if (0 != tvw)
    tvw->ReadElement(sSection.c_str(), "Title", sTitle);
  ptr()->MakeUsable();
//  if (ptr()->pcv())
//    if (!ptr()->pcv()->fUsable())
//      ptr()->pcv()->Freeze();
}

ColumnView::ColumnView(const ColumnView& col)
: Column(col)
{
  tvw = col.tvw;
  iWidth = col.iWidth;
  iDec = col.iDec;
  sTitle = col.sTitle;
}

void ColumnView::operator = (const ColumnView& col)
{
  tvw = col.tvw;
  Column::operator = (col);
  iWidth = col.iWidth;
  iDec = col.iDec;
  sTitle = col.sTitle;
}

void ColumnView::Store()
{
  if (0 == tvw || ptr() == NULL)
    return;
  String sSection = ptr()->sSection();
  tvw->WriteElement(sSection.c_str(), "Width", (long)iWidth);
  if (ptr()->fRealValues() || ptr()->fCoords())
    tvw->WriteElement(sSection.c_str(), "Decimals", (long)iDec);
  if (sTitle.length() > 0)
    tvw->WriteElement(sSection.c_str(), "Title", sTitle);
}

TableView::TableView(const FileName& fn)
: TablePtr(fn, String()),
  ac(iCols())
{
  FileName fnTbl;
  ReadElement("TableView", "Table", fnTbl);
  tbl = Table(fnTbl);
//  pts = tbl->pts();
  _iCols = tbl->iCols();
  _dm = tbl->dm();
  _iRecs = tbl->iRecs();
  DomainSort* ds = _dm->pdsrt();
  if (ds)
    _iRecs = ds->iSize();
  cvKey().iWidth = dm()->iWidth();
  if (_dm->pdnone()) {
    Table2DimPtr* t2d = dynamic_cast<Table2DimPtr*>(tbl.ptr());
    if (t2d) {
      cvKey().iWidth = 1 + t2d->dm1()->iWidth() + t2d->dm2()->iWidth();
    }
    else {
      int iW = 0;
      for (long r = _iRecs; r >= 1; r /= 10) iW++;
      cvKey().iWidth = max(2,iW);
    }
  }
  for (short c = 0; c < iCols(); ++c) {
    String sEntry("Col%i", c);
    String sColName;
    ReadElement("TableView", sEntry.c_str(), sColName);
    ac[c] = ColumnView(this, tbl->col(sColName));
  }
  ApplySorting();
}

TableView::TableView(const Table& t)
: TablePtr(t->fnObj, String()), tbl(t),
  ac(t->iCols())
{
  fChanged = false; // was set to true by the constructor
//  fErase = true;
  init(t.ptr());
}

TableView::TableView(TablePtr* ptr)
: TablePtr(ptr->fnObj, String()),
  ac(ptr->iCols())
{
  tbl.SetPointer(ptr);
  init(ptr);
}

TableView::TableView(const Domain& dom)
: TablePtr(dom->fnObj, String()),
  ac(dynamic_cast<DomainSort*>(dom.ptr())->iCols())
{
  fErase = true;
  _dm = dom;
  DomainSort* ds = dynamic_cast<DomainSort*>(dom.ptr());
//  pts = ds->pts();
  _iCols = ds->iCols();
  _iRecs = ds->iSize();
  _iOffset = 1;
  short c;
  int i;
  cvKey().iWidth = max(4, dom->iWidth());
  for (c = 0, i = 0; c < iCols(); ++i) {
    Column col = ds->col(i);
    if (col.ptr())
      ac[c++] = ColumnView(this, col);
  }
}


TableView::~TableView()
{
//  delete &ac;
  if (fErase)
    _unlink(fnObj.sFullName().c_str());
//  if (fChanged)
//    tbl->fChanged = true;

}

void TableView::init(TablePtr* ptr)
{
//  pts = ptr->pts();
  so = soASCENDING;
  _iCols = ptr->iCols();
  _iRecs = ptr->iRecs();
  _iOffset = ptr->iOffset();
  _dm = ptr->dm();
  DomainSort* ds = _dm->pdsrt();
  if (ds)
    _iRecs = ds->iSize();
  cvKey().iWidth = max(4, dm()->iWidth());
  if (_dm->pdnone()) {
    Table2DimPtr* t2d = dynamic_cast<Table2DimPtr*>(tbl.ptr());
    if (t2d) {
      cvKey().iWidth = 1 + t2d->dm1()->iWidth() + t2d->dm2()->iWidth();
    }
    else {
      int iW = 0;
      for (long r = _iRecs; r >= 1; r /= 10) iW++;
      cvKey().iWidth = max(4, iW);
    }
  }
  for (short c = 0; c < iCols(); ++c)
    ac[c] = ColumnView(this, ptr->col(c));
  // read sorting
  String sSortCol;
  ReadElement("TableView", "SortColumn", sSortCol);
  ColumnView& cvSort = cv(sSortCol);
  if (cvSort.fValid()) {
    _cvKey = cvSort;
    so = soASCENDING;
    String sSortOrder;
    ReadElement("TableView", "SortOrder", sSortOrder);
    if (fCIStrEqual(sSortOrder, "descending"))
      so = soDESCENDING; 
  }

  ApplySorting();
  _fReadOnly = ptr->fReadOnly();

  int i = 0;
  long iCols;
  ReadElement("TableView", "Columns", iCols);
  for (short c = 0; c < iCols; ++c) {
    String sEntry("Col%i", c);
    String sColName;
    ReadElement("TableView", sEntry.c_str(), sColName);
// Jelle: is solved in ReadElement and WriteElement:
//    sColName = sColName.sQuote(); // ReadPrivateProfileString strips single quotes
    int iC = iCol(sColName);
    if (iC < 0)
      continue;
    ColumnView cv = ac[iC];
    ac[iC] = ColumnView(); // next call doesn't call destructor of ColumnView
    ac.Remove(iC,1);
    ac.Insert(i,1);
    ac[i] = cv;
    i++;
  }
}

void TableView::Store()
{
  if (fCIStrEqual(fnObj.sExt, ".tvw")) {
    TablePtr::Store();
    WriteElement("Table", "Type", "TableView");
    WriteElement("TableView", "Table", tbl);
  }
  if (cvKey().fValid()) {
    WriteElement("TableView", "SortColumn", cvKey()->sName());
    WriteElement("TableView", "SortOrder", so == soASCENDING ? "ascending" : "descending");
  }
	else {
    WriteElement("TableView", "SortColumn", (char*)0);
    WriteElement("TableView", "SortOrder", (char*)0);
  }
  WriteElement("TableView", "Columns", iCols());
  for (int c = 0; c < iCols(); ++c)
	{
		if (ac[c].fValid())
		{
			String sEntry("Col%i", c);
			WriteElement("TableView", sEntry.c_str(), ac[c]->sNameQuoted());
			ac[c].Store();
		}
  }
}

short TableView::iCol(const String& sName) const
{
	String sUnqName = sName.sUnQuote();
	for (short i = 0; i < iCols(); i++)
	{
		if (ac[i].ptr() != 0 &&  ac[i]->sName() == sUnqName)
			return i;
	}		
	return -1;
}

short TableView::MoveCol(short iFrom, short iTo)
{
	if (iFrom < 0)
		return 0;
  ColumnView cv = ac[iFrom];
  ac[iFrom] = ColumnView(); // next call doesn't call destructor of ColumnView
  ac.Remove(iFrom,1);
  ac.Insert(iTo,1);
  ac[iTo] = cv;
  Store();
  return 0;
}

String TableView::sRow(long iRow) const
{
  Table2DimPtr* t2d = dynamic_cast<Table2DimPtr*>(tbl.ptr());
  if (t2d) {
    String s("%S|%S", t2d->sKey1(iRec(iRow)), t2d->sKey2(iRec(iRow)));
    return s;
  }
//	if (cvKey().ptr()) 
//		return cvKey()->sValue(iRec(iRow), cvKey().iWidth, cvKey().iDec);
  if (0 != dm()->pdnone()) {
      String s("%*li", cvKey().iWidth, iRec(iRow));
      return s;
  }
  else
    return dm()->sValueByRaw(iRec(iRow), cvKey().iWidth, cvKey().iDec);
}

String TableView::sValue(short iCol, long iRow, short iWid, short iDec) const
{
	if (ac[iCol].ptr() == NULL)
		return "";
  if (iWid == -1)
    iWid = ac[iCol].iWidth;
  if (iDec == -1)
    iDec = ac[iCol].iDec;
  return ac[iCol]->sValue(iRec(iRow),iWid,iDec);
}

double TableView::rValue(short iCol, long iRow) const
{
	if (ac[iCol].ptr() == NULL)	
		return rUNDEF;
  return ac[iCol]->rValue(iRec(iRow));
}

void TableView::PutVal(short iCol, long iRow, const String& s)
{
	try
	{
		if (ac[iCol].ptr() == NULL)
			return;
		ac[iCol]->PutVal(iRec(iRow), s);
		ac[iCol]->Updated();
		tbl->fChanged = true;
	}
	catch(const ErrorObject& err)
	{
		err.Show();
	}		
}

String TableView::sMinimum(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return "?";
	RangeReal rr = ac[iCol]->rrMinMax();
  int iWid = ac[iCol].iWidth;
  int iDec = ac[iCol].iDec;
  double rVal = rr.fValid() ? rr.rLo() : rUNDEF;
	return ac[iCol]->dvrs().sValue(rVal, iWid, iDec);
}

String TableView::sMaximum(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return "?";
	RangeReal rr = ac[iCol]->rrMinMax();
  int iWid = ac[iCol].iWidth;
  int iDec = ac[iCol].iDec;
  double rVal = rr.fValid() ? rr.rHi() : rUNDEF;
	return ac[iCol]->dvrs().sValue(rVal, iWid, iDec);
}

String TableView::sSum(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return "?";
	double r = ac[iCol]->rSum();
  int iWid = ac[iCol].iWidth;
  int iDec = ac[iCol].iDec;
	if ( r == rUNDEF)
		return "";
	
	String s("%*.*f", iWid, iDec, r);
	return s;
	//return ac[iCol]->dvrs().sValue(r, iWid, iDec);
}

String TableView::sMean(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return "?";
	double r = ac[iCol]->rMean();
  int iWid = ac[iCol].iWidth;
  int iDec = ac[iCol].iDec;
	return ac[iCol]->dvrs().sValue(r, iWid, iDec);
}

String TableView::sStdDev(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return "?";
	double r = ac[iCol]->rStdDev();
  int iWid = ac[iCol].iWidth;
  int iDec = ac[iCol].iDec;
	return ac[iCol]->dvrs().sValue(r, iWid, iDec);
}


bool TableView::fEditable(short iCol) const
{
	if (ac[iCol].ptr() == NULL)
		return false;
//  if (fDataReadOnly())
//    return false;
  return !ac[iCol]->fDataReadOnly();
}

void TableView::MakeUsable(short iCol)
{
	if ( ac[iCol].ptr() != NULL)
		ac[iCol]->MakeUsable();
}

void TableView::SetKey(short iCol)
{
  if (iCol < 0) {
    _cvKey = ColumnView();
	  cvKey().iWidth = max(4, dm()->iWidth());
	}
  else
    _cvKey = ac[iCol];
  Store(); // store sort info
  ApplySorting();
}

short TableView::iAddCol(const Column& col)
{
  String s = col->sName();
  short iRet = iCol(s);
  if (iRet == -1) {
    iRet = ac.iSize();
    ac.Append(1);
    ac[iRet] = ColumnView(this, col);
    ac[iRet].iWidth = col->dvrs().iWidth();
    ac[iRet].iDec = 0;
    if (col->fRealValues() || col->fCoords())
      ac[iRet].iDec = col->dvrs().iDec();
    _iCols += 1;
    Updated();
    tbl->AddCol(col);
    tbl->Store(); // to force writing of info in object definition file
    Store();
  }
  return iRet;
}

short TableView::iColNew(const String& s, const Domain& dm)
{
//  if (0 == pts)
//    return -1;
  short iCol = ac.iSize();
  ac.Append(1);
  Column col = tbl->colNew(s,dm);
  Updated();
  tbl->Updated();
  ac[iCol] = ColumnView(this, col);
  ac[iCol].iWidth = dm->iWidth();
  ac[iCol].iDec = 0;
  DomainValueReal* dvr = dm->pdvr();
  if (0 != dvr)
    ac[iCol].iDec = dvr->iDec();
  return iCol;
}

void TableView::RemoveCol(short iCol)
{
  tbl->RemoveCol(ac[iCol]);
  ac[iCol] = ColumnView(); // next call doesn't call destructor of ColumnView
  ac.Remove(iCol,1);
  _iCols -= 1;
  Updated();
  tbl->Updated();
}

long TableView::iRow(const String& sVal) const
{
  if (dm()->pdnone())
    return iRow(sVal.iVal());
  else if (0 == cvKey().ptr())
    return iRow(dm()->iRaw(sVal));
  else
    return 0; // search routine, see old TableView.
}

long TableView::iRow(long iRec) const
{
  if (0 == cvKey().ptr()) {
    DomainSort* ds = dynamic_cast<DomainSort*>(dm().ptr());
    if (ds)
      return ds->iOrd(iRec);
  }
  else {
//    if (iRec >= laRowRec.iLower() && iRec <= laRowRec.iUpper())
    if (iRec >= iOffset() && iRec <= iRecs()+iOffset()-1)
      return laRowRec[iRec];
  }
  return iRec + 1 - iOffset();
}

long TableView::iRec(long iRow) const
{
  if (0 == cvKey().ptr()) {
    DomainSort* ds = dynamic_cast<DomainSort*>(dm().ptr());
    if (ds)
      return ds->iKey(iRow);
  }
  else {
//    if (iRow >= laRecRow.iLower() && iRow <= laRecRow.iUpper())
    if (iRow >= 1 && iRow <= iNettoRecs())
      return laRecRow[iRow];
  }
  return iRow - 1 + iOffset();
}

void TableView::DeleteRec(long iStartRec, long iRecords)
{
	if ( cvKey().ptr() == 0 )
	{
		tbl->DeleteRec(iStartRec, iRecords);
 	}		
	else
	{
		iRecords = 1; // Delete only one rec til a solution for the renumbering of records is found
		for(int iR = iStartRec; iR < iStartRec + iRecords; ++iR)
		{
			long iTableRow = iRec(iR);
			tbl->DeleteRec(iTableRow, 1);
		}			
	}		
  tbl->Updated();
  _iRecs = tbl->iRecs();	
  _dm = tbl->dm();
  DomainSort* ds = _dm->pdsrt();
  if (ds)
    _iRecs = ds->iSize();
	if ( cvKey().ptr() != 0 )
			ApplySorting();
  Updated();	
}

long TableView::iRecNew(long iRecords)
{
  long iRet = tbl->iRecNew(iRecords);
  if (iRecords) {
    Updated();
    tbl->Updated();
  }
  _iRecs = tbl->iRecs();
  _dm = tbl->dm();
  DomainSort* ds = _dm->pdsrt();
  if (ds)
    _iRecs = ds->iSize();
  return iRet;
}

static void Swap(long i, long j, void* p)
{
  TableView* tv = static_cast<TableView*>(p);
  long h = tv->laRecRow[i];
  tv->laRecRow[i] = tv->laRecRow[j];
  tv->laRecRow[j] = h;
}

static bool fLessValue(long i, long j, void* p)
{
  TableView* tv = static_cast<TableView*>(p);
  long iVal = tv->cvKey()->iValue(tv->laRecRow[i]);
  if (iVal == iUNDEF)
    return false;
  long jVal = tv->cvKey()->iValue(tv->laRecRow[j]);
  if (jVal == iUNDEF)
    return true;
  return tv->so == TableView::soASCENDING ? iVal < jVal : iVal > jVal;
}

static bool fLessSort(long i, long j, void* p)
{
  TableView* tv = static_cast<TableView*>(p);
  long iVal = tv->cvKey()->iRaw(tv->laRecRow[i]);
  if (iVal == iUNDEF)
    return false;
  long jVal = tv->cvKey()->iRaw(tv->laRecRow[j]);
  if (jVal == iUNDEF)
    return true;
  DomainSort* ds = tv->cvKey()->dm()->pdsrt();
  return tv->so == TableView::soASCENDING ? ds->iOrd(iVal) < ds->iOrd(jVal) : ds->iOrd(iVal) > ds->iOrd(jVal);
}

//Jelle: _export is needed because rUNDEF is referenced in 'this' data segment
bool _export fLessRealTableView(long i, long j, void* p)
{
  TableView* tv = static_cast<TableView*>(p);
  double rVal1 = tv->cvKey()->rValue(tv->laRecRow[i]);
  if (rVal1 == rUNDEF)
       return false;
  double rVal2 = tv->cvKey()->rValue(tv->laRecRow[j]);
  if (rVal2 == rUNDEF)
    return true;
  return tv->so == TableView::soASCENDING ? rVal1 < rVal2 : rVal1 > rVal2;
}

static bool fLessString(long i, long j, void* p)
{
  TableView* tv = static_cast<TableView*>(p);
  String iVal = tv->cvKey()->sValue(tv->laRecRow[i],0);
  if (iVal == sUNDEF)
    return false;
  String jVal = tv->cvKey()->sValue(tv->laRecRow[j],0);
  if (jVal == sUNDEF)
    return true;
  return tv->so == TableView::soASCENDING ? iVal < jVal : iVal > jVal;
}

void TableView::ApplySorting()
{
	// Comment on the arrays (for future reference):
	// laRecRow returns (should return) a record given a row-nr
	// laRowRec returns (should return) a row nr given a record-nr
	// The number of rows excludes deleted records (= netto-size)
	// The number of records includes deleted records, and is thus more than #rows
  if (0 == cvKey().ptr()) {
    laRecRow.Resize(0);
    laRowRec.Resize(0);
  }
  else {
    long i;
//  laRecRow.Resize(iRecs(),1);
//  laRowRec.Resize(iRecs(),iOffset());
    laRecRow.Resize(iNettoRecs()+1);  // zero based now
    laRowRec.Resize(iRecs()+iOffset()); // zero based now
    DomainSort* ds = dynamic_cast<DomainSort*>(dm().ptr());
    if (ds)
      for (i = 1; i <= iNettoRecs(); ++i)
        laRecRow[i] = ds->iKey(i);
    else
      for (i = 1; i <= iNettoRecs(); ++i)
        laRecRow[i] = i - 1 + iOffset();
    DomainPtr* ptr = cvKey()->dm().ptr();
    if (ptr->pdsrt())
      QuickSort(1, iNettoRecs(), fLessSort, Swap, this);
//              (IlwisObjectPtrLessProc)&TableView::fLessSort,
//               (IlwisObjectPtrSwapProc)&TableView::Swap);
    else if (ptr->pdvr())
      QuickSort(1, iNettoRecs(), fLessRealTableView, Swap, this);
//    QuickSort(1, iRecs(),
//              (IlwisObjectPtrLessProc)&TableView::fLessReal,
//               (IlwisObjectPtrSwapProc)&TableView::Swap);
    else if (ptr->pdv())
      QuickSort(1, iNettoRecs(), fLessValue, Swap, this);
//    QuickSort(1, iRecs(),
//              (IlwisObjectPtrLessProc)&TableView::fLessValue,
//               (IlwisObjectPtrSwapProc)&TableView::Swap);
    else
      QuickSort(1, iNettoRecs(), fLessString, Swap, this);
//      QuickSort(1, iRecs(),
//                (IlwisObjectPtrLessProc)&TableView::fLessString,
//                 (IlwisObjectPtrSwapProc)&TableView::Swap);

    for (i = 1; i <= iNettoRecs(); ++i)
      laRowRec[laRecRow[i]] = i;
  }
}

void TableView::VirtualToStore(Column& col)
{
  col->BreakDependency();
  tbl->Updated();
  return;
// old code:
//  ColumnVirtual* pcv = col->pcv();
// if (pcv == 0)
//  return;
//  if (pcv->iRef > 3) // was already open
//    return;
  long iColInView = iCol(col->sNam);
//  TableStore* pts = tbl->pts();
  long iColInTable = tbl->iCol(col->sNam);
  col->BreakDependency();
  tbl->Updated();
//col.VirtualToStore();
  if (iColInTable >= 0)
    tbl->pts->ac[iColInTable] = col;
  if (iColInView < 0)
    return;
  ColumnView cv(this, col);
  cv.iWidth = ac[iColInView].iWidth;
  cv.iDec = ac[iColInView].iDec;
  cv.sTitle = ac[iColInView].sTitle;
  ac[iColInView] = cv;
}

void TableView::ReloadTable()
{
  // add columns to view that are not yet there
  for (int c = 0; c < tbl->iCols(); c++) {
    iAddCol(tbl->col(c)); // if already there then not added again
    tbl->col(c)->MakeUsable();
/*    ColumnVirtual* colv = tbl->col(c)->pcv();
    if (0 != colv)
      if (!colv->fUsable())
        colv->Freeze();*/
  }
}

void TableView::CheckNrRecs()
{
  if (tbl.fValid()) {
		tbl->CheckNrRecs();
		_iRecs = tbl->iRecs();
  }
	else {
    TablePtr::CheckNrRecs();
	}
	DomainSort* ds = _dm->pdsrt();
	if (ds)
		_iRecs = ds->iSize();
}

void TableView::Updated()
{
  IlwisObjectPtr::Updated();
  tbl->Updated();
}

const Column& TableView::col(int i) const
{
	if ( i >= 0 )
		return const_cast<const ColumnView&>(ac.ind(i));

	return colUNDEF;
}

const Column& TableView::col(const String& s) const
{
	int iIndex = iCol(s);
	if ( iIndex >= 0 )
		return const_cast<const ColumnView&>(ac.ind(iIndex));
	else
		return colUNDEF;
}

ColumnView& TableView::cv(int i) 
{ 
	if ( i >= 0  && i < ac.iSize())
		return ac[i]; 
	else
		return colvUNDEF;
}

ColumnView& TableView::cv(const String& s) 
{ 

	int iC = iCol(s);
	if ( iC >= 0 )
		return ac[iCol(s)]; 
	else
		return colvUNDEF;
}


void TableView::SetSortOrder(SortOrder _so)
{ 
  so = _so; 
}

const FileName& TableView::fnTbl() const
{
  return tbl->fnObj;
}

bool TableView::fDataReadOnly()
{
	return tbl->fDataReadOnly();
}
