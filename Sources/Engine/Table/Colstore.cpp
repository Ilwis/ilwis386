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
/* $Log: /ILWIS 3.0/Column/Colstore.cpp $
 * 
 * 12    21/11/00 16:53 Willem
 * ColumnStoreBase::create() now also maps stINT to ColumnLong
 * 
 * 11    21-11-00 14:45 Koolhoven
 * removed ColumnByte and ColumnInt. Use in these cases also ColumnLong
 * 
 * 10    2-11-00 15:05 Hendrikse
 * implemented the inline functions moved from colstore.h incl check on
 * fLoaded
 * Removed redundant comments and old code
 * 
 * 9     9/18/00 9:39a Martin
 * function to set and get the load state of a column. the loadtstate of
 * newly created columns is set to true,
 * 
 * 8     4/11/00 4:30p Wind
 * added member Representation for columns with Domain picture (for
 * efficiency)
 * 
 * 7     10-01-00 8:28a Martin
 * GetValue(... CoordBuf..) now return thr number of records actually used
 * 
 * 6     21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 5     7-12-99 11:22 Wind
 * added/improved locks
 * 
 * 4     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 3     9/08/99 10:13a Wind
 * adpated for quoted column  names
 * 
 * 2     3/11/99 12:16p Martin
 * Added support for Case insesitive 
// Revision 1.4  1998/09/16 17:25:26  Wim
// 22beta2
//
// Revision 1.3  1997/08/28 08:52:09  Wim
// Give an error when user tries to create a Coord column
//
// Revision 1.2  1997-07-25 13:01:12+02  Wim
// PutRaw() now calls by default PutVal(iKey,"?")
// useful to set undefs for special column types.
//
/* ColumnStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    1 Jul 98   10:00 am
*/
#define COLSTORE_C
#include "Engine\Table\COLSTORE.H"
//#include "Engine\Table\COLBYTE.H"
#include "Engine\Table\COLCOORD.H"
//#include "Engine\Table\COLINT.H"
#include "Engine\Table\COLLONG.H"
#include "Engine\Table\COLREAL.H"
#include "Engine\Table\COLSTRNG.H"
#include "Engine\Table\Colbinar.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Hs\tbl.hs"

void ColumnCoordNotSupportedError()
{
  throw ErrorObject(TR("Coordinate Columns are not yet supported"), 1001);
}

ColumnStoreBase::ColumnStoreBase(long iRecs, long iOffset, ColumnStore& _pcs, bool fCreate)
: fChanged(_pcs.ptr.fChanged), dvs(_pcs.ptr.dvs)
{
  _iRecs = iRecs;
  _iOffset = iOffset;
  fChanged = fCreate;
}

ColumnStoreBase* ColumnStoreBase::create(const Table& tbl, const String& sColName, ColumnStore& p)
{
  String sSection("%SCol:%S", tbl->sSectionPrefix, sColName.sQuote());
  String sType;
  ObjectInfo::ReadElement(sSection.c_str(), "StoreType", tbl->fnObj.sFullPath(true), sType);
  // type is store type, not meaning type
  if (fCIStrEqual("Byte" , sType) || 
		  fCIStrEqual("Int" , sType) || 
			fCIStrEqual("Long" , sType))
    return new ColumnLong(tbl->iRecs(), tbl->iOffset(), p, false);
  if (fCIStrEqual("Real" , sType))
    return new ColumnReal(tbl->iRecs(), tbl->iOffset(), p, false);
  if (fCIStrEqual("String" , sType))
    return new ColumnString(tbl->iRecs(), tbl->iOffset(), p, false);
  if (fCIStrEqual("Binary" , sType))
    return new ColumnBinary(tbl->iRecs(), tbl->iOffset(), p, false);
	if (fCIStrEqual("CoordBuf" , sType) )
		return new ColumnCoordBuf(tbl->iRecs(), tbl->iOffset(), p, false);
  if (fCIStrEqual("Coord" , sType))
     return new ColumnCoord(tbl->iRecs(), tbl->iOffset(), p, false);
  InvalidTypeError(tbl->fnObj, sSection.c_str(), sType);
  return 0;
}

ColumnStoreBase* ColumnStoreBase::create(long iRecs, long iOffset, ColumnStore& p, const DomainValueRangeStruct& dvs)
{
  ColumnStoreBase* pcs;
  switch(dvs.st()) {
    case stBIT:
    case stDUET:
    case stNIBBLE:
    case stBYTE:
	case stINT:
    case stLONG:
      pcs = new ColumnLong(iRecs, iOffset, p, true);
      break;
    case stREAL:
      pcs = new ColumnReal(iRecs, iOffset, p, true);
      break;
    case stSTRING:
      pcs = new ColumnString(iRecs, iOffset, p, true);
      break;
    case stBINARY:
      pcs = new ColumnBinary(iRecs, iOffset, p, true);
      break;
    case stCRD:
	case stCRD3D:
      pcs = new ColumnCoord(iRecs, iOffset, p, true);
      break;
    case stCOORDBUF:
			pcs = new ColumnCoordBuf(iRecs, iOffset, p, true);
			break;
    default:
      return 0;
  }
  return pcs;
}

ColumnStoreBase::~ColumnStoreBase()
{
}

long ColumnStoreBase::iRaw(long) const
{
  return iUNDEF;
}

void ColumnStoreBase::GetBufRaw(ByteBuf& b, long iKey, long iNr) const
{
}

void ColumnStoreBase::GetBufRaw(IntBuf& b, long iKey, long iNr) const
{
}

void ColumnStoreBase::GetBufRaw(LongBuf& b, long iKey, long iNr) const
{
}

void ColumnStoreBase::GetBufVal(LongBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  if (!dvs.fValues() || !dvs.fRawAvailable()) {
    for (int i = 0; i < iNr; ++i)
      b[i] = iUNDEF;
    return;
  }
  GetBufRaw(b, iKey, iNr);
  if (!dvs.fRawIsValue())
    for (int i = 0; i < iNr; ++i)
      b[i] = dvs.iValue(b[i]);
}

void ColumnStoreBase::GetBufVal(RealBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  if (!dvs.fValues() || !dvs.fRawAvailable()) {
    for (int i = 0; i < iNr; ++i)
      b[i] = rUNDEF;
    return;
  }
  LongBuf lb(b.iSize());
  GetBufRaw(lb, iKey, iNr);
  if (dvs.fRawIsValue())
    for (int i = 0; i < iNr; ++i)
      b[i] = doubleConv(lb[i]);
  else
    for (int i = 0; i < iNr; ++i)
      b[i] = dvs.rValue(lb[i]);
}

void ColumnStoreBase::GetBufVal(StringBuf& b, long iKey, long iNr, short iWid, short iDec) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  LongBuf lb(b.iSize());
  GetBufRaw(lb, iKey, iNr);
  for (int i = 0; i < iNr; ++i)
    b[i] = dvs.sValueByRaw(lb[i], iWid, iDec);
}

void ColumnStoreBase::GetBufVal(CoordBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  for (int i = 0; i < iNr; ++i)
    b[i] = crdUNDEF;
}

long ColumnStoreBase::iValue(long iKey) const
{
  if (!dvs.fValues())
    return iUNDEF;
  long raw = iRaw(iKey);
  return dvs.iValue(raw);
}

double ColumnStoreBase::rValue(long iKey) const
{
  if (!dvs.fValues())
    return rUNDEF;
  long raw = iRaw(iKey);
  return dvs.rValue(raw);
}

Coord ColumnStoreBase::cValue(long iKey) const
{
  return crdUNDEF;
}

String ColumnStoreBase::sValue(long iKey, short iWidth, short iDec) const
{
  if (dvs.fUseReals()) {
    double val = rValue(iKey);
    return dvs.sValue(val, iWidth, iDec);
  }
  long raw = iRaw(iKey);
  return dvs.sValueByRaw(raw, iWidth, iDec);
}

ColumnStore::ColumnStore(const Table& tbl, const String& sColName, ColumnPtr& p)
: ptr(p), csb(0), fLoaded(false)
{
//  ReadElement(sSection(), "DataReadOnly", _fDataReadOnly);
  csb = ColumnStoreBase::create(tbl, sColName, *this);
  csb->_iOffset = tbl->iOffset();
  csb->_iRecs = tbl->iRecs();
	if (0 != p.dm()->pdp())
		rpr = p.dm()->rpr();
}

ColumnStore::ColumnStore(const Table& tbl, const String& sColName, ColumnPtr& p, const DomainValueRangeStruct& dvs)
: ptr(p), csb(0), fLoaded(false)
//  : ColumnPtr(tbl,sColName,dvs)
{
//  ptr._fDataReadOnly = false;
  csb = ColumnStoreBase::create(tbl->iRecs(), tbl->iOffset(), *this, dvs);
	fLoaded = true; // is new column needs not to be loaded from disk
	if (0 != p.dm()->pdp())
		rpr = p.dm()->rpr();
}

ColumnStore::~ColumnStore()
{
  if (0 != csb)
    delete csb;
}  

long ColumnStore::iRaw(long iKey) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
		ptr.ptrTbl->LoadData();
  long i = csb->iRaw(iKey);
  return i;
}  

long ColumnStore::iValue(long iKey) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
      ptr.ptrTbl->LoadData();
  long i = csb->iValue(iKey);
  return i;
}  

double ColumnStore::rValue(long iKey) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  double r = csb->rValue(iKey);
  return r;
}  

String ColumnStore::sValue(long iKey, short iWidth, short iDec) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  String s = csb->sValue(iKey, iWidth, iDec);
  return s;
}  

Coord ColumnStore::cValue(long iKey) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  Coord c = csb->cValue(iKey);
  return c;
} 

void ColumnStore::GetBufRaw(ByteBuf& buf, long iKey, long iNr) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufRaw(buf, iKey, iNr);
}  

void ColumnStore::GetBufRaw(IntBuf& buf, long iKey, long iNr) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufRaw(buf, iKey, iNr);
}  

void ColumnStore::GetBufRaw(LongBuf& buf, long iKey, long iNr) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufRaw(buf, iKey, iNr);
}  

void ColumnStore::GetBufVal(LongBuf& buf, long iKey, long iNr) const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
      ptr.ptrTbl->LoadData();
  csb->GetBufVal(buf, iKey, iNr);
}  

void ColumnStore::GetBufVal(RealBuf& buf, long iKey, long iNr) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufVal(buf, iKey, iNr);
}  

void ColumnStore::GetBufVal(StringBuf& buf, long iKey, long iNr, short iWid, short iDec) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufVal(buf, iKey, iNr, iWid, iDec);
}  

void ColumnStore::GetBufVal(CoordBuf& buf, long iKey, long iNr) const 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->GetBufVal(buf, iKey, iNr);
} 

CoordinateSequence *ColumnStore::iGetValue(long iKey, bool fResize) const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
	return csb->iGetValue(iKey);
}

void ColumnStore::PutVal(long iKey, const CoordinateSequence *seq, long iSz)
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
	csb->PutVal(iKey, seq, iSz);
}

void ColumnStore::Store()
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (ptr.fErase) {
    ptr.WriteElement(ptr.sSection().c_str(), NULL, NULL);
    return;
  }  
//  ColumnPtr::Store();
//ptr.WriteEntry("Type", "ColumnStore");
  ptr.WriteEntry("StoreType", csb->sType());
  ptr.WriteEntry("Stored", true);
//ptr.WriteEntry("NrDepObjects", 0L);
}

CoordinateSequence *ColumnStoreBase::iGetValue(long iKeym, bool fResize) const
{
	return NULL;
}

void ColumnStoreBase::PutRaw(long iKey, long)
{
  PutVal(iKey, "?"); // by default put undef string.
}

void ColumnStoreBase::PutVal(long iKey, long iVal)
{
  if (!dvs.fValues())
    return;
  long raw;
  if (dvs.fRawIsValue())
    raw = iVal;
  else
    raw = dvs.iRaw(iVal);
   PutRaw(iKey, raw);
}

void ColumnStoreBase::PutVal(long iKey, double rVal)
{
  if (!dvs.fValues())
    return;
  long raw;
  if (dvs.fRawIsValue())
    raw = byteConv(rVal);
  else
    raw = dvs.iRaw(rVal);
  PutRaw(iKey, raw);
}

void ColumnStoreBase::PutVal(long iKey, const String& sVal)
{
  if (dvs.fUseReals()) 
  {
    double val = dvs.rValue(sVal);
    PutVal(iKey, val);
  }
  else {
    long raw = dvs.iRaw(sVal);
    PutRaw(iKey, raw);
  }
}

void ColumnStoreBase::PutVal(long, const Coord&)
{
}

void ColumnStoreBase::PutVal(long, const CoordinateSequence *seq, long )
{
}

void ColumnStoreBase::PutBufRaw(const ByteBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();

  long j = max(0L, -iKey);
  long iMax = iKey + iNr;
  for (int i = 0; j < iMax; ++j, ++i)
    PutRaw(j, b[i]);
}

void ColumnStoreBase::PutBufRaw(const IntBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();

  long j = max(0L, -iKey);
  long iMax = iKey + iNr;
  for (int i = 0; j < iMax; ++j, ++i)
    PutRaw(j, longConv(b[i]));
}

void ColumnStoreBase::PutBufRaw(const LongBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();

  long j = max(0L, -iKey);
  long iMax = iKey + iNr;
  for (int i = 0; j < iMax; ++j, ++i)
    PutRaw(j, b[i]);
}

void ColumnStoreBase::PutBufVal(const LongBuf& b, long iKey, long iNr)
{
  if (!dvs.fValues())
    return;

  LongBuf buf(b.iSize());
  if (dvs.fRawIsValue())
    for (int i = 0; i < b.iSize(); ++i)
      buf[i] = b[i];
  else  
    for (int i = 0; i < b.iSize(); ++i)
      buf[i] = dvs.iRaw(b[i]);
  PutBufRaw(buf, iKey, iNr);
}

void ColumnStoreBase::PutBufVal(const RealBuf& b, long iKey, long iNr)
{
  if (!dvs.fValues())
    return;
 
  LongBuf buf(b.iSize());
  if (dvs.fRawIsValue())
    for (int i = 0; i < b.iSize(); ++i)
      buf[i] = b[i];
  else
    for (int i = 0; i < b.iSize(); ++i)
      buf[i] = dvs.iRaw(b[i]);
  PutBufRaw(buf, iKey, iNr);
}

void ColumnStoreBase::PutBufVal(const StringBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();

  long j = max(0L, -iKey);
  long iMax = iKey + iNr;
  for (int i = 0; j < iMax; ++j, ++i)
    PutVal(j, b[i]);
}

void ColumnStoreBase::PutBufVal(const CoordBuf& b, long iKey, long iNr)
{
}

void ColumnStoreBase::Fill()
{
}

void ColumnStoreBase::DeleteRec(long iStartRec, long iRecs)
{
}    

void ColumnStoreBase::AppendRec(long iRecs)
{
}

bool ColumnStore::fConvertTo(const DomainValueRangeStruct& _dvrsTo, const Column& col)
{
  DomainValueRangeStruct dvrsTo = _dvrsTo;
  if (col.fValid())
    dvrsTo = col->dvrs();
  Table tbl(ptr.fnTbl);
  Column colNew(tbl, String("#tm%li", abs(ObjectTime::timCurr()) % 10000L), dvrsTo);
  tbl->RemoveCol(colNew);
  colNew->fErase = true;
  if (dvrs().fValues()) {
    if (dvrsTo.fValues()) {
    // read real values and write real values
      double rVal;
      for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
        rVal = rValue(i);
        colNew->PutVal(i, rVal);
      }   
    }
    else if (col.fValid() && (dvrs().rStep() == 1)) {
      // read long values and use them as record nr. in column col
      long iRec, iVal;
      double rVal;
      for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
        iRec = iValue(i);
        if (dvrsTo.fUseReals()) {
          rVal = col->rValue(iRec);
          colNew->PutVal(i, rVal);
        }
        else {
          iVal = col->iRaw(iRec);
          colNew->PutRaw(i, iVal);
        } 
      }   
    }  
  }
  else if (col.fValid()) {
    // read long raws and use them as record nr. in column col
    long iRec, iVal;
    double rVal;
    for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
      iRec = iRaw(i);
      if (dvrsTo.fUseReals()) {
        rVal = col->rValue(iRec);
        colNew->PutVal(i, rVal);
      }
      else {
        iVal = col->iRaw(iRec);
        colNew->PutRaw(i, iVal);
      }  
    } 
  }
  else { // if ((dvrs().st() == stSTRING) && dvrsTo.dm()->pdsrt()) {
    String sVal;
    for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
      sVal = sValue(i, 0);
      colNew->PutVal(i, sVal);
    }
  }
  ptr.SetDomainValueRangeStruct(dvrsTo);
  if (0 != csb)
    delete csb;
  csb = ColumnStoreBase::create(iRecs(), iOffset(), *this, dvrsTo);
  if (dvrsTo.fUseReals())
    for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
      double rVal = colNew->rValue(i);
      PutVal(i, rVal);
    }
  else if (dvrsTo.fRawAvailable())
    for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
      long iVal = colNew->iRaw(i);
      PutRaw(i, iVal);
    }
  else
    for (long i=iOffset(); i < iOffset()+iRecs(); i++) {
      String sVal = colNew->sValue(i);
      PutVal(i, sVal);
    }
  colNew = Column();
  ptr.Store();
//  tbl->Store();
  return true;
}

bool ColumnStore::fIsLoaded() const 
{
	return fLoaded;
}

void ColumnStore::Loaded(bool fValue) 
{
  fLoaded = fValue;
}

void ColumnStore::PutRaw(long iKey, long iRaw) 
{
	ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
	csb->PutRaw(iKey, iRaw);
} 

void ColumnStore::PutVal(long iKey, long iVal) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutVal(iKey, iVal);
}  

void ColumnStore::PutVal(long iKey, double rVal) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutVal(iKey, rVal);
}  

void ColumnStore::PutVal(long iKey, const Coord& cVal) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutVal(iKey, cVal);
}

void ColumnStore::PutVal(long iKey, const String& sVal) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutVal(iKey, sVal);
}

void ColumnStore::PutBufRaw(const ByteBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufRaw(buf, iKey, iNr);
}  

void ColumnStore::PutBufRaw(const IntBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufRaw(buf, iKey, iNr);
}   

void ColumnStore::PutBufRaw(const LongBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufRaw(buf, iKey, iNr);
}  

void ColumnStore::PutBufVal(const LongBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufVal(buf, iKey, iNr);
}   

void ColumnStore::PutBufVal(const RealBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufVal(buf, iKey, iNr);
}  

void ColumnStore::PutBufVal(const StringBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufVal(buf, iKey, iNr);
}

void ColumnStore::PutBufVal(const CoordBuf& buf, long iKey, long iNr) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->PutBufVal(buf, iKey, iNr);
}  

void ColumnStore::DeleteRec(long iStartRec, long iRecs) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->DeleteRec(iStartRec, iRecs);
  ptr._iRecs = csb->iRecs();
} 

void ColumnStore::AppendRec(long iRecs) 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (!fLoaded)
    ptr.ptrTbl->LoadData();
  csb->AppendRec(iRecs);
  ptr._iRecs = csb->iRecs();
} 

void ColumnStore::Fill() 
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  csb->Fill();
}



