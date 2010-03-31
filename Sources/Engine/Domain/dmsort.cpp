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
/* DomainSort
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   20 Oct 98    4:36 pm
*/
#define DMSORT_C
#include "Headers\toolspch.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\DAT.hs"
#include "Headers\Hs\DOMAIN.hs"
#include <set>
#include <algorithm>

#define HASHVAL 14097L*4

void throwError(const String& sErr)
{
    throw ErrorNotUnique(sErr);
}

WarningPrefixNotRenamed::WarningPrefixNotRenamed(String sErr)
	: ErrorObject(sErr)
{
}

static int iHash(const StringIndexDmSort& si) ;

DomainSort::DomainSort(const FileName& fn)
: DomainPtr(fn),
  fLoaded(false)
{
	ReadElement("DomainSort", "Prefix", sPrefix);
	sPrefix = sPrefix.sTrimSpaces();
	{
		// Check for internal domain
		// check if table section exists
		String sSection;
		if (!fCIStrEqual(fnObj.sExt , ".dom"))
			sSection = Table::sSectionPrefix(".dom");
		sSection &= "Table";
		String s;
		if (0 != ReadElement(sSection.scVal(), "Type", s)) {
			try {
				String sSec;
				if (!fCIStrEqual(fnObj.sExt , ".dom"))
					sSec = Table::sSectionPrefix(".dom");
				MutexFileName mut(fn, sSec);
				TablePtr* tp = Table::pGet(fn);
				if (0 != tp)
					tbl.SetPointer(tp);
				else 
					tbl.SetPointer(new TablePtr(fn, Table::sSectionPrefix(".dom")));
				tbl->Load(); // load column data
			}
			catch (ErrorNotFound&)  // in case the data file is missing
			{
				throw;
			}
			catch (ErrorDummy&)  // ErrorNotFound but hidden
			{
				throw;
			}
			catch (ErrorObject& err)
			{
				tbl = Table();
				err.Show();
			}
		}
	}
	if (tbl.fValid())
	{
		tbl->fChanged = false;
		tbl->DoNotStore(true);
		colStr = tbl->col("Name");
		colOrd = tbl->col("Ord");
		colInd = tbl->col("Ind");
		iHashSize = tbl->iRecs() == 0 ? 997 : min(HASHVAL, tbl->iRecs() * 2 + 1);
		
		Column col = tbl->col("Code");
		if (col.fValid()) 
			colCode = col;
		
		col = tbl->col("Description");
		if (col.fValid())
			colDesc = col;
		
		// next statements for compat. with 2.02:
		colStr->SetOwnedByTable(true);
		
		if (0 == pdid())
			colStr->SetDescription(SDATMsgClassName);
		else
			colStr->SetDescription(SDATMsgIDName);
		colOrd->SetOwnedByTable(true);
		colOrd->SetDescription(SDATMsgOrder);
		colInd->SetOwnedByTable(true);
		colInd->SetDescription(SDATMsgInverseOrder);
		if (colCode.fValid())
		{
			colCode->SetOwnedByTable(true);
			colCode->SetDescription(SDATMsgCode);
		}
		if (colDesc.fValid())
		{
			colDesc->SetOwnedByTable(true);
			colDesc->SetDescription(SDATMsgDescription);
		}
	}
	
	String sSorting;
	ReadElement("DomainSort", "Sorting", sSorting);
	dsType = dsMANUAL;
	if (fCIStrEqual(sSorting, "Manual"))
		dsType = dsMANUAL;
	else if (fCIStrEqual(sSorting, "Alphabetical"))
		dsType = dsALPHA;
	else if (fCIStrEqual(sSorting, "Alphanumeric"))
		dsType = dsALPHANUM;
	else if (fCIStrEqual(sSorting, "Alphabetical by code"))
		dsType = dsCODEALPHA;
	else if (fCIStrEqual(sSorting, "Alphanumeric by code"))
		dsType = dsCODEALNUM;
	
	// attribute table is loaded on demand, see tblAtt()
	String s;
	ReadElement("DomainSort", "AttributeTable", s);
	_fAttTable = s.length() != 0;
}

DomainSort::~DomainSort()
{
  if (tbl.fValid()) {
    if (fErase)
      tbl->fErase = true;
  }  
  if (fErase)
    try {
      Representation rp = rpr();
      // delete rpr of domain always!
      if (rp.fValid())
         rp->fErase = true;
    }
    catch (const ErrorObject&) {
    }
}

DomainSort::DomainSort(const FileName& fn, long nr, const String& sPrefx)
: DomainPtr(fn, true), iNr(0), sPrefix(sPrefx), fLoaded(false)
{
	fChanged = true;
	dsType = dsALPHANUM;
	_fAttTable = false;
	Resize(nr);
}

void DomainSort::ChangeName(long j, String& s)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	s &= String("_%li??", j);
	colStr->PutVal(j, s);
	if (colCode.fValid())
		colCode->PutVal(j,"");
}

String DomainSort::sGetPrefix()
{
	return sPrefix;
}

void DomainSort::SetPrefix(const String& sNewPrefix, Tranquilizer* trq)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (sNewPrefix == sPrefix)
		return;	// nothing to do

	String sOldPrefix = sPrefix;

	sPrefix = sNewPrefix;

	// If a table  is already built we now need to rename the
	// prefix in all items
	int iFails = 0; // Count how many items could not be renamed
	if (tbl.fValid())
	{
		if ( trq)
		{
			trq->SetTitle(SDMTitleDomainChangePrefix);
			trq->SetText(String(SDMMsgRenamingPrefix_SS.scVal(), sOldPrefix, sPrefix));
		}      

		for (int i = 1; i <= iSize(); i++)
		{
			if ( trq && trq->fUpdate(i, iSize())) 
			{
				// if cancelled revert to old prefix; however several items
				// will have been renamed already: this is NOT reverted (because the user want to stop...)
				// The user is notified of the number of classes that have not been renamed.
				sPrefix = sOldPrefix;
				iFails += iSize() - i + 1;
				break;
			}

			if (iOrd(i) != iUNDEF)
			{
				String sItem = sNameByRaw(i, 0);
				if (sItem.iPos(sOldPrefix) == 0)  
				{	
					// this item contains the prefix at position 0
					String sNew = sNewPrefix + sItem.sSub(sOldPrefix.length(), sItem.length() - sOldPrefix.length());
					try
					{
						SetVal(i, sNew);
					}
					catch (const ErrorNotUnique&)
					{
						// allow to pass, skip to next item, but do count them
						iFails++;
					}
				}
			}
		}
	}
	if (iFails > 0)
		throw WarningPrefixNotRenamed(String(SDMErrPrefixNotRenamed_I.scVal(), iFails));
	Updated();
}

void DomainSort::Store()
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (tbl.fValid())
		tbl->Store();

	DomainPtr::Store();
	char* s = 0;
	switch (dsType) {
		case dsMANUAL:		s = "Manual";       break;
		case dsALPHA:		s = "Alphabetical"; break;
		case dsALPHANUM:	s = "AlphaNumeric"; break;
		case dsCODEALPHA:	s = "Alphabetical by code"; break;
		case dsCODEALNUM:	s = "AlphaNumeric by code"; break;
	};
	WriteElement("DomainSort", "Sorting", s);
	WriteElement("DomainSort", "Prefix", sPrefix);
	if (!_fAttTable)
		WriteElement("DomainSort", "AttributeTable", (const char*)0);
}

StoreType DomainSort::stNeeded() const
{
  StoreType stp = st(1+iRecs());
  if (stp < stBYTE)
    stp = stBYTE;
  return stp;
}

String DomainSort::sValueByRaw(long iRaw, short iW, short) const
{
  if (iRaw == iUNDEF)
    return sUNDEF;
  if (iRaw <= 0 || iRaw > iSize())
    return sUNDEF;
  if (iW < 0)
    iW = iWidth();
  if (colStr.fValid()) {
    if (iOrd(iRaw) == iUNDEF)
      return "?";
    String sCode;
    String sValue;
    sValue = colStr->sValue(iRaw, iW);
    if (fCodesAvailable()) 
      sCode = colCode->sValue(iRaw, iW);
    if (iW == 0) {
      if (sCode != "" && !fCIStrEqual(sCode , sValue)) {
        sCode &= ": ";
        sCode &= sValue;
        return sCode;
      }
      else
        return sValue;
    }
    else if (fCodesAvailable() && iW <= 6 && sCode[0] != ' ')
      return sCode;
    else
      return sValue;
  }
  else {
    String s;
      s = String("%S %li", sPrefix, iRaw);
    if (iW == 0)
      return s;
    return String("%*S", -iW, s);
  }  
}

long DomainSort::iRaw(const String& _s) const
{
	String s = _s.sTrimSpaces();
	if ("?" == s || "" == s)
		return iUNDEF;
	
	if (!colStr.fValid()) 
	{
		long iRes = iUNDEF;
		if ((sPrefix.length() > 0) && fCIStrEqual(s.sLeft(sPrefix.length()), sPrefix)) 
		{
			int iRight = s.length() - sPrefix.length() - 1;
			if (iRight > 0)
				iRes = (s.sRight(iRight)).iVal();
		}
		else if (isdigit(s[0])) // also the number only is allowed
			iRes = s.iVal();
		if (iRes > 0 && iRes <= iRecs())
			return iRes;
		else
			return iUNDEF;
	}
	if (!fLoaded)		
		const_cast<DomainSort*>(this)->Load();
	
	String sCode(s);
	String sName(s);
	// Check if string contains code and name ("code:name")
	int iP = s.iPos(':');
	if (iP != shUNDEF)
	{
		sCode = s.sLeft(iP).sTrimSpaces();
		sName = s.sSub(iP + 1, s.length()).sTrimSpaces();
		long iCode = htCode.fValid() ? iFindHashCode(sCode) : iUNDEF;
		long iName = htName.fValid() ? iFindHashName(sName) : iUNDEF;
		long iIsOrd = iOrd(iName);         // present in the colName. (also true for next ifs)
		if (iCode != iUNDEF && iName != iUNDEF && iCode == iName && iIsOrd != iUNDEF)
			return iName;
		else
			return iUNDEF;
	}
	if (fHashCode()) 
	{
		long iRec = iFindHashCode(sCode); // iRec is not sufficient as the string is still
		long iIsOrd = iOrd(iRec);         // present in the colName. (also true for next ifs)
		if (iRec != iUNDEF && iIsOrd != iUNDEF)
			return iRec;
	}  
	if (fHashName()) 
	{
		long iRec = iFindHashName(sName);
		long iIsOrd = iOrd(iRec);
		if (iRec != iUNDEF && iIsOrd != iUNDEF)
			return iRec;
	}  
	if (fHashCodeName()) 
	{
		long iRec = iFindHashCodeName(s);
		long iIsOrd = iOrd(iRec);
		if (iRec != iUNDEF && iIsOrd != iUNDEF)
			return iRec;
	}  
	return iUNDEF;  
}

// This only works properly for adding to a new domain
void DomainSort::AddValues(vector<String>& values)
{
	if (!tbl.fValid())
		CreateColumns();

	int iSz = values.size();

	if ( iNr == 0 )
	{
		tbl->iRecNew(iSz);
		iNr = tbl->iRecs(); 
	}
	vector<String>::iterator cur = values.begin();
	Representation rep = rpr();
	RepresentationClass* rc = 0;
	if (rep.fValid())
		rc = rep->prc();

	iHashSize = min(HASHVAL, iNr * 2 + 1);
	FillHash();
	for (int iRaw = 1; iRaw <= iSz; ++iRaw)
	{
		AddHashName(*cur, iRaw);
		colStr->PutVal(iRaw, *cur);
		colOrd->PutRaw(iRaw,iRaw);
		colInd->PutRaw(iRaw,iRaw);

		if (rc) 
			rc->PutColor(iRaw, rc->clrRawDefault(iRaw));

		++cur;
	}
	Updated();
}

long DomainSort::iAdd(const String& _s, bool fAllNew)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);
	String s = _s.sTrimSpaces();
	long raw = iRaw(s);
	if (raw != iUNDEF)
		return raw;

	if (!tbl.fValid())
		CreateColumns();
	if (s == sUNDEF)
		return iUNDEF;
	if (s.length() == 0)  
		return iUNDEF;

	String sName = s, sCode;
	int iPosColon = s.iPos(':');
	if (iPosColon == shUNDEF)
		sName = s;
	else
	{
		sCode = s.sLeft(iPosColon);
		sName = s.sSub(iPosColon + 1, s.length() - iPosColon);
		sName = sName.sTrimSpaces();
	}

	int iOrdName = iOrd(sName);
	int iOrdCode = iOrd(sCode);
	if (iUNDEF != iOrdName)
	{
		if (iOrdCode == iOrdName)
			return iRaw(sName);
		if (pdid() != 0)
			throwError(SDATErrIDNotUnique);
		else
			throwError(SDATErrClassNotUnique);
	}
	if (iUNDEF != iOrdCode)
	{
		if (pdid() != 0)
			throwError(SDATErrIDCodeNotUnique);
		else
			throwError(SDATErrClassCodeNotUnique);
	}

	long iMax = iRecs();  
	if (!fAllNew) // For a new domain, no checking is needed
	{
		for (long i = 1; i <= iMax; ++i)
		{
			if (iOrd(i) == iUNDEF) 
	    {
		        raw = i;
			    break;
			}
		}
	}      
	Representation rep = rpr();
	Color clr;
	if (raw == iUNDEF) {
		raw = tbl->iRecNew();
	}  
	if (rep.fValid())
		clr = rep->clrRaw(raw);
	iNr += 1;

	colStr->PutVal(raw,sName);
	if (iNr >= 1.5*iHashSize && iNr <= HASHVAL ) 
	{
		iHashSize = min(HASHVAL, iNr * 2 + 1);
		FillHash();
	}
	AddHashName(sName, raw);
	colOrd->PutRaw(raw,iNr);
	colInd->PutRaw(iNr,raw);
	if ((sCode.length() != 0) && (!fCIStrEqual(sCode , sName)))
		SetCode(raw,sCode);
	else if (fCodesAvailable())
		SetCode(raw,"");
	ReSort();
	Updated();

	Table::EnlargeTables(this);
	if (rep.fValid()) {
		RepresentationClass* rc = rep->prc();
		if (rc) 
			rc->PutColor(raw,clr);
	}

	return raw;
}

void DomainSort::SetVal(long raw, const String& _s)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	String sValue = _s.sTrimSpaces();
	if ("?" == sValue || "" == sValue)
		return;
	if (!tbl.fValid())
		CreateColumns();
	long iR = iOrd(sValue);
	if (iR != iUNDEF) {
		if (raw == iRaw(sValue)) {
			String sVal = sValueByRaw(raw, 0);
			if (0 == strcmp(sVal, sValue)) // exactly the same
				return;
			// else small change (eg case)
		}
		else
		  throwError(SDATErrNameNotUnique);
	}
	if (fHashName()) {
		String sVal;
		if (fHashCodeName()) {
			sVal = sValueByRaw(raw, 0);
			RemoveHashCodeName(sVal);
		}  
		String sOldVal = colStr->sValue(raw,0);
		RemoveHashName(sOldVal);
		colStr->PutVal(raw, sValue);
		AddHashName(sValue, raw);
		if (fHashCodeName()) {
			sVal = sValueByRaw(raw, 0);
			AddHashCodeName(sVal, raw);
		}  
	}  
	else
		colStr->PutVal(raw,sValue);
	
	ReSort();
	Updated();
}

void DomainSort::SetCode(long raw, const String& _s)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	String sCode = _s.sTrimSpaces();
	if ("?" == sCode)
		return;
	if (("" == sCode) && !fCodesAvailable())
		return;
	long iR = iOrd(sCode);
	if (iR != iUNDEF) 
	{
		if (raw == iRaw(sCode)) 
		{
			String sCod = sCodeByRaw(raw, 0);
			if (0 == strcmp(sCod, sCode)) // exactly the same
				return;
			// else small change (eg case)
		}
		else
			throwError(SDATErrNameNotUnique);
	}
	if (!colCode.fValid())
		CreateCodeColumn();
	if (fHashCode()) {
		String sVal;
		if (fHashCodeName()) {
			sVal = sValueByRaw(raw, 0);
			RemoveHashCodeName(sVal);
		}  
		String sOldVal = colCode->sValue(raw, 0);
		RemoveHashCode(sOldVal);
		colCode->PutVal(raw, sCode);
		AddHashCode(sCode, raw);
		if (fHashCodeName()) {
			sVal = sValueByRaw(raw, 0);
			AddHashCodeName(sVal, raw);
		}  
	}  
	else
		colCode->PutVal(raw,sCode);
  
	ReSort();
	Updated();
}

void DomainSort::SetDescription(long raw, const String& _s)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  if (!fLoaded)	
	  Load();
  String sDesc = _s.sTrimSpaces();
  if (!colDesc.fValid())
    CreateDescriptionColumn();
  colDesc->PutVal(raw,sDesc);
  Updated();
}

long DomainSort::iKey(long ord) const
{
  if (!tbl.fValid())
    if (ord <= 0 || ord > iSize())
      return iUNDEF;
    else  
      return ord;
  if (!fLoaded)	
	  const_cast<DomainSort *>(this);
  return colInd->iRaw(ord);
}

long DomainSort::iOrd(long raw) const
{
  if (!tbl.fValid())
    return raw;
  if (!fLoaded)	
	  const_cast<DomainSort *>(this)->Load();  
  return colOrd->iRaw(raw);
}

long DomainSort::iOrd(const String& s) const
{
	if (s == "") return iUNDEF;
	if (!fLoaded)	
		const_cast<DomainSort*>(this)->Load();
	
	
	// Check if both code and name are specified ("code:name")
	int iP = s.iPos(':');
	if (iP != shUNDEF)
	{
		String sCode = s.sLeft(iP).sTrimSpaces();
		String sName = s.sSub(iP+1, s.length()).sTrimSpaces();
		long iCode = htCode.fValid() ? iFindHashCode(sCode) : iUNDEF;
		long iName = htName.fValid() ? iFindHashName(sName) : iUNDEF;
		if (iCode != iUNDEF && iName != iUNDEF && iCode == iName)
			return iOrd(iName);
		else
			return iUNDEF;
	}
	else
	{
		String sLoc = s.sTrimSpaces();
		long iCode = htCode.fValid() ? iFindHashCode(sLoc) : iUNDEF;
		if (iCode != iUNDEF) 
			return iOrd(iCode);

		long iName=htName.fValid() ? iFindHashName(sLoc) : iUNDEF;
		if (iName != iUNDEF) 
			return iOrd(iName);
	}
	return iUNDEF;
}    
        
void DomainSort::SetOrd(long raw, long ord)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  if (!tbl.fValid())
    CreateColumns();
  if (ord < 1)
    ord = 1;
  else if (ord > iNr)
    ord = iNr;
  long old = iOrd(raw);
  if (old == ord)
    return;
  if (old == iUNDEF) {
    old = iNr += 1;
  }  
  if (old < ord) {
    for (long i = old; i < ord; ++i) {
      long r = iKey(i+1);
      colOrd->PutRaw(r,i);
      colInd->PutRaw(i,r);
    }
  }
  else {
    for (long i = old; i > ord; --i) {
      long r = iKey(i-1);
      colOrd->PutRaw(r,i);
      colInd->PutRaw(i,r);
    }
  }
  colOrd->PutRaw(raw,ord);
  colInd->PutRaw(ord,raw);
  Updated();
}

void DomainSort::SetOrdWithoutIndex(long raw, long ord)
{
// This part is commented out because it makes the function very inefficient. 
//  The function is only used in TableCross and there this check isn't needed.
/*	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  if (!tbl.fValid())
    CreateColumns();*/
  if (ord < 1)
    ord = 1;
  else if (ord > iNr)
    ord = iNr;
  colOrd->PutVal(raw, ord);
  Updated();
}

void DomainSort::Delete(long raw)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (!tbl.fValid())
		CreateColumns();
	long old = iOrd(raw);
	colInd->PutRaw(old, iUNDEF);
	colOrd->PutRaw(raw, iUNDEF);
	String sVal;
	if (fHashName()) 
	{
		sVal = colStr->sValue(raw,0);
		RemoveHashName(sVal);
	}
  if (fHashCode()) 
	{
    sVal = colCode->sValue(raw, 0);
    RemoveHashCode(sVal);
	}
	if (fHashCodeName()) 
	{
		sVal = sValueByRaw(raw, 0);
		RemoveHashCodeName(sVal);
	}  
  iNr -= 1;
  for (long i = old; i <= iNr; ++i) {
    long r = iKey(i+1);
    colOrd->PutRaw(r,i);
    colInd->PutRaw(i,r);
  }
  Updated();
}

bool DomainSort::fValid(const String& sValue) const
{
  if (sValue == sUNDEF)
    return true;
  return iOrd(sValue) != iUNDEF;
}

long DomainSort::iFindCodeRaw(const String& sCode)
{
	return htCode.fValid() ? iFindHashCode(sCode) : iUNDEF;
}

long DomainSort::iFindNameRaw(const String& sName)
{
	return htName.fValid() ? iFindHashName(sName) : iUNDEF;
}

int DomainSort::iCols()
{
  if (!tbl.fValid())
    return 0;
  return tbl->iCols() - 2;
}

Column DomainSort::col(int i) const
{
  if (!tbl.fValid())
    return Column();
  Column col = tbl->col(i);
  String sColName = col->sName();
  if (//"Name" == sColName)
      (fCIStrEqual(sColName, "Ord"))
   || (fCIStrEqual(sColName, "Ind")))
  {
    col = Column();
  }
  return col;
}
/*
void DomainSort::AddCol(const Column& col)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (!tbl.fValid())
		CreateColumns();

	tbl->AddCol(col);
	Updated();
}*/

void DomainSort::CalcIndex(bool fUpdateRequired)
{
	if (!tbl.fValid())
		return;

	// fUpdateRequired is only false when called from the Load() function
	// There only the Index Column is rebuild in memory.
	if (fReadOnly() && fUpdateRequired)
		throw ErrorNotCreatedOpen(fnObj);

	long i;
	long iMax = iRecs();
	for (i = 1; i <= iMax; ++i)
		colInd->PutRaw(i, iUNDEF);
	for (i = 1; i <= iMax; ++i) {
		long raw = iOrd(i);
		if (raw > iUNDEF)
			colInd->PutRaw(raw,i);
	}

	// prevent storage of the recalculated Ind column for the 
	// DomainClass and DomainID constructors.
	colInd->fChanged = fUpdateRequired;
}

void ExtractStringNum(const String& sIn, String& sPre, double& rNum, String& sPost)
{
	int i = 0;
	while (i < sIn.length() && !isdigit(sIn[i]))
		i++;
	if (i == sIn.length())
	{
		sPre = sIn;
		rNum = rUNDEF;
		sPost = String();
	}
	else
	{
		sPre = sIn.sLeft(i);
		int iPos = i;
		while (i < sIn.length() && isdigit(sIn[i]))
			i++;
		rNum = sIn.sSub(iPos, i - iPos).rVal();
		sPost = sIn.sSub(i, sIn.length() - i);
	}
}

class CompareAlphabetical
{
public:
  CompareAlphabetical(DomainSort* ds) 
  {
    _ds = ds;
  }
  bool operator()(const long& a, const long& b)
  {
	  String s1 = _ds->sNameByRaw(a, 0).sVal();
	  if ((s1.length() != 0) && (s1[0] == '?'))
		  return false;
	  String s2 = _ds->sNameByRaw(b, 0).sVal();
	  if ((s2.length() != 0) && (s2[0] == '?'))
		  return true;
	  return _stricmp(s1.scVal(), s2.scVal()) < 0;
  }
  DomainSort* _ds;
};

class CompareCodeAlphabetical
{
public:
  CompareCodeAlphabetical(DomainSort* ds)
  {
    _ds = ds;
  }
  bool operator()(const long& a, const long& b)
  {
	  String s1 = _ds->sCodeByRaw(a, 0).sVal();
	  if ((s1.length() != 0) && (s1[0] == '?'))
		  return false;
	  String s2 = _ds->sCodeByRaw(b, 0).sVal();
	  if ((s2.length() != 0) && (s2[0] == '?'))
		  return true;

	  // use the Class/ID when the items do not have a code; put a space in front
	  // to distinguish the values from codes.
	  if ((s1.length() == 0) && (s2.length() == 0))
	  {
		  String s = _ds->sNameByRaw(a, 0).sVal();
		  if ((s.length() != 0) && (s[0] == '?'))
			  return false;
		  else
			  s1 = String(" %S", s);
		  s = _ds->sNameByRaw(b, 0).sVal();
		  if ((s.length() != 0) && (s[0] == '?'))
			  return true;
		  else
			  s2 = String(" %S", s);
	  }
	  return _stricmp(s1.scVal(), s2.scVal()) < 0;
  }
  DomainSort* _ds;
};

class CompareAlphaNumeric
{
public:
  CompareAlphaNumeric(DomainSort* ds)
  {
    _ds = ds;
  }
  bool operator()(const long& a, const long& b)
  {
	  String sA, sB;
	  sA = _ds->sNameByRaw(a, 0);
	  sB = _ds->sNameByRaw(b, 0);
	  if ((sA.length() != 0) && (sA[0] == '?'))
		  return false;
	  if ((sB.length() != 0) && (sB[0] == '?'))
		  return true;

	  String sPrefix1, sPrefix2;
		String sPostfix1, sPostfix2;
	  double rNum1, rNum2;
	  ExtractStringNum(sA, sPrefix1, rNum1, sPostfix1);
	  ExtractStringNum(sB, sPrefix2, rNum2, sPostfix2);

	  if (rNum1 != rUNDEF && rNum2 != rUNDEF)
		  if (fCIStrEqual(sPrefix1 , sPrefix2))
				if (abs(rNum1 - rNum2) < 1.0e-6)
					return _stricmp(sPostfix1.scVal(), sPostfix2.scVal()) < 0;
				else
					return rNum1 < rNum2;

	  return _stricmp(sA.scVal(), sB.scVal()) < 0;
  }
  DomainSort* _ds;
};

class CompareCodeAlphaNumeric
{
public:
  CompareCodeAlphaNumeric(DomainSort* ds)
  {
    _ds = ds;
  }
  bool operator()(const long& a, const long& b)
  {
		String s1 = _ds->sCodeByRaw(a, 0);
		if ((s1.length() != 0) && (s1[0] == '?'))
			return false;
		String s2 = _ds->sCodeByRaw(b, 0);
		if ((s2.length() != 0) && (s2[0] == '?'))
			return true;

		// use the Class/ID when the items do not have a code; put a space in front
		// to distinguish the values from codes.
		if ((s1.length() == 0) && (s2.length() == 0))
		{
			String s = _ds->sNameByRaw(a, 0);
			if ((s.length() != 0) && (s[0] == '?'))
				return false;
			else
				s1 = String(" %S", s);
			s = _ds->sNameByRaw(b, 0);
			if ((s.length() != 0) && (s[0] == '?'))
				return true;
			else
				s2 = String(" %S", s);
		}

	  String sPrefix1, sPrefix2;
		String sPostfix1, sPostfix2;
	  double rNum1, rNum2;
	  ExtractStringNum(s1, sPrefix1, rNum1, sPostfix1);
	  ExtractStringNum(s2, sPrefix2, rNum2, sPostfix2);

	  if (rNum1 != rUNDEF && rNum2 != rUNDEF)
		  if (fCIStrEqual(sPrefix1 , sPrefix2))
				if (abs(rNum1 - rNum2) < 1.0e-6)
					return _stricmp(sPostfix1.scVal(), sPostfix2.scVal()) < 0;
				else
					return rNum1 < rNum2;

		return _stricmp(s1.scVal(), s2.scVal()) < 0;
  }
  DomainSort* _ds;
};

class CompareNumerical
{
public:
  CompareNumerical(DomainSort* ds)
  {
    _ds = ds;
  }
  bool operator()(const long& a, const long& b)
  {
	  String sA, sB;
	  double rA, rB;
	  sA = _ds->sValueByRaw(a);
	  sB = _ds->sValueByRaw(b);
	  rA = sA.rVal();
	  if (rA == rUNDEF)
		  return true;
	  rB = sB.rVal();
	  if (rA == rUNDEF)
		  return false;
	  if (rA < rB) return true;
	  if (rA > rB) return false;

	  long iA, iB;
	  iA = sA.iVal();
	  iB = sB.iVal();
	  if (iA < iB) return true;
	  if (iA > iB) return false;

	  char *cA, *cB;
	  iA = strtol(sA.scVal(), &cA, 36);
	  iB = strtol(sB.scVal(), &cB, 36);
	  if (iA < iB) return true;
	  if (iA > iB) return false;
	  return _stricmp(cA, cB) < 0;
  }
  DomainSort* _ds;
};

void DomainSort::ReSort()
{
	if (dsType == dsALPHA)
		SortAlphabetical();
	else if (dsType == dsALPHANUM)
		SortAlphaNumeric();
	else if (dsType == dsCODEALPHA)
		SortCodeAlphabetical();
	else if (dsType == dsCODEALNUM)
		SortCodeAlphaNumeric();
}

void DomainSort::SortAlphabetical()
{
	if (!tbl.fValid())
		return;  
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  vector<long> index(iNr);
  int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
  CompareAlphabetical cds(this);
  stable_sort(index.begin(), index.end(), cds);
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
  index.resize(0); 
  Updated();
	CalcIndex();
}

void DomainSort::SortAlphaNumeric()
{
	if (!tbl.fValid())
		return;  
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  vector<long> index(iNr);
  int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
  CompareAlphaNumeric cds(this);
  stable_sort(index.begin(), index.end(), cds);
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
  index.resize(0); 
  Updated();
	CalcIndex();
}

void DomainSort::SortCodeAlphabetical()
{
	if (!tbl.fValid())
		return;  
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  vector<long> index(iNr);
  int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
  if (colCode.fValid()) {
    CompareCodeAlphabetical  cds(this);
    stable_sort(index.begin(), index.end(), cds);
  }
	else {
    CompareAlphabetical  cds(this);
    stable_sort(index.begin(), index.end(), cds);
  }
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
  index.resize(0); 
  Updated();
	CalcIndex();
}

void DomainSort::SortCodeAlphaNumeric()
{
	if (!tbl.fValid())
		return;  
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  vector<long> index(iNr);
  int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
	if (colCode.fValid()){
    CompareCodeAlphaNumeric cds(this);
    stable_sort(index.begin(), index.end(), cds);
  }	
  else {
    CompareAlphaNumeric cds(this);
    stable_sort(index.begin(), index.end(), cds);
  }
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
  index.resize(0); 
  Updated();
	CalcIndex();
}

void DomainSort::SortNumerical()
{
	if (!tbl.fValid())
		return;  
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

  vector<long> index(iNr);
  int i;
	for (i = 0; i < iNr; ++i)
		index[i] = iKey(1+i);
  CompareNumerical cds(this);
  stable_sort(index.begin(), index.end(), cds);
	for (i = 0; i < iNr; ++i)
		colOrd->PutRaw(index[i], 1+i);
  index.resize(0); 
  Updated();
	CalcIndex();
}

void DomainSort::SortRawValues()
{
	if (!tbl.fValid())
		return;

	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	long ord = 1;
	for (long i = 1; i <= iNr; ++i)
	if (iKey(i) != iUNDEF)
		colOrd->PutRaw(ord++, i);
	Updated();
	CalcIndex();
}

bool DomainSort::fEqual(const IlwisObjectPtr& ptr) const
{
	// DomainUniqueID by definition is not equal to any another domain (object)
	const DomainUniqueID* pdu = dynamic_cast<const DomainUniqueID*>(&ptr);
	if (pdu != 0)
		 return false;

	const DomainSort* pds = dynamic_cast<const DomainSort*>(&ptr);
	if (0 == pds)
		return false;

	if (iNr != pds->iNr)
		return false;

	// check class names
	long iMax = iRecs();
	for (long i=1; i<=iMax; i++) 
		if (sValueByRaw(i,0) != pds->sValueByRaw(i,0))
		return false;
	return true;
}

bool DomainSort::fContains(const DomainSort* pds) const
{
  if (0 == pds)
    return false;
  // check if class names exist in this domain
  for (long i=1; i<=pds->iNr; i++) 
    if (!fValid(pds->sValueByRaw(i,0)))
      return false;
  return true;
}

void DomainSort::CreateColumns()
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	FileName fn = fnObj;
	Domain dmString("string");
	TablePtr* pt = new TablePtr(fn, FileName(fn, String(".dm%c", ObjectInfo::cLastDataFileExtChar(fnObj, ".dm#"))), iNr, Table::sSectionPrefix(".dom"));
  pt->fUpdateCatalog = false; // was set by constructor
	if (!fCIStrEqual(fnObj.sExt , ".dom"))
		pt->sSectionPrefix = Table::sSectionPrefix(".dom");
	tbl.SetPointer(pt);
//  Domain dmType = Domain("int");
	Column colStrDum = pt->colNew("Name", dmString);
	colStrDum->SetOwnedByTable(true);
	if (0 == pdid())
		colStrDum->SetDescription(SDATMsgClassName);
	else
		colStrDum->SetDescription(SDATMsgIDName);
// sValueByRaw checks on existence of colStr !, so set it later
	for (long i = 1; i <= iNr; ++i)
		colStrDum->PutVal(i,sValueByRaw(i));
	colStr = colStrDum;
	colOrd = pt->colNew("Ord", DomainValueRangeStruct(0,LONG_MAX-1));
	colOrd->SetOwnedByTable(true);
	colOrd->SetDescription(SDATMsgOrder);
	for ( long i = 1; i <= iNr; ++i)
		colOrd->PutVal(i,i);  
	colInd = pt->colNew("Ind", DomainValueRangeStruct(0,LONG_MAX-1));
	colInd->SetOwnedByTable(true);
	colInd->SetDescription(SDATMsgInverseOrder);
	for ( long i = 1; i <= iNr; ++i)
		colInd->PutVal(i,i);    
	Updated();
	iHashSize = (iNr <= 0) ? 997 : min(HASHVAL, iNr * 2 + 1);

	CalcIndex();

	FillHash();
	Store();
}

void DomainSort::CreateCodeColumn()
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (!tbl.fValid())
		CreateColumns();
	colCode = tbl->colNew("Code", Domain("string"));
	colCode->SetOwnedByTable(true);
	colCode->SetDescription(SDATMsgCode);
	long iMax = iRecs();
	for (long i = 1; i <= iMax; ++i)
		colCode->PutVal(i,"");
	Updated();
	FillHash();
	Store();
}

void DomainSort::CreateDescriptionColumn()
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (!tbl.fValid())
		CreateColumns();
	colDesc = tbl->colNew("Description", Domain("string"));
	colDesc->SetOwnedByTable(true);
	colDesc->SetDescription(SDATMsgDescription);
	long iMax = iRecs();
	for (long i = 1; i <= iMax; ++i)
		colDesc->PutVal(i,"");
	Updated();
	Store();
}

String DomainSort::sCodeByRaw(long iRaw, short iWidth) const
{
  if (colCode.fValid())
    if (iOrd(iRaw) != iUNDEF)
      return colCode->sValue(iRaw, iWidth);
    else
      return "?";
  else
    return "";
}

String DomainSort::sNameByRaw(long iRaw, short iWidth) const
{
  if (colStr.fValid())
    if (iOrd(iRaw) != iUNDEF)
      return colStr->sValue(iRaw, iWidth);
    else
      return "?";
  else
    return sValueByRaw(iRaw,iWidth);
}

String DomainSort::sDescriptionByRaw(long iRaw) const
{
  if (colDesc.fValid())
    if (iOrd(iRaw) != iUNDEF)
      return colDesc->sValue(iRaw, 0);
    else
      return SDATMsgDeletedDomainItem;
  else
    return "";
}
 
bool DomainSort::fEqualVal(long iRaw, const String& sVal) const // checks code and name  
{
	String s = sVal.sTrimSpaces();
	String sName, sCode;

	int iPosColon = s.iPos(':');
	if (iPosColon == shUNDEF)
		sName = s;
	else
	{
		sCode = s.sLeft(iPosColon);
		sName = s.sSub(iPosColon + 1, s.length() - iPosColon);
	}
  
	if (fCodesAvailable())
		if (fCIStrEqual(colCode->sValue(iRaw,0), sCode))
			return true;

	if (tbl.fValid())
		return fCIStrEqual(colStr->sValue(iRaw,0), sName);

	s = String("%S %li", sPrefix, iRaw);
	return fCIStrEqual(s, sName);
}

Table DomainSort::tblAtt()
{
	return tblAttLoad();
}

Table DomainSort::tblAttLoad()
{
  FileName fnTbl;
  Table tbl;
  try {
    ReadElement("DomainSort", "AttributeTable", tbl);
    if (tbl.fValid()) 
      if (!fEqual(*tbl->dm().ptr())) {
        MessageBox(0, SDATErrWrongAttrDomain.scVal(), SDATErrLoadAttrTable.scVal(), MB_OK | MB_ICONSTOP | MB_TOPMOST);
        tbl = Table();
      }
  }
  catch (const ErrorObject& err) {
    err.Show(SDATErrLoadDomAttrTable);
    tbl = Table();
  }
  return tbl;

}

void DomainSort::SetAttributeTable(const Table& tbl)
{ 
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

    if (tbl.fValid()) 
    {
        if (!fEqual(*tbl->dm().ptr())) 
        {
            MessageBox(0, SDATErrWrongAttrDomain.scVal(), SDATErrLoadAttrTable.scVal(), MB_OK | MB_ICONSTOP| MB_TOPMOST);
            return;
        }
    }
    _fAttTable = tbl.fValid();
    if (fTblAtt()) 
    {
        WriteElement("DomainSort", "AttributeTable", tbl);
    }  
    else 
        WriteElement("DomainSort", "AttributeTable", (const char*)0);
}

void DomainSort::SetNoAttributeTable()
{ 
	SetAttributeTable(Table());
}

void DomainSort::Resize(long iNewNr)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	if (tbl.fValid()) 
	{
		if (iNewNr < iNr)
			tbl->DeleteRec(iNewNr+1, iNr-iNewNr);
		else if ((iNewNr > iNr))
			tbl->iRecNew(iNewNr-iNr);
	}
	iNr = iNewNr;
	Updated();
}

void DomainSort::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (tbl.fValid()) {
    FileName fnDat;
    ReadElement(String("%STableStore", tbl->sSectionPrefix).scVal(), "Data", fnDat);
    if (!fnDat.fValid())
      return;
    ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
    if (asSection != 0) {
      (*asSection) &= String("%STableStore", tbl->sSectionPrefix);
      (*asEntry) &= "Data";
    }
  }
  try {
    Representation rp = const_cast<DomainSort *>(this)->rpr();
    if (rp.fValid())
      if (fnObj == rp->fnObj)
        rp->GetDataFiles(afnDat, asSection, asEntry);
  }
  catch (const ErrorObject&) {
  }
}

static int iHash(const StringIndexDmSort& si) 
{
  unsigned long h = 0;
  int i=0;
  char *ps = const_cast<char *>(si.s.scVal());
  while (*ps)
    h = (h + (++i) * tolower(*ps++)) % HASHVAL;
  return (int)h;
} 

bool DomainSort::FillHash()
{
  long i;
  bool fNamesChanged=false;
  htName.Resize(iHashSize);
  for (i=1; i <= iSize(); ++i) {
    if (iOrd(i) != iUNDEF) {
      String s = sNameByRaw(i,0);
			if ( s == "?")
				continue;
      if (dse(s,"") == dseNONEEXIST)
        htName.add(StringIndexDmSort(s, i));
      else {
        ChangeName(i,s);
        htName.add(StringIndexDmSort(s, i));
        fNamesChanged=true;
      }
    }
  }
  if (colCode.fValid())
  {
    htCode.Resize(iHashSize);
    for (i=1; i <= iSize(); ++i)
    if (iOrd(i) != iUNDEF) {
      String s = sCodeByRaw(i,0);
      if (dse("",s) == dseNONEEXIST)
        htCode.add(StringIndexDmSort(s, i));
      else {
        s = sCodeByRaw(i,0);
        colCode->PutVal(i,"");
        htCode.add(StringIndexDmSort(s, i));
        fNamesChanged = true;
      }
    }
    htCodeName.Resize(iHashSize);
    for (i=1; i <= iSize(); ++i)
      if (iOrd(i) != iUNDEF)
        htCodeName.add(StringIndexDmSort(sValueByRaw(i,0), i));
  }

  if (fNamesChanged)
    Updated();
  return fNamesChanged;
}

long DomainSort::iFindHash(const HashTable<StringIndexDmSort>& hts, const String& s) const
{
  StringIndexDmSort& siHash = hts.get(StringIndexDmSort(s));
  return siHash.iInd;
}

void DomainSort::AddHash(HashTable<StringIndexDmSort>& hts, const String& s, long iInd)
{
  hts.add(StringIndexDmSort(s, iInd));
}

void DomainSort::RemoveHash(HashTable<StringIndexDmSort>& hts, const String& s)
{
  hts.remove(StringIndexDmSort(s));
}

long DomainSort::iMergeAdd(const String& sName, const String& sCode)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	long iRaw = htName.fValid() ? iFindHashName(sName) : iUNDEF;  // name of item to merge exists as code
	long iRawNameCode = htCode.fValid() ? iFindHashCode(sName) : iUNDEF;  // name of item to merge exists as code
	if (iRaw != iUNDEF && iRawNameCode == iUNDEF)
		return iUNDEF;

	// only add the new item:
	//   if Name and Code both are unique
	//   if Name already exists as code:
	//     - remove the existing code, add the new Name
	//     - add the new code only if unique
	DomainSortingType dst = dsType;
	long iStartInd = iNettoSize() + 1;
	dsType = dsMANUAL;
	if (!tbl.fValid())
		CreateColumns();
	long iCurrNr = tbl->iRecNew();
	Updated();
	Table::EnlargeTables(this);

	if (iRaw == iUNDEF)
	{
		++iNr;  // increase the number of domain items
		colOrd->PutRaw(iCurrNr, iStartInd);
		colInd->PutRaw(iStartInd, iCurrNr);

		long iRawCode = iOrd(sCode);   // 2nd code exists as name or code in current domain

		if (iOrd(iRawNameCode) != iUNDEF)
			SetCode(iRawNameCode, "");  // remove the code that will be merged as a name from second domain

		SetVal(iCurrNr, sName);
		
		if (iRawCode == iUNDEF) // Add code only if the code is not in the current domain
			SetCode(iCurrNr, sCode);
		else
			SetCode(iCurrNr, "");
	}

	dsType = dst;
	ReSort();

	Updated();

	return iCurrNr;
}

void DomainSort::Merge(const DomainSort* pdsrt, Tranquilizer* trq)
{
	if (fReadOnly())
		throw ErrorNotCreatedOpen(fnObj);

	Representation rpr1 = const_cast<DomainSort *>(this)->rpr();
	Representation rpr2 = const_cast<DomainSort *>(pdsrt)->rpr();
	RepresentationClass* rprc1 = 0;
	if (rpr1.fValid())
		rprc1 = rpr1->prc();
	RepresentationClass* rprc2 = 0;
	if (rpr2.fValid())
		rprc2 = rpr2->prc();
	long iNew = 0;
	if ( trq)
	{
		trq->SetTitle(SDATTextMergingDomains);
		trq->SetText(SDATTextSizeNewDomain);
	}
	bool fUsesUniqueID = pdsrt->pdUniqueID() != 0;
	if (!fUsesUniqueID)
	{
		for (long i = 1; i <= pdsrt->iSize(); ++i) 
		{
			if ( trq && trq->fUpdate(i, pdsrt->iSize())) 
				return;

			if (pdsrt->iOrd(i) != iUNDEF) 
			{
				String sClass = pdsrt->sNameByRaw(i, 0);
				long iRawValue = DomainSort::iRaw(sClass);
				long iRawNameCode = htCode.fValid() ? iFindHashCode(sClass) : iUNDEF;  // 2nd name exists as code
				if (iRawValue == iUNDEF || iRawNameCode != iUNDEF)
					iNew++;
			}
		}
	}		
	else
		iNew = pdsrt->iSize();
	
	if (iNew == 0)
		return;
	long iCurrNr = iSize();

	DomainSortingType dst = dsType;
	dsType = dsMANUAL;
	if (!tbl.fValid())
		CreateColumns();
	tbl->iRecNew(iNew);
	Updated();
	Table::EnlargeTables(this);
	long iStartInd=iNettoSize();
	if (trq) trq->SetText(SDATTextAddToDomain);
	for (int i = 1; i <= pdsrt->iSize(); ++i) 
	{
		if ( trq && trq->fUpdate(i, pdsrt->iSize())) 
			return ;

		if (pdsrt->iOrd(i) != iUNDEF) 
		{
			String sClass = pdsrt->sNameByRaw(i, 0);
			String sCode = pdsrt->sCodeByRaw(i, 0);
      long iRawValue = htName.fValid() ? iOrd(iFindHashName(sClass)) : iUNDEF;
			if ( iRawValue != iUNDEF && fUsesUniqueID)
			{
				String sNewClass = sClass;
				sNewClass &= "_R";
				int r=1;
				while ( iRaw(sNewClass) != iUNDEF )
					sNewClass = String("%S_R%d", sClass, ++r);
				iRawValue = iUNDEF;
				sClass = sNewClass;
			}				

			if (iRawValue == iUNDEF)
			{
				++iNr;
				++iCurrNr;
				colOrd->PutRaw(iCurrNr, ++iStartInd);
				colInd->PutRaw(iStartInd, iCurrNr);

				long iRawCode = iOrd(sCode);   // 2nd code exists as name or code in current domain
				long iRawNameCode = htCode.fValid() ? iFindHashCode(sClass) : iUNDEF;  // 2nd name exists as code

				if (iOrd(iRawNameCode) != iUNDEF)
					SetCode(iRawNameCode, "");  // remove the code that will be merged as a name from second domain

				SetVal(iCurrNr, sClass);
				
				if (pdsrt->fCodesAvailable() && iRawCode == iUNDEF) // Add code only if the code is not in the current domain
					SetCode(iCurrNr, sCode);
				else if (fCodesAvailable())
					SetCode(iCurrNr, "");

				if (pdsrt->fDescriptionsAvailable())
					SetDescription(iCurrNr, pdsrt->sDescriptionByRaw(i));
				else if (fDescriptionsAvailable())
					SetDescription(iCurrNr, "");

				if (0 != rprc1 )
				{
					if ( rprc2 )
						rprc1->PutColor(iCurrNr,rprc2->clrRaw(i));
					else
						rprc1->PutColor(iCurrNr, rprc1->clrRawDefault(iCurrNr));
				}
			}

				
		}
	}
	dsType = dst;
	ReSort();

	Updated();
}

DomainSortExistType DomainSort::dse(const String& sValue, const String& sCode)
{
  DomainSortExistType dseType = dseNONEEXIST;
  long iRaw = iUNDEF;
  if (!colStr.fValid()) 
  {
	  // sPrefix is never empty
      if (fCIStrEqual(sValue.sLeft(sPrefix.length()), sPrefix))
        iRaw = sValue.sRight(sValue.length()-2).iVal();
  }
  if (iRaw != iUNDEF)
    return dseVALUEEXIST;
  if (fHashCode() && (sCode.length() != 0)) {
    iRaw = iFindHashCode(sCode);
    if (iRaw != iUNDEF)
      dseType = dseCODEEXIST;
    else
    {
        iRaw = iFindHashName(sCode);
        if ( iRaw !=iUNDEF )
            dseType=dseCODEISNAME;
    }
  }  
  if (fHashName() && (sValue.length() != 0)) {
    iRaw = iFindHashName(sValue);
    if (iRaw != iUNDEF) {
      if (dseType == dseCODEEXIST)
        dseType = dseBOTHEXIST;
      else
        dseType = dseVALUEEXIST;
    }  
  }    
  return dseType;
}

ValueRange DomainSort::vrConstruct() const
{
  ValueRange _vr;
  int iDec = 0;
  double r, rMin = DBL_MAX, rMax = -DBL_MAX;
  for (long i=1; i <= iSize(); i++) {
    String sVal = sValueByRaw(i, 0);
    r = sVal.rVal();
    if (r == rUNDEF) 
      return ValueRange();
    char * p = strchr(sVal.sVal(), '.');
    if (0 != p)
      iDec = strlen(p+1);
    if (r < rMin)
      rMin = r;  
    if (r > rMax)
      rMax = r;  
  }
  if (rMin < rMax) {
    double rStep =1;
    for (int i=1; i <= iDec; ++i)
      rStep /= 10;
    return ValueRange(rMin, rMax, rStep);
  }  
  return ValueRange();
}


long DomainSort::iMaxStringWidth() const
{
  long iMaxS = 0;
  for (long i=1; i <= iSize(); i++) {
    String sVal = sValueByRaw(i, 0);
    if (sVal.length() > iMaxS)
      iMaxS = sVal.length();
  }
  return iMaxS;  
}

void DomainSort::GetObjectStructure(ObjectStructure& os)
{
	DomainPtr::GetObjectStructure(os);

	String sPrefix ;
	if ( tbl.fValid())
		sPrefix = tbl->sSectionPrefix;
	String sSection = sPrefix + "TableStore";
	os.AddFile(fnObj, sSection, "data");
	if ( os.fGetAssociatedFiles())
	{
		if ( os.fCopyLinkedTable() )
		{
			sSection = String("%SDomainSort", sPrefix);
			os.AddFile(fnObj, sSection, "AttributeTable");
		}			
	}		
	
}

void DomainSort::DoNotUpdate()
{
	DomainPtr::DoNotUpdate();
	tbl->DoNotUpdate();
}



void DomainSort::Load()
{
	fLoaded = true; 
  if (!tbl.fValid())
		CreateColumns();
	if (FillHash())
	{
		String sMsg(SDATWarnDomainChanged.scVal(), fnObj.sFile);
		MessageBox(0, sMsg.scVal(), "", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		Store(); // ?? bah but Updated() did not work
	}
	Time tim = objtime;
	CalcIndex(false);
	fChanged = false;
	objtime = tim;
}
