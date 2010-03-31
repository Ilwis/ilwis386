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
/* Record
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  MS   23 Sep 97   10:34 am
*/

#ifndef ILWREC_H
#define ILWREC_H
// Current implementation does map everything through the Column class
#include "Engine\Table\tbl.h"
#include "Engine\Table\Col.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\tblview.h"

class TableView;

namespace Ilwis
{


	class DATEXPORT Record
	{
	  friend class DATEXPORT TablePtr;
	protected:
	  TablePtr* tbl;
	  long iCurr;
	  Record(TablePtr* ptr, long iRow = 0): tbl(ptr), iCurr(iRow) {}
	public:
	  Record() { tbl = 0; iCurr = iUNDEF; }
	  bool fValid() const { return tbl != 0; }
	  bool fDeleted() const ;
	  int iFields() const		      { return tbl->iCols(); }
	  const Domain& dm(int iCol) const    { return tbl->col(iCol)->dm(); }
	  const Domain& dmKey() const	      { return tbl->dm(); }
	  String _export sCol(int iCol) const;
	  short _export iCol(const String&) const;
	  bool _export fEditable(int iCol) const;

	  void First()	{ iCurr = tbl->iOffset(); }
	  void _export Next();
	  void Last()	{ iCurr = tbl->iOffset() + tbl->iRecs() ; }
	  void _export Back(); //	{ iCurr--; if (fDeleted() || (iCurr < tbl->iOffset())) Last(); }
	  bool _export Goto(const String& sKey);

	  String sKey() const  { return tbl->dm()->sValueByRaw(iCurr); }
	  String sValue(int iCol) const { return tbl->col(iCol)->sValue(iCurr); }
	  String sValue(int iCol, int iWidth) const { return tbl->col(iCol)->sValue(iCurr, iWidth); }
	  void	 _export PutVal(int iCol, const String& s);
		int iRec() const { return iCurr; }
	};
};

inline Ilwis::Record TablePtr::recNew() 
    { return rec(iRecNew()); }
inline Ilwis::Record TablePtr::recFirst() const
    { return rec(iOffset()); }
inline Ilwis::Record TablePtr::recLast() const
    { return rec(iOffset()+iRecs()-1); }
inline Ilwis::Record TablePtr::rec(long iRow) const
{
  if (iRow < iOffset())
    iRow = iOffset();
  else if (iRow >= iOffset() + iRecs())
    iRow = iOffset() + iRecs() - ((iRecs() > 0) ? 1 : 0);
  return Ilwis::Record(const_cast<TablePtr*>(this),iRow);
}

inline Ilwis::Record TableView::recNew()
{ return rec(iRecNew()); }

#endif




