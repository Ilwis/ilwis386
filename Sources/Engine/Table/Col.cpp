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
/* $Log: /ILWIS 3.0/Column/Col.cpp $
* 
* 59    22-04-04 10:17 Willem
* [Bug=6471]
* - Changed: the call to GetNewestDependentObject now needs an array.
* This array is used to keep track of all files that already have been
* checked. Because of the recursive nature of the function indefinite
* loops are a danger. This is now prevented by first checking the array
* of checked objects
* 
* 58    27-05-03 9:22 Willem
* [Bug=6430]
* - Added: new Column::pGet() function to retrieve a column already in
* use by a table. This makes it possible to reuse columns that are
* already in memory. It also prevents administrative problems when
* overwriting existing columns
* - Changed: all column constructors and fInitForConstructor functions
* now use the new pGet function
* - Remove: unused ColumnPtr::pcolfind()
* 
* 57    22-04-03 15:02 Willem
* - changed: Before trying to open a Column, make sure that it really is
* an existing Column; This prevents loading of Columns that are still in
* the ODF, but are not referenced anymore
* 
* 56    21-03-03 14:30 Willem
* - Changed: Removed inappropriate and unnecessary scVal(); In the
* release version this caused a crash
* 
* 55    14-03-02 11:57 Koolhoven
* in ColumnPtr::create() removed the !TableInfo::fExistCol() check which
* is superfluous and takes (for tables with many columns) a long time
* 
* 54    1-03-02 18:15 Retsios
* CalcMinMax now correctly takes care of columns belonging to tables with
* deleted records.
* 
* 53    2/22/02 10:18a Martin
* added new constructor (and supporting functions) to accept columns with
* an expression and a dvrs. Needed for changing precission in a column
* definition form
* 
* 52    12/06/01 9:45a Martin
* added try catch to catch errors fro foreign format at an appropriate
* place
* 
* 51    10/30/01 10:05a Martin
* during loading of foreigndata no data may be written back to the column
* 
* 50    17-10-01 18:37 Koolhoven
* in CalcMinMax() set _rStdDev on 0 when we would get the sqrt from a
* negative number
* 
* 49    8/09/01 11:52 Hendrikse
* corrected PutLongInForeign etc in PutRaw() and PutVal() (3 X)
* 
* 48    8/07/01 2:25p Martin
* added putting data in foreign files to the column  put functions
* 
* 47    5/04/01 11:26 Willem
* - In fInitForConstructor(): Before adding the new column, first load
* the original table. This is needed to avoid column number mismatch
* between the table in memory and on disk
* - Removed comments and unused code
* 
* 46    15-03-01 8:40a Martin
* back to the original unclear situation were value drop out if they fall
* outside a recalculated column valluerange. Seems to be intended (??)
* 
* 45    8-03-01 2:02p Martin
* the char * name of a table is first put into a FileName to parse it
* correctly (in initforcounstructor)
* 
* 44    6-03-01 2:21p Martin
* misplaced bracket caused columns with the same name to beadded as new
* columns
* 
* 43    1-03-01 12:31p Martin
* added quoting for tables with names like 'I am a table.tab'.tbt
* 
* 42    12-02-01 4:04p Martin
* misunderstood why independent columns could not be deleted if they are
* tabelowned. It will now give an error if such a thing is tried
* 
* 41    16-01-01 10:24a Martin
* recalculated independent columns are fully recalculated again to
* prevent problems with valuerange that do not fit in the original column
* 
* 40    16-01-01 8:43a Martin
* wrong solution to the tableref problem when loading tables. Removed,
* must be locally solved
* 
* 39    15-01-01 2:14p Martin
* iRef is for the owning table is increased as the owning table may
* not(!!) leave until the column leaves. Else loading will go wrong
* 
* 38    21-12-00 10:10a Martin
* added the pGetVirtualObject() function to access the virtual object
* embedded in the object. 
* 
* 37    18-12-00 11:47a Martin
* the -quiet parm was passed onto the splitColumnAndTable functions. It
* did not understand this. Any extra parms are split off on that level
* 
* 36    21-11-00 10:03a Martin
* GetObjectDependencies and GetObjectStructure implemented for columns
* 
* 35    17-11-00 15:27 Koolhoven
* made Column::fInitForConstructor() a little bit more readable
* 
* 34    11/02/00 10:10a Martin
* changed the oldgetobjectstructure to a new form (old was from weeks
* back)
* 
* 33    5-10-00 1:02p Martin
* LoadData before a column is saved Saving unloaded tables is not a good
* idea.
* 
* 32    9/19/00 9:08a Martin
* removed superflous store protection. Is now arranged at a higher level
* 
* 31    9/18/00 9:38a Martin
* function to check the load state of a column and the to set it
* 
* 30    12-09-00 9:13a Martin
* added guards to prevent a store to ODF when not wanted
* 
* 29    11-09-00 10:12a Martin
* changed the objectstructure function
* 
* 28    8-09-00 3:31p Martin
* added function for fChanged correctly setting and for retrieving
* ObjectStructure
* 
* 27    18/08/00 11:43 Willem
* - The dmFromStrings() function now does not add sUNDEF to the domain
* anymore
* - removed a lot of commented code
* 
* 26    8/15/00 5:17p Wind
* quotes in SplitTableAndColumn
* 
* 25    7/11/00 1:01p Wind
* added function to check for unique values in column (used in
* JoinWizard)
* 
* 24    16-06-00 20:50 Koolhoven
* removed const qualifier in function pcBuf()
* 
* 23    13-06-00 3:42p Martin
* added init of three vars (_rSum etc)
* 
* 22    4/05/00 3:51p Wind
* bug when deleting records for virtual column
* 
* 21    3/21/00 2:47p Wind
* new implementation of dmFromStrings()
* 
* 20    3/21/00 12:32p Wind
* minmax for booelan columns
* 
* 19    15-03-00 16:49 Wind
* min max for bool column
* set sum to undefined if all records undefined
* 
* 18    13-03-00 15:17 Wind
* added mean, stddev and sum per column for display in tablewindow
* statistics pane
* 
* 17    1-03-00 10:50 Wind
* tried to improve dmFromStrings(), but it crashes; code is between
* comments
* 
* 16    15-02-00 16:12 Wind
* check for cyclic definition when changing expression
* 
* 15    15-02-00 11:20 Wind
* wrong error message 'column not found'
* 
* 14    14-02-00 16:12 Wind
* solved bug 682
* 
* 13    17-01-00 8:14a Martin
* columns check now if they exist including their prefix
* 
* 12    10-01-00 8:28a Martin
* GetValue(... CoordBuf..) now return thr number of records actually used
* 
* 11    21-12-99 12:58p Martin
* added a domain and column Coordbuf based on domain binary to be able to
* read and store dynamically coordbufs in a table
* 
* 10    7-12-99 11:22 Wind
* added/improved locks
* 
* 9     18-11-99 11:02a Martin
* Added missed function name  from 2.23 port
* 
* 8     29-10-99 12:57 Wind
* case sensitive stuff 
* thread safe stuff
* 
* 7     25-10-99 13:14 Wind
* making thread save (2); not yet finished
* 
* 6     22-10-99 12:56 Wind
* thread save access (not yet finished)
* 
* 5     9/29/99 10:04a Wind
* added case insensitive string comparison
* 
* 4     9/27/99 11:18a Wind
* changed calls to static funcs ObjectInfo::ReadElement and WriteElement
* to member function calls
* 
* 3     9/08/99 10:13a Wind
* adpated for quoted column  names
* 
* 2     3/11/99 12:15p Martin
* Added support for Case insesitive 
// Revision 1.13  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.12  1997/09/27 13:20:26  Wim
// ColumnPtr::fConvertTo() if dependent column set and recalculate.
//
// Revision 1.11  1997-09-26 19:38:08+02  Wim
// Simplified dmFromStrings()
//
// Revision 1.10  1997-09-11 20:40:41+02  Wim
// Allow to break dependencies even when source objects are gone
//
// Revision 1.9  1997-08-28 11:42:00+02  Wim
// SetValueRange() also unfreezes pcv now just like SetDomainValueRangeStruct
//
// Revision 1.8  1997-08-25 11:15:26+02  Wim
// PutBufVal(LongBuf ...) was calling pcs->PutBufRaw()
//
// Revision 1.7  1997-08-21 14:43:44+02  Wim
// sExpression() now gets expression of column, not of table
//
// Revision 1.6  1997-08-14 14:38:41+02  Wim
// Do not store in a filename which is empty
//
// Revision 1.5  1997-08-13 08:56:09+02  Wim
// SetDomainValueRangeStruct better check on same dm
//
// Revision 1.4  1997-08-11 11:21:31+02  martin
// The InitForConstructor function is now a boolean function. In some cases the function does all
// the work. In that cases any code outside the function must know that no further processing is needed
// ( e.g Aggregate to another table). The function returns a boolean to indicate this.
//
// Revision 1.3  1997/08/06 16:59:26  Wim
// Additional Info load and store implemented
//
// Revision 1.2  1997-08-01 13:56:10+02  Wim
// pcs and pcv were never destructed
//
/* Column, ColumnPtr
Copyright Ilwis System Development ITC
march 1995, by Wim Koolhoven
Last change:  WK    1 Jul 98   10:03 am
*/
#define COL_C

#pragma warning( disable : 4786 )

#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Table\COLSTRNG.H"
#include "Engine\Table\Colbinar.h"
#include "Engine\Table\COLCOORD.H"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Scripting\Script.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\map.hs"


IlwisObjectPtrList Column::listCol;

ColumnPtr* Column::pGet(const Table& tbl, const String& sCol)
{
	String sCol1 = tbl->sSectionPrefix + sCol;
	for (DLIterCP<IlwisObjectPtr> iter(&listCol); iter.fValid(); ++iter)
	{
		ColumnPtr* pCol = static_cast<ColumnPtr*>(iter());
		String sCol2 = pCol->sSectionPrefix + pCol->sNam;
		if ((pCol->fnTbl == tbl->fnObj) && fCIStrEqual(sCol1, sCol2))
			return pCol;
	}
	return 0;
}

Column::Column()
: IlwisObject(listCol)
{
}

Column::Column(const Column& col)
: IlwisObject(listCol, col.pointer())
{
}

// sColName can contain a reference to a table:
// tbl.col
// In that case use the table specified in the sColName string
// otherwise use the tblDflt table
Column::Column(const Table& tblDflt, const String& sColName)
: IlwisObject(listCol)
{
	String sCol;
	String sTable;
	SplitTableAndColumn(tblDflt, sColName, sTable, sCol);
	sCol = sCol.sUnQuote();
	Table tbl = tblDflt;
	bool fSecondaryTable = sTable.length() != 0;
	if (fSecondaryTable)
		tbl = Table(sTable);

	if (tbl.fValid())
	{
		// Check if the column is already there and in the table
		ColumnPtr *pCol = pGet(tbl, sCol);
		if (pCol != 0)
			SetPointer(pCol);
		else
			SetPointer(ColumnPtr::create(tbl, sCol));

		if (fSecondaryTable)
		{
			tbl->AddCol(*this);
			// as the tbl gets out of scope the data from the table must be loaded,
			// else you create a column that can no longer load itself as its tblPtr member
			// has become invalid;
			tbl->LoadData();			
		}
	}
}

void Column::SplitTableAndColumn(const Table& tblDflt, const String& sTableAndColName, String& sTable, String& sCol)
{
	// col
	// table.col
	// table.ext.col
	// col.x or col.y if col == coordinate column  (not yet)
	ParmList pl(sTableAndColName);
	String sTabCol =  pl.sGet(0); 
	String s = sTabCol; // strips off any command parms
	char *p;
	p = s.strrchrQuoted('.');
	if (p != 0)
	{
		int iTblNameSize = p-s.sVal();
		sTable = sTabCol.sLeft(iTblNameSize);
		sTable = sTable.sUnQuote();
		sCol = sTabCol.sRight(sTabCol.length()-iTblNameSize-1);
	}
	else
		sCol = sTableAndColName;
}

Column::Column(const String& sTableAndColName)
: IlwisObject(listCol)
{
	fInitForConstructor(sTableAndColName, Table());
}

Column::Column(const String& sTableAndColName, const Table& tblDefault)
: IlwisObject(listCol)
{
	fInitForConstructor(sTableAndColName, tblDefault);
}

bool Column::fInitForConstructor(const String& sTableAndColName, const Table& tblDefault)
{
	String sCol;
	String sTable;
	SplitTableAndColumn(tblDefault, sTableAndColName, sTable, sCol);
	sCol = sCol.sUnQuote();
	Table tbl = tblDefault;
	bool fSecondaryTable = sTable.length() != 0;
	if (fSecondaryTable)
	{
		FileName fn(sTable);
		tbl = Table(fn.sFullPathQuoted());
	}

	if (tbl.fValid())	
	{
		// Check if the column is already there and in the table
		ColumnPtr *pCol = pGet(tbl, sCol);
		if (pCol != 0)
			SetPointer(pCol);
		else
			SetPointer(ColumnPtr::create(tbl, sCol));

		return true;  // OK to add to the table later
	}
	return false;
}


Column::Column(const Table& tblDefault, const String& sColName, const DomainValueRangeStruct& dvs)
: IlwisObject(listCol)
{
	String sCol;
	String sTable;

	SplitTableAndColumn(tblDefault, sColName, sTable, sCol);
	sCol = sCol.sUnQuote();
	if (sTable.length() != 0) {
		Table tbl(sTable);
		SetPointer(ColumnPtr::create(tbl, sCol, dvs));
		tbl->AddCol(*this);
	}
	else if (tblDefault.fValid()) {
		SetPointer(ColumnPtr::create(tblDefault, sCol, dvs));
		tblDefault->AddCol(*this);
	}
	if (pointer())
		pointer()->objtime = ObjectTime::timCurr();
}

Column::Column(const Table& tblDefault, const String& sColName,
			   const String& sExpression)
			   : IlwisObject(listCol)
{
	bool fAdd = fInitForConstructor(tblDefault, sColName, sExpression, Domain());
	if (fAdd && tblDefault.fValid() && fValid())
		tblDefault->AddCol(*this);
}

Column::Column(const Table& tblDefault, const String& sColName,
			   const String& sExpression, const DomainValueRangeStruct& dvr)
			   : IlwisObject(listCol)
{
	bool fAdd = fInitForConstructor(tblDefault, sColName, sExpression, dvr);
	if (fAdd && tblDefault.fValid() && fValid())
		tblDefault->AddCol(*this);
}

Column::Column(const Table& tblDefault, const String& sColName,
			   const String& sExpression, const Domain& dm)
			   : IlwisObject(listCol)
{
	fInitForConstructor(tblDefault, sColName, sExpression, dm);
}

bool Column::fInitForConstructor(const Table& tblDefault, const String& sColName,
								 const String& sExpression, const DomainValueRangeStruct& dvr)
{
	Domain dom = dvr.dm();
	ValueRange vr = dvr.vr();
	bool fReturn = true;
	String sCol;
	String sTable;
	SplitTableAndColumn(tblDefault, sColName, sTable, sCol);
	TokenizerBase tkn(new TextInput(sCol));
	Token tok=tkn.tokGet();
	String sColNme = tok.sVal();
	sColNme = sColNme.sUnQuote();
	tok=tkn.tokGet();
	if (tok.sVal() == "{")
		ScriptPtr::fScanDomainValueRange(tok, tkn, dom, vr);

	Table tbl = tblDefault;
	bool fSecondaryTable = sTable.length() != 0;
	if (fSecondaryTable)
		tbl = Table(sTable);

	fReturn = tbl->sName() == tblDefault->sName();

	if (tbl.fValid())
	{
		ColumnPtr* pcol = pGet(tbl, sColNme);
		if (pcol != 0)
		{
			// The column is being reused...
			SetPointer(pcol);
			// Now make sure it uses the correct expression and also
			// has the correct domain/vr
			pcol->Replace(sExpression);
		}
		else
			SetPointer(ColumnPtr::create(tbl, sColNme, sExpression, DomainValueRangeStruct(dom,vr)));

		if (fSecondaryTable && fValid())
		{
			// Make sure the table is loaded before adding the new column
			// Otherwise the LoadData() function will have a mismatch in
			// number of columns on disk and in memory
			tbl->LoadData();
			tbl->AddCol(*this);
		}
	}

	if (pointer())
		pointer()->objtime = ObjectTime::timCurr();

	return fReturn;
}

bool Column::fInitForConstructor(const Table& tblDefault, const String& sColName,
								 const String& sExpression, const Domain& dm)
{
	Domain dom;
	ValueRange vr;
	bool fReturn = true;
	String sCol;
	String sTable;
	SplitTableAndColumn(tblDefault, sColName, sTable, sCol);
	TokenizerBase tkn(new TextInput(sCol));
	Token tok=tkn.tokGet();
	String sColNme = tok.sVal();
	sColNme = sColNme.sUnQuote();
	tok=tkn.tokGet();
	if (tok.sVal() == "{")
		ScriptPtr::fScanDomainValueRange(tok, tkn, dom, vr);
	if (!dom.fValid())
		dom = dm;

	Table tbl = tblDefault;
	bool fSecondaryTable = sTable.length() != 0;
	if (fSecondaryTable)
		tbl = Table(sTable);

	fReturn = tbl->sName() == tblDefault->sName();

	if (tbl.fValid())
	{
		ColumnPtr* pcol = pGet(tbl, sColNme);
		if (pcol != 0)
		{
			// The column is being reused...
			SetPointer(pcol);
			// Now make sure it uses the correct expression and also
			// has the correct domain/vr
			pcol->Replace(sExpression);
		}
		else
			SetPointer(ColumnPtr::create(tbl, sColNme, sExpression, DomainValueRangeStruct(dom,vr)));

		if (fSecondaryTable && fValid())
		{
			// Make sure the table is loaded before adding the new column
			// Otherwise the LoadData() function will have a mismatch in
			// number of columns on disk and in memory
			tbl->LoadData();
			tbl->AddCol(*this);
		}
	}

	if (pointer())
		pointer()->objtime = ObjectTime::timCurr();

	return fReturn;
}

void ColumnPtr::BreakDependency()
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return;
	delete pcv;
	pcv = 0;
	fChanged = true;

	ObjectInfo::WriteElement(sSection().c_str(), "Type", fnObj, "ColumnStore");
	Store();
	ObjectInfo::WriteElement(sSection().c_str(), "NrDepObjects", fnObj, 0L);
}

bool Column::fUsedInOpenColumns() const
{
	ILWISSingleLock(&listCol.csAccess, TRUE, SOURCE_LOCATION);
	bool f = false;
	for (DLIterCP<IlwisObjectPtr> iter(&listCol); iter.fValid(); ++iter) {
		if (iter()->fDependent())
			if (static_cast<ColumnPtr*>(iter())->objdep.fUses(*this))
				return true;
	}
	return false;
}

static bool fColCalc(const String& sSection, const FileName& fnTbl)
{
	String s;
	ObjectInfo::ReadElement(sSection.c_str(), "Calc", fnTbl, s);
	if (s == "True")
		return true;
	// for downward  comp. with 2.02:
	String sExpr;
	if (0 == ObjectInfo::ReadElement(sSection.c_str(), "Expression", fnTbl, sExpr))
		return false;
	String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
	if (sFunc.length() > 0) {
		if (fCIStrEqual(sFunc.sLeft(3), "agg")  || (fCIStrEqual(sFunc.sLeft(15) , "ColumnAggregate")) ||
			fCIStrEqual(sFunc.sLeft(4), "join") || (fCIStrEqual(sFunc.sLeft(10) , "ColumnJoin")) ||
			fCIStrEqual(sFunc.sLeft(3), "cum")   || (fCIStrEqual(sFunc , "ColumnCumulative")) ||
			fCIStrEqual(sFunc , "ColumnLeastSquaresFit") || (fCIStrEqual(sFunc , "ColumnSemiVariogram")))
			return false;
	}
	return true;
}

String ColumnPtr::sType() const
{
	if (0 != pcv)
		return pcv->sType();
	else if (fDependent()) {
		if (fColCalc(sSection().c_str(), fnTbl))
			return "Column Calculate";
		return "Dependent Column";
	}
	else
		return "Column";
}

String ColumnPtr::sTableAndColumnName(const String& sRelPath)
{
	String sTable;
	if (fCIStrEqual(fnTbl.sExt, ".tbt"))
		sTable = fnTbl.sRelativeQuoted(false, sRelPath);
	else
		sTable = fnTbl.sRelativeQuoted(true, sRelPath);
	return String("%S.%S", sTable, sNameQuoted());
}

ColumnPtr* ColumnPtr::create(const Table& tbl, const String& sColName)
{
	if (sColName.length() == 0)
		return 0;

	// check if column already exists
	Column col = tbl->col(sColName);
	if (col.fValid())
		return col.ptr();
	return new ColumnPtr(tbl, sColName);
}

ColumnPtr* ColumnPtr::create(const Table& tbl, const String& sColName,
							 const DomainValueRangeStruct& dvs)
{
	// check if column already exists
	Column col = tbl->col(sColName);
	if (col.fValid()) {
		if (col->dvrs() == dvs) // same domain and value range
			return col.ptr();
		else
			return 0;
	}
	return new ColumnPtr(tbl, sColName, dvs);

	return NULL;
}


ColumnPtr* ColumnPtr::create(const Table& tbl, const String& sColName,
							 const String& sExpression, const DomainValueRangeStruct& dvs)
{
	Column col;
	try
	{
		// check if column already exists:
		// Added extra check to prevent left-overs in the ODF to open a non-existing Column
		// If it exists open it
		if (tbl->iCol(sColName) != -1)  
			col = Column(tbl, sColName);
	}
	catch (const ErrorObject&)
	{
	}

	if (col.fValid()) 
	{
		if (!col->fDependent()) 
		{
			if (col->fReadOnly())
				NotCreatedError(String("Column %S", sColName));

			// calculate result column
			ColumnPtr* colp = ColumnPtr::create(tbl, String("%S___", sColName), sExpression, col->dvrs());
			if (0 == colp)
				return 0;

			colp->fErase = true;
			colp->Calc();

			// copy result to column
			// moet efficienter, op grond van domeinen
			long r;
			if (col->dvrs().fValues())
				for (r = col->iOffset(); r < col->iOffset() + col->iRecs(); ++r)
					col->PutVal(r, colp->rValue(r));
			else
				for (r = col->iOffset(); r < col->iOffset() + col->iRecs(); ++r)
					col->PutVal(r, colp->sValue(r,0));

			col->Updated();
			tbl->Updated();
			delete colp;
		}
		else  // column virtual
		{
			// replace expression
			try
			{
				col->Replace(sExpression);
				if (dvs.dm().fValid())
					col->SetDomainValueRangeStruct(dvs);
				col->Calc();
			}
			catch (const ErrorObject& err)
			{
				err.Show();
				return col.ptr();
			}
		}
		tbl->AddCol(col);  // 16/1/97 - Wim - otherwise reused column name will now get 0 references!
		return col.ptr();					
	}

	ColumnPtr* ptr = new ColumnPtr(tbl, sColName, true);//dvs);
	ptr->pcv = ColumnVirtual::create(tbl, sColName, *ptr, sExpression, dvs);
	if (0 != ptr->pcv)
	{
		ptr->SetDomainChangeable(ptr->pcv->fDomainChangeable());
		ptr->SetValueRangeChangeable(ptr->pcv->fValueRangeChangeable());
		ptr->SetExpressionChangeable(ptr->pcv->fExpressionChangeable());
	}
	return ptr;
}


ColumnPtr::ColumnPtr(const Table& t, const String& sColName, bool fCreate)
: IlwisObjectPtr(t->fnObj, fCreate), sNam(sColName), pcs(0), pcv(0), fLoadingForeignData(false)
{
	_rStdDev = _rMean = _rSum = rUNDEF;
	sSectionPrefix = t->sSectionPrefix;
	_fReadOnly = t->fReadOnly();
	_fOwnedByTable = false;
	_dmKey = t->dm();
	_iOffset = t->iOffset();
	_iRecs = t->iRecs();
	fnTbl = t->fnObj;
	ptrTbl = dynamic_cast<TablePtr *>(t.pointer());
	if (fCreate)
		return;
	SetAdditionalInfoFlag(false); // this has been read in by constructor IlwisObjectPtr
	SetAdditionalInfo(String());
	SetDescription("");
	ReadElement(sSection().c_str(), "Description", sDescription);

	long iInfoLines = iUNDEF;
	String sTemp=sSection().c_str();
	String sAddInfo("%S: AdditionalInfo", sSection());
	ReadElement(sAddInfo.c_str(), "Lines", iInfoLines);
	if (iInfoLines > 0) {
		SetAdditionalInfoFlag(true);
		String sInfo;
		for (int i=0; i < iInfoLines; ++i) {
			String s;
			ReadElement(sAddInfo.c_str(), String("Line%i", i).c_str(), s);
			sInfo &= s;
			sInfo &= "\r\n";
		}
		SetAdditionalInfo(sInfo);
	}

	Domain dom;
	ReadElement(sSection().c_str(), "Domain", dom);
	dvs.SetDomain(dom);
	if (dvs.fValues()) {
		ValueRange vr;
		ReadElement(sSection().c_str(), "Range", vr);
		if (vr.fValid())
			dvs.SetValueRange(vr);
	}
	ReadElement(sSection().c_str(), "Time", objtime);
	if (!fReadOnly()) {
		ReadElement(sSection().c_str(), "ReadOnly", _fReadOnly);
	}
	ReadElement(sSection().c_str(), "OwnedByTable", _fOwnedByTable);
	if (dvs.fValues()) {
		if (dvs.fRealValues()) {
			ReadElement(sSection().c_str(), "MinMax", _rrMinMax);
			_riMinMax = RangeInt(longConv(_rrMinMax.rLo()),
				longConv(_rrMinMax.rHi()));
		}
		else {
			ReadElement(sSection().c_str(), "MinMax", _riMinMax);
			_rrMinMax = RangeReal(doubleConv(_riMinMax.iLo()), doubleConv(_riMinMax.iHi()));
		}
		ReadElement(sSection().c_str(), "Sum", _rSum);
		ReadElement(sSection().c_str(), "Mean", _rMean);
		ReadElement(sSection().c_str(), "StdDev", _rStdDev);
	}

	bool fStored = true;
	ReadElement(sSection().c_str(), "Stored", fStored);
	if (fStored || !fDependent() )
		pcs = new ColumnStore(t, sColName, *this);
	if (fDependent()) {
		if (fColCalc(sSection().c_str(), fnTbl)) {
			SetDomainChangeable(true);
			SetValueRangeChangeable(true);
			SetExpressionChangeable(true);
		}
	}
}

ColumnPtr::ColumnPtr(const Table& t, const String& sColName, const DomainValueRangeStruct& _dvs)
: IlwisObjectPtr(t->fnObj), pcs(0), pcv(0),
sNam(sColName), dvs(_dvs),fLoadingForeignData(false)
{
	_rStdDev = _rMean = _rSum = rUNDEF;
	sSectionPrefix = t->sSectionPrefix;
	SetDescription("");
	_fReadOnly = false;
	SetAdditionalInfoFlag(false); // this has been read in by constructor IlwisObjectPtr
	SetAdditionalInfo(String());
	_fOwnedByTable = false;
	_dmKey = t->dm();
	_iOffset = t->iOffset();
	_iRecs = t->iRecs();
	fnTbl = t->fnObj;
	objtime = ObjectTime::timCurr();
	ptrTbl = dynamic_cast<TablePtr *>(t.pointer());
	pcs = new ColumnStore(t, sColName, *this, dvs);
}

ColumnPtr::~ColumnPtr()
{
	if (fErase && ptrTbl->fStore() )
		WriteElement(sSection().c_str(), (char*)0, (char*)0);
	if (0 != pcs) {
		delete pcs;
		pcs = 0;
	}  
	if (0 != pcv) {
		delete pcv;
		pcv = 0;
	} 
}

void ColumnPtr::StoreTime()
{
	WriteElement(sSection().c_str(), "Time", objtime);
}

void ColumnPtr::Store()
{
	//	if ( ptrTbl && ptrTbl->fStore() == false)
	//		return;
	if (fnObj.sFile.length() == 0)  // empty file name
		return;
	if (fErase || (sNam[0] == '#')/* temp column*/) {
		WriteElement(sSection().c_str(), NULL, NULL);
		return;
	}
	if ((0 != pcv) && (sDescription == ""))
		SetDescription(pcv->sExpression());
	WriteBaseInfo(sSection().c_str());
	WriteElement(sSection().c_str(), "Domain", dm());
	WriteElement(sSection().c_str(), "Range", vr());
	WriteElement(sSection().c_str(), "ReadOnly", _fReadOnly);
	WriteElement(sSection().c_str(), "OwnedByTable", fOwnedByTable());
	if (dvs.fValues()) {
		if (dvs.fRealValues())
			WriteElement(sSection().c_str(), "MinMax", _rrMinMax);
		else
			WriteElement(sSection().c_str(), "MinMax", _riMinMax);
		WriteElement(sSection().c_str(), "Sum", _rSum);
		WriteElement(sSection().c_str(), "Mean", _rMean);
		WriteElement(sSection().c_str(), "StdDev", _rStdDev);
	}
	else {
		WriteElement(sSection().c_str(), "MinMax", (char*)0);
		WriteElement(sSection().c_str(), "Sum", (char*)0);
		WriteElement(sSection().c_str(), "Mean", (char*)0);
		WriteElement(sSection().c_str(), "StdDev", (char*)0);
	}
	if (0 != pcs)
		pcs->Store();
	if (0 != pcv)
		pcv->Store();
	String s;
	if (0 == ReadElement(sSection().c_str(), "Type", s))
		WriteElement(sSection().c_str(), "Type", "ColumnStore");

	String sAddInfo("%s: AdditionalInfo", sSection().c_str());
	if (!fAdditionalInfo())
		WriteElement(sAddInfo.c_str(), (char*)NULL, (char*)NULL);
	else {
		String s;
		short iLines = 0;
		String sInfo = sAdditionalInfo();
		for (unsigned int i=0; i < sInfo.length(); ++i) {
			char c = sInfo[i];
			if (c == '\n') {
				WriteElement(sAddInfo.c_str(), String("Line%i", iLines).c_str(), s);
				s = String();
				iLines++;
			}
			else
				s &= c;
		}
		if (sInfo[sInfo.length()-1] != '\n') {
			WriteElement(sAddInfo.c_str(), String("Line%i", iLines).c_str(), s);
			iLines++;
		}
		WriteElement(sAddInfo.c_str(), "Lines", iLines);
	}
}


String ColumnPtr::sName(bool, const String&) const
{
	return sNam;
}

String ColumnPtr::sNameQuoted(bool, const String&) const
{
	return sNam.sQuote(false);
}

void ColumnPtr::SetDomainValueRangeStruct(const DomainValueRangeStruct& dvrs)
{
	// Check for equality of domains
	if (dm().fValid() && dvrs.dm().fValid())
	{
		// Check for equality of special domains without filenames first
		if (!dm()->fnObj.fValid() && !dvrs.dm()->fnObj.fValid())
			if (dm() == dvrs.dm())
				return;

		if (dm()->fnObj.fValid() && dvrs.dm()->fnObj.fValid())
			if (dm()->fnObj == dvrs.dm()->fnObj)
			{
				if (vr() == dvrs.vr())  // also works OK in case of invalid vr's
					return;
			}
	}
	dvs = dvrs;
	if (0 != pcv)
	{
		pcv->SetDomainValueRangeStruct(dvs);
		pcv->UnFreeze();
	}
	Updated();
}

void ColumnPtr::SetValueRange(const ValueRange& vr)
{
	dvs.SetValueRange(vr);
	if (0 != pcv) {
		pcv->SetDomainValueRangeStruct(dvs);
		pcv->UnFreeze();
	}
	Updated();
}

void ColumnPtr::SetMinMax(const RangeInt& ri)
{
	_riMinMax = ri;
	fChanged = true;
}

void ColumnPtr::SetMinMax(const RangeReal& rr)
{
	_rrMinMax = rr;
	fChanged = true;
}

String ColumnPtr::sSection() const
{
	return String("%SCol:%S", sSectionPrefix, sNameQuoted());
}

int ColumnPtr::ReadEntry(const char* sEntry, String& sValue)
{
	return ReadElement(sSection().c_str(), sEntry, sValue);
}

long ColumnPtr::iReadEntry(const char* sEntry)
{
	return iReadElement(sSection().c_str(), sEntry);
}

double ColumnPtr::rReadEntry(const char* sEntry)
{
	return rReadElement(sSection().c_str(), sEntry);
}

bool ColumnPtr::WriteEntry(const char* sEntry, const String& sValue)
{
	if (fErase)
		return false;
	return WriteElement(sSection().c_str(), sEntry, sValue);
}

bool ColumnPtr::WriteEntry(const char* sEntry, const char* sValue)
{
	if (fErase)
		return false;
	return WriteElement(sSection().c_str(), sEntry, sValue);
}


bool ColumnPtr::WriteEntry(const char* sEntry, bool fValue)
{
	if (fErase)
		return false;
	return WriteElement(sSection().c_str(), sEntry, fValue);
}

bool ColumnPtr::WriteEntry(const char* sEntry, long iValue)
{
	if (fErase)
		return false;
	return WriteElement(sSection().c_str(), sEntry, iValue);
}

bool ColumnPtr::WriteEntry(const char* sEntry, double rValue)
{
	if (fErase)
		return false;
	return WriteElement(sSection().c_str(), sEntry, rValue);
}

long ColumnPtr::iRaw(long iKey) const
{
	if (0 != pcs)
		return pcs->iRaw(iKey);
	if (0 != pcv)
		return pcv->iComputeRaw(iKey);
	return iUNDEF;
}

long ColumnPtr::iValue(long iKey) const
{
	if (!dvs.fValues())
		return iUNDEF;
	long raw = iRaw(iKey);
	return dvs.iValue(raw);
}

double ColumnPtr::rValue(long iKey) const
{
	if (!dvs.fValues())
		return rUNDEF;
	if (dvs.fUseReals()) {
		if (0 != pcs)
			return pcs->rValue(iKey);
		if (0 != pcv)
			return pcv->rComputeVal(iKey);
	}
	long raw = iRaw(iKey);
	return dvs.rValue(raw);
}

Coord ColumnPtr::cValue(long iKey) const
{
	if (0 != pcs)
		return pcs->cValue(iKey);
	if (0 != pcv)
		return pcv->cComputeVal(iKey);
	return crdUNDEF;
}

String ColumnPtr::sValue(long iKey, short iWidth, short iDec) const
{
	if (0 != pcs)
		return pcs->sValue(iKey, iWidth, iDec);
	if (0 != pcv)
		return pcv->sComputeVal(iKey, iWidth, iDec);
	return sUNDEF;
}

RangeReal ColumnPtr::rrMinMax()
{
	if (!dvs.fValues())
		return RangeReal();
	if (fChanged || _rrMinMax.rLo() > _rrMinMax.rHi())  // not yet set
		CalcMinMax();
	return _rrMinMax;
}

RangeInt ColumnPtr::riMinMax()
{
	if (!dvs.fValues())
		return RangeInt();
	if (fChanged || _riMinMax.iLo() > _riMinMax.iHi())  // not yet set
		CalcMinMax();
	return _riMinMax;
}

double ColumnPtr::rMean()
{
	if (!dvs.fValues())
		return rUNDEF;
	if (fChanged || _rMean == rUNDEF)  // not yet set
		CalcMinMax();
	return _rMean;
}

double ColumnPtr::rStdDev() 
{
	if (!dvs.fValues())
		return rUNDEF;
	if (fChanged || _rStdDev == rUNDEF)  // not yet set
		CalcMinMax();
	return _rStdDev;
}

double ColumnPtr::rSum() 
{
	if (!dvs.fValues())
		return rUNDEF;
	if (fChanged || _rSum == rUNDEF)  // not yet set
		CalcMinMax();
	return _rSum;
}



void ColumnPtr::CalcMinMaxSelection(const vector<long>& rows, RangeReal& rrMinMax, double& sum, double& stdev, double& avg) {
	if (!dvs.fValues()) {
		return;
	}
	bool all = rows.size() == 0;

	vector<bool> selection(this->iRecs());
	std::fill(selection.begin(), selection.end(), false);
	for(int i = 0; i < rows.size(); ++i)
		selection[ rows[i] - 1] = true;

	if (!fCalculated())
		Calc();
	int iRec = 0;
	long iRecord = 0;

	double rSum = 0, rSum2 = 0;
	bool fHasDeletedRecords = false; // default
	DomainSort * ds = dynamic_cast<DomainSort*>(dmKey().ptr());
	if (ds)
		if (ds->iSize() > ds->iNettoSize())
			fHasDeletedRecords = true;
	if (dvs.fRealValues()) {    //  Jelle - 23/12/96  weer terug gezet vanwege ander bij effecten
		double rMin = DBL_MAX;
		double rMax = -DBL_MAX;
		double r;
		if (!fHasDeletedRecords)
		{
			RealBuf buf(1000);
			long i = 0;
			for (; i < iRecs() / 1000; i++) {
				GetBufVal(buf, iOffset()+i*1000, 1000);
				for (long j = 0; j < 1000; ++j) {
					if (!all) {
						if ( selection[iRecord++] == false)
							continue;
					}
					try {
						r = buf[j];
					}
					catch (...) {
						continue;
					}
					if (r == rUNDEF)
						continue;
					iRec++;
					if (r < rMin)
						rMin = r;
					if (r > rMax)
						rMax = r;
					rSum += r;
					rSum2 += r*r;
				}
			}
			long iRecsLeft = iRecs() % 1000;
			if (iRecsLeft > 0) {
				GetBufVal(buf, iOffset()+i*1000, iRecsLeft);
				for (long j = 0; j < iRecsLeft ; ++j) {
					if (!all) {
						if ( selection[iRecord++] == false)
							continue;
					}
					try {
						r = buf[j];
					}
					catch (...) {
						continue;
					}
					if (r == rUNDEF)
						continue;
					iRec++;
					if (r < rMin)
						rMin = r;
					if (r > rMax)
						rMax = r;
					rSum += r;
					rSum2 += r*r;
				}
			}
		}
		else // Table has deleted Records - "double" case
		{
			for (long i = iOffset(); i < iOffset() + ds->iNettoSize(); ++i)
			{
				try {
					r = rValue(ds->iKey(i));
				}
				catch (...) {
					continue;
				}
				if (r == rUNDEF)
					continue;
				iRec++;
				if (r < rMin)
					rMin = r;
				if (r > rMax)
					rMax = r;
				rSum += r;
				rSum2 += r*r;
			}
		}
		rrMinMax = RangeReal(rMin, rMax);
	}
	else {
		long iMin = LONG_MAX;
		long iMax = -LONG_MAX;
		long l;
		if (!fHasDeletedRecords)
		{
			LongBuf buf(1000);
			long i = 0;
			for (; i < iRecs() / 1000; i++) {
				GetBufVal(buf, iOffset()+i*1000, 1000);
				for (long j = 0; j < 1000; ++j) {
					if (!all) {
						if ( selection[iRecord++] == false)
							continue;
					}
					if ( (l = buf[j]) == iUNDEF)
						continue;
					iRec++;
					if (l < iMin)
						iMin = l;
					if (l > iMax)
						iMax = l;
					rSum += l;
					rSum2 += ((double)l)*l;

				}
			}
			long iRecsLeft = iRecs() % 1000;
			if (iRecsLeft > 0) {
				GetBufVal(buf, iOffset()+i*1000, iRecsLeft);
				for (long j = 0; j < iRecsLeft ; ++j) {
					if (!all) {
						if ( selection[iRecord++] == false)
							continue;
					}
					if ((l = buf[j]) ==  iUNDEF)
						continue;
					iRec++;
					if (l < iMin)
						iMin = l;
					if (l > iMax)
						iMax = l;
					rSum += l;
					rSum2 += ((double)l)*l;
				}
			}
		}
		else // Table has deleted Records - "long" case
		{
			for (long i = iOffset(); i < iOffset() + ds->iNettoSize(); ++i)
			{
				try {
					l = iValue(ds->iKey(i));
				}
				catch (...) {
					continue;
				}
				if (l == iUNDEF)
					continue;
				iRec++;
				if (l < iMin)
					iMin = l;
				if (l > iMax)
					iMax = l;
				rSum += l;
				rSum2 += ((double)l)*l;
			}
		}
		rrMinMax = RangeReal(doubleConv(iMin), doubleConv(iMax));
	}
	sum = iRec != 0 ? rSum : rUNDEF;

	if (iRec > 0) {
		avg = rSum / iRec;
		if (iRec > 1) {
			long double r = rSum2 * ((long double) iRec ) - ((long double)rSum)*rSum;
			if (r < 0)
				stdev = 0;
			else
				stdev = sqrt(r / iRec / (iRec-1)); 
		}
		else
			stdev = 0;
	}
	else {
		avg = stdev = rUNDEF;
	}
	if (dm()->pdbool() || dm()->pdbit()) {
		avg = stdev = rUNDEF;
	}
}

void ColumnPtr::CalcMinMax()
{
	if (!dvs.fValues()) {
		return;
	}

	if (!fCalculated())
		Calc();
	int iRec = 0;
	double rSum = 0, rSum2 = 0;
	bool fHasDeletedRecords = false; // default
	DomainSort * ds = dynamic_cast<DomainSort*>(dmKey().ptr());
	if (ds)
		if (ds->iSize() > ds->iNettoSize())
			fHasDeletedRecords = true;
	if (dvs.fRealValues()) {    //  Jelle - 23/12/96  weer terug gezet vanwege ander bij effecten
		double rMin = DBL_MAX;
		double rMax = -DBL_MAX;
		double r;
		if (!fHasDeletedRecords)
		{
			RealBuf buf(1000);
			long i = 0;
			for (; i < iRecs() / 1000; i++) {
				GetBufVal(buf, iOffset()+i*1000, 1000);
				for (long j = 0; j < 1000; ++j) {
					try {
						r = buf[j];
					}
					catch (...) {
						continue;
					}
					if (r == rUNDEF)
						continue;
					iRec++;
					if (r < rMin)
						rMin = r;
					if (r > rMax)
						rMax = r;
					rSum += r;
					rSum2 += r*r;
				}
			}
			long iRecsLeft = iRecs() % 1000;
			if (iRecsLeft > 0) {
				GetBufVal(buf, iOffset()+i*1000, iRecsLeft);
				for (long j = 0; j < iRecsLeft ; ++j) {
					try {
						r = buf[j];
					}
					catch (...) {
						continue;
					}
					if (r == rUNDEF)
						continue;
					iRec++;
					if (r < rMin)
						rMin = r;
					if (r > rMax)
						rMax = r;
					rSum += r;
					rSum2 += r*r;
				}
			}
		}
		else // Table has deleted Records - "double" case
		{
			for (long i = iOffset(); i < iOffset() + ds->iNettoSize(); ++i)
			{
				try {
					r = rValue(ds->iKey(i));
				}
				catch (...) {
					continue;
				}
				if (r == rUNDEF)
					continue;
				iRec++;
				if (r < rMin)
					rMin = r;
				if (r > rMax)
					rMax = r;
				rSum += r;
				rSum2 += r*r;
			}
		}
		_rrMinMax = RangeReal(rMin, rMax);
		_riMinMax = RangeInt(longConv(rMin), longConv(rMax));
	}
	else {
		long iMin = LONG_MAX;
		long iMax = -LONG_MAX;
		long l;
		if (!fHasDeletedRecords)
		{
			LongBuf buf(1000);
			long i = 0;
			for (; i < iRecs() / 1000; i++) {
				GetBufVal(buf, iOffset()+i*1000, 1000);
				for (long j = 0; j < 1000; ++j) {
					l = buf[j];
					if (l == iUNDEF)
						continue;
					iRec++;
					if (l < iMin)
						iMin = l;
					if (l > iMax)
						iMax = l;
					rSum += l;
					rSum2 += ((double)l)*l;

				}
			}
			long iRecsLeft = iRecs() % 1000;
			if (iRecsLeft > 0) {
				GetBufVal(buf, iOffset()+i*1000, iRecsLeft);
				for (long j = 0; j < iRecsLeft ; ++j) {
					l = buf[j];
					if (l == iUNDEF)
						continue;
					iRec++;
					if (l < iMin)
						iMin = l;
					if (l > iMax)
						iMax = l;
					rSum += l;
					rSum2 += ((double)l)*l;
				}
			}
		}
		else // Table has deleted Records - "long" case
		{
			for (long i = iOffset(); i < iOffset() + ds->iNettoSize(); ++i)
			{
				try {
					l = iValue(ds->iKey(i));
				}
				catch (...) {
					continue;
				}
				if (l == iUNDEF)
					continue;
				iRec++;
				if (l < iMin)
					iMin = l;
				if (l > iMax)
					iMax = l;
				rSum += l;
				rSum2 += ((double)l)*l;
			}
		}
		_rrMinMax = RangeReal(doubleConv(iMin), doubleConv(iMax));
		_riMinMax = RangeInt(iMin, iMax);
	}
	_rSum = iRec != 0 ? rSum : rUNDEF;

	if (iRec > 0) {
		_rMean = rSum / iRec;
		if (iRec > 1) {
			long double r = rSum2 * ((long double) iRec ) - ((long double)rSum)*rSum;
			if (r < 0)
				_rStdDev = 0;
			else
				_rStdDev = sqrt(r / iRec / (iRec-1)); 
		}
		else
			_rStdDev = 0;
	}
	else {
		_rMean = _rStdDev = rUNDEF;
	}
	if (dm()->pdbool() || dm()->pdbit()) {
		_rMean = _rStdDev = rUNDEF;
	}
}

void ColumnPtr::Updated()
{
	IlwisObjectPtr::Updated();
	_riMinMax = RangeInt();
	_rrMinMax = RangeReal();
	WriteElement(sSection().c_str(), "MinMax", (char*)0);
	WriteElement(sSection().c_str(), "Sum", (char*)0);
	WriteElement(sSection().c_str(), "Mean", (char*)0);
	WriteElement(sSection().c_str(), "StdDev", (char*)0);
	if (!fErase)
		StoreTime();
}

CoordinateSequence *ColumnPtr::iGetValue(long iKey, bool fResize) const
{
	if (0!=pcs)
		return pcs->iGetValue(iKey);

	return NULL;
}

void ColumnPtr::GetBufRaw(ByteBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufRaw(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufRaw(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = 0;
}

void ColumnPtr::GetBufRaw(IntBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufRaw(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufRaw(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = shUNDEF;
}

void ColumnPtr::GetBufRaw(LongBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufRaw(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufRaw(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = iUNDEF;
}

void ColumnPtr::GetBufVal(LongBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufVal(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufVal(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = iUNDEF;
}

void ColumnPtr::GetBufVal(RealBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufVal(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufVal(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = rUNDEF;
}

void ColumnPtr::GetBufVal(CoordBuf& b, long iKey, long iNr) const
{
	if (0 != pcs)
		pcs->GetBufVal(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufVal(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = crdUNDEF;
}

void ColumnPtr::GetBufVal(StringBuf& b, long iKey, long iNr, short iWid, short iDec) const
{
	if (0 != pcs)
		pcs->GetBufVal(b,  iKey, iNr);
	else if (0 != pcv)
		pcv->ComputeBufVal(b,  iKey, iNr);
	else
		for (int i = 0; i < b.iSize(); ++i)
			b[i] = sUNDEF;
}

Domain ColumnPtr::dmFromStrings(const FileName& fnDom, DomainType dmt)
{
	set<String> values;

	long iMax = iOffset() + iRecs();
	long iStart = iOffset();
	String s;
	for (long i=iStart; i < iMax; i++)
	{
		s = sValue(i,0);
		if (s != sUNDEF)
			values.insert(s);
	}
	Domain dm(fnDom, 0, dmt);
	DomainSort *ds = dm->pdsrt();
	vector<String> vv(values.size());
	copy(values.begin(), values.end(), vv.begin());
	ds->AddValues(vv);
	return dm;
}

DomainInfo ColumnPtr::dminf() const
{
	return DomainInfo(fnObj, sSection().c_str());
}

void ColumnPtr::Rename(const String& sNewColName)
{
	const_cast<String&>(sNam) = sNewColName;
}

bool ColumnPtr::fConvertTo(const DomainValueRangeStruct& dvs, const Column& col)
{
	if (col.fValid() && col->dvrs() == dvs)
		return false;
	else if (dvrs() == dvs)
		return false;
	bool f = false;
	if (0 != pcs && 0 == pcv)
		f = pcs->fConvertTo(dvs, col);
	if (col.fValid())
		SetDomainValueRangeStruct(col->dvrs());
	else
		SetDomainValueRangeStruct(dvs);
	if (0 != pcv)
		Calc();
	return f;
}

bool ColumnPtr::fMonotone(bool fAscending) const
{
	if (iRecs() == 0)
		return false;
	double r2;
	double r1 = rValue(iOffset());
	for (long i=iOffset()+1; i < iOffset()+iRecs()-1; ++i) {
		r2 = rValue(i);
		if (fAscending && (r2 <= r1))
			return false;
		else if (r2 >= r1)
			return false;
		r1 = r2;
	}
	return true;
}
void ColumnPtr::PutRaw(long iKey, long iRaw)
{
	if (0 != pcs)
		pcs->PutRaw(iKey, iRaw);
	if ( ptrTbl->fUseAs() )
		ptrTbl->PutLongInForeign(sName(), iKey, iRaw);		
}

void ColumnPtr::PutVal(long iKey, long iVal)
{
	if (0 != pcs)
		pcs->PutVal(iKey, iVal);
	if ( ptrTbl->fUseAs() )
		ptrTbl->PutLongInForeign(sName(), iKey, iVal);		
}

void ColumnPtr::PutVal(long iKey, double rVal)
{
	if (0 != pcs)
		pcs->PutVal(iKey, rVal);
	if ( ptrTbl->fUseAs() )
		ptrTbl->PutRealInForeign(sName(), iKey, rVal);	
}

void ColumnPtr::PutVal(long iKey, const Coord& cVal)
{
	if (0 != pcs)
		pcs->PutVal(iKey, cVal);
}

void ColumnPtr::PutVal(long iKey, const CoordinateSequence *seq, long iSz)
{
	if (0 != pcs)
		pcs->PutVal(iKey, seq, iSz);
}

void ColumnPtr::PutVal(long iKey, const String& sVal)
{
	if ( ptrTbl->fUseAs() && !fLoadingForeignData)
		ptrTbl->PutStringInForeign(sName(), iKey, sVal);	
	if (0 != pcs)
		pcs->PutVal(iKey, sVal);
}

void ColumnPtr::PutBufRaw(const ByteBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufRaw(buf, iKey, iNr);
}

void ColumnPtr::PutBufRaw(const IntBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufRaw(buf, iKey, iNr);
}

void ColumnPtr::PutBufRaw(const LongBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufRaw(buf, iKey, iNr);
}

void ColumnPtr::PutBufVal(const LongBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufVal(buf, iKey, iNr);
}

void ColumnPtr::PutBufVal(const RealBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufVal(buf, iKey, iNr);
}

void ColumnPtr::PutBufVal(const StringBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufVal(buf, iKey, iNr);
}

void ColumnPtr::PutBufVal(const CoordBuf& buf, long iKey, long iNr)
{
	if (0 != pcs)
		pcs->PutBufVal(buf, iKey, iNr);
}

void ColumnPtr::DeleteRec(long iStartRec, long iRecs)
{
	if (0 != pcs)
		pcs->DeleteRec(iStartRec, iRecs);
	else
		_iRecs -= iRecs;
}

void ColumnPtr::AppendRec(long iRecs)
{
	if (0 != pcs)
		pcs->AppendRec(iRecs);
	else
		_iRecs += iRecs;
}

void ColumnPtr::Fill()
{
	if (0 != pcs)
		pcs->Fill();
}

bool ColumnPtr::fDependent() const
{
	if (0 != pcv)
		return true;
	String s;
	ReadElement(sSection().c_str(), "Type", s);
	return (s.length() != 0) && (!fCIStrEqual(s , "ColumnStore"));
}

String ColumnPtr::sExpression() const
{
	if (0 != pcv)
		return pcv->sExpression();

	String s;
	ReadElement(sSection().c_str(), "Expression", s);

	return s;
}

bool ColumnPtr::fCalculated() const
// returns true if a calculated result exists
{
	if (!fDependent())
		return IlwisObjectPtr::fCalculated();
	return 0 != pcs;
}

bool ColumnPtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
	if (!fDependent())
		return IlwisObjectPtr::fDefOnlyPossible();
	bool f;
	if (0 != ReadElement(sSection().c_str(), "DefOnlyPossible", f))
		return f;
	return false;
}

void ColumnPtr::Calc(bool fMakeUpToDate)
// calculates the result
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenColumnVirtual();
	if (fMakeUpToDate)
		if (!objdep.fUpdateAll())
			return;
	if (0 != pcv)
	{
		pcv->Freeze();
	}		
}


void ColumnPtr::DeleteCalc()
// deletes calculated  result
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenColumnVirtual();
	if (0 != pcv)
		pcv->UnFreeze();
}

void ColumnPtr::OpenColumnVirtual()
{
	if (0 != pcv) // already there
		return;
	if (!fDependent())
		return;
	try {
		objdep = ObjectDependency(fnObj, sName());
		pcv = ColumnVirtual::create(Table(fnTbl), sName(), *this);
		// for downward compatibility with 2.02 :
		String s;
		if (0 == ReadElement(sSection().c_str(), "DomainInfo", s)) {
			SetDomainChangeable(pcv->fDomainChangeable());
			SetValueRangeChangeable(pcv->fValueRangeChangeable());
			SetExpressionChangeable(pcv->fExpressionChangeable());
		}
	}
	catch (const ErrorObject&) {
		pcv = 0;
		objdep = ObjectDependency();
	}
}

void ColumnPtr::CreateColumnStore()
{
	if (0 != pcv)
		pcv->CreateColumnStore();
}

void ColumnPtr::Replace(const String& sExpr)
{
	ColumnVirtual* pcvx = ColumnVirtual::create(Table(fnTbl), sName(), *this, sExpr, DomainValueRangeStruct()/*, dvrs()*/);
	if (0 == pcvx)
		return;
	Column col;
	col.SetPointer(this);
	if (objdep.fUses(col)) {
		delete pcvx;
		// reset to previous
		pcvx = ColumnVirtual::create(Table(fnTbl), sName(), *this, sExpression(), dvrs());
		delete pcvx;
		throw ErrorObject(TR("Cyclic definition"));
	}
	if (0 != pcv)
		delete pcv;
	pcv = pcvx;
	if (0 != pcv) {
		SetDomainChangeable(pcv->fDomainChangeable());
		SetValueRangeChangeable(pcv->fValueRangeChangeable());
		SetGeoRefChangeable(pcv->fGeoRefChangeable());
		SetExpressionChangeable(pcv->fExpressionChangeable());
		pcv->UnFreeze();
	}
}

bool ColumnPtr::fOwnedByTable() const
{
	return _fOwnedByTable;
}

void ColumnPtr::SetOwnedByTable(bool f)
{
	_fOwnedByTable = f;
}

String ColumnPtr::sObjectSection() const
{
	return sSection().c_str();
}

void ColumnPtr::GetNewestDependentObject(String& sObjName, ObjectTime& tmNewer) const
{
	Array<FileName> afnChecked;
	ObjectDependency::GetNewestDependentObject(fnObj, sName(), objtime, sObjName, tmNewer, afnChecked);
}

DomainValueRangeStruct ColumnPtr::dvrsDefault(const String& sExpression) const
{
	const_cast<ColumnPtr *>(this)->OpenColumnVirtual();
	if (0 == pcv)
		return DomainValueRangeStruct();
	return pcv->dvrsDefault(sExpression);
}

void ColumnPtr::DependencyNames(Array<String>& asNames) const
{ 
	ObjectDependency::ReadNames(sSection().c_str(), fnObj, asNames);
}

bool ColumnPtr::fMergeDomainForCalc(Domain& dm, const String& sExpr)
{
	OpenColumnVirtual();
	if (0 == pcv)
		return true;
	return pcv->fMergeDomainForCalc(dm, sExpr);
}

bool ColumnPtr::fBinary() const
{
	ColumnBinary* cb = dynamic_cast<ColumnBinary*>(pcs->csb);
	return 0 != cb;
}

BinMemBlock ColumnPtr::binValue(long iKey) const
{
	ColumnBinary* cb = dynamic_cast<ColumnBinary*>(pcs->csb);
	if (0 == cb)
		throw ErrorObject(TR("Binary value asked for non-binary Column"), -1);
	BinMemBlock bmb;
	cb->GetVal(iKey, bmb);
	return bmb;
}

void ColumnPtr::GetVal(long iKey, BinMemBlock& bmb) const
{
	ColumnBinary* cb = dynamic_cast<ColumnBinary*>(pcs->csb);
	if (0 == cb)
		throw ErrorObject(TR("Binary value asked for non-binary Column"), -1);
	cb->GetVal(iKey, bmb);
}

void ColumnPtr::PutVal(long iKey, const BinMemBlock& bmb)
{
	ColumnBinary* cb = dynamic_cast<ColumnBinary*>(pcs->csb);
	if (0 == cb)
		throw ErrorObject(TR("Binary value asked for non-binary Column"), -1);
	cb->PutVal(iKey, bmb);
}

ColumnCoordBuf *ColumnPtr::pcbuf()
{
	return dynamic_cast<ColumnCoordBuf*>(pcs->pcsb());
}


bool ColumnPtr::fUniqueValues() const
{
	if (fRawAvailable()) {
		set<int> values;
		pair<set<int>::iterator, bool> p;
		for (int i=iOffset(); i < iOffset()+iRecs()-1; ++i) {
			p = values.insert(iRaw(i));
			if (!p.second)
				return false;
		}
	}
	else if (fUseReals()) {
		set<double> values;
		pair<set<double>::iterator, bool> p;
		for (int i=iOffset(); i < iOffset()+iRecs()-1; ++i) {
			p = values.insert(rValue(i));
			if (!p.second)
				return false;
		}
	}
	else {
		set<String> values;
		pair<set<String>::iterator, bool> p;
		for (int i=iOffset(); i < iOffset()+iRecs()-1; ++i) {
			p = values.insert(sValue(i));
			if (!p.second)
				return false;
		}
	}
	return true;
}

void ColumnPtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();

}

bool ColumnPtr::fIsLoaded() const
{
	if ( pcs)
		return pcs->fIsLoaded();

	return false;
}

void ColumnPtr::Loaded(bool fValue)
{
	if ( pcs )
		pcs->Loaded(fValue);
}

void ColumnPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles() )
	{	
		String sSection("%SCol:%S", ptrTbl->sSectionPrefix, sName());			
		os.AddFile(fnObj, sSection, "Domain", ".dom");			
	}
}

void ColumnPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	ObjectDependency od(fnObj, sName());
	Array<FileName> afnNames;
	od.Names(afnNames);
	FileName fnSelf(fnObj);
	for(unsigned int i=0; i< afnNames.size(); ++i)
	{
		if ( afnNames[i] != fnObj ) // prevent circles
			afnObjDep &= afnNames[i];

	}
}

IlwisObjectVirtual *ColumnPtr::pGetVirtualObject() const
{
	return pcv;
}

void ColumnPtr::SetLoadingForeignData(bool fYesNo)
{
	fLoadingForeignData = fYesNo;
}

bool ColumnPtr::fGetLoadingForeignData() const
{
	return fLoadingForeignData;
}
