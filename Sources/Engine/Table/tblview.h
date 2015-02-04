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
/* $Log: /ILWIS 3.0/TableWindow/tblview.h $
 * 
 * 10    8/01/01 4:33p Martin
 * record is put into a namspace
 * 
 * 9     5/15/00 5:11p Wind
 * made ApplySorting() public (needed for pasting in tables)
 * 
 * 8     4/11/00 3:22p Wind
 * added fReadOnly that calls TablePtr::fReadOnly()  (bug 1367)
 * 
 * 7     15-03-00 9:50 Wind
 * added function fbTbl()
 * 
 * 6     14-03-00 11:13 Wind
 * - added command sorton colname -sortorder=ascending|descending
 * - added to context sensitive menu of column
 * - added sort order to sort column dialogue
 * 
 * 5     13-03-00 15:17 Wind
 * added mean, stddev and sum per column for display in tablewindow
 * statistics pane
 * 
 * 4     26-01-00 9:11a Martin
 * moved two functions (cv(...) from h file to cpp file to enable
 * boundschecking
 * 
 * 3     8-12-99 12:25 Koolhoven
 * Header comment
 * 
 * 2     7-12-99 18:24 Koolhoven
 * Show minimum and maximum of columns in TableSummaryPaneView
// Revision 1.4  1998/09/16 17:25:30  Wim
// 22beta2
//
// Revision 1.3  1997/09/12 14:30:28  Wim
// Virtual functions col() are no longer inline
//
// Revision 1.2  1997-07-25 12:50:34+02  Wim
// Updated() added, useful in TableWindow
//
/* TableView, ColumnView
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   12 Sep 97    4:18 pm
*/

#ifndef ILWTBLVIEW_H
#define ILWTBLVIEW_H
#include "Engine\Table\tbl.h"
#include "Engine\Table\Col.h"

class DATEXPORT ColumnView;
class DATEXPORT TableView;

class _export ColumnView: public Column
{
public:
  ColumnView();
  ColumnView(const TableView*, const Table& tbl, const String& sColName);
  ColumnView(const TableView*, const Column&);
  ColumnView(const ColumnView&);
  void operator = (const ColumnView&);
  TableView* tview() const { return tvw; }
  int iWidth;
  int iDec;
  String sTitle;
//  String sDef;
  void Store();
private:
  TableView* tvw;
};
inline ColumnView undef(const ColumnView&) { return ColumnView(); }

/*#ifdef TBLVIEW_C
#pragma option -Jgd
static Array<ColumnView> acvwDummy;
#pragma option -Jgx
#endif*/

class _export TableView: public TablePtr
{
  friend class ColumnView;
public:
  enum SortOrder { soASCENDING = 0, soDESCENDING = 1  };
  virtual void Store();
  TableView(const FileName&);
  TableView(const Table&);
  TableView(const Domain&);
  TableView(TablePtr*);
  ~TableView();
  short iCol(const String&) const;
  short MoveCol(short iFrom, short iTo);
  String sRow(long iRow) const;
  String sValue(short iCol, long iRow, short iWidth=-1, short iDec=-1) const;
  double rValue(short iCol, long iRow) const;
  void PutVal(short iCol, long iRow, const String&);
  String sMinimum(short iCol) const;
  String sMaximum(short iCol) const;
  String sMean(short iCol) const;
  String sStdDev(short iCol) const;
  String sSum(short iCol) const;
  String sValueToString(short iCol, double rVal, bool fSum) const;
  bool fEditable(short iCol) const;
  void MakeUsable(short iCol);
  ColumnView& cvKey() { return _cvKey; }
  const ColumnView& cvKey() const { return _cvKey; }
  void SetKey(short iCol);
  long iRec(long iRow) const; // { return iRow; } // till sorting exists!!!
  long iRow(const String&) const;
  long iRow(long iRec) const;
  const Column& col(int i) const;
  const Column& col(const String& s) const;
  ColumnView& cv(int i); // { return ac[i]; }
  ColumnView& cv(const String& s); // { return ac[iCol(s)]; }
  short iColNew(const String&, const Domain&);
  short iAddCol(const Column&);
  void RemoveCol(short);
  void DeleteRec(long iStartRec, long iRecs=1);
  long iRecNew(long iRecs = 1);
	Ilwis::Record recNew();// { return rec(iRecNew()); }
  void VirtualToStore(Column& col);
  void ReloadTable();
  void CheckNrRecs(); 
  void Updated();
  void SetSortOrder(SortOrder so);
  SortOrder so;
  const FileName& fnTbl() const;
	virtual bool fDataReadOnly();
  void ApplySorting();
private:
  void init(TablePtr*);
  Table tbl;
  ColumnView _cvKey;
  Array<ColumnView> ac;
public:  
  ArrayLarge<long> laRecRow, laRowRec;
//void Swap(long, long);
//bool fLessValue(long, long) const;
//bool fLessSort(long, long) const;
//bool fLessReal(long, long) const;
//bool fLessString(long, long) const;
};

#endif





