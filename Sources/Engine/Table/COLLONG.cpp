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
/* ColumnLong
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    4:02 pm
*/

#include "Engine\Table\COLLONG.H"

ColumnLong::ColumnLong(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs) //, iOffset)
{
}

ColumnLong::~ColumnLong()
{
}

String ColumnLong::sType() const
{
  return "Long";
}

long ColumnLong::iRaw(long iKey) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return iUNDEF;
  return buf[iKey-iOffset()];
}

void ColumnLong::GetBufRaw(ByteBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = byteConv(buf[j-iOffset()]);
}

void ColumnLong::GetBufRaw(IntBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = shortConv(buf[j-iOffset()]);
}

void ColumnLong::GetBufRaw(LongBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = buf[j-iOffset()];
}

void ColumnLong::PutRaw(long iKey, long iRaw)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
	int iShift = iOffset();
	if ( iKey - iShift >= 0 )	
	  buf[iKey-iShift ] = iRaw;

}

void ColumnLong::PutBufRaw(const ByteBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();	
  for (long j = iKey; i < iNr; ++j, ++i)
		if ( j - iShift >= 0 )		
			buf[j-iShift] = b[i];

}

void ColumnLong::PutBufRaw(const IntBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();	
  for (long j = iKey; i < iNr; ++j, ++i)
  {
		if ( j - iShift >= 0 )		
			buf[j-iShift] = longConv(b[i]);
  }
}

void ColumnLong::PutBufRaw(const LongBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
	int iShift = iOffset();
  for (long j = iKey; i < iNr; ++j, ++i)
	{
		if ( j - iShift >= 0 )
			buf[j-iShift] = b[i];
	}		
}

void ColumnLong::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnLong::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = iUNDEF;
  _iRecs = buf.iSize();
}

void ColumnLong::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = iUNDEF;
}
