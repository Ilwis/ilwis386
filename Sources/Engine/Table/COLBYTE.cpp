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
/* ColumnByte
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    4:02 pm
*/

#include "Engine\Table\COLBYTE.H"
#include "Engine\Domain\Dmvalue.h"

ColumnByte::ColumnByte(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs) //, iOffset)
{
}

ColumnByte::~ColumnByte()
{
}

String ColumnByte::sType() const
{
  return "Byte";
}

long ColumnByte::iRaw(long iKey) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return iUNDEF;
  byte b = buf[iKey-iOffset()];
  if (b == 0)
    if (dvs.iRawUndef() == 0)
      return iUNDEF;
  return b;
}

void ColumnByte::GetBufRaw(ByteBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = buf[j-iOffset()];
}

void ColumnByte::GetBufRaw(IntBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  if (dvs.iRawUndef() != 0)
    for (long j = iKey; i < iNr; ++j, ++i)
      b[i] = buf[j-iOffset()];
  else {
    byte bb;
    for (long j = iKey; i < iNr; ++j, ++i) {
      bb = buf[j-iOffset()];
      if (bb == 0)
        if (dvs.iRawUndef() == 0) {
          b[i] = shUNDEF;
          continue;
        }
      b[i] = bb;
    }
  }
}

void ColumnByte::GetBufRaw(LongBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  byte bb;
  if (dvs.iRawUndef() != 0)
    for (long j = iKey; i < iNr; ++j, ++i)
      b[i] = buf[j-iOffset()];
  else
    for (long j = iKey; i < iNr; ++j, ++i) {
      bb = buf[j-iOffset()];
      if (bb == 0)
        if (dvs.iRawUndef() == 0) {
          b[i] = iUNDEF;
          continue;
        }
      b[i] = bb;
    }
}

void ColumnByte::PutRaw(long iKey, long iRaw)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
  buf[iKey-iOffset()] = byteConv(iRaw);
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnByte::PutBufRaw(const ByteBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = b[i];
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnByte::PutBufRaw(const IntBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = byteConv(b[i]);
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnByte::PutBufRaw(const LongBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = byteConv(b[i]);
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnByte::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnByte::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = 0;
  _iRecs = buf.iSize();
}

void ColumnByte::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = 0;
}
