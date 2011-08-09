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
/* $Log: /ILWIS 3.0/BasicDataStructures/tblstore.cpp $
 * 
 * 46    22-04-04 10:22 Willem
 * [Bug=6471]
 * Added protection to avoid access outside the boundaries of the column
 * array "ac"
 * 
 * 45    3-11-03 18:14 Retsios
 * colNew now really returns an undef column if needed (instead of an
 * invalid column that goes out of scope)
 * 
 * 44    27-05-03 9:29 Willem
 * [Bug=6430]
 * - Added: Function to store active column names and their number in the
 * ODF; this is combined in the RemoveCol function, which used to remove
 * only the [col:<name>] section of the ODF without adjusting the rest of
 * the ODF. The result after RemoveCol is a consistent ODF.
 * 
 * 43    22-04-03 15:05 Willem
 * - Added: Boolean property (member plus accessor functions) to indicate
 * that a TableStore has already been loaded from disk.
 * - Changed: Removed code that changed the number of Columns in a
 * TableStore, when reading from disk. This is not allowed, because newly
 * calculated Columns just added in memory are wrongfully deleted this
 * way.
 * - Changed: Some code cleanup
 * 
 * 42    9-01-03 16:56 Willem
 * - Changed: The readonly status of a TableStore is now determined by the
 * R/O status of an existing datafile, not by its presence
 * 
 * 41    10/30/01 8:30a Martin
 * useas file should not try to write a datafile member to the odf
 * 
 * 40    9/24/01 9:58a Martin
 * LoadBinary has become virtual. Derivatives of TableStore (e.g.
 * TableBinary) may now overrule this function and load in their own way
 * 
 * 39    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 38    9-08-01 12:19 Koolhoven
 * TableStore::Store() fUseAs() is a function, do not store the pointer
 * but the return value
 * 
 * 37    8/07/01 12:06p Martin
 * writes and read now properly the useas flag to the odf and added
 * functions for putting data in foreign objects
 * 
 * 36    16-03-01 17:55 Koolhoven
 * protected TableStore::LoadBinary() against not all columns valid
 * 
 * 35    8-03-01 2:03p Martin
 * column section is also removed if the column is deleted from the table
 * 
 * 34    5-03-01 17:10 Koolhoven
 * protected RemoveCol() against invalid columns in the array
 * 
 * 33    2-03-01 19:02 Koolhoven
 * protect RemoveCol() against invalid columns, should not be needed
 * 
 * 32    23/02/01 14:59 Willem
 * colNew() functions now only return a valid column in case the domain is
 * valid.
 * 
 * 31    12-02-01 9:22a Martin
 * made the error of tablestore somewhat more clear (i hope)
 * 
 * 30    21-12-00 14:50 Koolhoven
 * colNew() functions adapted:
 * - assert on empty name
 * -  set loaded flag on
 * 
 * 29    8-12-00 11:46 Koolhoven
 * TableStore::colNew() replaces dots with underscores to prevent illegal
 * names
 * 
 * 28    28/11/00 9:54 Willem
 * The check whether the data file of a table exists now first checks if
 * the data file name is valid.
 * 
 * 27    17/11/00 12:23 Willem
 * Added checking for the limitations for Ilwis 1.4 tables: maximum nr of
 * records and columns
 * 
 * 26    16/11/00 12:38 Willem
 * Export table to Ilwis 1.4 now trims spaces of strings and truncates the
 * strings to 20 characters
 * 
 * 25    10/11/00 11:02 Willem
 * TableStore now throws an error when the data file is not found
 * 
 * 24    2-11-00 15:08 Hendrikse
 * placed check on  fLoaded flags earlier, to prevent repeated call of
 * LoadBinary() in PutRaw() 
 * 
 * 23    11/01/00 2:00p Martin
 * removed superflous GetObjectStructure. They are all handled in the
 * relevant ilwisobject
 * 
 * 22    30-10-00 19:34 Koolhoven
 * protected LoadBinary() against invalid value (undef or zero) of iNrCols
 * 
 * 21    11-10-00 2:14p Martin
 * section prefix added when reading the actual number of columns in the
 * binary file
 * 
 * 20    5-10-00 10:43a Martin
 * tblstore will on loadtime not use the number of columns in the tableptr
 * but the number of columns that the ODF has. This may differ as
 * temporary columns may have been created
 * 
 * 19    9/26/00 11:35a Martin
 * in removecol prevented the store of the column. This will modify the
 * file. Tables should only store themselves (and their columns) when
 * explicitely told to do it.
 * 
 * 18    22/09/00 9:41 Willem
 * TableStore now checks if there are columns before traversing all
 * records.
 * 
 * 17    9/19/00 9:05a Martin
 * improved DoNoStore function for tables. It will now also protect the
 * columns
 * 
 * 16    9/18/00 9:37a Martin
 * the table loads itself (abd puts its state on loaded) only if
 * explicitly requested.
 * 
 * 15    12-09-00 9:13a Martin
 * added guards to prevent a store to ODF when not wanted
 * 
 * 14    11-09-00 10:06a Martin
 * changed the structure of ObjectStructure object to include odf entry
 * information
 * 
 * 13    8-09-00 3:21p Martin
 * added function to set the fChanged member of all the 'members of an
 * object.
 * added function to retrieve the 'structure' of an object (filenames)
 * 
 * 12    5/17/00 5:11p Wind
 * removed a very strange 'improvement' which caused that new virtual
 * tables never stored their data in a binary file
 * 
 * 11    21-02-00 4:35p Martin
 * Added a function to quickly add a whole set of values (AddValues)
 * 
 * 10    18-01-00 12:16 Wind
 * made Load and StoreBinary more efficient
 * 
 * 9     21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 8     29-10-99 12:59 Wind
 * case sensitive stuff
 * 
 * 7     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 6     9/29/99 5:47p Wind
 * debugged quoted column names
 * 
 * 5     9/27/99 11:13a Wind
 * changed calls to static ObjectInfo::ReadElement and WriteElement to
 * member function calls
 * 
 * 4     9/08/99 9:01a Wind
 * added support  for quoted column names
 * 
 * 3     15-03-99 10:08 Koolhoven
 * Header comments
 * 
 * 2     3/12/99 12:03p Martin
 * Case insensitive support added
// Revision 1.8  1998/10/19 12:13:31  Wim
// In DeleteRec() check of not to delete too much records take care of offset
// to allow to delete the last record
//
// Revision 1.7  1998-10-09 13:03:21+01  Wim
// In DeleteRec() never delete more records when there are.
//
// Revision 1.6  1998-09-16 18:25:30+01  Wim
// 22beta2
//
// Revision 1.5  1997/09/18 17:06:34  Wim
// Take care of special extensions for histograms
//
// Revision 1.4  1997-08-21 19:32:22+02  Wim
// When adding records append all columns not only nonvirtual ones
//
// Revision 1.3  1997-08-14 14:38:41+02  Wim
// Do not store in a filename which is empty
//
// Revision 1.2  1997-08-12 18:11:22+02  Wim
// Do not store table during colNew()
//
/* TableStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   19 Oct 98    1:12 pm
*/
#include "Engine\Table\tblstore.h"
#include "Engine\Table\tbltbl.h"
#include "Engine\Table\TBLBIN.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\Colbinar.h"
#include "Engine\Table\COLCOORD.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmcoord.h"
#include "Headers\Hs\CONV.hs"
#include "Headers\Hs\Table.hs"

static Column colUNDEF;

TableStore::TableStore(const FileName& fn, TablePtr& p)
	: ac(p.iCols()), fLoaded(false),
	ptr(p), fErase(false)
{
	String sEntry, sColName;
	int c;
	String s;
	if (0 == ptr.ReadElement(ptr.sSection("TableStore").c_str(), "StoreTime", timStore))
		timStore = ptr.objtime;
	ptr.ReadElement(ptr.sSection("TableStore").c_str(), "Data", ptr.fnDat);
	bool fUseAs;
	ptr.ReadElement(ptr.sSection("TableStore").c_str(), "UseAs", fUseAs);		
	ptr.SetUseAs(fUseAs);
	if (ptr.fnDat.fValid() && !File::fExist(ptr.fnDat))
		NotFoundError(ptr.fnDat);

	// intentional code block:
	{ 
		Table t;
	    t.SetPointer(&ptr);
	    ptr.iRef++; // destructor checks on nr. ref.
	    for (c = 0; c < iCols(); ++c)
		{
			sEntry = String("Col%i", c);
			ptr.ReadElement(ptr.sSection(String("TableStore")).c_str(), sEntry.c_str(), sColName);
			sColName = sColName.sQuote(); // single quotes are stripped by ReadPrivateProfileString
			try
			{
				ac[c] = Column(t, sColName);
			}
			catch (const ErrorObject& err)
			{
				String sErr = err.sShowError() + "\n";
				sErr += String(TR("Could not create column: %S").c_str(), sColName);
				ErrorObject(sErr).Show();
			}
	    }
	}
	
	ptr.iRef--; // see iRef++
	
	ptr._fReadOnly = false;
	if (File::fExist(ptr.fnDat))
		ptr._fReadOnly = _access(ptr.fnDat.sFullName().c_str(), 2) == -1;  // mode 2: check for write permission
}

TableStore::TableStore(const FileName& fnFil, TablePtr& p, const FileName& _fnDat)
: ac(p.iCols()), ptr(p), fErase(false), fLoaded(false)
{
	// Jelle: next line appeared mysteriously between ssv 10 and 11, and is completely wrong:
	//if ( _fnDat.sFullPath() == "") return; // in memory object no data files needed
	// may be the following was meant
	if ( ptr.fnObj.sFullPath() == "") return; // in memory object no data files needed
  if (!_fnDat.fValid()) {
    ptr.fnDat = fnFil;
    ptr.fnDat.sExt[ptr.fnDat.sExt.size() - 1] = '#';
    if (fCIStrEqual(".his", fnFil.sExt))
      ptr.fnDat.sExt = ".hi#";
    else if (fCIStrEqual(".hsa", fnFil.sExt))
      ptr.fnDat.sExt = ".ha#";
    else if (fCIStrEqual(".hss", fnFil.sExt))
      ptr.fnDat.sExt = ".hs#";
    else if (fCIStrEqual(".hsp", fnFil.sExt))
      ptr.fnDat.sExt = ".hp#";
  }
  else
    ptr.fnDat = _fnDat;
}

TableStore::~TableStore()
{
  if (ptr.fErase || fErase) {
    _unlink(ptr.fnDat.sFullName().c_str());
    ptr.WriteElement(ptr.sSection("TableStore").c_str(), (char*)0, (char*)0);
    // delete column info
    for (long c = 0; c < iCols(); ++c) {
      String sColName;
      String sEntry = String("Col%i", c);
      if (0 != ptr.ReadElement(ptr.sSection("TableStore").c_str(), sEntry.c_str(), sColName))
        ptr.WriteElement(ptr.sSection(String("Col:%S", sColName)).c_str(), (char*)0, (char*)0);
    }
  } 
}

void TableStore::UnStore(const FileName& fnObj)
{
  FileName fnData;
  if (ObjectInfo::ReadElement("TableStore", "Data", fnObj, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  ObjectInfo::WriteElement("TableStore", (char*)0, fnObj, (char*)0);
}

void TableStore::SetErase(bool f)
{
  fErase = f;
} 

// Rewrite the column names information:
// - renew the names of the still existing columns
// - update the column count
void TableStore::StoreColNames()
{
	for (short c = 0; c < iCols(); ++c)
	{
		String sEntry("Col%i", c);
		ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), sEntry.c_str(), ptr.fnObj, ac.ind(c)->sNameQuoted());
	}
	// Update the column count
	ObjectInfo::WriteElement(ptr.sSection("Table").c_str(), "Columns", ptr.fnObj, iCols());
}

void TableStore::Store()
{
	if (ptr.fnObj.sFile.length() == 0)  // empty file name
		return;
	long iOldNrCol = iCols();
	ptr.ReadElement(ptr.sSection("Table").c_str(), "Columns", iOldNrCol);
	if ( ptr.fUseAs() == false)
		ptr.WriteElement(ptr.sSection("TableStore").c_str(), "Data", ptr.fnDat);
	ptr.WriteElement(ptr.sSection("TableStore").c_str(), "StoreTime", timStore);
	ptr.WriteElement(ptr.sSection("TableStore").c_str(), "UseAs", ptr.fUseAs());	
	long iNonErasedCols = 0;
	short c = 0;
	for (; c < iCols(); ++c) {
		String sEntry("Col%i", iNonErasedCols);
		ac[c]->Store();
		if (!ac.ind(c)->fErase) {
			ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), sEntry.c_str(), ptr.fnObj, ac.ind(c)->sNameQuoted());
			++iNonErasedCols;
		}
	}
	for (c = iNonErasedCols; c < iOldNrCol; ++c) {
		String sEntry("Col%i", c);
		ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), sEntry.c_str(), ptr.fnObj, (const char*)0);
	}  
	ObjectInfo::WriteElement(ptr.sSection("Table").c_str(), "Columns", ptr.fnObj, iNonErasedCols);
}

void TableStore::SetDataFile(const FileName& fn)
{
	ptr.fnDat = fn;
}

long TableStore::iCol(const String& sName) const
{
	String sNam = sName.sUnQuote();
	for (long i = 0; i < iCols(); i++)
		if (fCIStrEqual(ac[i]->sName(), sNam))
			return i;
		return -1;
}

const Column& TableStore::colNew(const String& sName, const Domain& dm, const ValueRange& vr)
{
	return colNew(sName, DomainValueRangeStruct(dm, vr));
}

const Column& TableStore::colNew(const String& sName, const DomainValueRangeStruct& dvs)
{
	// If domain is invalid, do not create column
	if (!dvs.dm().fValid())
		return colUNDEF;

	ASSERT (sName != "");
	Column col;
	{	
		// a dot indicates table.column and though should never be allowed in a new column name
		String sColName = sName;
		replace(sColName.begin(), sColName.end(), '.', '_');
		if ("" == sColName) // should never happen, see assert
			sColName = "Col";
		
		Table t;
		t.SetPointer(&ptr);
		ptr.iRef++; // destructor checks on nr. ref.
		Column c(t, sColName, dvs);
		if ( c.fValid() )
		{
			c->fChanged = false;
			c->Loaded(true);
			col = c;
			ptr.fChanged = false;  // do not call store yet
		}			
	}
	ptr.iRef--;
	ptr.fChanged = true;

	if (col.fValid())
		return AddCol(col); 
	else
		return colUNDEF;
}

const Column& TableStore::col(int c) const
{
	if (c < ac.size())
		return const_cast<Column&> (ac.ind(c));
	return colUNDEF;
}

const Column& TableStore::col(const String& sName) const
{
	String sNam = sName.sUnQuote();
	for (short c = 0; c < iCols(); ++c)
		if (ac.ind(c).fValid())
			if (fCIStrEqual(sNam , ac.ind(c)->sName()))
				return ac.ind(c);
	return colUNDEF;
}

const Column& TableStore::AddCol(const Column& col)
{
	if (!col.fValid())
		return col;
	assert(col->fnTbl == ptr.fnObj);
	int i=0;
	for (; i < iCols(); i++)
		if (ac.ind(i).ptr() == col.ptr()) // exists already
			return ac.ind(i);
	ptr.Updated();
	ac &= col;
	i = iCols();
	ptr._iCols += 1;
	return ac.ind(i);  // col kan tijdelijk zijn !!!
}

void TableStore::RemoveCol(Column& col)
{
	if (!col.fValid())
		return;

	for (int i = 0; i < iCols(); ++i)
		if (!ac[i].fValid() || ac[i] == col)
		{
			ac[i] = Column(); // next call will not call destructor
			ac.Remove(i,1);
			ptr._iCols -= 1;
			col->fErase = true;

			ptr.Updated();
			// Remove ALL the column info from the table ODF
			String sSec = ptr.sSectionPrefix + "col:" + col->sName();
			ObjectInfo::RemoveSection(ptr.fnObj, sSec);
		}
	// make sure that the list of column names and the column count
	// are correct on disk and match the memory image
	StoreColNames();
}

void TableStore::DeleteRec(long iStartRec, long iRecords)
{
  if (iRecords <= 0)
    return;
  if (iStartRec + iRecords > iRecs() + iOffset())
    iRecords = iRecs() + iOffset() - iStartRec;
  if (iRecords > iRecs())
    iRecords = iRecs();
  if (iRecords <= 0)
    return;
  ptr._iRecs -= iRecords;
  ptr.Updated();
  for (short c = 0; c < iCols(); ++c) {
    Column col = ac.ind(c);
    if (col->fDependent())
      col->DeleteCalc();
    else
      col->DeleteRec(iStartRec, iRecords);
  }
}

long TableStore::iRecNew(long iRecords)
{
  long rec = iRecNewWithoutRecsUpdate(iRecords);
  ptr._iRecs += iRecords;
  return rec;
}

bool TableStore::fWriteAvailable() const
{
  return false;
}

long TableStore::iRecNewWithoutRecsUpdate(long iRecords)
{
  long rec = iOffset() + iRecs();
  if (iRecords) {
    ptr.Updated();
    for (short c = 0; c < iCols(); ++c) {
      Column col = ac.ind(c);
	  if ( col.fValid()) {
		  col->AppendRec(iRecords);
		  if (col->fDependent())
			col->DeleteCalc();
	  }
    }
  }
  return rec;
}

void TableStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  ObjectInfo::Add(afnDat, ptr.fnDat, ptr.fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

void TableStore::GetObjectDependencies(Array<FileName>& afnObjDep)
{
  for (short i = 0; i < iCols(); i++)
    ac[i]->GetObjectDependencies(afnObjDep);
}

enum ColumnType { ctRAW, ctREAL, ctSTRING, ctBINARY, ctCRD,ctCRD3D };

// The LoadBinary function is only meant to read column data that is not yet
// in memory from disk.
// It cannot make assumptions about the number of columns that are already
// in memory (temporary columns, virtual columns being calculated)
// The function may not remove columns!
void TableStore::LoadBinary()
{
	if (fLoaded)  // nothing to do, data already in memory
		return;

	long c;
	int iNrCols = iCols();
	// the number of columns may not be the same as the binary file contains
	// so reread the number from the ODF. e.g. Histograms create a temporary column before reading
	// the data
	String sSectionString;
	sSectionString = ptr.sSectionPrefix != "" ? String("%STable", ptr.sSectionPrefix) : "Table";
	ptr.ReadElement( sSectionString.c_str(), "Columns", iNrCols);
	if (iNrCols <= 0)
		return;

	Array<ColumnInfo> acinf(iNrCols);
	Array<bool> fValid(iNrCols);
	for (c = 0; c < iNrCols; c++)
	{
		fValid[c] = ac[c].fValid();
		if (!fValid[c])
			acinf[c] = ColumnInfo(ptr.fnObj, c);
	}
	Array<bool> fRaw(iNrCols);
	for (c = 0; c < iNrCols; c++)
		if (fValid[c])
			fRaw[c] = ac[c]->dvrs().fRawAvailable();
		else
			fRaw[c] = acinf[c].dminf().fRawAvailable();
		
	Array<ColumnType> act(iNrCols);
	for (c = 0; c < iNrCols; c++) 
		if (fValid[c])
		{
			if (ac[c]->dvrs().fRawAvailable())
				act[c] = ctRAW;
			else if (ac[c]->st() == stREAL)
				act[c] = ctREAL;
			else if (ac[c]->st() == stSTRING)
				act[c] = ctSTRING;
			else if (ac[c]->st() == stCRD)
				act[c] = ctCRD;
			else if (ac[c]->st() == stCRD3D)
				act[c] = ctCRD3D;
			else if (ac[c]->st() == stBINARY)
				act[c] = ctBINARY;
			else if (ac[c]->st() == stCOORDBUF)
				act[c] = ctBINARY;
		}
		else
		{
			if (acinf[c].dminf().fRawAvailable())
				act[c] = ctRAW;
			else if (acinf[c].st() == stREAL)
				act[c] = ctREAL;
			else if (acinf[c].st() == stSTRING)
				act[c] = ctSTRING;
			else if (acinf[c].st() == stCRD)
				act[c] = ctCRD;
			else if (acinf[c].st() == stCRD3D)
				act[c] = ctCRD3D;
			else if (acinf[c].st() == stBINARY)
				act[c] = ctBINARY;
			else if (acinf[c].st() == stCOORDBUF)
				act[c] = ctBINARY;
		}
		
	Array<bool> fStored(iNrCols);
	for (c = 0; c < iNrCols; c++)
	{
		String s;
		String sColName;
		if (fValid[c])
			sColName = ac[c]->sName();
		else
			sColName = acinf[c].sName();

		ptr.ReadElement(ptr.sSection(String("Col:%S", sColName)).c_str(), "Stored", s);
		if (s.length())
			fStored[c] = s.fVal();
		else
			fStored[c] = true;
	}
	//Array<bool> fCoord(iNrCols);
	//for (c = 0; c < iNrCols; c++)
	//	if (fValid[c])
	//		fCoord[c] = ac[c]->st() == stCRD;
	//	else
	//		fCoord[c] = acinf[c].st() == stCRD;

	File fil(ptr.fnDat.sFullName(), facRO);
	{ // skip header
		char sHeader[128];
		fil.Read(128, sHeader);
	}
	// Mark the columns and the TableStore as Loaded.
	// This is necessary before getting the data from disk, because ColumnPtr->PutRaw
	// uses the fLoaded flag already to determine if the data must be loaded from disk
	long iValidCols = iNrCols;
	for (c = 0; c < iValidCols; c++)
	{ 
		if (!fValid[c])
		{
			long cc=c;
			for (; cc < iNrCols-1; ++cc)
			{
				ac[cc] = ac[cc+1];
			}
			ac[cc] = Column();
			--iValidCols;
			--c;
		}
		else
			ac[c]->Loaded(true);
	}
	fLoaded = true;

	// read the column data from disk
	String sVal;
	long iRaw;
	double rVal;
	for (long r = 0; r < iRecs(); r++)
	{
		long rec = r + iOffset();
		for (c = 0; c < iValidCols; c++)
		{
			if (!fStored[c])
				continue;
			switch (act[c])
			{
				case ctRAW: {
					fil.Read(sizeof(iRaw), (void*)&iRaw);
					if (fValid[c])
						ac[c]->PutRaw(rec, iRaw);
					break;
				}
				case ctREAL: {
					fil.Read(sizeof(rVal),(void*)&rVal);
					if (fValid[c])
						ac[c]->PutVal(rec, rVal);
					break;
				}
				case ctCRD: {
					Coord crd;
					fil.Read(sizeof(double), &crd.x);
					fil.Read(sizeof(double), &crd.y);
					if (fValid[c])
						ac[c]->PutVal(rec, crd);
					break;
				}
				case ctCRD3D: {
					Coord crd;
					fil.Read(sizeof(double), &crd.x);
					fil.Read(sizeof(double), &crd.y);
					fil.Read(sizeof(double), &crd.z);
					if (fValid[c])
						ac[c]->PutVal(rec, crd);
					break;
				}
				case ctSTRING: {
					fil.Read(sVal);
					if (fValid[c])
						ac[c]->PutVal(rec, sVal);
					break;
				}
				case ctBINARY: {
					long iSize;
					fil.Read(sizeof(iSize), &iSize);
					BinMemBlock binMem(iSize,0);
					fil.Read(iSize, const_cast<void*>(binMem.ptr()));
					if (fValid[c])
						ac[c]->PutVal(rec, binMem);
					break;
				}
			} // switch act[c]
		}
	}
}

void TableStore::StoreAsBinary(const FileName& fnDat)
{
  if (!fnDat.fValid())
    return;
  if (File::fExist(fnDat))
    if (_access(fnDat.sFullName().c_str(), 2)==-1) // read only
      return; 
//  WaitCursor wc;
  ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), "Type", ptr.fnObj, "TableBinary");
  FileName fnBackUp = fnDat;
  fnBackUp.sExt = ".BAK";
  FileName fnTmp = fnDat;
  fnTmp.sExt = ".###";
  //bool fOk;
  try {
    File fil(fnTmp, facCRT);
    fil.KeepOpen(true);
    { // write header
      char sHeader[128];
      memset(sHeader, 0, 128);
      strcpy(sHeader, "ILWIS 2.00 Table\r\n\032");
      fil.Write(128, sHeader);
    }

    Array<ColumnType> act(iCols());
    int c;
    for (c = 0; c < iCols(); c++) {
      if (ac[c]->dvrs().fRawAvailable())
        act[c] = ctRAW;
      else if (ac[c]->st() == stREAL)
        act[c] = ctREAL;
      else if (ac[c]->st() == stSTRING)
        act[c] = ctSTRING;
	  else if (ac[c]->st() == stCRD)
		  act[c] = ctCRD;
	  else if (ac[c]->st() == stCRD3D)
		  act[c] = ctCRD3D;
      else if (ac[c]->st() == stBINARY)
        act[c] = ctBINARY;
      else if (ac[c]->st() == stCOORDBUF)
        act[c] = ctBINARY;
    }

    Array<bool> fRaw(iCols());
    // write all data in all columns
    String sVal;
    long iRaw;
    double rVal;
	if (iCols() > 0)
	    for (long r = 0; r < iRecs(); r++) {
	      long rec = r + iOffset();
	      for (c = 0; c < iCols(); c++) {
	        if (ac[c]->fErase)
	          continue;
	        switch (act[c]) {
	          case ctRAW: {
	            iRaw = ac[c]->iRaw(rec);
	            fil.Write(sizeof(iRaw), (void*)&iRaw);
	            break;
	          }
	          case ctREAL: {
	            rVal = ac[c]->rValue(rec);
	            fil.Write(sizeof(rVal), (void*)&rVal);
	            break;
	          }
	          case ctCRD: {
	            Coord crd = ac[c].ptr()->cValue(rec);
	            fil.Write(sizeof(double), &crd.x);
	            fil.Write(sizeof(double), &crd.y);
	            break;
	          }
			  case ctCRD3D: {
	            Coord crd = ac[c].ptr()->cValue(rec);
	            fil.Write(sizeof(double), &crd.x);
	            fil.Write(sizeof(double), &crd.y);
				fil.Write(sizeof(double), &crd.z);
	            break;
	          }
	          case ctSTRING: {
	            sVal = ac[c]->sValue(rec,0);
	            fil.Write(sVal);
	            break;
	          }
	          case ctBINARY: {
	            BinMemBlock binMem;
	            ac[c]->GetVal(rec, binMem);
	            long iSize = binMem.iSize();
	            fil.Write(sizeof(iSize), &iSize);
	            fil.Write(iSize, binMem.ptr());
	            break;
	          }
	        } // switch act[c]
	      }
	    }
    // delete back up file if everything went ok
  }
  catch (const FileErrorObject& ferr) {
    ferr.Show();
    return;
  }
  String sBackUp = fnBackUp.sFullName();
  String sDat = fnDat.sFullName();
  String sTmp = fnTmp.sFullName();
  if (File::fExist(fnBackUp))
    _unlink(sBackUp.c_str());
  rename(sDat.c_str(), sBackUp.c_str());
//    unlink(sDat);
  rename(sTmp.c_str(), sDat.c_str());
  _unlink(sBackUp.c_str());
}

void TableStore::StoreAsTBL(const FileName& fnDat)
{
	TableStore::Store();
	ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), "Type", ptr.fnObj, "TableTBL");
	// make back up of original file
	FileName fnBackUp = fnDat;
	fnBackUp.sExt = ".BAK";
	FileName fnTmp = fnDat;
	fnTmp.sExt = ".###";
	//bool fOk;
	// create Ilwis 1.4 table
	{
		File fil(fnTmp, facCRT);
		fil.KeepOpen(true);

		String str;
		int c;
		// write header
		Array<bool> fBinary(iCols());
		for (c = 0; c < iCols(); c++)
			fBinary[c] = ac[c]->st() == stBINARY || ac[c]->st() == stCOORDBUF;
		for (c = 0; c < iCols(); c++) {
			if (ac[c]->fErase)
				continue;
			str &= ac[c]->sName();
			if (!ac[c]->dvrs().fValues())
				str &= "$ ";
			else
				switch (ac[c]->st())
			{
		        case stBIT:
		        case stDUET:
		        case stNIBBLE:
		        case stBYTE:
					str &= "# ";  break;
		        case stINT:
					str &= "% "; break;
		        case stLONG:
					str &= "! ";    break;
		        case stREAL:
					str &= "& ";    break;
		        default:
					assert (0 == 1);  // no stCRD yet
			}
		}
		// write offset
		str &= String("#%li", iOffset());
		fil.WriteLnAscii(str);

		// write all data in all columns
		String s;
		for (long r = 0; r < iRecs(); r++) {
			long rec = r + iOffset();
			String str;
			for (c = 0; c < iCols(); c++)
			{
				if (ac[c]->fErase)
					continue;
				if (fBinary[c]) // skip column
					continue;

				s = ac[c]->sValue(rec,0).sTrimSpaces();

				if (!ac[c]->dvrs().fValues())
				{
					if (s.length() == 0) // no empty strings allowed
						s = String(" ");
					if (s.length() > 20)
						s = s.sLeft(20);
					// replace spaces by underscore
					for (int i=0; i < s.length(); i++)
						if (s[i] == ' ') s[i] = '_';
				}
				str &= s;
				str &= ' ';
			}
			fil.WriteLnAscii(str);
		}
	}
	// delete back up file if everything went ok
	String sBackUp = fnBackUp.sFullName();
	String sDat = fnDat.sFullName();
	String sTmp = fnTmp.sFullName();
	if (File::fExist(fnBackUp))
		_unlink(sBackUp.c_str());

	rename(sDat.c_str(), sBackUp.c_str());
	rename(sTmp.c_str(), sDat.c_str());
}


void TableStore::Export(const FileName& fn) const
{
	if (iCols() > 100)
	{
		String sErr(TR("ILWIS 1.4 can only have less than 100 columns.\nTable %S contains %i columns.").c_str(), ptr.fnObj.sFullName(), iCols());
		throw ErrorObject(sErr);
	}
	if (iRecs() > 16000)
	{
		String sErr(TR("ILWIS 1.4 only allows 16000 records for number columns.\nTable %S contains %i records.").c_str(), ptr.fnObj.sFullName(), iRecs());
		throw ErrorObject(sErr);
	}

	File fil(fn, facCRT);
	fil.SetErase(true);
	fil.KeepOpen(true);

	// write header
	bool fNameCol = ptr.dm()->pdsrt() != 0;
	bool fStringColCreated = fNameCol;
	String str;
	if (fNameCol)
		str &= "Name$ ";
	for (long c = 0; c < iCols(); c++) {
		if (ac[c]->fErase)
			continue;

		String sName = ac[c]->sName();
		if (sName.length() > 20)
			sName = sName.sLeft(20);
		str &= sName;
		if (!ac[c]->dvrs().fValues())
		{
			str &= "$ ";
			fStringColCreated = true;
		}
		else if (ac[c]->dvrs().fRealValues())
			str &= "& ";
		else
			switch (ac[c]->st()) {
        case stBIT:
        case stDUET:
        case stNIBBLE:
        case stBYTE:
			str &= "# "; break;
        case stINT:
			str &= "% "; break;
        case stLONG:
			str &= "! "; break;
		}
	}
	if (fStringColCreated && iRecs() > 2900)
	{
		String sErr(TR("ILWIS 1.4 only allows 2900 records for string columns.\nTable %S contains %i records.").c_str(), ptr.fnObj.sFullName(), iRecs());
		throw ErrorObject(sErr);
	}

	// write offset
	str &= String("#%li", iOffset());
	fil.WriteLnAscii(str);

	// write all data in all columns
	String s;
	for (long r = 0; r < iRecs(); r++) {
		long rec = r + iOffset();
		String str;
		if (fNameCol) {
			str = ptr.dm()->sValueByRaw(rec,0).sTrimSpaces();
			if (str.length() > 20)
				str = str.sLeft(20);
			// replace spaces by underscore
			for (unsigned int i=0; i < str.length(); i++)
				if (str[i] == ' ') 
					str[i] = '_';
				str &= ' ';
		}
		for (int c = 0; c < iCols(); c++)
		{
			if (ac[c]->fErase)
				continue;
			s = ac[c]->sValue(rec).sTrimSpaces();
			if (!ac[c]->dvrs().fValues())
			{
				if (s.length() == 0) // no empty strings allowed
					s = String(" ");
				if (s.length() > 20)
					s = s.sLeft(20);
				// replace spaces by underscore
				for (unsigned int i=0; i < s.length(); i++)
					if (s[i] == ' ') 
						s[i] = '_';
			}
			str &= s;
			str &= ' ';
		}
		fil.WriteLnAscii(str);
	}
	fil.SetErase(false);  // OK, success!
}

void TableStore::DoNotUpdate()
{
	for(unsigned int i=0; i < ac.size(); ++i )	
		if ( ac[i].fValid())
			ac[i]->DoNotUpdate();
}

void TableStore::Loaded(bool fValue)
{
	fLoaded = fValue;
	for(unsigned int i=0; i < ac.size(); ++i )	
		if ( ac[i].fValid())
			ac[i]->Loaded(fValue);
}

void TableStore::DoNotStore(bool fValue)
{
	for(unsigned int i=0; i < ac.size(); ++i )	
		if ( ac[i].fValid())
			ac[i]->DoNotStore(fValue);
}






