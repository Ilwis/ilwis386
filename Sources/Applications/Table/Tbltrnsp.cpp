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
/*
// $Log: /ILWIS 3.0/Table/Tbltrnsp.cpp $
 * 
 * 11    12/04/01 16:05 Willem
 * - fUsedInOtherObjects now reuses the local Tranquilizer of
 * TableTranspose
 * - Removed old commented code
 * 
 * 10    5-03-01 11:45 Koolhoven
 * removed UnFreeze() function
 * moved some general initialization to the Init() function
 * 
 * 9     2-03-01 19:03 Koolhoven
 * protect UnFreeze() against 0 == pts
 * 
 * 8     11-12-00 11:39 Hendrikse
 * In fFreezing() , the column filling 
 * For tables with domain sort :
 * 1*  I removed the special treatment of non-alphanumeric class names
 * 2* corrcted the record counter of col->PutVal:
 *  col->PutVal(iRec+1, s);
 * 
 * 7     9/27/00 3:21p Martin
 * wrong size used for an array that holds the column numbers
 * 
 * 6     13-12-99 12:37 Wind
 * changed aiCol to zero based
 * 
 * 5     9/10/99 1:08p Wind
 * adpated to changes in constructors of FileName
 * 
 * 4     9/08/99 12:54p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:28a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.6  1998/10/20 09:25:05  Wim
// In fFreezing create a new domain when the number of records should change.
// The dmsort::Resize() function is not trustable
// and I also judge that dangerous
//
// Revision 1.5  1998-10-14 18:25:35+01  Wim
// Total incomprehensible magic in fFreezing() removed.
// ptr.iRecNew(), .DeleteRec(), ptr.iRecNew().
// because original number is already ok!
//
// Revision 1.4  1998-09-16 18:25:32+01  Wim
// 22beta2
//
// Revision 1.3  1997/08/14 18:31:42  Wim
// Update domain in fFreezing()
//
// Revision 1.2  1997-08-13 16:45:27+02  Wim
// All columns set table owned and read only in fFreezing()
//
/* TableTranspose
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK   20 Oct 98   10:23 am
*/

#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"
#include "Applications\Table\TBLTRNSP.H"
#include "Engine\Table\tbl.h"
#include "Engine\Table\tblstore.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

const char* TableTranspose::sSyntax()
{
  return "TableTranspose(tbl,coldom)\n"
"TableTranspose(tbl,coldom,colvalrange)";
}

IlwisObjectPtr * createTableTranspose(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableTranspose::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableTranspose(fn, (TablePtr &)ptr);
}

TableTranspose* TableTranspose::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 3))
    throw ErrorExpression(sExpr, sSyntax());
  Table tbl(as[0], fn.sPath());
  ValueRange vr;
  if (iParms == 3)
    vr = ValueRange(as[2]);
  DomainValueRangeStruct dvrs(Domain(as[1], fn.sPath()), vr);
  return new TableTranspose(fn, p, tbl, dvrs);
}

TableTranspose::TableTranspose(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  FileName fnMap;
  ReadElement("TableTranspose", "Table", tbl);
  objdep.Add(tbl.ptr());
  ReadElement("TableTranspose", "ColumnDomain", dmCol);
  ReadElement("TableTranspose", "ColumnValueRange", vrCol);
  Init();
}

TableTranspose::TableTranspose(const FileName& fn, TablePtr& p, const Table& table,
                               const DomainValueRangeStruct& dvrs)
: TableVirtual(fn, p, true), //FileName(fn, ".tb#", true), Domain("none")),
  tbl(table)
{
  Domain dmTbl = tbl->dm();
  if ((0 == dmTbl->pdnone()) && (0 == dmTbl->pdsrt()))
    throw ErrorObject(WhatError(String(TR("Only domain class/ID/none allowed: '%S'").c_str(), dmTbl->sName(true, fnObj.sPath())),
       		errTableTranspose), fnObj);
  if (tbl->iRecs() > 1000)
    throw ErrorObject(WhatError(String(TR("No more than 1000 records allowed: '%S'").c_str(), dmTbl->sName(true, fnObj.sPath())),
       		errTableTranspose+1), fnObj);
  dmCol = dvrs.dm();
  vrCol = dvrs.vr();
  Domain dmNew(FileName(fnObj, ".dom"), tbl->iCols(), dmtCLASS);
  DomainSort* pdsrt = dmNew->pdsrt();
  pdsrt->dsType = DomainSort::dsMANUAL;
  for (long i=0; i < tbl->iCols(); i++) {
    pdsrt->SetVal(i+1, tbl->col(i)->sName());
    pdsrt->SetOrd(i+1, i+1);
  }
//  _dm = dmNew;
  ptr.SetDomain(dmNew);
  ptr.iRecNew(pdsrt->iSize());
  
  objdep.Add(tbl.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  Init();
}

void TableTranspose::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "TableTranspose";
  htpFreeze = "ilwisapp\\transpose_table_algorithm.htm";
}

TableTranspose::~TableTranspose()
{
}

void TableTranspose::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableTranspose");
  WriteElement("TableTranspose", "Table", tbl);
  WriteElement("TableTranspose", "ColumnDomain", dmCol);
  WriteElement("TableTranspose", "ColumnValueRange", vrCol);
}

bool TableTranspose::fFreezing()
{
	DomainSort* pdsrt = dm()->pdsrt();
	pdsrt->dsType = DomainSort::dsMANUAL;
	if (pdsrt->iSize() != tbl->iCols())
	{
		Domain dmOld = dm();
		FileName fn = FileName::fnUnique(FileName(fnObj, ".dom"));
		Domain dmNew(fn, tbl->iCols(), dmtCLASS);
		pdsrt = dmNew->pdsrt();
		pdsrt->dsType = DomainSort::dsMANUAL;
		for (long i=0; i < tbl->iCols(); i++) {
			pdsrt->SetVal(i+1, tbl->col(i)->sName());
			pdsrt->SetOrd(i+1, i+1);
		}
		ptr.SetDomain(dmNew);
		ptr.Store();
		bool fUsedInOtherObjects = dmOld->fUsedInOtherObjects(0, false, &trq);
		bool fStillOpen = dmOld->iRef > 1;
		if (!fUsedInOtherObjects && !fStillOpen)
			dmOld->fErase = true;
	}
	for (long i=0; i < tbl->iCols(); i++) {
		pdsrt->SetVal(i+1, tbl->col(i)->sName());
		pdsrt->SetOrd(i+1, i+1);
	}
	
	// remove columns
	for (long i=0; i < iCols(); ++i) {
		Column col = pts->col(i);
		pts->RemoveCol(col);
	}
	
	DomainValueRangeStruct dvrsField(dmCol, vrCol);
	Domain dmTbl = tbl->dm();
	
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Create columns"));
	if (dmTbl->pdnone()) {
		for (long iRec=1; iRec <= tbl->iRecs(); ++iRec) {
			String sColName("Col%li", iRec);
			if (trq.fUpdate(iRec, tbl->iRecs()))
				return false;
			Column col = pts->colNew(sColName, dvrsField);
			col->SetOwnedByTable(true);
			col->SetReadOnly(true);
			//      _iCols += 1;
		}  
		trq.SetText(TR("Fill columns"));
		for (long iCol=1; iCol <= iCols(); ++iCol) {
			if (trq.fUpdate(iCol, iCols()))
				return false;
			Column col = pts->col(iCol-1);
			for (long iRec=1; iRec <= iRecs(); ++iRec) {
				String s = tbl->col(iRec-1)->sValue(iCol, 0);
				col->PutVal(iRec, s);
			}
		} 
	}
	else {
		DomainSort* pdsrt = dmTbl->pdsrt();
		LongArray aiCol(pdsrt->iSize()+1); // zero based		
		for (long iRec=1; iRec <= pdsrt->iSize(); ++iRec) 
		{
			long iKey = pdsrt->iKey(iRec);
			if (iKey == iUNDEF)
				aiCol[iRec] = iUNDEF;
			try {
				String sCol = pdsrt->sValueByRaw(iKey,0);
				if (trq.fUpdate(iRec, tbl->iRecs()))
					return false;
				Column col = pts->colNew(sCol, dvrsField);
				col->SetReadOnly(true);
			}
			catch (const ErrorObject& err) {
				err.Show();
				String sColName("Col%li", iKey);
				Column col = pts->colNew(sColName, dvrsField);
			}  
			aiCol[iRec] = iCols();
		}  
		trq.SetText(TR("Fill columns"));
		for (long iCol=1; iCol <= iCols(); ++iCol) {
			if (trq.fUpdate(iCol, iCols()))
				return false;
			Column col = pts->col(iCol-1);
			for (long iRec=0; iRec < iRecs(); ++iRec) {
				String s = tbl->col(iRec)->sValue(pdsrt->iKey(iCol), 0);
				col->PutVal(iRec+1, s);
			}
		} 
	}
	// set columns table owned and read only
	for (long iCol=0; iCol < iCols(); ++iCol) {
		Column col = pts->col(iCol);
		col->SetOwnedByTable(true);
		col->SetReadOnly(true);
	}
	
	return true;
}


String TableTranspose::sExpression() const
{
  return String("TableTranspose(%S,%S)",
                  tbl->sNameQuoted(true, fnObj.sPath()), dmCol->sName());
}
