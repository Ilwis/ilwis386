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
/* ColumnString
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  JEL  27 May 97    1:58 pm
*/

#include "Engine\Table\COLSTRNG.H"

ColumnString::ColumnString(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs),  buf(iRecs) //, iOffset)
{
}

ColumnString::~ColumnString()
{
}

String ColumnString::sType() const
{
  return "String";
}

void ColumnString::PutVal(long iKey, const String& sVal)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
  buf[iKey-iOffset()] = sVal.sTrimSpaces();
//Updated();
//TablePtr::Changed(fnTbl);
}

String ColumnString::sValue(long iKey, short iWidth,short) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return sUNDEF;
  if (iWidth == 0)
    return buf[iKey-iOffset()];
  if (iWidth < 0)
    iWidth = dm()->iWidth();
  return String("%*S", -iWidth, buf[iKey-iOffset()]);
}

void ColumnString::GetBufVal(StringBuf& b, long iKey, long iNr, short, short) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    b[i] = buf[j-iOffset()];
}

void ColumnString::PutBufVal(const StringBuf& b, long iKey, long iNr)
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    buf[j-iOffset()] = b[i].sTrimSpaces();
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnString::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec-iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnString::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = sUNDEF;
  _iRecs = buf.iSize();

}

void ColumnString::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = sUNDEF;
}




