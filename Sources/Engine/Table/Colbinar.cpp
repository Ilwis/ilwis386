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
/* ColumnBinary
   Copyright Ilwis System Development ITC
   march 1996, by Jelle Wind
	Last change:  WK    4 Mar 98    5:07 pm
*/
#define ILWCOLUMNBINARY_C
#include "Engine\Table\Colbinar.h"

ColumnBinary::ColumnBinary(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs) //, iOffset)
{
}

ColumnBinary::~ColumnBinary()
{
}

String ColumnBinary::sType() const
{
  return "Binary";
}

long ColumnBinary::iBufSize(long iKey) const
{
	iKey -= iOffset();
  if ((iKey < 0) || (iKey >= iRecs()))
		return 0;
	return buf[iKey].iSize();
}

void ColumnBinary::PutVal(long iKey, const BinMemBlock& binMem)
{
	iKey -= iOffset();
  if ((iKey < 0) || (iKey >= iRecs()))
		return;
  buf[iKey] = binMem;
}

void ColumnBinary::GetVal(long iKey, BinMemBlock& binMem) const
{
	iKey -= iOffset();
  binMem = const_cast<ColumnBinary *>(this)->buf.ind(iKey);
}

const BinMemBlock& ColumnBinary::bmbGetVal(long iKey) const
{
	iKey -= iOffset();
  return buf.ind(iKey);
}

void ColumnBinary::GetBufVal(Buf<BinMemBlock>& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey - iOffset(); i < iNr; ++j, ++i)
    b[i] = const_cast<ColumnBinary *>(this)->buf.ind(j);
}

void ColumnBinary::PutBufVal(const Buf<BinMemBlock>& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = b[i];
}

void ColumnBinary::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnBinary::AppendRec(long iRecs)
{
  buf.Append(iRecs);
  _iRecs = buf.iSize();
}

BinMemBlock::BinMemBlock()
{
  _iSize = 0;
  _ptr = 0;
}

BinMemBlock::BinMemBlock(long iSiz, const void* p)
{
  _iSize = iSiz;
  _ptr = 0;
  if (_iSize > 0) {
    _ptr = new char[_iSize];
    if (p)
      memcpy(_ptr, p, _iSize);
  }
}

BinMemBlock::BinMemBlock(const BinMemBlock& bmb)
{
  _ptr = 0;
  _iSize = bmb.iSize();
  if (_iSize > 0) {
    _ptr = new char[_iSize];
    memcpy(_ptr, bmb._ptr, _iSize);
  }
}

void BinMemBlock::operator = (const BinMemBlock& bmb)
{
	if (&bmb == this)
		return;
  if (_ptr)
    delete [] _ptr;
  _ptr = 0;
  _iSize = bmb.iSize();
  if (_iSize > 0) {
    _ptr = new char[_iSize];
    memcpy(_ptr, bmb._ptr, _iSize);
  }
}

BinMemBlock::~BinMemBlock()
{
  if (_ptr)
    delete [] _ptr;
  _ptr = 0;
}





