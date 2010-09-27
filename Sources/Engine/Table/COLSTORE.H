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
// $Log: /ILWIS 3.0/Column/COLSTORE.H $
 * 
 * 9     2-11-00 15:03 Hendrikse
 * many inline functions like PutVal moved to cpp file an omplemented ther
 * with check on fLoaded
 * 
 * 8     9/18/00 9:39a Martin
 * function to set and get the load state of a column. the loadtstate of
 * newly created columns is set to true,
 * 
 * 7     4/11/00 4:30p Wind
 * added member Representation for columns with Domain picture (for
 * efficiency)
 * 
 * 6     10-01-00 8:28a Martin
 * GetValue(... CoordBuf..) now return thr number of records actually used
 * 
 * 5     21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 4     7-12-99 11:22 Wind
 * added/improved locks
 * 
 * 3     25-10-99 10:40 Wind
 * comments
 * 
 * 2     22-10-99 12:56 Wind
 * thread save access (not yet finished)
*/
// Revision 1.3  1998/09/16 17:25:26  Wim
// 22beta2
//
// Revision 1.2  1997/08/01 08:48:58  Wim
// destructor of ColumnStoreBase was not virtual,
// so members of derived classes (buffers with large amounts of data!) were never destructed
//
/* ColumnStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK    1 Aug 97   10:48 am
*/

#ifndef ILWCOLSTORE_H
#define ILWCOLSTORE_H
#include "Engine\Table\Col.h"
#include "Engine\Table\tbl.h"
#include "Engine\Representation\Rpr.h"

class DATEXPORT ColumnStore;

class DATEXPORT ColumnStoreBase
{
  friend DATEXPORT ColumnStore;
public:
  ColumnStoreBase(long iRecs, long iOffset, ColumnStore& pcs, bool fCreate = false);
  virtual ~ColumnStoreBase();
  static ColumnStoreBase* create(long iRecs, long iOffset, ColumnStore&, const DomainValueRangeStruct&);
  static ColumnStoreBase* create(const Table&, const String& sColName, ColumnStore& p);
  virtual long iRaw(long iKey) const;
  virtual long iValue(long iKey) const;
  virtual double rValue(long iKey) const;
  virtual String sValue(long iKey, short iWidth=-1, short iDec=-1) const;
  virtual Coord cValue(long iKey) const;
	virtual CoordinateSequence *iGetValue(long iRec,bool fResize = false) const;
  virtual void GetBufRaw(ByteBuf&, long iKey=0, long iNr=0) const;
  virtual void GetBufRaw(IntBuf& , long iKey=0, long iNr=0) const;
  virtual void GetBufRaw(LongBuf&, long iKey=0, long iNr=0) const;
  virtual void GetBufVal(LongBuf&, long iKey=0, long iNr=0) const;
  virtual void GetBufVal(RealBuf&, long iKey=0, long iNr=0) const;
  virtual void GetBufVal(StringBuf&, long iKey=0, long iNr=0, short iWid=0, short iDec=-1) const;
  virtual void GetBufVal(CoordBuf&, long iKey, long iNr) const;
  virtual void PutRaw(long iKey, long iRaw);
  virtual void PutVal(long iKey, long iVal);
  virtual void PutVal(long iKey, double rVal);
  virtual void PutVal(long iKey, const Coord& cVal);
  virtual void PutVal(long iKey, const String& sVal);
	virtual void PutVal(long iKey, const CoordBuf& Buf, long iSz);
  virtual void PutBufRaw(const ByteBuf&, long iKey=0, long iNr=0);
  virtual void PutBufRaw(const IntBuf& , long iKey=0, long iNr=0);
  virtual void PutBufRaw(const LongBuf&, long iKey=0, long iNr=0);
  virtual void PutBufVal(const LongBuf&, long iKey=0, long iNr=0);
  virtual void PutBufVal(const RealBuf&, long iKey=0, long iNr=0);
  virtual void PutBufVal(const StringBuf&, long iKey=0, long iNr=0);
  virtual void PutBufVal(const CoordBuf&, long iKey, long iNr);
  virtual void DeleteRec(long iStartRec, long iRecs = 1);
  virtual void AppendRec(long iRecs = 1);
  virtual void Fill();
  long iOffset() const { return _iOffset; }
  long iRecs() const { return _iRecs; }
  ValueRange& vr() { return const_cast<DomainValueRangeStruct &>(dvs).vr(); }
  const Domain& dm() const { return dvs.dm(); }
  virtual String sType() const=0;
protected:
  bool& fChanged;
  const DomainValueRangeStruct& dvs;
  long _iRecs, _iOffset;
};

class DATEXPORT ColumnStore
{
  friend class DATEXPORT ColumnPtr;
  friend class DATEXPORT ColumnVirtual;
  friend class DATEXPORT ColumnStoreBase;
public:
  ColumnStore(const Table& tbl, const String& sColName, ColumnPtr&);
  ColumnStore(const Table& tbl, const String& sColName, ColumnPtr&, const DomainValueRangeStruct& dvs);
  ~ColumnStore();
  void Store();
  long iRaw(long iKey) const;
  long iValue(long iKey) const;
  double rValue(long iKey) const;
  String sValue(long iKey, short iWidth=-1, short iDec=-1) const;
  Coord cValue(long iKey) const;
	CoordinateSequence  *iGetValue(long iRec, bool fResize = false) const;
  void GetBufRaw(ByteBuf& buf, long iKey=0, long iNr=0) const;
  void GetBufRaw(IntBuf& buf, long iKey=0, long iNr=0) const;
  void GetBufRaw(LongBuf& buf, long iKey=0, long iNr=0) const;
  void GetBufVal(LongBuf& buf, long iKey=0, long iNr=0) const;
  void GetBufVal(RealBuf& buf, long iKey=0, long iNr=0) const;
  void GetBufVal(StringBuf& buf, long iKey=0, long iNr=0, short iWid=0, short iDec=-1) const;
  void GetBufVal(CoordBuf& buf, long iKey, long iNr) const;
  void _export PutRaw(long iKey, long iRaw);
	void PutVal(long iKey, const CoordBuf& cb, long iSz);
  void PutVal(long iKey, long iVal);
  void _export PutVal(long iKey, double rVal);
  void PutVal(long iKey, const Coord& cVal);
  void PutVal(long iKey, const String& sVal);
  void PutBufRaw(const ByteBuf& buf, long iKey=0, long iNr=0);
  void PutBufRaw(const IntBuf& buf, long iKey=0, long iNr=0);
  void PutBufRaw(const LongBuf& buf, long iKey=0, long iNr=0);
  void PutBufVal(const LongBuf& buf, long iKey=0, long iNr=0);
  void PutBufVal(const RealBuf& buf, long iKey=0, long iNr=0);
  void PutBufVal(const StringBuf& buf, long iKey=0, long iNr=0);
  void PutBufVal(const CoordBuf& buf, long iKey, long iNr);
  void DeleteRec(long iStartRec, long iRecs = 1);
  void AppendRec(long iRecs = 1);
  void Fill();
  ColumnStoreBase* pcsb() {
    return csb;
  }
  bool fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
  long iRecs() const { return ptr.iRecs(); }
  long iOffset() const { return ptr.iOffset(); }
  Domain dm() const { return ptr.dm(); }
  const DomainValueRangeStruct& dvrs() const { return ptr.dvrs(); }
  void _export Loaded(bool fValue);
  bool _export fIsLoaded() const;
private:  
  ColumnStoreBase* csb;
  ColumnPtr& ptr;
  ObjectTime timStore;
  Representation rpr; // only for column with DomainPicture (for efficiency)
  bool fLoaded;

};


#endif // ILWCOLSTORE_H




