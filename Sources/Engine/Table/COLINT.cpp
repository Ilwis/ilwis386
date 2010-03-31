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
/* ColumnInt
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    4:02 pm
*/

#include "Engine\Table\COLINT.H"

ColumnInt::ColumnInt(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs) //, iOffset)
{
}

ColumnInt::~ColumnInt()
{
}

String ColumnInt::sType() const
{
  return "Int";
}

long ColumnInt::iRaw(long iKey) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return iUNDEF;
  return longConv((short)buf[iKey-iOffset()]);
}

void ColumnInt::GetBufRaw(ByteBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = byteConv((short)buf[j-iOffset()]);
}

void ColumnInt::GetBufRaw(IntBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = buf[j-iOffset()];
}

void ColumnInt::GetBufRaw(LongBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = longConv((short)buf[j-iOffset()]);
}

void ColumnInt::PutRaw(long iKey, long iRaw)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
	if ( iKey - iOffset() >= 0)
		buf[iKey-iOffset()] = shortConv(iRaw);
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnInt::PutBufRaw(const ByteBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();	
  for (long j = iKey; i < iNr; ++j, ++i)
		if ( j - iShift >= 0 )		
			buf[j-iShift] = b[i];
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnInt::PutBufRaw(const IntBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();	
  for (long j = iKey; i < iNr; ++j, ++i)
		if ( j - iShift >= 0 )		
			buf[j-iShift] = b[i];
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnInt::PutBufRaw(const LongBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();	
  for (long j = iKey; i < iNr; ++j, ++i)
		if ( j - iShift >= 0 )		
			buf[j-iShift] = shortConv(b[i]);
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnInt::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnInt::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = shUNDEF;
  _iRecs = buf.iSize();
}

void ColumnInt::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = shUNDEF;
}




