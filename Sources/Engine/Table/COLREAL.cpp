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
/* ColumnReal
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    1:52 pm
*/

#include "Engine\Table\COLREAL.H"

ColumnReal::ColumnReal(long iRecs, long iOffset, ColumnStore& cs, bool fCreate)
: ColumnStoreBase(iRecs, iOffset, cs, fCreate),  buf(iRecs)//, iOffset)
{
  if (fCreate)
    Fill();  // NAN protection for not used domain values
}

ColumnReal::~ColumnReal()
{
}

String ColumnReal::sType() const
{
  return "Real";
}

double ColumnReal::rValue(long iKey) const
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
    return rUNDEF;
  return buf[iKey-iOffset()];
}

void ColumnReal::GetBufVal(RealBuf& b, long iKey, long iNr) const
{
  if (iNr == 0 || iNr > b.iSize())
    iNr = b.iSize();
  int i = 0;
  for (long j = iKey; i < iNr; ++j, ++i)
    try {
      b[i] = buf[j-iOffset()];
    }
    catch (...) {
      b[i] = rUNDEF;
    }
}

void ColumnReal::PutVal(long iKey, double rVal)
{
  if ((iKey < iOffset()) || (iKey >= iOffset()+iRecs()))
		return;
  buf[iKey-iOffset()] = rVal;
//Updated();
//TablePtr::Changed(fnTbl);
}

void ColumnReal::PutBufVal(const RealBuf& b, long iKey, long iNr)
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

void ColumnReal::DeleteRec(long iStartRec, long iRecs)
{
  buf.Remove(iStartRec - iOffset(), iRecs);
  _iRecs = buf.iSize();
}

void ColumnReal::AppendRec(long iRcs)
{
  buf.Append(iRcs);
  for (unsigned long i = iRecs(); i < buf.iSize(); i++)
    buf[i] = rUNDEF;
  _iRecs = buf.iSize();
}

void ColumnReal::Fill()
{
  for (unsigned long i=0; i < buf.iSize(); i++)
    buf[i] = rUNDEF;
}




