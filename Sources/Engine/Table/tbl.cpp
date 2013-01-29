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


#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\TBLBIN.H"
#include "Engine\Table\tbltbl.h"
#include "Engine\Table\tbl2dim.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Base\DataObjects\strng.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\Table.hs"
//for import ASCII
#include "Headers\Hs\CONV.hs"
#include "Engine\DataExchange\TableDelimited.h"
#include "Engine\DataExchange\TableIlwis14.h"
#include "Engine\DataExchange\TableDBF.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\Base\DataObjects\URL.h"
//#include "Client\ilwis.h"
#include "Engine\Base\System\mutex.h"

IlwisObjectPtrList Table::listTbl;

static Column colUNDEF;

FileName Table::fnFromExpression(const String& sExpr)
{
	FileName fnTbl(sExpr, ".tbt", false);
	if (File::fExist(fnTbl)) 
		return fnTbl;
	return FileName();
}

Table::Table()
: IlwisObject(listTbl)
{
}

Table::Table(const FileName& fn)
: IlwisObject(listTbl)//, pGet(fn))
{
	String sSecPrf = Table::sSectionPrefix(fn.sExt);
	if (fCIStrEqual(sSecPrf.sLeft(3), fn.sExt.sRight(3)))
		sSecPrf = String();
	MutexFileName mut(fn, sSecPrf);
	TablePtr* p = Table::pGet(fn);
	if (p) // if already open return it
		SetPointer(p);
	else {
		SetPointer(TablePtr::create(fn, sSecPrf));
		if (ptr())
			ptr()->Load();
	}
}

Table::Table(const FileName& fn, const Domain& dm)
: IlwisObject(listTbl)
{
	SetPointer(new TablePtr(fn, FileName(fn, ".TB#"), dm, String()));
}

Table::Table(const FileName& fn, const String& sExpression)
: IlwisObject(listTbl)
{
	bool fLoadColumns;
	MutexFileName mut(fn);
	SetPointer(TablePtr::create(fn, sExpression, fLoadColumns));
	if (fLoadColumns)
		ptr()->Load();
}

Table::Table(const String& sExpression)
: IlwisObject(listTbl)
{
	bool fLoadColumns;
	FileName fn = fnFromExpression(sExpression); 
	if (fn.fValid()) {
		String sSecPrf = Table::sSectionPrefix(fn.sExt);
		if (fCIStrEqual(sSecPrf.sLeft(3), fn.sExt.sRight(3)))
			sSecPrf = String();
		MutexFileName mut(fn);
		TablePtr* p = Table::pGet(fn);
		if (p) // if already open return it
			SetPointer(p);
		else { 
			SetPointer(TablePtr::create(fn, sSecPrf));
			ptr()->Load();
		}
	}
	else {
		SetPointer(TablePtr::create(FileName(), sExpression, fLoadColumns));
		if (fLoadColumns)
			ptr()->Load();
	}
}

Table::Table(const String& sExpression, const String& sPath)
: IlwisObject(listTbl)
{
	bool fLoadColumns;
	FileName fn = fnFromExpression(sExpression); 
	if (fn.fValid()) {
		fn.Dir(sPath);
		String sSecPrf = Table::sSectionPrefix(fn.sExt);
		if (fCIStrEqual(sSecPrf.sLeft(3), fn.sExt.sRight(3)))
			sSecPrf = String();
		MutexFileName mut(fn);
		TablePtr* p = Table::pGet(fn);
		if (p) // if already open return it
			SetPointer(p);
		else { 
			SetPointer(TablePtr::create(fn, sSecPrf));
			ptr()->Load();
		}
	}
	else {
		SetPointer(TablePtr::create(FileName::fnPathOnly(sPath), sExpression, fLoadColumns));
		if (fLoadColumns)
			ptr()->Load();
	}
}

Table::Table(const Table& tbl)
: IlwisObject(listTbl, tbl.pointer())
{
}

String Table::sSectionPrefix(const String& sExt)
{
	if (fCIStrEqual(sExt, ".dom"))
		return "dom:";
	else if (fCIStrEqual(sExt,".rpr"))
		return "rpr:";
	else if (fCIStrEqual(sExt, ".grf"))
		return "grf:";
	return String();
}

void Table::Export(const FileName& fn) const
{
	if (!ptr())
		return;
	FileName fnExport = fn;
	if (fnExport.sExt.length() == 0)
		fnExport.sExt = ".tbl";
	if (fnExport == ptr()->fnObj)
		NotCreatedError(fnExport);
	ptr()->Export(fnExport);
}

void TablePtr::Export(const FileName& fn)
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return;
	pts->Export(fn);
}

TablePtr* Table::pGet(const FileName& fn)
{
	if (!fn.fValid())
		return 0;
	String sSecPrf = Table::sSectionPrefix(fn.sExt);
	if (fCIStrEqual(sSecPrf.sLeft(3), fn.sExt.sRight(3)))
		sSecPrf = String();
	ILWISSingleLock sl(&listTbl.csAccess, TRUE, SOURCE_LOCATION);

	for (DLIterCP<IlwisObjectPtr> iter(&listTbl); iter.fValid(); ++iter) {
		TablePtr* tb = static_cast<TablePtr*>(iter());
		if ((0 != tb) &&
			(fn == tb->fnObj) &&
			fCIStrEqual(tb->sSectionPrefix, sSecPrf))
			return tb;
	}
	return 0;
}

TablePtr* TablePtr::create(const FileName& fn, const String& sSecPrf)
{
	if (!File::fExist(fn))
		NotFoundError(fn);

	MutexFileName mut(fn, sSecPrf);
	TablePtr* p = Table::pGet(fn);
	if (p) // if already open return it
		return p;
	String sType;
	ObjectInfo::ReadElement(String("%STable", sSecPrf).c_str(), "Type", fn, sType);
	if ("" == sType)
		return 0;
	if (fCIStrEqual("Table2Dim", sType))
		return new Table2DimPtr(fn);
	return new TablePtr(fn, sSecPrf);
}

String TablePtr::sType() const
{
	if (0 != ptv)
		return ptv->sType();
	else if (fDependent()) {
		// read from obj def file if available
		String s;
		if (0 != ReadElement("TableVirtual", "DependentType", s))
			return s;
		return "Dependent Table";
	}
	else
		return "Table";
}

TablePtr* TablePtr::create(const FileName& fn, const String& sExpression, bool& fLoadColumns)
{
	fLoadColumns = false;
	String sFormat = IlwisObjectPtr::sParseFunc(sExpression).sTrimSpaces();
	if ( fCIStrEqual(sFormat , "table"))
		return ImportTable(fn, sExpression);

	if (fn.sFile.length() == 0)
	{
		// no file name, this means that:
		// case 1: sExpression contains table name
		// case 2: sExpression contains any other virtual table

		// check if sExpression is an existing table on disk
		FileName fnTbl(sExpression, ".tbt", false);
		if (File::fExist(fnTbl)) { // case 1
			MutexFileName mut(fnTbl, Table::sSectionPrefix(fnTbl.sExt));
			// see if table is already open (if it is in the list of open tables)
			TablePtr* p = Table::pGet(fnTbl);
			if (p) // if already open return it
				return p;
			// open table and return it
			String sSecPrf = Table::sSectionPrefix(fn.sExt);
			if (fCIStrEqual(sSecPrf.sLeft(3), fn.sExt.sRight(3)))
				sSecPrf = String();
			fLoadColumns = true;
			return new TablePtr(fnTbl, sSecPrf);
		}
	}
	// case 2:
	FileName filnam = FileName(fn.sFullNameQuoted(true), ".tbt", false);
	if (File::fExist(filnam)) {
		MutexFileName mut(filnam, Table::sSectionPrefix(filnam.sExt));
		// see if table is already open (if it is in the list of open tables)
		TablePtr* p = Table::pGet(filnam);
		if (p) // if already open then can't create it
			NotCreatedError(filnam);

		if (((fCIStrEqual(filnam.sExt,".his"))
			|| (fCIStrEqual(filnam.sExt,".hsp"))
			|| (fCIStrEqual(filnam.sExt,".hsa"))
			|| (fCIStrEqual(filnam.sExt,".hss")))
			&& ObjectInfo::fUpToDate(filnam)) {
				fLoadColumns = true;
				return new TablePtr(filnam, String(), false);
		}
	}
	// If we arrived here, 'filnam' does not exist, or it exists and is not up-to-date
	if (!filnam.fValid())
		NotFoundError(FileName(sExpression, ".tbt", false));
	// Rename filnam if it exists to same name with extension .ba~ for backup purposes
	bool fBackupMade = false;
	if (File::fExist(filnam))
	{
		File(filnam).Rename(FileName(filnam, ".ba~", true).sShortName());
		fBackupMade = true;
	}
	// Note: if we don't rename (but just leave it) the TableVirtual::create below gets
	// confused (see histograms section) and assumes the existing filnam is up-to-date
	// and already loaded into *p
	// Now attempt creating a new TablePtr
	TablePtr* p = new TablePtr(filnam, String(), true);
	bool fPrevErase = p->fErase;
	p->fErase = true;
	try
	{
		p->ptv = TableVirtual::create(filnam, *p, sExpression);
	}
	catch (ErrorObject& err)
	{
		delete p;
		throw err;
	}
	p->fErase = fPrevErase;
	if (0 != p->ptv)
	{
		p->SetDomainChangeable(p->ptv->fDomainChangeable());
		p->SetValueRangeChangeable(p->ptv->fValueRangeChangeable());
		p->SetExpressionChangeable(p->ptv->fExpressionChangeable());
	}
	// Now restore backed-up file if the process failed, or delete backup if succeeded
	if (fBackupMade)
		if (0 != p->ptv) // TablePtr creation succeeded => delete backup
			File::Remove(FileName(filnam, ".ba~", true).sShortName().c_str());
		else // TablePtr creation failed; restore the backup that we have
			File(FileName(filnam, ".ba~", true).sShortName()).Rename(filnam.sShortName());

	return p;
}



TablePtr::TablePtr(const FileName& fn, const String& sSecPrf, bool fCreate)
: IlwisObjectPtr(fn, fCreate), pts(0), ptv(0)
{
	sSectionPrefix = sSecPrf;
	if (fCIStrEqual(sSectionPrefix.sLeft(3) , fn.sExt.sRight(3)))
		sSectionPrefix = String();
	_iCols = _iRecs = 0;
	_iOffset = 1;
	if (fCreate)
		return;
	if (!File::fExist(fn))
		NotFoundError(fn);

	Domain dom;
	ReadElement(sSection("Table").c_str(), "Domain", dom);
	if (!dom.fValid())
		dom = Domain("none");
	SetDomain(dom);
	ObjectInfo::ReadElement(sSection("Table").c_str(), "Columns", fnObj, _iCols);
	if (_iCols == shUNDEF)
		_iCols = 0;
	if (_iCols < 0)
		throw ErrorObject(String(TR("Error opening table %S: Number of columns must be positiv").c_str(), fnObj.sRelativeQuoted()));
	_iRecs = iReadElement(sSection("Table").c_str(), "Records");
	if (_iRecs == iUNDEF)
		_iRecs = 0;
	if (_iRecs < 0)
		throw ErrorObject(String(TR("Error opening table %S: Number of records must be positiv").c_str(), fnObj.sRelativeQuoted()));
	if (0 != dom->pdv()) {
		if (0 != dom->pdvr())
			_iOffset = 0;
		else
			_iOffset = dom->pdv()->iRaw(dom->pdv()->riMinMax().iLo());
	}
	else if (dm()->pdp())
		_iOffset = 0;
	else if (dm()->pdnone()) {
		long iOff = iReadElement(sSection("Table").c_str(), "Offset");
		if (iOff != iUNDEF)
			_iOffset = iOff;
	}
}

void TablePtr::Load()
{
	if (0 != pts)
		return;
	String s;
	MutexFileName mut(fnObj);
	if (0 != ReadElement(sSection("TableStore").c_str(), "Type", s)) {
		if (fCIStrEqual("TableBinary" , s))
			pts = new TableBinary(fnObj, *this);
		else if (fCIStrEqual("TableTBL" , s))
			pts = new TableTBL(fnObj, *this);
		else if (fCIStrEqual("TableDelimited" , s))
			pts = new TableDelimited(fnObj, *this);
		else if ( fCIStrEqual("TableDBF" , s))
			pts = new TableDBF(fnObj, *this);
		else if ( fCIStrEqual("TableForeign" , s))
		{
			ParmList pm;
			ForeignFormat *ff = ForeignFormat::Create(fnObj, pm);
			if (ff)
			{
				String sTest = pm.sCmd();
				TableForeign *tf = new TableForeign(fnObj, *this, pm);
				pts = tf;
				if ( ff->GetTableDomain() != 0 && ff->AssociatedMapType() == ForeignFormat::mtTable)
					SetDomain(*(ff->GetTableDomain()));			
				tf->SetLoading(true); // will prevent writebacks during loading
				ff->LoadTable(this);
				tf->SetLoading(false);			
				delete ff;
			}
		}
		else
			InvalidTypeError(fnObj, "TableStore", s);
	}
	CheckNrRecs();
	{ Table t;
	t.SetPointer(this);
	bool fCalculated = false;
	for (long i=0; i< iCols(); ++i) {
		Column cl = col(i);
		if (cl.fValid())
			if (!cl->fCalculated()) {
				cl->Calc();
				fCalculated = true;
			}
	}
	iRef++; // otherwise is 'this' deleted in destructor of Table t
	fChanged = false; // 16/6/00 Wim: prevent Store() in destructor of t
	}
	iRef--; // see iRef++
}

TablePtr::TablePtr(const FileName& fn, const FileName& _fnDat, const Domain& dm, const String& sSecPrf)
: IlwisObjectPtr(fn, true, ".tbt"), pts(0), ptv(0), fnDat(_fnDat)
{
	sSectionPrefix = sSecPrf;
	if (fCIStrEqual(sSectionPrefix.sLeft(3), fn.sExt.sRight(3)))
		sSectionPrefix = String();
	SetDomain(dm);
	_iCols = 0;

	pts = new TableBinary(fn, *this, fnDat);
}

TablePtr::TablePtr(const FileName& fn, const Domain& dm,  ParmList& pm, const String& sSecPrf)
: IlwisObjectPtr(fn, true, ".tbt"), pts(0), ptv(0)
{
	sSectionPrefix = sSecPrf;
	SetDomain(dm);
	_iCols = 0;
	pts = new TableForeign(fn, *this, pm);
}

TablePtr::TablePtr(const FileName& fn, const FileName& _fnDat, long iRecs, const String& sSecPrf)
: IlwisObjectPtr(fn, true, ".tbt"), pts(0), ptv(0), fnDat(_fnDat)
{
	sSectionPrefix = sSecPrf;
	if (fCIStrEqual(sSectionPrefix.sLeft(3), fn.sExt.sRight(3)))
		sSectionPrefix = String();
	SetDomain(Domain("string"));
	_iCols = 0;
	_iRecs = iRecs;
	_iOffset = 1;
	pts = new TableBinary(fn, *this, fnDat);
}

void TablePtr::BreakDependency()
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	delete ptv;
	ptv = 0;
	fChanged = true;

	ObjectInfo::WriteElement("Table", "Type", fnObj, "TableStore");
	Store();
}


TablePtr::~TablePtr()
{
	if (0 != pts) {
		delete pts;
		pts = 0;
	}  
	if (0 != ptv) {
		delete ptv;
		ptv = 0;
	}  
}

void TablePtr::Store()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if ((0 != ptv) && (sDescription == ""))
		sDescription = ptv->sExpression();
	if (fnObj.sFile.length() == 0)  // empty file name
		return;
	if (fCIStrEqual(fnObj.sExt, ".tbt") || fCIStrEqual(fnObj.sExt, ".his") || fCIStrEqual(fnObj.sExt, ".hsa") 
		|| fCIStrEqual(fnObj.sExt, ".hss") || fCIStrEqual(fnObj.sExt, ".hsp")
		|| fCIStrEqual(fnObj.sExt, ".ta2")) {
			IlwisObjectPtr::Store();
			WriteElement("Ilwis", "Type", "Table");
	}
	else
		WriteBaseInfo(sSection("Table").c_str());  
	WriteElement(sSection("Table").c_str(), "Domain", dm());
	WriteElement(sSection("Table").c_str(), "Columns", (long)iCols());
	WriteElement(sSection("Table").c_str(), "Records", iRecs());
	if (dm()->pdnone())
		if (iOffset() != 1)
			WriteElement(sSection("Table").c_str(), "Offset", iOffset());
	if (0 != pts)
		pts->Store();
	else
		TableStore::UnStore(fnObj);
	if (0 != ptv)
		ptv->Store();
	String s;
	if (0 == ReadElement(sSection("Table").c_str(), "Type", s))
		WriteElement(sSection("Table").c_str(), "Type", "TableStore");
	fChanged = false;
}

void TablePtr::SetDomain(const Domain& dom)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	_dm = dom;
	_iRecs = 0;
	_iOffset = 1;
	DomainSort* ds = dynamic_cast<DomainSort*>(dm().ptr());
	if (ds) {
		_iRecs = ds->iSize();
	}
	else {
		DomainValueInt* dvi = dm()->pdvi();
		if (dvi) {
			RangeInt ri = dvi->riMinMax();
			_iOffset = dvi->iRaw(ri.iLo());
			_iRecs = ri.iHi() - ri.iLo() + 1;
		}
		else {
			DomainImage* di = dm()->pdi();
			if (di) {
				_iOffset = 0;
				_iRecs = 256;
			}
			else {
				DomainPicture* dp = dm()->pdp();
				if (dp) {
					_iOffset = 0;
					_iRecs = dp->iColors();
				}
				else {
					DomainBit* db = dynamic_cast<DomainBit*>(dm().ptr());
					if (db) {
						_iOffset = 0;
						_iRecs = 2;
					}
					else {
						DomainBool* db = dynamic_cast<DomainBool*>(dm().ptr());
						if (db) {
							_iOffset = 0;
							_iRecs = 2;
						}
						else {
							if (!dm()->pds() && !dm()->pdnone())
								InvalidTypeError(fnObj, "Domain", typeid(*(dm().ptr())).name());
						}
					}
				}
			}
		}
	}
}

void TablePtr::CheckNrRecs()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	DomainSort* ds = dm()->pdsrt();
	if (ds) {
		long iNrNew = ds->iSize() - iRecs();
		if (iNrNew > 0) {
			iRecNew(0); // synchronizing if needed
			iNrNew = ds->iSize() - iRecs();
			if (iNrNew > 0)
				iRecNew(iNrNew);
		}
	}
}

long TablePtr::iNettoRecs() const
{
	long _iNettoRecs = _iRecs;
	DomainSort* ds = _dm->pdsrt();
	if (ds)
		_iNettoRecs = ds->iNettoSize();
	return _iNettoRecs;
}

bool TablePtr::fDataReadOnly()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (fReadOnly())
		return true;
	if (0 == pts)
		Load();
	if (0 == pts)
		return true;
	if (!pts->fWriteAvailable()) // e.g. TableCommaDelimited
		return true;
	// it should be able to add columns to dependent tables
	return false;
}

void TablePtr::Changed(const FileName& fnTbl, bool fChanged)
{
	TablePtr* ptr = Table::pGet(fnTbl);
	if (0 != ptr)
		ptr->Updated();
}

void TablePtr::Updated()
{
	IlwisObjectPtr::Updated();
}

void Table::EnlargeTables(const DomainPtr* dm)
{
	ILWISSingleLock sl(&listTbl.csAccess, TRUE);
	for (DLIterP<IlwisObjectPtr> iter(&listTbl); iter.fValid(); ++iter) {
		TablePtr* ptr = static_cast<TablePtr*>(iter());
		if (dm == ptr->dm().ptr())
			ptr->CheckNrRecs();
	}
}


const Column& TablePtr::colNew(const String& sName, const DomainValueRangeStruct& dvs)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pts && dvs.dm().fValid())
		return pts->colNew(sName, dvs);
	return colUNDEF;
}

const Column& TablePtr::colNew(const String& sName, const Domain& dm, const ValueRange& vr)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	return colNew(sName, DomainValueRangeStruct(dm, vr));
}

long TablePtr::iCol(const String& sName) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pts)
		return pts->iCol(sName);
	return -1;
}

const Column& TablePtr::col(int c) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pts)
		return pts->col(c);
	return colUNDEF;
}

const Column& TablePtr::col(const String& sName) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != pts)
		return pts->col(sName);
	return colUNDEF;
}

const Column& TablePtr::AddCol(const Column& col)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pts)
		return pts->AddCol(col);
	return colUNDEF;
}

void TablePtr::RemoveCol(Column& col)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pts)
		pts->RemoveCol(col);
}

void TablePtr::DeleteRec(long iStartRec, long iRecords)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pts)
		pts->DeleteRec(iStartRec, iRecords);
	else if (iRecords > 0)
		_iRecs -= iRecords;
}

long TablePtr::iRecNew(long iRecords)
{
	if ( iRecords <= 0)
		return 0;
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != pts)
		return pts->iRecNew(iRecords);
	long rec = iRecs() + iOffset();
	_iRecs += iRecords;
	return rec;
}

DomainInfo TablePtr::dminf() const
{
	return DomainInfo(fnObj, sSection("Table").c_str());
}

void TablePtr::Rename(const FileName& fnNew)
{
	IlwisObjectPtr::Rename(fnNew);
}

void TablePtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
	if (0 != pts)
		pts->GetDataFiles(afnDat, asSection, asEntry);
}

void TablePtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	ObjectDependency::Read(fnObj, afnObjDep);
	if (0 != pts)
		pts->GetObjectDependencies(afnObjDep);
}

String TablePtr::sSection(const String& sSect) const
{
	String s = sSectionPrefix;
	s &= sSect;
	return s;
}

String TablePtr::sSection(const char* sSect) const
{
	String s(sSect);
	return sSection(s);
}

bool TablePtr::fDependent() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != ptv)
		return true;
	String s;
	ObjectInfo::ReadElement("Table", "Type", fnObj, s);
	return fCIStrEqual(s, "TableVirtual");
}

String TablePtr::sExpression() const
{
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csAccess), TRUE, SOURCE_LOCATION);
	if (0 != ptv)
		return ptv->sExpression();
	String s;
	s = IlwisObjectPtr::sExpression();
	if (s.length() == 0) // for downward compatibility with 2.02
		ObjectInfo::ReadElement("TableVirtual", "Expression", fnObj, s);
	return s;
}

bool TablePtr::fCalculated() const
// returns true if a calculated result exists
{
	if (!fDependent())
		return IlwisObjectPtr::fCalculated();
	ILWISSingleLock sl(const_cast<CCriticalSection*>(&csCalc), TRUE);
	return 0 != pts;
} 

bool TablePtr::fDefOnlyPossible() const
// returns true if data can be retrieved without complete calculation (on the fly)
{
	if (!fDependent())
		return IlwisObjectPtr::fDefOnlyPossible();
	bool f;
	if (0 != ObjectInfo::ReadElement("TableVirtual", "DefOnlyPossible", fnObj, f))
		return f;
	return false;
}

void TablePtr::Calc(bool fMakeUpToDate)
// calculates the result     
{
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenTableVirtual();
	if (fMakeUpToDate)
		if (!objdep.fUpdateAll())
			return;
	if (0 != ptv)
		ptv->Freeze();
}


void TablePtr::DeleteCalc()
// deletes calculated  result     
{
	if (0 == pts)
		return;
	ILWISSingleLock sl(&csCalc, TRUE, SOURCE_LOCATION);
	OpenTableVirtual();
	if (0 != ptv) {
		ptv->UnFreeze();
		_iCols = 0;
		_iRecs = 0;
	}
}

void TablePtr::OpenTableVirtual()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != ptv) // already there
		return;

	if (!fDependent())
		return;

	try
	{
		ptv = TableVirtual::create(fnObj, *this);
		objdep = ObjectDependency(fnObj);
	}
	catch (const ErrorObject& err)
	{
		err.Show();
		ptv = 0;
		objdep = ObjectDependency();
	}
}

void TablePtr::Replace(const String& sExpr)
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != ptv)
		delete ptv;
	ptv = 0;
	ptv = TableVirtual::create(fnObj, *this, sExpr);
	Updated();
	ptv->UnFreeze();
}

void TablePtr::CreateTableStore()
{
	ILWISSingleLock sl(&csAccess, TRUE, SOURCE_LOCATION);
	if (0 != ptv)
		ptv->CreateTableStore();
}

Table2DimPtr* TablePtr::pt2d() const
{
	return dynamic_cast<Table2DimPtr*>
		(const_cast<TablePtr*>(this));
}

struct FileNameAndString {
	FileName *fn;
	String * s;
};



TablePtr* TablePtr::ImportTable(const FileName& fnTable, const String& sExpression) 
{
	TableStore* ts;
	try 
	{
		FileName fnObj = fnTable;
		Array<String> as;
		IlwisObjectPtr::iParseParm(sExpression, as);
		vector<ClmInfo> columnInfo;
		String sErrMsg;
		String sTableDomain("none");
		int iTemp,iTemp1, iSkip = iUNDEF;	
		bool fUseAs = false;

		int iNrArg = as.iSize();
		if ( iNrArg < 3 )
			throw ErrorObject(String(TR("Too few parameters")));

		FileName fnIn(as[0]);
		if (!File::fExist(fnIn))
			throw ErrorObject(String(TR("Import table '%S' not found").c_str(), fnIn.sRelative()));

		FileName fnBinary(fnObj);
		fnBinary.sExt = ".tb#";

		TableExternalFormat::InputFormat eFormat = TableExternalFormat::ifGetFormat(as[1]);

		int iSpecFields = 0; // count the fields with a type specifier (Ilwis 1.4)
		switch ( eFormat )
		{
		case TableExternalFormat::ifDBF:
			TableDBF::Scan(fnIn, columnInfo);
			break;
		case TableExternalFormat::ifIlwis14:
			TableIlwis14::Scan(fnIn, iSkip, iSpecFields, columnInfo);
			break;
		case TableExternalFormat::ifSpace:
		case TableExternalFormat::ifComma:
		case TableExternalFormat::ifFixed:
			if ( iNrArg < 4 )
				TableDelimited::Scan(fnIn, iSkip, eFormat, columnInfo);
			break;
		default:
			throw ErrorObject(String("%S", TR("Unknown input type")));
		}

		if ( fCIStrEqual("UseAs", as[2]))
		{
			// UseAs for table import this way is not supported
			// The table routines can not handle TableExternalFormat as UseAs
			throw ErrorObject(TR("UseAs not available for this type of table")); 
		}

		int iArgCnt = 3;

		if (iNrArg > iArgCnt && as[iArgCnt].iVal() != iUNDEF )
		{
			iSkip = as[iArgCnt].iVal();
			++iArgCnt;
		}
		else
		{
			if ( iSkip == iUNDEF) 
				iSkip = 0;
		}
		if (iSkip < 0)
			throw ErrorObject(TR("Skip lines must be a positive number"));


		if (iNrArg > iArgCnt )
			sTableDomain = as[iArgCnt] != "" ? as[iArgCnt] : "none";

		++iArgCnt;

		int iColumn = 0;
		int iNrKeys = 0;

		while ( iArgCnt < iNrArg ) 
		{
			ClmInfo cinf;
			//retrieve column data from the expression
			if (eFormat == TableExternalFormat::ifFixed)  
			{
				long iTemp = as[iArgCnt].iVal();
				if (iTemp <= 0)
					throw ErrorObject(TR("Fixed Format: no or invalid column width found"));
				cinf.iColumnWidth = iTemp;
				iArgCnt++;
			}

			String sTemp = as[iArgCnt];
			if (sTemp.length() > 0)
			{
				cinf.sUndefValue = (sTemp.iPos('(') == shUNDEF) ? as[iArgCnt++] : sUNDEF;

				sTemp = as[iArgCnt];
				if (sTemp.length() > 0)
				{
					iTemp = sTemp.iPos('(');
					cinf.sColumnName = sTemp.sSub(0,iTemp).sTrimSpaces();
				}					
			}				

			if ( cinf.sColumnName == "")
				throw ErrorObject(String(TR("Empty column name specified for column %d. ").c_str(), iColumn + 1));

			cinf.dvs = DomainValueRangeStruct(0.0, 1.0,0.0); //add Domain element

			String sDom;
			Array<String> asParts;
			String sDomain="none";
			if (sTemp.length() > 0 )
			{
				iTemp1 = sTemp.iPos(')');
				sDom =  sTemp.sSub(iTemp+1,iTemp1-iTemp-1).sTrimSpaces();

				size_t iWhere = sDom.find("{");
				size_t iWhere2 = sDom.find("}");
				if ( iWhere < iWhere2)
				{
					sDomain = sDom.substr(0, iWhere);
					sDom = sDom.substr(iWhere, sDom.size());
					Split(sDom,asParts,"{:,}"); // for value: `{`min:max[:precision]`}`
					// for class/ID: `{`<Class | ID | Extend | Key>* `}`
				}
				else
					sDomain = sDom;
			}				

			//--------------------------
			if (sDom.length() == 0 || sDomain.length() == 0)
			{
				String sErr = String(TR("Empty domain specified for column %d. ").c_str(), iColumn + 1);
				throw ErrorObject(sErr);
			}

			FileName fnDom = FileName(sDomain, ".dom");
			fnDom = IlwisObjectPtr::fnCheckPath(fnDom);

			if ( !File::fExist(fnDom)) // this is a new domain so it must be created later on
			{
				fnDom = FileName(sDomain, ".dom");
				cinf.fCreate = true;
				cinf.fExtend = true; // new domains may always be etxended
			}
			else // the domain exists and may be used
				cinf.dtDomainType = Domain::dmt(fnDom);

			cinf.fnDomain = fnDom;

			double rVal = rUNDEF;
			if (asParts.size() > 1) 
				rVal = asParts[0].rVal();
			bool fIsSystemValue = fCIStrEqual(sDomain.sLeft(5),"value");
			if (fIsSystemValue || rVal != rUNDEF )
			{
				double rStep = 1.0;
				cinf.dtDomainType = dmtVALUE;
				// The {min:max:step} string has been split into the asParts array
				// the step is optional; if missing 1 (one) is assumed
				if (asParts.iSize() >= 2)
				{
					rStep = asParts.iSize() >= 3 ? asParts[2].rVal() : 1.0;
					double rMin = asParts[0].rVal();
					double rMax = asParts[1].rVal();
					if (rMin == rUNDEF || rMax == rUNDEF || rStep == rUNDEF)
						throw ErrorObject(String(TR("Invalid domain or value range specified: '%S'").c_str(), sDom));
					cinf.dvs = DomainValueRangeStruct(rMin, rMax, rStep);
				}
				if (!fIsSystemValue)
					cinf.fCreate = true;
			}
			else // class or id domain, check which elements are present
			{

				for(unsigned int i=0; i< asParts.iSize(); ++i)
				{
					String sEntry = asParts[i].sTrimSpaces();
					if ( fCIStrEqual(sEntry , "Class") )
					{
						cinf.dtDomainType = dmtCLASS;
						continue;
					}
					if ( fCIStrEqual(sEntry , "ID") )
					{
						cinf.dtDomainType = dmtID;
						continue;
					}
					if ( fCIStrEqual(sEntry , "Extend") )
					{
						cinf.fExtend = true;
						continue;
					}
					if ( fCIStrEqual(sEntry , "Key") )
					{ 
						if ( iNrKeys != 0)
							throw ErrorObject(TR("Only one key column per table may be used"));
						++iNrKeys;
						cinf.fKey = true;
						sTableDomain="none";
						continue;
					}
					else
						throw ErrorObject(String(TR("Unexpected word ' %S 'found").c_str(), asParts[i]));
				}
			}

			//--------------------------

			if ( eFormat < TableExternalFormat::ifDBF )
				columnInfo.push_back(cinf);
			else
			{
				// copy columnwidth and record count:
				// these are not in syntax but may come from scan so retain these values
				cinf.iColumnWidth = columnInfo[iColumn].iColumnWidth; 
				cinf.iNrRecs = columnInfo[iColumn].iNrRecs;
				columnInfo[iColumn] = cinf;
			}

			iArgCnt++;
			iColumn++;

		} 
		if ( fnObj.sExt == "" )
			fnObj.sExt = ".tbt";
		TablePtr *p = new TablePtr(fnObj, String(), true);

		Domain dmTbl = Domain (sTableDomain);
		p->SetDomain(dmTbl);

		switch (eFormat )
		{
		case TableExternalFormat::ifComma:
		case TableExternalFormat::ifSpace:
		case TableExternalFormat::ifFixed:
			ts = new TableAsciiDelimited (fnIn, fnObj, *p, fnBinary, iSkip, eFormat, columnInfo);
			break;
		case TableExternalFormat::ifIlwis14:    
			ts = new TableIlwis14(fnIn, fnObj, *p, fnBinary, iSkip, columnInfo);
			break;
		case TableExternalFormat::ifDBF:
			ts = new TableDBF(fnIn, fnObj, *p, fnBinary, columnInfo);
			break;
		}
		if (fUseAs) 
		{
			p->pts = ts;
			p->Store();
		}
		else 
		{
			p->pts = ts;
			ts->Store();
			ts->StoreAsBinary(fnBinary);
			p->pts = new TableBinary(fnBinary, *p);
			delete ts;
		}
		return p;
	}
	catch (ErrorObject& err) 
	{
		err.Show();
		return 0;
	}
	return 0;
}

void TablePtr::SetDataFile(const FileName& fn)
{
	fnDat = fn;
}

void TablePtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	os.AddFile( fnObj, "TableStore", "Data");
	if (os.fGetAssociatedFiles())
	{	
		os.AddFile( fnObj, "Table", "Domain", ".dom");		
	}
	for (int iCol = 0; iCol < iCols(); ++iCol)
	{
		const Column& clm = col(iCol);
		if (!clm.fValid())
			continue;
		clm->GetObjectStructure(os);
	}	
}

bool TablePtr::fUsesDependentObjects() const
{
	for ( int iCol = 0; iCol < iCols(); ++iCol )
	{
		const Column& clm = col(iCol);
		if (!clm.fValid())
			continue;
		if (clm->fDependent())
			return true;
	}		
	return false;
}

void TablePtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();
	if ( pts )
		pts->DoNotUpdate();

}

void TablePtr::LoadData()
{
	if ( pts )
	{
		pts->LoadBinary();
	}        
}

void TablePtr::Loaded(bool fValue)
{
	if ( pts )
		pts->Loaded(fValue);
}

void TablePtr::DoNotStore(bool fValue)
{
	IlwisObjectPtr::DoNotStore(fValue);
	if ( pts )
		pts->DoNotStore(fValue);	
}

IlwisObjectVirtual *TablePtr::pGetVirtualObject() const
{
	return ptv;
}

void TablePtr::PutStringInForeign(const String& sCol, long iRec, const String& str)
{
	if ( pts )
	{
		pts->PutStringInForeign(sCol, iRec, str);
	}		
}
void TablePtr::PutByteInForeign(const String& sCol, long iRec, char str)
{
}
void TablePtr::PutUByteInForeign(const String& sCol, long iRec, unsigned char bV)
{
}
void TablePtr::PutULongInForeign(const String& sCol, long iRec, unsigned long iV)
{
}
void TablePtr::PutLongInForeign(const String& sCol, long iRec, long iV)	
{
}
void TablePtr::PutRealInForeign(const String& sCol, long iRec, double rV)		
{
}
void TablePtr::PutBoolInForeign(const String& sCol, long iRec, bool fV)		
{
}

void TablePtr::PutCoordInForeign(const String& sCol, long iRec, Coord cV)		
{
	if ( pts )
	{
		pts->PutCoordInForeign(sCol, iRec, cV);
	}
}

void TablePtr::SetFileName(const FileName& fn)
{
	IlwisObjectPtr::SetFileName(fn);
	fnDat = FileName(fn, ".tb#");
}
